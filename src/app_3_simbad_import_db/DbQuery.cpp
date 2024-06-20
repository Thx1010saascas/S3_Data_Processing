#include "DbQuery.h"

DbQuery::DbQuery(const string& connectionString)
{
    _dbWriteConnection = new pqxx::connection(connectionString);

    if (!_dbWriteConnection->is_open())
    {
        throw invalid_argument(format("Error opening Postgres database."));
    }
}

DbQuery::~DbQuery()
{
    delete _dbWriteConnection;
    _dbWriteConnection = nullptr;
}

long long DbQuery::getRecordCountAsync() const
{
    auto txn = pqxx::transaction(*_dbWriteConnection);

    const auto row = txn.exec1("SELECT (CASE WHEN c.reltuples < 0 THEN NULL WHEN c.relpages = 0 THEN float8 '0' ELSE c.reltuples / c.relpages END * (pg_catalog.pg_relation_size(c.oid) / pg_catalog.current_setting('block_size')::int))::bigint FROM pg_catalog.pg_class c WHERE  c.oid = 'simbad'::regclass;");

    txn.commit();

    if(row[0].is_null())
        return 0;

    return row[0].as<long long>();
}

long long DbQuery::getLastRecordIndexAsync() const
{
    auto txn = pqxx::transaction(*_dbWriteConnection);

    const auto row = txn.exec1("SELECT MAX(index) FROM simbad");

    txn.commit();

    if(row[0].is_null())
        return -1;

    return row[0].as<long long>();
}
