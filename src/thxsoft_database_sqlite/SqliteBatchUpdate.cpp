#include "SqliteBatchUpdate.h"
#include <format>
#include <fmt/core.h>

namespace thxsoft::database::sqlite
{
    SqliteBatchUpdate::SqliteBatchUpdate(sqlite3* db, const std::string& upsertTemplate, const int maxSqlSize)
        :   _db(db),
            _upsertTemplate(upsertTemplate.data()),
            _maxSqlSize(maxSqlSize)
    {
        _sql = std::make_shared<std::string>();
    }

    void SqliteBatchUpdate::appendStatement(const std::string& statement) const
    {
        if(!_sql->empty())
            _sql->append(",");

        _sql->append("(" + statement + ")");

        if(_sql->length() >= _maxSqlSize)
            exec();
    }

    void SqliteBatchUpdate::exec() const
    {
        if(!_sql->empty())
        {
            const auto statement = fmt::format(fmt::runtime(_upsertTemplate), *_sql);

            char *zErrMsg;
            sqlite3_exec(_db, statement.c_str(), nullptr, nullptr, &zErrMsg);

            if(zErrMsg != nullptr)
            {
                auto msg = std::string(zErrMsg);
                sqlite3_free(zErrMsg);

                throw std::invalid_argument(std::format("Upsert error {}: {}", msg, statement));
            }

            _sql->clear();
        }
    }

    void SqliteBatchUpdate::commit() const
    {
        exec();
    }
}
