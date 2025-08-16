#include "ThxWeb.h"

std::string ThxWeb::encodedUrl(CURL *curl, std::string url)
{
    auto pos=url.find('?');

    if(pos == std::string::npos)
        return url;

    auto ret=url.substr(0,pos+1);
    auto tmpstr = std::string();
    char *escapedstr = nullptr;
    url = url.substr(pos+1,std::string::npos);
    auto type = "=";
    do
    {
        pos = url.find('=');

        const auto pos2 = url.find('&');

        if(pos == std::string::npos && pos2 == std::string::npos)
            break;

        if(pos < pos2){
            type = "=";
        } else {
            type = "&";
            pos = pos2;
        }

        tmpstr = url.substr(0,pos);
        url = url.substr(pos+1,std::string::npos);

        try
        {
            escapedstr = curl_easy_escape(curl,tmpstr.c_str(),tmpstr.length());
        }
        catch (std::exception&)
        {
            curl_free(escapedstr);
            throw;
        }

        ret.append(escapedstr);
        curl_free(escapedstr);
        ret.append(type);
    } while (true);

    try
    {
        escapedstr = curl_easy_escape(curl,url.c_str(), url.length());
    }
    catch (std::exception&)
    {
        curl_free(escapedstr);
        throw;
    }

    ret.append(escapedstr);
    curl_free(escapedstr);

    return ret;
}
