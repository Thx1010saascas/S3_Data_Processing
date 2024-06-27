cmake_minimum_required(VERSION 3.28)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(VCPKG_TARGET_TRIPLET x64-mingw-static)
set(VCPKG_TARGET_HOST_TRIPLET x64-mingw-static)
include(/Users/steve/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake)

set(CURL_USE_STATIC_LIBS TRUE)

project(S3_Data_Processing)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE "Debug")
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BUILD_TYPE "Release")
endif()

set(ThxSoftBase "c:/Programming")
# No need to touch
set(ThxSoftInstallers "${ThxSoftBase}/Libs/Releases")
set(ThxSoftLibrary "${ThxSoftBase}/Libs/MinGW/${BUILD_TYPE}")
set(CMAKE_INSTALL_PREFIX ${ThxSoftLibrary})
set(ThxSoftIncludes "${ThxSoftLibrary}/Include")
set(ThxSoftLibs "${ThxSoftLibrary}/Lib")
set(ThxSoftDLLs "${ThxSoftLibrary}/Bin")

link_directories("${ThxSoftLibs}")
include_directories("${ThxSoftIncludes}")

add_subdirectory(src)
