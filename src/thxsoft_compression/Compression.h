#pragma once
#include <memory>
#include <string>

struct Compression {

    static std::string decompress(const std::string& inFilePath);
    static std::shared_ptr<std::ifstream> decompressToStream(const std::string& inFilePath, const std::string& outFilePath);


private:
    static std::string decompress_gzip(const char* buffer, long long length);

};
