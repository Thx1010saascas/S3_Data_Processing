#include "ExportToSql.h"
#include <spdlog/spdlog.h>
#include "DatabaseUtils.h"
#include "DbReader.h"

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

        finaliseTableCeation();
        vacuumTable();

        sqlite3_close_v2(_db);
    }

    void ExportToSql::append(const CelestialObject* celestialObject) const
    {
        _batchAdder->addRow(
            DatabaseUtils::asDbString(celestialObject->sectorId),
            DatabaseUtils::asDbString(celestialObject->name1),
            DatabaseUtils::asDbString(celestialObject->name2),
            DatabaseUtils::asDbString(celestialObject->name3),
            DatabaseUtils::asDbString(celestialObject->teff.has_value() ? std::to_string(static_cast<int>(round(celestialObject->teff.value()))) : ""),
            DatabaseUtils::asDbString(celestialObject->spectralType.has_value() ? celestialObject->spectralType.value() : ""),
            DatabaseUtils::asDbString(celestialObject->isBinary),
            DatabaseUtils::asDbString(static_cast<int>(DbReader::myRound(celestialObject->x))),
            DatabaseUtils::asDbString(static_cast<int>(DbReader::myRound(celestialObject->y))),
            DatabaseUtils::asDbString(static_cast<int>(DbReader::myRound(celestialObject->z)))
            );
    }

    void ExportToSql::createTable(const string& tableName) const
    {
        sqlite3_exec(_db, "PRAGMA journal_mode = MEMORY", nullptr, nullptr, nullptr);
        sqlite3_exec(_db, "PRAGMA locking_mode = EXCLUSIVE", nullptr, nullptr, nullptr);

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
                                          "Z INTEGER NOT NULL);", tableName);

        sqlite3_exec(_db, createTableCommand.c_str(), nullptr, nullptr, nullptr);
    }

    void ExportToSql::finaliseTableCeation() const
    {
        spdlog::info("Indexing database column 'Sector'...");
        sqlite3_exec(_db, std::format("CREATE INDEX Sector_Idx ON {0} (Sector  ASC);", ExportTableName).c_str(), nullptr, nullptr, nullptr);

        spdlog::info("Indexing database column 'Name1'...");
        sqlite3_exec(_db, std::format("CREATE INDEX Name1_Idx ON {0} (lower(Name1) ASC);", ExportTableName).c_str(), nullptr, nullptr, nullptr);

        spdlog::info("Indexing database column 'Name2'...");
        sqlite3_exec(_db, std::format("CREATE INDEX Name2_Idx ON {0} (lower(Name2) ASC);", ExportTableName).c_str(), nullptr, nullptr, nullptr);

        spdlog::info("Indexing database column 'Name3'...");
        sqlite3_exec(_db, std::format("CREATE INDEX Name3_Idx ON {0} (lower(Name3) ASC);", ExportTableName).c_str(), nullptr, nullptr, nullptr);
    }

    void ExportToSql::vacuumTable() const
    {
        spdlog::info("Vacuuming database...");
        sqlite3_exec(_db, "vacuum", nullptr, nullptr, nullptr);
    }
}
