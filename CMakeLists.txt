cmake_minimum_required(VERSION 3.28)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

#################################
# Start Root Config Settings
#################################
# Full paths are required for intellisense to work.
set(ThxSoftBase "C:/Programming") # Where Thxsoft libs folder lives.
set(VCPKG_ROOT "C:/Users/steve/.vcpkg-clion/vcpkg") # VCPKG install root.

# If using a MINGW then set USING_MINGW to something like USING_MINGW=1.
#################################
# End Root Config Settings
#################################

if (DEFINED USING_MINGW)
    set(VCPKG_TRIPLET x64-mingw-static)
    add_link_options(-static-libgcc -static-libstdc++ --static)
else()
    set(VCPKG_TRIPLET x64-windows-static)
    # Needed if we are using a x64-windows-static VCPKG triplet.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_TYPE "Debug")
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BUILD_TYPE "Release")
endif()

set(AppInstallers "${ThxSoftBase}/Installers")
set(ThxSoftLibrary "${ThxSoftBase}/Libs/${VCPKG_TRIPLET}/${BUILD_TYPE}")
set(ThxSoftIncludes "${ThxSoftLibrary}/Include")
set(ThxSoftLibs "${ThxSoftLibrary}/Lib")
set(ThxSoftDLLs "${ThxSoftLibrary}/Bin")

set(VCPKG_TARGET_TRIPLET ${VCPKG_TRIPLET})
set(VCPKG_TARGET_HOST_TRIPLET ${VCPKG_TRIPLET})

link_directories("${ThxSoftLibs}")
include_directories("${ThxSoftIncludes}")

include(${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
include_directories(${VCPKG_ROOT}/installed/${VCPKG_TRIPLET}/include)

project(S3_Data_Processing)

add_subdirectory(src)