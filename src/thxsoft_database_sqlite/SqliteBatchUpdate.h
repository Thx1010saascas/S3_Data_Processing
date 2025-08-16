#pragma once
#include <string>
#include <sqlite3.h>
#include <memory>

namespace thxsoft::database::sqlite
{
    class SqliteBatchUpdate {
    public:
        explicit SqliteBatchUpdate(sqlite3* db, const std::string& upsertTemplate, int maxSqlSize = 2 * 1024 * 1024);

        void commit() const;

        template<typename... T>
        void addRow(T ...args) const
        {
            std::string statement;
            auto processOne = [&] (const auto& input)
            {
                if(!statement.empty())
                    statement.append(",");

                statement.append(input);
            };
            (processOne(args), ...);

            appendStatement(statement);
        }

    private:
        void exec() const;
        void appendStatement(const std::string& statement) const;

        std::shared_ptr<std::string> _sql;
        sqlite3* _db = nullptr;
        const char* _upsertTemplate;
        const int  _maxSqlSize;
    };
}
