#include "ExportToSql.h"
#include <spdlog/spdlog.h>
#include "DatabaseUtils.h"

using namespace pqxx;
using namespace thxsoft::database;

namespace thxsoft::export_s3_sectors
{
    ExportToSql::ExportToSql(const string& dbFilePath, const bool deleteExistingDb)
    {
        if(deleteExistingDb)
            filesystem::remove(dbFilePath);

        if (sqlite3_open(dbFilePath.c_str(), &_db))
            throw invalid_argument(std::format("Error opening database: {}.", sqlite3_errmsg(_db)));

        createTable(ExportTableName);

        spdlog::info("Writing data to {}.", dbFilePath);

        _batchAdder = make_shared<SqliteBatchUpdate>(_db, _upsertString);
    }

    ExportToSql::~ExportToSql()
    {
        sqlite3_close(_db);
    }

    void ExportToSql::commit() const
    {
        _batchAdder->commit();

        spdlog::info("Vacuuming Sqlite databse...");
        sqlite3_exec(_db, "vacuum", nullptr, nullptr, nullptr);

        sqlite3_close_v2(_db);
    }

    void ExportToSql::append(const CelestialObject* celestialObject) const
    {
        _batchAdder->addRow(
            DatabaseUtils::asDbString(celestialObject->sectorId),
            DatabaseUtils::asDbString(celestialObject->name1),
            DatabaseUtils::asDbString(celestialObject->name2),
            DatabaseUtils::asDbString(celestialObject->name3),
            DatabaseUtils::asDbString(celestialObject->teff),
            DatabaseUtils::asDbString(celestialObject->spectralType),
            DatabaseUtils::asDbString(celestialObject->isBinary),
            DatabaseUtils::asDbString(static_cast<int>(celestialObject->x)),
            DatabaseUtils::asDbString(static_cast<int>(celestialObject->y)),
            DatabaseUtils::asDbString(static_cast<int>(celestialObject->z))
            );
    }

    void ExportToSql::createTable(const string& tableName) const
    {
        const auto createTableCommand = std::format("CREATE TABLE IF NOT EXISTS {0} ("
                                          "Sector TEXT NOT NULL,"
                                          "Name1 TEXT NULL,"
                                          "Name2 TEXT NULL,"
                                          "Name3 TEXT NULL,"
                                          "Teff INTEGER NULL,"
                                          "SpectralType TEXT NULL,"
                                          "IsBinary INTEGER NOT NULL,"
                                          "X INTEGER NOT NULL,"
                                          "Y INTEGER NOT NULL,"
                                          "Z INTEGER NOT NULL);"
                                          "CREATE INDEX Sector_Idx ON {0} (Sector);"
                                          "CREATE INDEX Name1_Idx ON {0} (lower(Name1));"
                                          "CREATE INDEX Name2_Idx ON {0} (lower(Name2));"
                                          "CREATE INDEX Name3_Idx ON {0} (lower(Name3));", tableName);

        sqlite3_exec(_db, createTableCommand.c_str(), nullptr, nullptr, nullptr);
    }
}
