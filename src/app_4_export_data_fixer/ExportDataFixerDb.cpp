#include "ExportDataFixerDb.h"
#include <spdlog/spdlog.h>
#include <pqxx/pqxx>
#include "Thx.h"
#include "DatabaseUtils.h"

using namespace pqxx;
using namespace thxsoft::database;

namespace thxsoft::export_data_fixer_db
{
    ExportDataFixerDb::ExportDataFixerDb(const string& connectionString)
    {
        _dbReadConnection = make_shared<connection>(connectionString);

        if (!_dbReadConnection->is_open())
            throw invalid_argument(std::format("Error opening database."));

        _batchAdder = make_shared<PostgresBatchUpdate>(connectionString, _upsertString);
    }

    void ExportDataFixerDb::startQuery(const function<void(const FixerRow *)>& func)
    {
        try {
            auto work = new transaction(*_dbReadConnection);

            FixerRow fr;

            work->for_stream(_selectString, [&]
                (const long long index, const char* name1, const char* name2, const char* name3, const char* name4) {

                fr.index = index;
                fr.name1 = name1 == nullptr ? "" : name1;
                fr.name2 = name2 == nullptr ? "" : name2;
                fr.name3 = name3 == nullptr ? "" : name3;
                fr.name4 = name4 == nullptr ? "" : name4;

                func(&fr);
            });

            delete work;
        }
        catch (const exception &e)
        {
            SPDLOG_ERROR("startQuery error", e.what());
            throw;
        }
    }

    void ExportDataFixerDb::append(const long long index, const string& name1, const string& name2, const string& name3) const
    {
        _batchAdder->addRow(
            DatabaseUtils::asDbString(index),
            DatabaseUtils::asDbString(name1),
            DatabaseUtils::asDbString(name2),
            DatabaseUtils::asDbString(name3));
    }

    void ExportDataFixerDb::commit() const
    {
        _batchAdder->commit();
    }
}
