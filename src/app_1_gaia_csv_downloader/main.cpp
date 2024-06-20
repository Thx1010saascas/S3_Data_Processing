#include <iostream>
#include <filesystem>
#include <fstream>

#include <ConcurrentJob.h>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <utility>
#include "gumbo.h"
#include "LoggingSetup.h"
#include "Thx.h"

using namespace std;

static bool _stop;
void getCsvDownloads(const string& url, vector<string>& fileLinks, vector<int>& fileSizes);
void showSyntax();

struct ThreadData
{
    ThreadData(const filesystem::path& csvFilePath, const int fileNumber, string  url)
        :   csvFilePath(csvFilePath),
            fileNumber(fileNumber),
            url(move(url))
    {
    }

    const filesystem::path csvFilePath;
    const int fileNumber;
    const string url;
};

void find_definitions(vector<string>& links, vector<int>& sizes, const GumboNode* node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    const auto attr = gumbo_get_attribute(&node->v.element.attributes, "href");
    if (attr != nullptr && strstr(attr->value, "GaiaSource_") != nullptr)
    {
        const auto line = string(node->v.element.original_tag.data);
        const auto newLineIndex = line.find('\n');
        const auto fileSize = Thx::trim(line.substr(newLineIndex - 20, 20));
        sizes.push_back(stoi(fileSize));

        links.emplace_back(attr->value);
    }

    const GumboVector* children = &node->v.element.children;
    for (int i = 0; i < children->length; ++i)
    {
        find_definitions(links, sizes, static_cast<GumboNode *>(children->data[i]));
    }
}
int downloadCsv(const ThreadData* data)
{
    try
    {
        spdlog::info("Downloading file #{:L} {}.", data->fileNumber, data->csvFilePath.filename().string());

        ofstream fileOutputStream(data->csvFilePath, ios::binary);

        curlpp::Easy request;

        request.setOpt(curlpp::options::Url(data->url));
        request.setOpt(curlpp::options::WriteStream(&fileOutputStream));

        request.perform();

        fileOutputStream.flush();

        delete data;
    }
    catch (exception& e)
    {
        spdlog::error(format("Error processing file #{:L}: {}", data->fileNumber, e.what()));
    }

    return 1;
}

int main(const int argc, const char *argv[])
{
    try
    {
        if(argc < 3 || argc > 4)
        {
            showSyntax();
            return -1;
        }

        LoggingSetup::SetupDefaultLogging("logs/1_GaiaCsvDownloader.log");

        const auto url = filesystem::path(argv[1]);
        const auto csvPath = filesystem::path(argv[2]);
        const auto maxConcurrentDownloads = argc == 4 ? stol(argv[3]) : 50;

        auto concurrentJobs = vector<future<int>>();

        vector<future<int>> concurrentJobs1;
        spdlog::info("Downloading Gaia CSV data.");

        auto fileNumber = 0;
        auto fileNames = vector<string>();
        auto fileSizes = vector<int>();
        getCsvDownloads(url.string(), fileNames, fileSizes);

        for(auto i=0; i<fileNames.size(); ++i)
        {
            ++fileNumber;

            if(!_stop && GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState(0x58) < 0)
            {
                spdlog::warn("**** Stop requested, wait for current processes to finish.");
                _stop = true;
            }

            const auto fileName = fileNames[i];
            const auto fileSize = fileSizes[i];
            const auto csvFilePath = (csvPath / fileName);
            const auto fileLink = (url / fileName);

            if(exists(csvFilePath))
            {
                if (file_size(csvFilePath) == fileSize)
                {
                    spdlog::info("Skipping #{:L} - {}.", fileNumber, fileName);
                    continue;
                }

                spdlog::warn("Size mismatch #{:L} - Re-downloading {}.", fileNumber, fileName);
                remove(csvFilePath);
            }

            auto data = new ThreadData(ref(csvFilePath), fileNumber, fileLink.string());

            concurrentJobs.push_back(async(launch::async, downloadCsv, data));

            ConcurrentJob::waitForAvailableThread(&concurrentJobs, maxConcurrentDownloads);
        }

        ConcurrentJob::waitForThreadsToFinish(&concurrentJobs);

        curl_global_cleanup();

        return 0;
    }
    catch (const exception &e)
    {
        spdlog::error(e.what());
        return 1;
    }
}

void getCsvDownloads(const string& url, vector<string>& fileLinks, vector<int>& fileSizes)
{
    ostringstream os;
    os << curlpp::options::Url(url);
    const auto streamData = os.str();

    GumboOutput *output = gumbo_parse_with_options(&kGumboDefaultOptions, streamData.data(), streamData.length());

    find_definitions(fileLinks, fileSizes, output->root);

    if(fileLinks.size() != fileSizes.size())
        throw exception("Gaia website structure changed! Aborting.");

    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

void showSyntax()
{
    cerr << "Syntax: GaiaCsvDownloader <Gaia CSV Location URL> <CSV Download Folder> [Max Concurrent Downloads = 50]" << endl;
    cerr << "Download Gaia CSV data to a folder." << endl;
}
