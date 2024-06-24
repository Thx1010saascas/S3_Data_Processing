cmake_minimum_required(VERSION 3.28)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(VCPKG_TARGET_TRIPLET x64-windows-static)
# Needed because we are using x64-windows-static
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
include(/Users/steve/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(S3_Data_Processing)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE "Debug")
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BUILD_TYPE "Release")
endif()

set(ThxSoftBase "C:/Programming/Libs")
set(ThxSoftInstallers "C:/Programming/Libs/Installers")
set(ThxSoftLibrary "C:/Programming/Libs/${BUILD_TYPE}")
set(ThxSoftIncludes "${ThxSoftLibrary}/Include")
set(ThxSoftLibs "${ThxSoftLibrary}/Lib")
set(ThxSoftDLLs "${ThxSoftLibrary}/Bin")

link_directories("${ThxSoftLibs}")
include_directories("${ThxSoftIncludes}")

add_subdirectory(src)