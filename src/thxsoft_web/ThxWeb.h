#pragma once
#include <string>
#include <curl/curl.h>

struct ThxWeb
{
    static std::string encodedUrl(CURL *curl, std::string url);
};
