set(TARGET_NAME app_1_gaia_csv_downloader)

add_executable(${TARGET_NAME}
        main.cpp
)
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME "1_GaiaDownloadCsvs")

find_package(spdlog REQUIRED)
find_package(unofficial-curlpp CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(unofficial-gumbo CONFIG REQUIRED)

include("${ThxSoftLibrary}/cmake/thxsoft_common/thxsoft_commonTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_web/thxsoft_webTargets.cmake")

target_link_libraries(${TARGET_NAME}
        PRIVATE
        thxsoft_common::thxsoft_common
        thxsoft_web::thxsoft_web
        spdlog::spdlog_header_only
        unofficial::curlpp::curlpp
        unofficial::gumbo::gumbo)

#add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:${TARGET_NAME}> $<TARGET_RUNTIME_DLLS:${TARGET_NAME}>
#        COMMAND_EXPAND_LISTS
#)

install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${ThxSoftInstallers})
#install(IMPORTED_RUNTIME_ARTIFACTS unofficial::curlpp::curlpp RUNTIME DESTINATION ${ThxSoftInstallers})
#install(IMPORTED_RUNTIME_ARTIFACTS CURL::libcurl RUNTIME DESTINATION ${ThxSoftInstallers})
#install(IMPORTED_RUNTIME_ARTIFACTS fmt::fmt RUNTIME DESTINATION ${ThxSoftInstallers})

#add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        "${ThxSoftDLLs}"
#        $<TARGET_FILE_DIR:${TARGET_NAME}>)
