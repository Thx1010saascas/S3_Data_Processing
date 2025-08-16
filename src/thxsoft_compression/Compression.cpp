#include "Compression.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "zlib.h"

std::string Compression::decompress(const std::string& inFilePath, const size_t maxBytes)
{
    auto inFile = std::ifstream(inFilePath, std::ios::binary | std::ios::in);

    inFile.ignore(std::numeric_limits<std::streamsize>::max() );
    const auto length = inFile.gcount();
    inFile.clear();
    inFile.seekg(0, std::ios_base::beg);

    auto buffer = std::vector<char>(length);
    inFile.read(buffer.data(), length);

    const auto output =  decompress_gzip(buffer.data(), length);

    return output;
}

std::shared_ptr<std::ifstream> Compression::decompress(const std::string& inFilePath, const std::string& outFilePath, const bool asText, const size_t maxBytes)
{
    const auto inPath = std::filesystem::path(inFilePath);
    const auto outPath = std::filesystem::path(outFilePath);
    const auto fullOutputFilePath = outPath / inPath.filename();

    std::ifstream file(inFilePath, std::ios::binary);

    const auto data = decompress(inFilePath, maxBytes);

    auto mode = std::ios::out;

    if(!asText)
        mode |= std::ios::binary;

    auto outFile = std::ofstream(fullOutputFilePath, mode);

    outFile.write(data.c_str(), data.length());

    file.close();
    outFile.flush();
    outFile.close();

    auto inFile = std::make_shared<std::ifstream>(std::ifstream(fullOutputFilePath, mode));

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

