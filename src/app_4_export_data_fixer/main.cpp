#include "GreekLatinConverter.h"
#include <iostream>
#include "ExportDataFixerDb.h"
#include "LoggingSetup.h"
#include "Thx.h"

using namespace std;
using namespace thxsoft::export_data_fixer_db;

int main(const int argc, const char *argv[])
{
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
            // *, ** and V*
            const auto name6 = GreekLatinConverter::fixName(fr->name6);
            const auto name7 = GreekLatinConverter::fixName(fr->name7);
            const auto name8 = GreekLatinConverter::fixName(fr->name8);

            if(name6 != fr->name6 || name7 != fr->name7 || name8 != fr->name8)
            {
                fixCount++;

                db.append(fr->index, name6, name7, name8);
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
