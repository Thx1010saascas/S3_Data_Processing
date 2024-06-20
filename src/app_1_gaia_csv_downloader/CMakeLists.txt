add_executable(app_1_gaia_csv_downloader
        main.cpp
#        ${HEADER_FILES}
)
set_target_properties(app_1_gaia_csv_downloader PROPERTIES OUTPUT_NAME "1_GaiaDownloadCsvs")

find_package(spdlog REQUIRED)
find_package(unofficial-curlpp CONFIG REQUIRED)
find_package(unofficial-gumbo CONFIG REQUIRED)

include("${ThxSoftLibrary}/cmake/thxsoft_common/thxsoft_commonTargets.cmake")
include("${ThxSoftLibrary}/cmake/thxsoft_web/thxsoft_webTargets.cmake")

target_link_libraries(app_1_gaia_csv_downloader
        PRIVATE
        thxsoft_common::thxsoft_common
        thxsoft_web::thxsoft_web
        spdlog::spdlog_header_only
        unofficial::curlpp::curlpp
        unofficial::gumbo::gumbo)

add_custom_command(TARGET app_1_gaia_csv_downloader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ThxSoftDLLs}"
        $<TARGET_FILE_DIR:app_1_gaia_csv_downloader>)
