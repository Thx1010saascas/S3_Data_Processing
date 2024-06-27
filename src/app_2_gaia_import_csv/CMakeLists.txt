set(TARGET_NAME "app_2_gaia_import_csv")
add_executable(${TARGET_NAME}
        main.cpp
        main.cpp
        ExportToSql.cpp
        ExportToSql.h
        GaiaRowProcessor.h
        GaiaRowProcessor.cpp
        ExportProgressManager.cpp
        ExportProgressManager.h
)
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "2_GaiaImportCsvs")

find_package(spdlog REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

include("${ThxSoftLibrary}/cmake/thxsoft_astronomy/thxsoft_astronomyTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_compression/thxsoft_compressionTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_common/thxsoft_commonTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database/thxsoft_databaseTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_database_postgres/thxsoft_database_postgresTargets.cmake")

target_link_libraries(${TARGET_NAME}
        PRIVATE
        thxsoft_database_postgres::thxsoft_database_postgres
        thxsoft_compression::thxsoft_compression
        thxsoft_astronomy::thxsoft_astronomy
        spdlog::spdlog_header_only
        libpqxx::pqxx
        ZLIB::ZLIB
)

install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${ThxSoftInstallers})
