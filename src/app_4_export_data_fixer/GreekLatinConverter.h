#pragma once
#include <string>
#include <map>

using namespace std;

namespace thxsoft::export_data_fixer_db
{
    class GreekLatinConverter {

    public:
        static string fixName(const string&);

    private:
        typedef map<string, string> LanguageData;
        static LanguageData _LanguageData;
    };
}