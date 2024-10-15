#pragma once
#include "SqliteBatchUpdate.h"
#include "PostgresBatchUpdate.h"
#include "CelestialObject.h"

using namespace thxsoft::database::sqlite;
using namespace thxsoft::database::postgres;

namespace thxsoft::export_s3_sectors
{
    class ExportToSql {
    public:
        explicit ExportToSql(const std::string&, bool deleteExistingDb);
        ~ExportToSql();

        void append(const CelestialObject* celestialObject) const;
        void commit() const;

    private:
        const std::string ExportTableName = "Stars";
        sqlite3* _db;
        std::shared_ptr<SqliteBatchUpdate> _batchAdder;
        std::shared_ptr<pqxx::connection> _dbWriteConnection;
        const std::string _upsertString = "INSERT INTO " + ExportTableName + " (Sector,Name1,Name2,Name3,Teff,SpectralType,IsBinary,X,Y,Z) VALUES {};";
        void createTable(const std::string& tableName) const;
        void finaliseTableCreation() const;
        void vacuumTable() const;
    };
}
