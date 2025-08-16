#pragma once
#include <string>
#include <pqxx/pqxx>

namespace thxsoft::database::postgres
{
    class PostgresBatchUpdate {
    public:
        explicit PostgresBatchUpdate(const std::string& connectionString, const std::string& upsertTemplate, int maxSqlSize = 5 * 1024 * 1024);
        ~PostgresBatchUpdate();
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

        pqxx::connection* _dbWriteConnection;
        std::shared_ptr<std::string> _sql;
        const char * _upsertTemplate;
        const int  _maxSqlSize;
    };
}
