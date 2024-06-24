set(TARGET_NAME app_4_export_data_fixer)
add_executable(${TARGET_NAME}
        main.cpp
        GreekLatinConverter.cpp
        GreekLatinConverter.h
        ExportDataFixerDb.cpp
        ExportDataFixerDb.h
)
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "4_ExportDataFixer")

find_package(spdlog REQUIRED)
find_package(libpqxx CONFIG REQUIRED)

include("${ThxSoftLibrary}/cmake/thxsoft_common/thxsoft_commonTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database/thxsoft_databaseTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database_postgres/thxsoft_database_postgresTargets.cmake")

target_link_libraries(${TARGET_NAME}
        PRIVATE
        thxsoft_common::thxsoft_common
        thxsoft_database::thxsoft_database
        thxsoft_database_postgres::thxsoft_database_postgres
        libpqxx::pqxx
        spdlog::spdlog_header_only
)

target_link_libraries(${TARGET_NAME}
        PRIVATE
        thxsoft_database_postgres
)

install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${ThxSoftInstallers})
