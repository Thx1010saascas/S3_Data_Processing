#pragma once
#include <string>
#include <pqxx/pqxx>

using namespace std;

class DbQuery {
public:
    explicit DbQuery(const string& connectionString);
    ~DbQuery();

    long long getRecordCountAsync() const;
    long long getLastRecordIndexAsync() const;

private:
    pqxx::connection* _dbWriteConnection;
};
