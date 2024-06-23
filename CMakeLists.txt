cmake_minimum_required(VERSION 3.28)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

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
set(ThxSoftLibrary "${ThxSoftBase}/Libs/${BUILD_TYPE}")
set(ThxSoftIncludes "${ThxSoftLibrary}/Include")
set(ThxSoftLibs "${ThxSoftLibrary}/Lib")
set(ThxSoftDLLs "${ThxSoftLibrary}/Bin")

link_directories("${ThxSoftLibs}")
include_directories("${ThxSoftIncludes}")

add_subdirectory(src)
