#include <atomic>
#include <fstream>
#include <iostream>
#include "Thx.h"
#include "CsvParser.h"
#include "ExportToSql.h"
#include "ConcurrentJob.h"
#include "ExportProgressManager.h"
#include "GaiaRowProcessor.h"
#include "LoggingSetup.h"
#include <Compression.h>

void showSyntax();

static bool g_stopProcessing;
static std::string g_postgresCnxString;
static std::atomic<long long> g_recordsProcessedCount;
static std::atomic<long long>  g_recordsAddedCount;
static double g_minParallax;

void stop()
{
    spdlog::warn("**** Stop requested, wait for current processes to finish.");
    g_stopProcessing = true;
}

struct ThreadData
{
    ThreadData(ExportProgressManager& exportProgressManager, const int filesProcessedCount, std::string csvFilePath, std::string decompressPath, std::string fileName, std::chrono::time_point<std::chrono::steady_clock> startTime)
        : exportProgressManager(exportProgressManager),
          filesProcessedCount(filesProcessedCount),
          csvFilePath(std::move(csvFilePath)),
          decompressPath(std::move(decompressPath)),
          fileName(std::move(fileName)),
          startTime(startTime)
    {
    }
    ExportProgressManager& exportProgressManager;
    const int filesProcessedCount;
    const std::string csvFilePath;
    const std::string decompressPath;
    const std::string fileName;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

int processCsv(const ThreadData* data)
{
    try
    {
        spdlog::info("Processing file #{:L} {}.", data->filesProcessedCount, data->fileName);

        const auto dataStream = Compression::decompressToStream(data->csvFilePath, data->decompressPath);

        auto csvParser = CsvParser(dataStream);

        // Not read from the CSV so we have to add them here.
        csvParser.appendColumn(GaiaRowProcessor::TeffSourceColumnName);
        csvParser.appendColumn(GaiaRowProcessor::LuminosityColumnName);
        csvParser.appendColumn(GaiaRowProcessor::RadiusColumnName);

        auto logTime = std::chrono::steady_clock::now();

        const ExportToSql dbWriter(g_postgresCnxString);

        while(csvParser.readLine())
        {
            g_recordsProcessedCount += 1;
            if(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - logTime).count() >= 10)
            {
                spdlog::info("Processed {:L}, added {:L} in {}.", g_recordsProcessedCount.load(), g_recordsAddedCount.load(), Thx::toDurationString(data->startTime));

                logTime = std::chrono::steady_clock::now();
            }

            if(!GaiaRowProcessor::processRow(g_minParallax, csvParser))
                continue;

            g_recordsAddedCount += 1;

            dbWriter.append(csvParser);
        }

        dbWriter.commit();

        dataStream->close();

        data->exportProgressManager.add(data->fileName);
        data->exportProgressManager.writeState();

        const auto decompressFilePath = std::filesystem::path(data->decompressPath) / data->fileName;

        remove(decompressFilePath);

        delete data;
    }
    catch (std::exception& e)
    {
        spdlog::error(std::format("Error processing file: {}: {}", data->fileName, e.what()));
    }
    return 1;
}

int main(const int argc, const char *argv[])
{
#ifdef  __MINGW32__
    Thx::setCustomLocale();
#else
    std::locale::global(std::locale("en_AU"));
#endif

    try
    {
        if(argc < 4 || argc > 5)
        {
            showSyntax();
            return -1;
        }

        LoggingSetup::setupDefaultLogging("logs/2_GaiaImportCsvs.log");

#if !__has_include("Windows.h")
        signal(SIGINT, [] (int signum)
        {
            stop();
        });
#endif

        constexpr int MaxThreadLimit = 15;
        const auto csvFolder = argv[1];
        const auto decompressPath = std::filesystem::temp_directory_path() / "Decompress";
        g_postgresCnxString = argv[2];
        g_minParallax = 1.0/(std::stoi(argv[3]) / 3.26156378) * 1000; // Parallax to ly

        auto maxThreadCount = argc == 5 ? std::stoi(argv[4]) : 5;

        if(maxThreadCount > MaxThreadLimit)
        {
            spdlog::warn("The maximum thread limit is {}.", MaxThreadLimit);
            maxThreadCount = MaxThreadLimit;
        }

        remove_all(decompressPath);
        create_directory(decompressPath);

        const auto startTime = std::chrono::steady_clock::now();

        ExportProgressManager exportProgressManager((std::filesystem::path(csvFolder) / "ProcessingState.log").string());

        auto concurrentJobs = std::vector<std::future<int>>();

        spdlog::info("Importing Gaia CSV data.");

        auto filesProcessedCount = 0;

        for (const auto& csvFile : std::filesystem::directory_iterator(csvFolder))
        {
#if __has_include("Windows.h")
            if(!g_stopProcessing && GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState(0x58) < 0)
                stop();
#endif

            if(g_stopProcessing)
                break;

            if(csvFile.is_directory())
                continue;

            const auto& csvFilePath = csvFile.path();

            if(csvFilePath.extension().string() != ".gz" || csvFilePath.filename().string().starts_with("."))
                continue;

            filesProcessedCount++;

            const auto fileName = csvFilePath.filename().string();

            if(exportProgressManager.isComplete(fileName))
            {
                spdlog::info("Skipping file #{:L} {}.", filesProcessedCount, fileName);
                continue;
            }

            auto data = new ThreadData(exportProgressManager, filesProcessedCount, csvFilePath, decompressPath.string(), fileName, startTime);

            concurrentJobs.push_back(async(std::launch::async, processCsv, data));

            ConcurrentJob::waitForAvailableThread(&concurrentJobs, maxThreadCount);
        }

        ConcurrentJob::waitForThreadsToFinish(&concurrentJobs);

        spdlog::info("Processed {:L} records in {}.", g_recordsProcessedCount.load(), Thx::toDurationString(startTime));

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
    std::cerr << "Syntax: ImportGaiaCsv <Gaia CSV Folder> <Postgres Connection> <Distance Ly> [MaxThreads = 5]" << std::endl;
    std::cerr << "Import Gaia data within the specified number of ligh years to a Postgres database." << std::endl;
}
