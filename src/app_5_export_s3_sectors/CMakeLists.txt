set(TARGET_NAME app_5_export_s3_sectors)
add_executable(${TARGET_NAME}
        main.cpp
        DbReader.cpp
        DbReader.h
        ExportToSql.cpp
        ExportToSql.h
        Sector.h
        CelestialObject.h
)
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "5_ExportS3Database"    )

find_package(spdlog REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

include("${ThxSoftLibrary}/cmake/thxsoft_astronomy/thxsoft_astronomyTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_common/thxsoft_commonTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database/thxsoft_databaseTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database_postgres/thxsoft_database_postgresTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database_sqlite/thxsoft_database_sqliteTargets.cmake")

target_link_libraries(${TARGET_NAME}
        PRIVATE
        thxsoft_astronomy::thxsoft_astronomy
        thxsoft_common::thxsoft_common
        thxsoft_database::thxsoft_database
        thxsoft_database_postgres::thxsoft_database_postgres
        thxsoft_database_sqlite::thxsoft_database_sqlite
        spdlog::spdlog_header_only
        unofficial::sqlite3::sqlite3
        libpqxx::pqxx
)

install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${ThxSoftInstallers})
