#include <fstream>
#include <iostream>
#include <Thx.h>
#include <CsvParser.h>
#include <ExportToSql.h>
#include <Compression.h>
#include "ConcurrentJob.h"
#include "ExportProgressManager.h"
#include "GaiaRowProcessor.h"
#include "LoggingSetup.h"

using namespace std;

void showSyntax();

static bool _stop;
static string _postgresCnxString;
static volatile long long _recordsProcessedCount;
static volatile long long _recordsAddedCount;
static double _minParallax;

struct ThreadData
{
    ThreadData(ExportProgressManager& exportProgressManager, const int filesProcessedCount, string csvFilePath, string decompressPath, string fileName, chrono::time_point<chrono::steady_clock> startTime)
        : exportProgressManager(exportProgressManager),
          filesProcessedCount(filesProcessedCount),
          csvFilePath(move(csvFilePath)),
          decompressPath(move(decompressPath)),
          fileName(move(fileName)),
          startTime(startTime)
    {
    }
    ExportProgressManager& exportProgressManager;
    const int filesProcessedCount;
    const string csvFilePath;
    const string decompressPath;
    const string fileName;
    chrono::time_point<chrono::steady_clock> startTime;
};

int processCsv(const ThreadData* data)
{
    try
    {
        spdlog::info("Processing file #{:L} {}.", data->filesProcessedCount, data->fileName);

        const auto dataStream = Compression::decompress(data->csvFilePath, data->decompressPath, true, 2000000000);

        auto csvParser = CsvParser(dataStream);

        // Not read from the CSV so we have to add them here.
        csvParser.appendColumn(GaiaRowProcessor::TeffSourceColumnName);
        csvParser.appendColumn(GaiaRowProcessor::LuminosityColumnName);
        csvParser.appendColumn(GaiaRowProcessor::RadiusColumnName);

        auto logTime = chrono::high_resolution_clock::now();

        const ExportToSql dbWriter(_postgresCnxString);

        while(csvParser.readLine())
        {
            if(!_stop && GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState(0x58) < 0)
            {
                spdlog::warn("**** Stop requested, wait for current processes to finish.");
                _stop = true;
            }

            _recordsProcessedCount++;
            if(chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now() - logTime).count() >= 10)
            {
                spdlog::info("Processed {:L}, added {:L} in {}.", _recordsProcessedCount, _recordsAddedCount, Thx::toDurationString(data->startTime));

                logTime = chrono::high_resolution_clock::now();
            }

            if(!GaiaRowProcessor::processRow(_minParallax, csvParser))
                continue;

            _recordsAddedCount++;

            dbWriter.append(csvParser);
        }

        dbWriter.commit();

        dataStream->close();
        
        data->exportProgressManager.add(data->fileName);
        data->exportProgressManager.writeState();

        const auto decompressFilePath = filesystem::path(data->decompressPath) / data->fileName;

        remove(decompressFilePath);

        delete data;
    }
    catch (exception& e)
    {
        spdlog::error(format("Error processing file: {}: {}", data->fileName, e.what()));
    }
    return 1;
}

int main(const int argc, const char *argv[])
{
    try
    {
        if(argc < 4 || argc > 5)
        {
            showSyntax();
            return -1;
        }

        LoggingSetup::SetupDefaultLogging("logs/2_GaiaImportCsvs.log");

        const int MaxThreadLimit = 15;
        const auto csvPath = argv[1];
        const auto decompressPath = filesystem::current_path() / "Decompress";
        _postgresCnxString = argv[2];
        _minParallax = 1.0/(stoi(argv[3]) / 3.26156378) * 1000; // Parallax to ly

        auto maxThreadCount = argc == 5 ? stoi(argv[4]) : 5;

        if(maxThreadCount > MaxThreadLimit)
        {
            spdlog::warn("The maximum thread limit is {}.", MaxThreadLimit);
            maxThreadCount = MaxThreadLimit;
        }

        remove_all(decompressPath);
        create_directory(decompressPath);

        const auto startTime = chrono::high_resolution_clock::now();

        ExportProgressManager exportProgressManager((filesystem::path(csvPath) / "ProcessingState.log").string());

        auto concurrentJobs = vector<future<int>>();

        spdlog::info("Importing Gaia CSV data.");

        auto filesProcessedCount = 0;

        for (const auto& path : filesystem::directory_iterator(csvPath))
        {
            if(_stop)
            {
                break;
            }

            if(path.is_directory())
                continue;

            const string csvFilePath = path.path().string();

            if(!csvFilePath.ends_with(".gz"))
                continue;

            filesProcessedCount++;

            const auto fileName = path.path().filename().string();

            if(exportProgressManager.isComplete(fileName))
            {
                spdlog::info("Skipping file #{:L} {}.", filesProcessedCount, fileName);
                continue;
            }

            auto data = new ThreadData(exportProgressManager, filesProcessedCount, csvFilePath, decompressPath.string(), fileName, startTime);

            concurrentJobs.push_back(async(launch::async, processCsv, data));

            ConcurrentJob::waitForAvailableThread(&concurrentJobs, maxThreadCount);
        }

        ConcurrentJob::waitForThreadsToFinish(&concurrentJobs);

        spdlog::info("Processed {:L} records in {}.", _recordsProcessedCount, Thx::toDurationString(startTime));

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
    cerr << "Syntax: ImportGaiaCsv <Gaia CSV Folder> <Postgres Connection> <Distance Ly> [MaxThreads = 5]" << endl;
    cerr << "Import Gaia data within the specified number of ligh years to a Postgres database." << endl;
}
