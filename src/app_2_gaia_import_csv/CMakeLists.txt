set(TARGET_NAME app_2_gaia_import_csv)
add_executable(${TARGET_NAME}
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
find_package(fmt REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(ZLIB REQUIRED)

target_link_libraries(${TARGET_NAME}
        PRIVATE
        thxsoft_database_postgres
        thxsoft_compression
        thxsoft_astronomy
        spdlog::spdlog_header_only
        libpqxx::pqxx
        fmt::fmt-header-only
        ZLIB::ZLIB
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