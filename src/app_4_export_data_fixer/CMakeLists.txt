add_executable(app_4_export_data_fixer
        main.cpp
        GreekLatinConverter.cpp
        GreekLatinConverter.h
        ExportDataFixerDb.cpp
        ExportDataFixerDb.h
)
set_target_properties(app_4_export_data_fixer PROPERTIES OUTPUT_NAME "4_ExportDataFixer")

find_package(spdlog REQUIRED)
find_package(libpqxx CONFIG REQUIRED)

include("${ThxSoftLibrary}/cmake/thxsoft_common/thxsoft_commonTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database/thxsoft_databaseTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database_postgres/thxsoft_database_postgresTargets.cmake")

target_link_libraries(app_4_export_data_fixer
        PRIVATE
        thxsoft_common::thxsoft_common
        thxsoft_database::thxsoft_database
        thxsoft_database_postgres::thxsoft_database_postgres
        libpqxx::pqxx
        spdlog::spdlog_header_only
)

target_link_libraries(app_4_export_data_fixer
        PRIVATE
        thxsoft_database_postgres
)
