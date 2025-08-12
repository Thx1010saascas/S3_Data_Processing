#include "DbReader.h"

#include <iostream>
#include <queue>
#include <spdlog/spdlog.h>
#include <pqxx/pqxx>
#include "Thx.h"
#include "Sector.h"
#include "AstronomyConverter.h"
#include "SpectralClassifier.h"

using namespace pqxx;

namespace thxsoft::export_s3_sectors
{
    DbReader::DbReader(const std::string& connectionString, const int searchRangeLimitLy, const int objectBitMask)
        : _searchRangeLimitLy(searchRangeLimitLy), _objectBitMask(objectBitMask)
    {
        _dbReadConnection = make_shared<connection>(connectionString);

        spectralClassifier = std::make_shared<astronomy::SpectralClassifier>();

        if (_dbReadConnection->is_open())
        {
            spdlog::debug("Opened database successfully: {}", _dbReadConnection->dbname());
        }
        else
        {
            SPDLOG_ERROR("Error opening DbReader database.");
        }

        _start = std::chrono::steady_clock::now();
    }

    void DbReader::getStars(const bool getNonGaiaStars, const std::function<void(const CelestialObject*)>& func) const
    {
        try
        {
            const auto searchQuery = getNonGaiaStars ? _nonGaiaObjectsQuery : _gaiaObjectsQuery;

            auto searchFromId = 0L;
            const auto maxId = getMaxIdAsync(getNonGaiaStars ? "simbad" : "gaia");

            CelestialObject celestialObject;
            auto dbTransaction = transaction(*_dbReadConnection);

            while (searchFromId <= maxId + QueryPageSize)
            {
                auto searchToId = searchFromId + QueryPageSize;

                const auto selectString = fmt::format(fmt::runtime(searchQuery), searchFromId, searchToId, _searchRangeLimitLy, _objectBitMask);

                if (searchToId % 1000000 == 0)
                {
                    spdlog::info("Processed to index {:L} to {}.", searchToId, Thx::toDurationString(_start));
                }

                dbTransaction.for_stream(selectString, [&]
                     (const long long index,const std::optional<std::string> name,const std::optional<std::string> name_wolf,
                         const std::optional<std::string> name_ross,const std::optional<std::string> name_s,
                         const std::optional<std::string> name_ss,const std::optional<std::string> name_vs,
                         const std::optional<std::string> name_hip,const std::optional<std::string> name_hd,
                         const std::optional<std::string> name_gj,const std::optional<std::string> name_wise,
                         const std::optional<std::string> name_2mass, const std::optional<std::string> name_gaia,
                         const std::optional<std::string> name_tyc, const std::optional<std::string> name_ngc,
                         const double parallax, const std::optional<double> teff, const std::optional<std::string>& spectralType,
                         const std::optional<double> metalicity, const std::optional<double> luminosity, const std::optional<double> radius,
                         const std::optional<double> magVB, const double x, const double y, const double z,
                         const std::optional<long long> sourceId,const bool isBinary, const std::optional<int> type) {
                    auto nameQueue = std::queue<std::string>();

                    if(!teff.has_value())
                    {
                        //Stars have to have a Teff.
                        if(!type.has_value() || (type.value() & static_cast<int>(Star)) > 0)
                            return;
                    }

                    auto name2 = name;
                    auto name_ss2 = name_ss;
                    if(name.has_value() && name->starts_with("**"))
                    {
                        name_ss2 = name;
                        name2 = std::nullopt;
                    }

                    pushNameString(nameQueue, name2);
                    pushNameString(nameQueue, name_wolf.has_value() ? name_wolf : name_ross);
                    pushNameString(nameQueue, name_s.has_value() ? name_s->substr(2) : name_vs.has_value() ? name_vs->substr(3) : name_vs);

                    if(name_hip.has_value())
                        pushNameString(nameQueue, name_hip);

                    pushNameString(nameQueue, name_hd.has_value() ? name_hd : name_gj.has_value() ? name_gj : name_wise);
                    pushNameString(nameQueue, name_2mass.has_value() ? name_2mass : name_gaia);
                    pushNameString(nameQueue, name_tyc.has_value() ? name_tyc : name_ngc);

                    if(name_ss2.has_value())
                        pushNameString(nameQueue, name_ss2->substr(3));

                    std::string exportName1 = popNameString(nameQueue);
                    std::string exportName2 = popNameString(nameQueue);;
                    std::string exportName3 = popNameString(nameQueue);;
                    std::string exportName4 = popNameString(nameQueue);;

                    celestialObject.index = index;
                    celestialObject.name1 = exportName1;
                    celestialObject.name2 = exportName2;
                    celestialObject.name3 = exportName3;
                    celestialObject.name4 = exportName4;
                    celestialObject.parallax = parallax;
                    celestialObject.teff = teff;
                    celestialObject.spectralType = spectralType;
                    celestialObject.metallicity = metalicity;
                    celestialObject.luminosity = luminosity;
                    celestialObject.radius = radius;
                    celestialObject.magnitudeVorB = magVB;
                    celestialObject.x = x;
                    celestialObject.y = z; // Swapped Y,Z for Dan
                    celestialObject.z = y;
                    celestialObject.sourceId = sourceId;
                    celestialObject.isBinary = isBinary;
                    celestialObject.type = static_cast<SimbadObjectTypes>(type.has_value() ? type.value() : 0);

                    const auto sx = GetSectorNumber(celestialObject.x);
                    const auto sy = GetSectorNumber(celestialObject.y);
                    const auto sz = GetSectorNumber(celestialObject.z);

                    celestialObject.sectorId = std::format("X{}{}Y{}{}Z{}{}",
                                                        sx >= 0 ? 'P' : 'N', abs(sx),
                                                        sy >= 0 ? 'P' : 'N', abs(sy),
                                                        sz >= 0 ? 'P' : 'N', abs(sz));

                     if (!celestialObject.spectralType.has_value() && celestialObject.teff)
                     {
                         double mag; // Puke, there has to be a better way.
                         const double* absoluteMagnitude = nullptr;
                         if (celestialObject.magnitudeVorB.has_value())
                         {
                             mag = astronomy::AstronomyConverter::toAbsoluteMagnitude(celestialObject.parsecs, celestialObject.magnitudeVorB.value());

                             absoluteMagnitude = &mag;
                         }

                         celestialObject.spectralType = spectralClassifier->getSpectralType(*celestialObject.teff, absoluteMagnitude);
                     }

                     func(&celestialObject);
                 });

                searchFromId = searchToId;
            }

            dbTransaction.commit();
        }
        catch (const std::exception& e)
        {
            SPDLOG_ERROR("startQuery error", e.what());
            throw;
        }
    }

    void DbReader::pushNameString(std::queue<std::string>& queue, const std::optional<std::string>& name)
    {
        if(name.has_value())
            queue.push(name.value());
    }

    std::string DbReader::popNameString(std::queue<std::string>& queue)
    {
        if(queue.empty())
            return "";

        const auto value = queue.front();

        queue.pop();

        return value;
    }

    long long DbReader::getMaxIdAsync(const std::string& tableName) const
    {
        auto dbTransaction = transaction(*_dbReadConnection);

        const auto result = dbTransaction.exec1(std::format("SELECT MAX(id) FROM {};", tableName));

        dbTransaction.commit();

        return result[0].as<long long>();
    }

    double DbReader::GetSectorNumber(const double v)
    {
        auto number = v;

        if (abs(number) > 100)
            number = round(number / Sector::SectorLengthLy);
        else
        {
            number /= Sector::HalfSectorLengthLy;

            if (number >= 0)
                number = floor(number);
            else
                number = ceil(number);
        }

        return number;
    }
}
