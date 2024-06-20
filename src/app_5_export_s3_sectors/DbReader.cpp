#include "DbReader.h"
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

        _start = chrono::high_resolution_clock::now();
    }

    void DbReader::getStars(const bool getNonGaiaStars, const function<void(const CelestialObject *)>& func) const
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

                if(searchToId % 1000000 == 0)
                {
                    spdlog::info("Processed to index {:L} to {}.", searchToId, Thx::toDurationString(_start));
                }

                dbTransaction.for_stream(selectString, [&]
                (const long long index, const char* name1, const char* name2, const char* name3, const char* name4, const double parallax, const optional<int> teff,
                    const char* spectralType, const optional<double> metalicity, const optional<double> luminosity, const optional<double> radius,
                    const optional<double> magVB, const double x, const double y, const double z, const optional<long long> sourceId,
                    const bool isBinary, const optional<int> type){

                    celestialObject.index = index;
                    celestialObject.name1 = name1 == nullptr ? "" : name1;
                    celestialObject.name2 = name2 == nullptr ? "" : name2;
                    celestialObject.name3 = name3 == nullptr ? "" : name3;
                    celestialObject.name4 = name4 == nullptr ? "" : name4;
                    celestialObject.parallax = parallax;
                    celestialObject.teff = teff.has_value() ? &teff.value() : nullptr;
                    celestialObject.spectralType = spectralType == nullptr ? "" : spectralType;
                    celestialObject.metallicity = metalicity.has_value() ? &metalicity.value() : nullptr;
                    celestialObject.luminosity = luminosity.has_value() ? &luminosity.value() : nullptr;
                    celestialObject.radius = radius.has_value() ? &radius.value() : nullptr;
                    celestialObject.magnitudeVorB = magVB.has_value() ? &magVB.value() : nullptr;
                    celestialObject.x = x;
                    celestialObject.y = y;
                    celestialObject.z = z;
                    celestialObject.sourceId = sourceId.has_value() ? &sourceId.value() : nullptr;
                    celestialObject.isBinary = isBinary;
                    celestialObject.type = static_cast<SimbadObjectTypes>(type.has_value() ? type.value() : 0);

                    const auto sx = GetSectorNumber(celestialObject.x);
                    const auto sy = GetSectorNumber(celestialObject.y);
                    const auto sz = GetSectorNumber(celestialObject.z);

                    celestialObject.sectorId = std::format("X{}{}Y{}{}Z{}{}",
                        sx >= 0 ? 'P' : 'N', abs(sx),
                        sy >= 0 ? 'P' : 'N', abs(sy),
                        sz >= 0 ? 'P' : 'N', abs(sz));

                    if(celestialObject.spectralType.empty() && celestialObject.teff != nullptr)
                    {
                        double mag; // Puke, there has to be a better way.
                        const double* absoluteMagnitude = nullptr;
                        if(celestialObject.magnitudeVorB != nullptr)
                        {
                            mag = astronomy::AstronomyConverter::toAbsoluteMagnitude(celestialObject.parsecs, *celestialObject.magnitudeVorB);

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
        catch (const exception &e)
        {
            SPDLOG_ERROR("startQuery error", e.what());
            throw;
        }
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
