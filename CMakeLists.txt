cmake_minimum_required(VERSION 3.31)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

#################################
# Start Root Config Settings
#################################
# Full paths are required for intellisense to work.
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(ThxSoftBase "C:/Programming") # Where Thxsoft libs folder lives.
    set(VCPKG_ROOT "%HOMEPATH%/.vcpkg-clion/vcpkg")
    set(USING_MINGW "1")
else()
    set(ThxSoftBase "$ENV{HOME}/Programming")
    set(VCPKG_ROOT "$ENV{HOME}/.vcpkg-clion/vcpkg")
endif()

# If using a MINGW then set USING_MINGW to something like USING_MINGW=1.
#################################
# End Root Config Settings
#################################

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    if (USING_MINGW STREQUAL "1")
        set(VCPKG_TRIPLET x64-mingw-static)
        add_link_options(-static-libgcc -static-libstdc++ --static)
    else()
        set(VCPKG_TRIPLET x64-windows-static-md)
        # Needed if we are using a x64-windows-static VCPKG triplet.
        #set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
else()
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        #set(VCPKG_TRIPLET x64-osx)
        set(VCPKG_TRIPLET arm64-osx)
    endif()
endif()

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

if (IS_DEBUG STREQUAL "1")
    add_compile_definitions("IS_DEBUG=1")
    set(BUILD_TYPE "Debug")
else()
    set(BUILD_TYPE "Release")
endif()

set(AppInstallers "${ThxSoftBase}/Installers")
set(ThxSoftLibrary "${ThxSoftBase}/Libs/${CMAKE_HOST_SYSTEM_NAME}/${VCPKG_TRIPLET}/${BUILD_TYPE}")
set(ThxSoftIncludes "${ThxSoftLibrary}/Include")
set(ThxSoftLibs "${ThxSoftLibrary}/Lib")
set(ThxSoftDLLs "${ThxSoftLibrary}/Bin")

message("ThsSoftLibrary: ${ThxSoftLibrary}")

set(VCPKG_TARGET_TRIPLET ${VCPKG_TRIPLET})
set(VCPKG_TARGET_HOST_TRIPLET ${VCPKG_TRIPLET})

link_directories("${ThxSoftLibs}")
include_directories("${ThxSoftIncludes}")

include("${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
include_directories("${VCPKG_ROOT}/installed/${VCPKG_TRIPLET}/include")

project(S3)

set(RuntimeDependencies "${PROJECT_SOURCE_DIR}/RuntimeDependencies")

add_subdirectory(src)
