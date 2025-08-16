#pragma once
#include <memory>
#include <string>

struct Compression {

    static std::string decompress(const std::string& inFilePath, size_t maxBytes = 1000000000);
    static std::shared_ptr<std::ifstream> decompress(const std::string& inFilePath, const std::string& outFilePath, bool asText = true, size_t maxBytes = 1000000000);


private:
    static std::string decompress_gzip(const char* buffer, long long length);

};
