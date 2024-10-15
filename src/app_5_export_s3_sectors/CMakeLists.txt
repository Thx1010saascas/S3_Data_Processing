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
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "5_ExportS3Database")

find_package(spdlog REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

include("${ThxSoftLibrary}/cmake/thxsoft_astronomy/thxsoft_astronomyTargets.cmake")
include_directories("${ThxSoftIncludes}/thxsoft_astronomy")
include("${ThxSoftLibrary}/cmake/thxsoft_common/thxsoft_commonTargets.cmake")
include_directories("${ThxSoftIncludes}/thxsoft_common")
include("${ThxSoftLibrary}/cmake/thxsoft_database/thxsoft_databaseTargets.cmake")
include_directories("${ThxSoftIncludes}/thxsoft_database")
include("${ThxSoftLibrary}/cmake/thxsoft_database_postgres/thxsoft_database_postgresTargets.cmake")
include_directories("${ThxSoftIncludes}/thxsoft_database_postgres")
include("${ThxSoftLibrary}/cmake/thxsoft_database_sqlite/thxsoft_database_sqliteTargets.cmake")
include_directories("${ThxSoftIncludes}/thxsoft_database_sqlite")

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

install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${AppInstallers})
install(FILES ${RuntimeDependencies}/libcrypto-3-x64.dll DESTINATION ${AppInstallers})
install(FILES ${RuntimeDependencies}/libssl-3-x64.dll DESTINATION ${AppInstallers})
install(FILES ${RuntimeDependencies}/libiconv-2.dll DESTINATION ${AppInstallers})
install(FILES ${RuntimeDependencies}/libintl-9.dll DESTINATION ${AppInstallers})
install(FILES ${RuntimeDependencies}/libpq.dll DESTINATION ${AppInstallers})

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${RuntimeDependencies}
        $<TARGET_FILE_DIR:${TARGET_NAME}>)