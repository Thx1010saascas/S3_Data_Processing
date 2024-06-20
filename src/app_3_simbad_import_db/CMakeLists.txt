add_executable(app_3_simbad_import_db
        main.cpp
        ExportToSql.cpp
        ExportToSql.h
        SimbadRowProcessor.cpp
        SimbadRowProcessor.h
        DbQuery.cpp
        DbQuery.h)
set_target_properties(app_3_simbad_import_db PROPERTIES OUTPUT_NAME "3_SimbadImportData")

find_package(spdlog REQUIRED)
find_package(unofficial-curlpp CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)

include("${ThxSoftLibrary}/cmake/thxsoft_astronomy/thxsoft_astronomyTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_common/thxsoft_commonTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database/thxsoft_databaseTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database_postgres/thxsoft_database_postgresTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_web/thxsoft_webTargets.cmake")

target_link_libraries(app_3_simbad_import_db
        PRIVATE
        thxsoft_astronomy::thxsoft_astronomy
        thxsoft_common::thxsoft_common
        thxsoft_database::thxsoft_database
        thxsoft_database_postgres::thxsoft_database_postgres
        thxsoft_web::thxsoft_web
        spdlog::spdlog_header_only
        unofficial::curlpp::curlpp
        libpqxx::pqxx
)

add_custom_command(TARGET app_3_simbad_import_db POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ThxSoftDLLs}"
        $<TARGET_FILE_DIR:app_3_simbad_import_db>)
