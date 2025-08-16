#include "Compression.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "zlib.h"

std::string Compression::decompress(const std::string& inFilePath)
{
    auto inFile = std::ifstream(inFilePath, std::ios::binary | std::ios::in);
    const auto length = static_cast<long long>(std::filesystem::file_size(inFilePath));
    auto buffer = std::vector<char>(length);
    inFile.read(buffer.data(), length);

    const auto output =  decompress_gzip(buffer.data(), length);

    return output;
}

std::shared_ptr<std::ifstream> Compression::decompressToStream(const std::string& inFilePath, const std::string& outFilePath)
{
    const auto inPath = std::filesystem::path(inFilePath);
    const auto outPath = std::filesystem::path(outFilePath);
    const auto fullOutputFilePath = outPath / inPath.filename();

    std::ifstream file(inFilePath, std::ios::binary);

    const auto data = decompress(inFilePath);

    auto outFile = std::ofstream(fullOutputFilePath, std::ios::out | std::ios::binary);

    outFile.write(data.c_str(), static_cast<long long>(data.length()));

    file.close();
    outFile.flush();
    outFile.close();

    auto inFile = std::make_shared<std::ifstream>(std::ifstream(fullOutputFilePath, std::ios::in | std::ios::binary));

    return inFile;
}

#define MOD_GZIP_ZLIB_WINDOWSIZE 15

std::string Compression::decompress_gzip(const char* buffer, const long long length)
{
    z_stream zs = {};                        // z_stream is zlib's control structure

    if (inflateInit2(&zs, MOD_GZIP_ZLIB_WINDOWSIZE + 16) != Z_OK)
        throw(std::runtime_error("inflateInit failed while decompressing."));

    zs.next_in = (Bytef*)buffer;
    zs.avail_in = length;

    int ret;
    char outbuffer[32768];
    std::string outstring;

    // get the decompressed bytes blockwise using repeated calls to inflate
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out) {
            outstring.append(outbuffer,
                             zs.total_out - outstring.size());
        }

    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
        std::ostringstream oss;
        oss << "Exception during zlib decompression: (" << ret << ") "
            << zs.msg;
        throw(std::runtime_error(oss.str()));
    }

    return outstring;
}

