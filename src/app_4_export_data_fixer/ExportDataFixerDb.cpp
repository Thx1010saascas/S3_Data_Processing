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
                (const long long index, const char* name6, const char* name7, const char* name8) {

                fr.index = index;
                fr.name6 = name6 == nullptr ? "" : name6;
                fr.name7 = name7 == nullptr ? "" : name7;
                fr.name8 = name8 == nullptr ? "" : name8;

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

    void ExportDataFixerDb::append(const long long index, const string& name6, const string& name7, const string& name8) const
    {
        _batchAdder->addRow(
            DatabaseUtils::asDbString(index),
            DatabaseUtils::asDbString(name6),
            DatabaseUtils::asDbString(name7),
            DatabaseUtils::asDbString(name8));
    }

    void ExportDataFixerDb::commit() const
    {
        _batchAdder->commit();
    }
}
