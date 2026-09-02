cmake_minimum_required(VERSION 3.16)

#-----------------------------------------------------
# project definition
#-----------------------------------------------------

project(
        lettershark
        VERSION 1.1.0
        LANGUAGES CXX
)

#-----------------------------------------------------
# global C++-settings
#-----------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#-----------------------------------------------------
# warnings & compiler-flags
#-----------------------------------------------------

if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#-----------------------------------------------------
# Qt
#-----------------------------------------------------

set(MSYS2_ROOT "C:/msys64/mingw64" CACHE PATH "MSYS2 MINGW64 root")
list(APPEND CMAKE_PREFIX_PATH "${MSYS2_ROOT}")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

#-----------------------------------------------------
# third-party dependencies
#-----------------------------------------------------

find_path(LIBARCHIVE_INCLUDE_DIR
        NAMES archive.h
        PATHS "${MSYS2_ROOT}/include"
)
find_library(LIBARCHIVE_LIBRARY
        NAMES archive
        PATHS "${MSYS2_ROOT}/lib"
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(QUAZIP_QT_MAJOR_VERSION 6 CACHE STRING "" FORCE)
add_subdirectory(third_party/quazip)

#-----------------------------------------------------
# subdirectories
#-----------------------------------------------------

add_subdirectory(src)
