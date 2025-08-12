#include <chrono>
#include <iostream>
#include <fstream>

#include "ConcurrentJob.h"
#include "DbQuery.h"
#include "ExportToSql.h"
#include "LoggingSetup.h"
#include "SimbadRowProcessor.h"
#include "Thx.h"
#include "ThxWeb.h"


void showSyntax();
std::string getBaseQuery();

// http://simbad.u-strasbg.fr/simbad/sim-tap/sync?REQUEST=doQuery&LANG=ADQL&FORMAT=csv&QUERY=
// https://simbad.cds.unistra.fr/simbad/sim-tap/sync?REQUEST=doQuery&LANG=ADQL&FORMAT=csv&QUERY=
struct ThreadData
{
    ThreadData(std::string csvFilePath, std::string decompressPath, std::string fileName, std::chrono::time_point<std::chrono::steady_clock> startTime)
        : csvFilePath(std::move(csvFilePath)),
          decompressPath(std::move(decompressPath)),
          fileName(std::move(fileName)),
          startTime(startTime)
    {
    }
    const std::string csvFilePath;
    const std::string decompressPath;
    const std::string fileName;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

size_t write_data(const char *ptr, const size_t size, const size_t nmemb, void *userdata) {
    auto *stream = static_cast<std::ostream*>(userdata);
    const size_t count = size * nmemb;
    stream->write(ptr, count);
    return count;
}

int main(const int argc, const char *argv[])
{
#ifdef  __MINGW32__
    Thx::setCustomLocale();
#else
    std::locale::global(std::locale("en-AU"));
#endif

    try
    {
        if(argc != 4)
        {
            showSyntax();
            return -1;
        }

        LoggingSetup::setupDefaultLogging("logs/3_SimbadImportData.log");

        const auto simbadUrl = std::string(argv[1]);
        const auto *const postgresCnxString = argv[2];
        const auto minParallax = 1.0/(std::stoi(argv[3]) / 3.26156378) * 1000; // Parallax to ly

        const auto startTime = std::chrono::steady_clock::now();

        spdlog::info("Importing Simbad data.");

        auto logTime = std::chrono::steady_clock::now();
        auto stop = false;
        const ExportToSql dbWriter(postgresCnxString);
        const DbQuery dbQuery(postgresCnxString);

        auto nextRecordIndex = dbQuery.getLastRecordIndexAsync();
        auto recordsImportedCount = dbQuery.getRecordCountAsync();

        if(recordsImportedCount > 0)
            spdlog::info("Resuming the upload from record index {:L} with {:L} entries in the database.", nextRecordIndex == -1 ? 0 : nextRecordIndex, recordsImportedCount);

        nextRecordIndex = nextRecordIndex < 0 ? 0 : nextRecordIndex;

        curl_global_init(CURL_GLOBAL_ALL);

        const auto curl = curl_easy_init();

        const auto stringOutputStream = std::make_shared<std::stringstream>();
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, stringOutputStream.get());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300);
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 20 * 1024 * 1024);

        while(!stop)
        {
            stringOutputStream->str("");
            stringOutputStream->clear();

            const auto url = ThxWeb::encodedUrl(curl, fmt::format(fmt::runtime(simbadUrl + getBaseQuery()), nextRecordIndex));
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

            if(const auto res = curl_easy_perform(curl); res != CURLE_OK)
                throw std::runtime_error(std::format("Failed to download data: {0}", curl_easy_strerror(res)));

            auto csvParser = CsvParser(stringOutputStream);

            csvParser.appendColumn(SimbadRowProcessor::NameWolfColumnName);
            csvParser.appendColumn(SimbadRowProcessor::NameRossColumnName);
            csvParser.appendColumn(SimbadRowProcessor::NameSColumnName);
            csvParser.appendColumn(SimbadRowProcessor::NameSSColumnName);
            csvParser.appendColumn(SimbadRowProcessor::NameVSColumnName);
            csvParser.appendColumn(SimbadRowProcessor::NameHipColumnName);
            csvParser.appendColumn(SimbadRowProcessor::NameHdColumnName);
            csvParser.appendColumn(SimbadRowProcessor::NameGjColumnName);
            csvParser.appendColumn(SimbadRowProcessor::NameWiseColumnName);
            csvParser.appendColumn(SimbadRowProcessor::Name2MassColumnName);
            csvParser.appendColumn(SimbadRowProcessor::NameGaiaColumnName);
            csvParser.appendColumn(SimbadRowProcessor::NameTycColumnName);
            csvParser.appendColumn(SimbadRowProcessor::NameNgcColumnName);

            while(csvParser.readLine())
            {
                // if(!stop && GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState(0x58) < 0)
                // {
                //     spdlog::warn("**** Stop requested, wait for current processes to finish.");
                //     stop = true;
                // }

                nextRecordIndex = csvParser.getValueAsInt64(SimbadRowProcessor::IndexColumnName).value() + 1;

                if(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - logTime).count() >= 10)
                {
                    spdlog::info("Processed {:L}, added {:L} in {}.", nextRecordIndex, recordsImportedCount, Thx::toDurationString(startTime));

                    logTime = std::chrono::steady_clock::now();
                }

                if(!SimbadRowProcessor::processRow(minParallax, csvParser))
                    continue;

                recordsImportedCount++;

                dbWriter.append(csvParser);
            }

            if (csvParser.lineNumber() == 0)
                stop = true;
        }

        dbWriter.commit();

        spdlog::info("Finished {:L}, added {:L} records in {}.", nextRecordIndex, recordsImportedCount, Thx::toDurationString(startTime));

        curl_easy_cleanup(curl);
        curl_global_cleanup();

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
    std::cerr << "Syntax: ImportSimbadDb <Simbad DB URL> <Postgres Connection> <Distance Ly>" << std::endl;
    std::cerr << "Import Simbad data within the specified number of ligh years to a Postgres database." << std::endl;
}

std::string getBaseQuery()
{
    const std::string BaseQuery = "SELECT TOP 60000 "
    "oid as index,"
    "main_id,"
    "otypes as type,"
    "ids as name,"
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