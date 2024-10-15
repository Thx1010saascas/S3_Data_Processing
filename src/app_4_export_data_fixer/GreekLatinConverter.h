#pragma once
#include <string>
#include <map>


namespace thxsoft::export_data_fixer_db
{
    class GreekLatinConverter {

    public:
        static std::string fixName(const std::string&);

    private:
        typedef std::map<std::string, std::string> LanguageData;
        static LanguageData _LanguageData;
    };
}