add_executable(app_5_export_s3_sectors
        main.cpp
        DbReader.cpp
        DbReader.h
        ExportToSql.cpp
        ExportToSql.h
        Sector.h
        CelestialObject.h
)
set_target_properties(app_5_export_s3_sectors PROPERTIES OUTPUT_NAME "5_ExportS3Database")

include("${ThxSoftLibrary}/cmake/thxsoft_astronomy/thxsoft_astronomyTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_common/thxsoft_commonTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database/thxsoft_databaseTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database_postgres/thxsoft_database_postgresTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database_sqlite/thxsoft_database_sqliteTargets.cmake")

target_link_libraries(app_5_export_s3_sectors
        PRIVATE
        thxsoft_astronomy::thxsoft_astronomy
        thxsoft_common::thxsoft_common
        thxsoft_database::thxsoft_database
        thxsoft_database_postgres::thxsoft_database_postgres
        thxsoft_database_sqlite::thxsoft_database_sqlite
)
