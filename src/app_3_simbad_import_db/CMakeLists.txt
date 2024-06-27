set(TARGET_NAME "app_3_simbad_import_db")
add_executable(${TARGET_NAME}
        main.cpp
        ExportToSql.cpp
        ExportToSql.h
        SimbadRowProcessor.cpp
        SimbadRowProcessor.h
        DbQuery.cpp
        DbQuery.h)
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "3_SimbadImportData")

find_package(spdlog REQUIRED)
find_package(CURL REQUIRED)
find_package(libpqxx CONFIG REQUIRED)

include("${ThxSoftLibrary}/cmake/thxsoft_astronomy/thxsoft_astronomyTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_common/thxsoft_commonTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database/thxsoft_databaseTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database_postgres/thxsoft_database_postgresTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_web/thxsoft_webTargets.cmake")

target_link_libraries(${TARGET_NAME}
        PRIVATE
        thxsoft_astronomy::thxsoft_astronomy
        thxsoft_common::thxsoft_common
        thxsoft_database::thxsoft_database
        thxsoft_database_postgres::thxsoft_database_postgres
        thxsoft_web::thxsoft_web
        spdlog::spdlog_header_only
        CURL::libcurl
        libpqxx::pqxx
)

install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${ThxSoftInstallers})
