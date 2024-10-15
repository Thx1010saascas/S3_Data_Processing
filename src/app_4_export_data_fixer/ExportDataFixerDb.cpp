#include "ExportDataFixerDb.h"
#include <spdlog/spdlog.h>
#include <pqxx/pqxx>
#include "Thx.h"
#include "DatabaseUtils.h"

using namespace pqxx;
using namespace thxsoft::database;

namespace thxsoft::export_data_fixer_db
{
    ExportDataFixerDb::ExportDataFixerDb(const std::string& connectionString)
    {
        _dbReadConnection = make_shared<connection>(connectionString);

        if (!_dbReadConnection->is_open())
            throw std::invalid_argument(std::format("Error opening database."));

        _batchAdder = make_shared<PostgresBatchUpdate>(connectionString, _upsertString);
    }

    void ExportDataFixerDb::startQuery(const std::function<void(const FixerRow *)>& func)
    {
        try {
            auto work = new transaction(*_dbReadConnection);

            FixerRow fr;

            work->for_stream(_selectString, [&]
                (const long long index, const char* new_name_s, const char* new_name_ss, const char* new_name_vs) {

                fr.index = index;
                fr.name_s = new_name_s == nullptr ? "" : new_name_s;
                fr.name_ss = new_name_ss == nullptr ? "" : new_name_ss;
                fr.name_vs = new_name_vs == nullptr ? "" : new_name_vs;

                func(&fr);
            });

            delete work;
        }
        catch (const std::exception &e)
        {
            SPDLOG_ERROR("startQuery error", e.what());
            throw;
        }
    }

    void ExportDataFixerDb::append(const long long index, const std::string& name_s, const std::string& name_ss, const std::string& name_vs) const
    {
        _batchAdder->addRow(
            DatabaseUtils::asDbString(index),
            DatabaseUtils::asDbString(name_s),
            DatabaseUtils::asDbString(name_ss),
            DatabaseUtils::asDbString(name_vs));
    }

    void ExportDataFixerDb::commit() const
    {
        _batchAdder->commit();
    }
}
