#pragma once
#include <string>
#include <pqxx/pqxx>


class DbQuery {
public:
    explicit DbQuery(const std::string& connectionString);
    ~DbQuery();

    long long getRecordCountAsync() const;
    long long getLastRecordIndexAsync() const;

private:
    pqxx::connection* _dbWriteConnection;
};
