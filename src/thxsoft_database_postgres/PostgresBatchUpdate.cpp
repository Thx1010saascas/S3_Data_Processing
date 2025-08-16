#include "PostgresBatchUpdate.h"
#include <spdlog/spdlog.h>

using namespace pqxx;

namespace thxsoft::database::postgres
{
    PostgresBatchUpdate::PostgresBatchUpdate(const std::string& connectionString, const std::string& upsertTemplate, const int maxSqlSize)
        : _upsertTemplate(upsertTemplate.data()),
          _maxSqlSize(maxSqlSize)
    {
        _dbWriteConnection = new connection(connectionString);

        if (!_dbWriteConnection->is_open())
        {
            throw std::invalid_argument(std::format("Error opening Postgres database."));
        }

        _sql = std::make_shared<std::string>();
    }

    PostgresBatchUpdate::~PostgresBatchUpdate()
    {
        delete _dbWriteConnection;
        _dbWriteConnection = nullptr;
    }

    void PostgresBatchUpdate::appendStatement(const std::string& statement) const
    {
        if(!_sql->empty())
            _sql->append(",");

        _sql->append("(" + statement + ")");

        if(_sql->length() >= _maxSqlSize)
            exec();
    }

    void PostgresBatchUpdate::exec() const
    {
        if(_sql->empty())
            return;

        const auto statement = fmt::format(fmt::runtime(_upsertTemplate), *_sql);

        auto txn = transaction(*_dbWriteConnection);

        txn.exec(statement);

        txn.commit();

        _sql->clear();
    }

    void PostgresBatchUpdate::commit() const
    {
        exec();
    }
}