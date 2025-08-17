#include <iostream>
#include "DbReader.h"
#include "ExportToSql.h"
#include "LoggingSetup.h"
#include "Thx.h"

using namespace thxsoft::database;
using namespace thxsoft::export_s3_sectors;

void showSyntax();

int main(const int argc, const char *argv[])
{
#ifdef  __MINGW32__
    Thx::setCustomLocale();
#else
    std::locale::global(std::locale("en_AU"));
#endif

    try
    {
        if(argc < 5 || argc > 6)
        {
            showSyntax();
            return -1;
        }

        LoggingSetup::setupDefaultLogging("logs/5_ExportS3Database.log");

        auto sqliteDbPath = std::filesystem::path(argv[1]);
        const auto postgresCnxString = argv[2];
        const auto distanceLy = std::stoi(argv[3]);
        const auto appendMode = argc == 6 ? std::strcmp(argv[5], "append") == 0 : false;

        auto objectType = 0;

        const auto start = std::chrono::steady_clock::now();

        for(const auto& type : Thx::split(argv[4], ","))
        {
            try
            {
                objectType |= SimbadObjectTypesMap.at(type);
            }
            catch (std::exception&)
            {
                std::cerr << "Object type '" << type << "' was not found. Check your spelling and capitalisation.";
                showSyntax();
                return -1;
            }
        }

        const auto deleteExistingDb = !appendMode;

        sqliteDbPath /= std::format("S3_{}ly{}.db", distanceLy, appendMode ? "" : (std::string("_") + argv[4]));

        spdlog::info("Writing to '{}'", sqliteDbPath.string());

        const DbReader dbReader(postgresCnxString, distanceLy, objectType);

        auto addedCount = 0;

        const ExportToSql dbWriter(sqliteDbPath.string(), deleteExistingDb);

        spdlog::info("Processing Simbad data.");

        auto testSuccessCount = 0;

        dbReader.getStars(true, [&] (const CelestialObject* celestialObject)
        {
            // Sanity checks
            if(celestialObject->name1 == "Sol")
            {
                if(celestialObject->sectorId != "XN267YP0ZP1")
                    throw pqxx::data_exception("Sol sector is in the wrong place!");

                testSuccessCount++;
            }
            else if(celestialObject->name1 == "Sirius")
            {
                if(round(celestialObject->x) != -26676 ||
                    round(celestialObject->y) != -5 ||
                    round(celestialObject->z) != 59)
                    throw pqxx::data_exception("Sirius sector is in the wrong place!");

                testSuccessCount++;
            }

            dbWriter.append(celestialObject);

            if(++addedCount % 100000 == 0)
            {
                spdlog::info("Added {:L} objects in {}.", addedCount, Thx::toDurationString(start));
            }
        });

        if(testSuccessCount != 2)
            throw pqxx::data_exception("Tests did not pass.");

        spdlog::info("Processing Gaia data.");

        dbReader.getStars(false, [&] (const CelestialObject* celestialObject)
        {
            dbWriter.append(celestialObject);

            if(++addedCount % 100000 == 0)
            {
                spdlog::info("Added {:L} objects in {}.", addedCount, Thx::toDurationString(start));
            }
        });

        dbWriter.commit();

        spdlog::info("Added {:L} objects in {}.", addedCount, Thx::toDurationString(start));

        return 0;
    }
    catch (const std::exception &e)
    {
        spdlog::error(e.what());
        return 1;
    }
}

void showSyntax()
{
    std::cerr << "Syntax: ExportS3Sectors <SQlite DB Folder> <Postgres Connection> <Distance Ly> <Object Types (comma separated)> <Append Db>" << std::endl;
    std::cerr << "Export Simbad and Gaia data within the specified number of ligh years to an Sqlite database." << std::endl;
    std::cerr << "If <AppendDB> == 'append', then append to the 'stars_xxxly.db' file, otherwise delete it. If not append, then the Object Type is added to the file name. Defauls to false." << std::endl;
    std::cerr << "<ObjectType> defines the type of object to be exported." << std::endl;
    std::cerr << "    Star             - By itself, 'Star' includes NeutronStar, BrownDwarf, WhiteDwarf, LowMassStar" << std::endl;
    std::cerr << "    StarGroup" << std::endl;
    std::cerr << "    LowMassStar" << std::endl;
    std::cerr << "    BrownDwarf" << std::endl;
    std::cerr << "    Planet" << std::endl;
    std::cerr << "    Cluster" << std::endl;
    std::cerr << "    OpenCluster" << std::endl;
    std::cerr << "    GlobularCluster" << std::endl;
    std::cerr << "    XRay" << std::endl;
    std::cerr << "    NeutronStar" << std::endl;
    std::cerr << "    WhiteDwarf" << std::endl;
    std::cerr << "    Nebula" << std::endl;
    std::cerr << "    SuperNovae" << std::endl;
    std::cerr << "    PartOfGalaxy" << std::endl;
    std::cerr << "    Galaxy" << std::endl;
    std::cerr << "    ActiveGalaxyNucleus" << std::endl;
}
