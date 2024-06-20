#include <iostream>
#include <fstream>

#include "ConcurrentJob.h"
#include "DbQuery.h"
#include "ExportToSql.h"
#include "LoggingSetup.h"
#include "SimbadRowProcessor.h"
#include "Thx.h"
#include "ThxWeb.h"

using namespace std;

void showSyntax();
string getBaseQuery();

// http://simbad.u-strasbg.fr/simbad/sim-tap/sync?REQUEST=doQuery&LANG=ADQL&FORMAT=csv&QUERY=
// https://simbad.cds.unistra.fr/simbad/sim-tap/sync?REQUEST=doQuery&LANG=ADQL&FORMAT=csv&QUERY=
struct ThreadData
{
    ThreadData(string csvFilePath, string decompressPath, string fileName, chrono::time_point<chrono::steady_clock> startTime)
        : csvFilePath(move(csvFilePath)),
          decompressPath(move(decompressPath)),
          fileName(move(fileName)),
          startTime(startTime)
    {
    }
    const string csvFilePath;
    const string decompressPath;
    const string fileName;
    chrono::time_point<chrono::steady_clock> startTime;
};

int main(const int argc, const char *argv[])
{
    try
    {
        if(argc != 4)
        {
            showSyntax();
            return -1;
        }

        LoggingSetup::SetupDefaultLogging("logs/3_SimbadImportData.log");

        const auto simbadUrl = string(argv[1]);
        const auto postgresCnxString = argv[2];
        const auto minParallax = 1.0/(stoi(argv[3]) / 3.26156378) * 1000; // Parallax to ly

        const auto startTime = chrono::high_resolution_clock::now();

        spdlog::info("Importing Simbad data.");

        auto logTime = chrono::high_resolution_clock::now();
        auto stop = false;
        const ExportToSql dbWriter(postgresCnxString);
        const DbQuery dbQuery(postgresCnxString);

        auto nextRecordIndex = dbQuery.getLastRecordIndexAsync();
        auto recordsImportedCount = dbQuery.getRecordCountAsync();

        if(recordsImportedCount > 0)
            spdlog::info("Resuming the upload from record index {:L} with {:L} entries in the database.", nextRecordIndex == -1 ? 0 : nextRecordIndex, recordsImportedCount);

        nextRecordIndex = nextRecordIndex < 0 ? 0 : nextRecordIndex;

        curlpp::Easy request;
        const auto result = make_shared<stringstream>();
        const auto handle = request.getCurlHandle().getHandle();
        request.setOpt(cURLpp::Options::WriteStream(result.get()));
        request.setOpt(cURLpp::Options::Timeout(300));
        request.setOpt(cURLpp::Options::BufferSize(20 * 1024 * 1024));

        while(!stop)
        {
            result->str("");
            result->clear();

            request.setOpt(curlpp::options::Url(
                ThxWeb::encodedUrl(handle,
                    vformat(simbadUrl + getBaseQuery(), make_format_args(nextRecordIndex)))));

            request.perform();

            auto csvParser = CsvParser(result);

            while(csvParser.readLine())
            {
                if(!stop && GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState(0x58) < 0)
                {
                    spdlog::warn("**** Stop requested, wait for current processes to finish.");
                    stop = true;
                }

                nextRecordIndex = csvParser.getValueAsInt64(SimbadRowProcessor::IndexColumnName).value() + 1;

                if(chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - logTime).count() >= 10)
                {
                    spdlog::info("Processed {:L}, added {:L} in {}.", nextRecordIndex, recordsImportedCount, Thx::toDurationString(startTime));

                    logTime = chrono::high_resolution_clock::now();
                }

                if(!SimbadRowProcessor::processRow(minParallax, csvParser))
                    continue;

                recordsImportedCount++;

                dbWriter.append(csvParser);

                if (csvParser.lineNumber() == 0)
                    break;
            }
        }

        dbWriter.commit();

        curl_global_cleanup();

        spdlog::info("Finished {:L}, added {:L} records in {}.", nextRecordIndex, recordsImportedCount, Thx::toDurationString(startTime));

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
    cerr << "Syntax: ImportSimbadDb <Simbad DB URL> <Postgres Connection> <Distance Ly>" << endl;
    cerr << "Import Simbad data within the specified number of ligh years to a Postgres database." << endl;
}

string getBaseQuery()
{
    const string BaseQuery = "SELECT TOP 60000 "
    "oid as index,"
    "main_id,"
    "otypes as type,"
    "ids as name1,"
    "'' as name2,"
    "'' as name3,"
    "'' as name4,"
    "0 as g_source_id,"
    "ra,"
    "dec,"
    "1 as glat,"
    "1 as glon,"
    "plx_value as parallax,"
    "teff,"
    "0 as radius,"
    "0 as luminosity,"
    "fe_h,"
    "B as b,"
    "V as v,"
    "G as g_mag,"
    "sptype as spectral_type,"
    "0 as object_type "
    "FROM basic "
        "INNER JOIN ids ON ids.oidref = oid "
        "INNER JOIN alltypes ON alltypes.oidref = oid "
        "LEFT OUTER JOIN mesSpT mesSpT ON mesSpT.oidref = oid AND mesSpT.mespos = 1 "
        "LEFT OUTER JOIN mesFe_h ON mesFe_h.oidref = oid AND mesFe_h.mespos = 1 "
        "LEFT OUTER JOIN allfluxes on allfluxes.oidref = oid "
    "WHERE  oid >= {} AND plx_qual != 'E' AND coo_qual != 'E' AND "
            "ra IS NOT NULL AND plx_value >= 0.3261563711 ORDER BY oid";

    return BaseQuery;
}