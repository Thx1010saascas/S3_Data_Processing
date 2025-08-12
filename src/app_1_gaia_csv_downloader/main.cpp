#include <iostream>
#include <filesystem>
#include <fstream>
#include <utility>
#include <ConcurrentJob.h>
#ifndef NOMINMAX
    #define NOMINMAX 
#endif
#define byte win_byte_override
#include <curl/curl.h>
#include "gumbo.h"
#include "LoggingSetup.h"
#include "Thx.h"

static bool _stop;
void getCsvDownloads(const std::string& url, std::vector<std::string>& fileLinks, std::vector<int>& fileSizes);
void showSyntax();

struct ThreadData
{
    ThreadData(std::filesystem::path  csvFilePath, const int fileNumber, std::string  url)
        :   csvFilePath(std::move(csvFilePath)),
            fileNumber(fileNumber),
            url(std::move(url))
    {
    }

    const std::filesystem::path csvFilePath;
    const int fileNumber;
    const std::string url;
};

void find_definitions(std::vector<std::string>& links, std::vector<int>& sizes, const GumboNode* node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
    {
        return;
    }

    const auto attr = gumbo_get_attribute(&node->v.element.attributes, "href");
    if (attr != nullptr && strstr(attr->value, "GaiaSource_") != nullptr)
    {
        const auto line = std::string(node->v.element.original_tag.data);
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
    auto *stream = static_cast<std::ostream*>(userdata);
    const size_t count = size * nmemb;
    stream->write(ptr, count);
    return count;
}
int downloadCsv(const ThreadData* data)
{
    try
    {
        spdlog::info("Downloading file #{:L} {}.", data->fileNumber, data->csvFilePath.filename().string());

        std::ofstream fileOutputStream(data->csvFilePath, std::ios::binary);

        const auto curl = curl_easy_init();

        curl_easy_setopt(curl, CURLOPT_URL, data->url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fileOutputStream);

        if(const auto res = curl_easy_perform(curl); res != CURLE_OK)
            throw std::runtime_error(std::format("Failed to download file '{0}': {1}", data->csvFilePath.filename().string(), curl_easy_strerror(res)));

        fileOutputStream.flush();

        curl_easy_cleanup(curl);

        delete data;
    }
    catch (std::exception& e)
    {
        spdlog::error(std::format("Error processing file #{:L}: {}", data->fileNumber, e.what()));
    }

    return 1;
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
        if(argc < 3 || argc > 4)
        {
            showSyntax();
            return -1;
        }

        LoggingSetup::setupDefaultLogging("logs/1_GaiaCsvDownloader.log");

        const auto url = std::filesystem::path(argv[1]);
        const auto csvPath = std::filesystem::path(argv[2]);
        const auto maxConcurrentDownloads = argc == 4 ? std::stol(argv[3]) : 50;

        if (!exists(csvPath))
            create_directory(csvPath);

        auto concurrentJobs = std::vector<std::future<int>>();

        curl_global_init(CURL_GLOBAL_ALL);

        std::vector<std::future<int>> concurrentJobs1;
        spdlog::info("Downloading Gaia CSV data.");

        auto fileNumber = 0;
        auto fileNames = std::vector<std::string>();
        auto fileSizes = std::vector<int>();
        getCsvDownloads(url.string(), fileNames, fileSizes);

        for(auto i=0; i<fileNames.size(); ++i)
        {
            ++fileNumber;

#if __has_include("Windows.h")
            if(!_stop && GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState(0x58) < 0)
            {
                spdlog::warn("**** Stop requested, wait for current processes to finish.");
                _stop = true;
            }
#endif
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

            auto data = new ThreadData(std::ref(csvFilePath), fileNumber, fileLink.string());

            concurrentJobs.push_back(async(std::launch::async, downloadCsv, data));

            ConcurrentJob::waitForAvailableThread(&concurrentJobs, maxConcurrentDownloads);
        }

        ConcurrentJob::waitForThreadsToFinish(&concurrentJobs);

        curl_global_cleanup();

        return 0;
    }
    catch (const std::exception &e)
    {
        spdlog::error(e.what());
        return 1;
    }
}

void getCsvDownloads(const std::string& url, std::vector<std::string>& fileLinks, std::vector<int>& fileSizes)
{
    std::ostringstream os;

    const auto curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &os);

    if(const auto res = curl_easy_perform(curl); res != CURLE_OK)
        throw std::runtime_error(std::format("Failed to get file list from '{0}': {1}", url, curl_easy_strerror(res)));

    os.flush();

    const auto downloadData = os.str();

    curl_easy_cleanup(curl);

    GumboOutput *output = gumbo_parse_with_options(&kGumboDefaultOptions, downloadData.data(), downloadData.length());

    find_definitions(fileLinks, fileSizes, output->root);

    if(fileLinks.size() != fileSizes.size())
        throw std::runtime_error("Gaia website structure changed! Aborting.");

    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

void showSyntax()
{
    std::cerr << "Syntax: GaiaCsvDownloader <Gaia CSV Location URL> <CSV Download Folder> [Max Concurrent Downloads = 50]" << std::endl;
    std::cerr << "Download Gaia CSV data to a folder." << std::endl;
}
