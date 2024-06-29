#include <iostream>
#include "DbReader.h"
#include "ExportToSql.h"
#include "LoggingSetup.h"
#include "Thx.h"

using namespace std;
using namespace thxsoft::database;
using namespace thxsoft::export_s3_sectors;

void showSyntax();

double myRound(double x)
{
    return floor( x + 0.5 );
    return x/(abs(x)) * (abs(x)+0.5);
}

int main(const int argc, const char *argv[])
{
    try
    {
        auto aaa11 = static_cast<int>(roundf(1.4));
        auto aaa21 = static_cast<int>(roundf(-1.4));
        auto aaa1 = static_cast<int>(roundf(1.5));
        auto aaa2 = static_cast<int>(roundf(-1.5));
        auto aaa12 = static_cast<int>(roundf(1.6));
        auto aaa23 = static_cast<int>(roundf(-1.6));

        auto wwaaa11 = static_cast<int>(myRound(1.4));
        auto waaa21 = static_cast<int>(myRound(-1.4));
        auto waaa1 = static_cast<int>(myRound(1.5));
        auto waaa2 = static_cast<int>(myRound(-1.5));
        auto waaa12 = static_cast<int>(myRound(1.6));
        auto waaa23 = static_cast<int>(myRound(-1.6));

        if(argc < 5 || argc > 6)
        {
            showSyntax();
            return -1;
        }

        LoggingSetup::SetupDefaultLogging("logs/5_ExportS3Database.log");

        auto sqliteDbPath = filesystem::path(argv[1]);
        const auto postgresCnxString = argv[2];
        const auto distanceLy = stoi(argv[3]);
        const auto appendMode = argc == 6 ? argv[5] == "append" : false;

        auto objectType = 0;

        const auto start = chrono::steady_clock::now();

        for(const auto& type : Thx::split(argv[4], ","))
        {
            try
            {
                objectType |= SimbadObjectTypesMap.at(type);
            }
            catch (exception&)
            {
                cerr << "Object type '" << type << "' was not found. Check your spelling and capitalisation.";
                showSyntax();
                return -1;
            }
        }

        const auto deleteExistingDb = !appendMode;

        sqliteDbPath /= format("S3_{}ly{}.db", distanceLy, appendMode ? "" : (string("_") + argv[4]));

        spdlog::info("Writing to '{}'", sqliteDbPath.string());

        const DbReader dbReader(postgresCnxString, distanceLy, objectType);

        auto addedCount = 0;

        const ExportToSql dbWriter(sqliteDbPath.string(), deleteExistingDb);

        spdlog::info("Processing Simbad data.");

        dbReader.getStars(true, [&] (const CelestialObject* celestialObject)
        {
            dbWriter.append(celestialObject);

            if(++addedCount % 100000 == 0)
            {
                spdlog::info("Added {:L} objects in {}.", addedCount, Thx::toDurationString(start));
            }
        });

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
    catch (const exception &e)
    {
        spdlog::error(e.what());
        return 1;
    }
}

void showSyntax()
{
    cerr << "Syntax: ExportS3Sectors <SQlite DB Folder> <Postgres Connection> <Distance Ly> <Object Types (comma separated)> <Append Db>" << endl;
    cerr << "Export Simbad and Gaia data within the specified number of ligh years to an Sqlite database." << endl;
    cerr << "If <AppendDB> == 'append', then append to the 'stars_xxxly.db' file, otherwise delete it. If not append, then the Object Type is added to the file name. Defauls to false." << endl;
    cerr << "<ObjectType> defines the type of object to be exported." << endl;
    cerr << "    Star             - By itself, 'Star' includes NeutronStar, BrownDwarf, WhiteDwarf, LowMassStar" << endl;
    cerr << "    StarGroup" << endl;
    cerr << "    LowMassStar" << endl;
    cerr << "    BrownDwarf" << endl;
    cerr << "    Planet" << endl;
    cerr << "    Cluster" << endl;
    cerr << "    OpenCluster" << endl;
    cerr << "    GlobularCluster" << endl;
    cerr << "    XRay" << endl;
    cerr << "    NeutronStar" << endl;
    cerr << "    WhiteDwarf" << endl;
    cerr << "    Nebula" << endl;
    cerr << "    SuperNovae" << endl;
    cerr << "    PartOfGalaxy" << endl;
    cerr << "    Galaxy" << endl;
    cerr << "    ActiveGalaxyNucleus" << endl;
}
