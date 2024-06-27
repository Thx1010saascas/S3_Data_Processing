set(TARGET_NAME "app_5_export_s3_sectors")
add_executable(${TARGET_NAME}
        main.cpp
        DbReader.cpp
        DbReader.h
        ExportToSql.cpp
        ExportToSql.h
        Sector.h
        CelestialObject.h
)
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "5_ExportS3Database")

find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)

include("${ThxSoftLibrary}/cmake/thxsoft_astronomy/thxsoft_astronomyTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_common/thxsoft_commonTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database/thxsoft_databaseTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database_postgres/thxsoft_database_postgresTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database_sqlite/thxsoft_database_sqliteTargets.cmake")

target_link_libraries(${TARGET_NAME}
        PRIVATE
        fmt::fmt
        libpqxx::pqxx
        SQLite::SQLite3
        spdlog::spdlog_header_only
        thxsoft_astronomy::thxsoft_astronomy
        thxsoft_common::thxsoft_common
        thxsoft_database::thxsoft_database
        thxsoft_database_postgres::thxsoft_database_postgres
        thxsoft_database_sqlite::thxsoft_database_sqlite
)

install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${ThxSoftInstallers})
