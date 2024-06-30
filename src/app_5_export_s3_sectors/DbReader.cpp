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
    DbReader::DbReader(const string& connectionString, const int searchRangeLimitLy, const int objectBitMask)
        : _searchRangeLimitLy(searchRangeLimitLy), _objectBitMask(objectBitMask)
    {
        _dbReadConnection = make_shared<connection>(connectionString);

        spectralClassifier = make_shared<astronomy::SpectralClassifier>();

        if (_dbReadConnection->is_open())
        {
            spdlog::debug("Opened database successfully: {}", _dbReadConnection->dbname());
        }
        else
        {
            SPDLOG_ERROR("Error opening DbReader database.");
        }

        _start = chrono::steady_clock::now();
    }

    void DbReader::getStars(const bool getNonGaiaStars, const function<void(const CelestialObject*)>& func) const
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

                const auto selectString = vformat(searchQuery, make_format_args(searchFromId, searchToId, _searchRangeLimitLy, _objectBitMask));

                if (searchToId % 1000000 == 0)
                {
                    spdlog::info("Processed to index {:L} to {}.", searchToId, Thx::toDurationString(_start));
                }

                dbTransaction.for_stream(selectString, [&]
                     (const long long index,const char* name,const char* name_wolf,const char* name_ross,
                         const char* name_s,const char* name_ss,const char* name_vs,const char* name_hip,
                         const char* name_hd,const char* name_gj,const char* name_wise,
                         const char* name_2mass, const char* name_gaia,const char* name_tyc, const char* name_ngc,
                         const double parallax, const optional<double> teff, const optional<string>& spectralType,
                         const optional<double> metalicity, const optional<double> luminosity, const optional<double> radius,
                         const optional<double> magVB, const double x, const double y, const double z,
                         const optional<long long> sourceId,const bool isBinary, const optional<int> type) {
                    auto nameQueue = queue<string>();

                    if(!teff.has_value())
                    {
                        //Stars have to have a Teff.
                        if(!type.has_value() || type.value() & static_cast<int>(Star) > 0)
                            return;
                    }

                    if(name != nullptr && strstr(name, "**") != nullptr)
                    {
                        name_ss = name;
                        name = nullptr;
                    }

                    pushNameString(nameQueue, name);
                    pushNameString(nameQueue, name_wolf != nullptr ? name_wolf : name_ross);
                    pushNameString(nameQueue, name_s != nullptr ? name_s : name_vs);

                    if(name_hip != nullptr)
                        pushNameString(nameQueue, name_hip);

                    pushNameString(nameQueue, name_hd != nullptr ? name_hd : name_gj != nullptr ? name_gj : name_wise);
                    pushNameString(nameQueue, name_2mass != nullptr ? name_2mass : name_gaia);
                    pushNameString(nameQueue, name_tyc != nullptr ? name_tyc : name_ngc);

                    if(name_ss != nullptr)
                        pushNameString(nameQueue, name_ss);

                    string exportName1 = popNameString(nameQueue);
                    string exportName2 = popNameString(nameQueue);;
                    string exportName3 = popNameString(nameQueue);;
                    string exportName4 = popNameString(nameQueue);;

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
                    celestialObject.y = y;
                    celestialObject.z = z;
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
        catch (const exception& e)
        {
            SPDLOG_ERROR("startQuery error", e.what());
            throw;
        }
    }

    void DbReader::pushNameString(queue<string>& queue, const char* name)
    {
        if(name != nullptr)
        {
            const string str = name;
            queue.push(str);
        }
    }

    string DbReader::popNameString(queue<string>& queue)
    {
        if(queue.empty())
            return "";

        const auto value = queue.front();

        queue.pop();

        return value;
    }

    long long DbReader::getMaxIdAsync(const string& tableName) const
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
