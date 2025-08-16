set(TARGET_NAME app_3_simbad_import_db)
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
find_package(CURL CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)

target_link_libraries(${TARGET_NAME}
        PRIVATE
        thxsoft_astronomy
        thxsoft_common
        thxsoft_database
        thxsoft_database_postgres
        thxsoft_web
        spdlog::spdlog_header_only
        CURL::libcurl
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