#include "GreekLatinConverter.h"
#include <iostream>
#include "ExportDataFixerDb.h"
#include "LoggingSetup.h"
#include "Thx.h"

using namespace std;
using namespace thxsoft::export_data_fixer_db;

int main(const int argc, const char *argv[])
{
#ifdef  __MINGW32__
    Thx::setCustomLocale();
#else
    locale::global(locale("en-AU"));
#endif

    try
    {
        if(argc != 2)
        {
            cerr << "Syntax: app_4_export_data_fixer <Postgres Connection>" << endl;
            cerr << "Populates the 'export_overrides' table with Latin and Greek abbreviations converted to full names." << endl;
            cerr << "After creation, rows can be manually updated to force difference names or spectral type." << endl;
            return -1;
        }

        LoggingSetup::SetupDefaultLogging("Logs/4_ExportDataFixer.log");

        const auto start = chrono::steady_clock::now();
        const char* postgresCnxString = argv[1];
        ExportDataFixerDb db(postgresCnxString);

        auto fixCount = 0;
        auto totalCount = 0;


        db.startQuery([&] (const FixerRow* fr)
        {
            const auto name_s = GreekLatinConverter::fixName(fr->name_s);
            const auto name_ss = GreekLatinConverter::fixName(fr->name_ss);
            const auto name_vs = GreekLatinConverter::fixName(fr->name_vs);

            if(name_s != fr->name_s || name_ss != fr->name_ss || name_vs != fr->name_vs)
            {
                fixCount++;

                db.append(fr->index, name_s, name_ss, name_vs);
            }

            if(++totalCount % 10000 == 0)
            {
                spdlog::info("Processed {:L} candidates and fixed {:L} entries in {}.", totalCount, fixCount, Thx::toDurationString(start));
            }
        });

        db.commit();

        spdlog::info("Processed {:L} candidates and fixed {:L} entries in {}.", totalCount, fixCount, Thx::toDurationString(start));
    }
    catch (const exception &e)
    {
        spdlog::error(e.what());
        return 1;
    }
}
