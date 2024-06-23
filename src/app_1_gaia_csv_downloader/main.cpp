#include <iostream>
#include <filesystem>
#include <fstream>
#include <ConcurrentJob.h>
#include <curl/curl.h>
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
size_t write_data(const char *ptr, const size_t size, const size_t nmemb, void *userdata) {
    auto *stream = static_cast<ostream*>(userdata);
    const size_t count = size * nmemb;
    stream->write(ptr, count);
    return count;
}
int downloadCsv(const ThreadData* data)
{
    try
    {
        spdlog::info("Downloading file #{:L} {}.", data->fileNumber, data->csvFilePath.filename().string());

        ofstream fileOutputStream(data->csvFilePath, ios::binary);

        const auto curl = curl_easy_init();

        curl_easy_setopt(curl, CURLOPT_URL, data->url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fileOutputStream);

        if(const auto res = curl_easy_perform(curl); res != CURLE_OK)
            throw runtime_error(format("Failed to download file '{0}': {1}", data->csvFilePath.filename().string(), curl_easy_strerror(res)));

        fileOutputStream.flush();

        curl_easy_cleanup(curl);

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

        if (!exists(csvPath))
            create_directory(csvPath);

        auto concurrentJobs = vector<future<int>>();

        curl_global_init(CURL_GLOBAL_ALL);

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

    const auto curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &os);

    if(const auto res = curl_easy_perform(curl); res != CURLE_OK)
        throw runtime_error(format("Failed to get file list from '{0}': {1}", url, curl_easy_strerror(res)));

    os.flush();

    const auto downloadData = os.str();

    curl_easy_cleanup(curl);

    GumboOutput *output = gumbo_parse_with_options(&kGumboDefaultOptions, downloadData.data(), downloadData.length());

    find_definitions(fileLinks, fileSizes, output->root);

    if(fileLinks.size() != fileSizes.size())
        throw runtime_error("Gaia website structure changed! Aborting.");

    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

void showSyntax()
{
    cerr << "Syntax: GaiaCsvDownloader <Gaia CSV Location URL> <CSV Download Folder> [Max Concurrent Downloads = 50]" << endl;
    cerr << "Download Gaia CSV data to a folder." << endl;
}
