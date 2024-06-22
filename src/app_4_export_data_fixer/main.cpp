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

        const auto start = chrono::high_resolution_clock::now();
        const char* postgresCnxString = argv[1];
        ExportDataFixerDb db(postgresCnxString);

        auto fixCount = 0;
        auto totalCount = 0;


        db.startQuery([&] (const FixerRow* fr)
        {
            const auto name1 = GreekLatinConverter::fixName(fr->name1);
            const auto name2 = GreekLatinConverter::fixName(fr->name2);
            const auto name3 = GreekLatinConverter::fixName(fr->name3);

            if(name1 != fr->name1 || name2 != fr->name2 || name3 != fr->name3)
            {
                fixCount++;

                db.append(fr->index, name1, name2, name3);
            }

            if(++totalCount % 10000 == 0)
            {
                spdlog::info("Exported {:L} names and fixed {:L} entries in {}.", totalCount, fixCount, Thx::toDurationString(start));
            }
        });

        db.commit();

        spdlog::info("Exported {:L} names and fixed {:L} entries in {}.", totalCount, fixCount, Thx::toDurationString(start));
    }
    catch (const exception &e)
    {
        spdlog::error(e.what());
        return 1;
    }
}
