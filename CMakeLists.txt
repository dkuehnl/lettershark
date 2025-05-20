cmake_minimum_required(VERSION 3.16)

project(log_viewer VERSION 1.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(MSYS2_ROOT "C:/msys64/mingw64" CACHE PATH "MSYS2 MINGW64 root")
list(APPEND CMAKE_PREFIX_PATH "${MSYS2_ROOT}")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

find_path(LIBARCHIVE_INCLUDE_DIR
  NAMES archive.h
  PATHS "${MSYS2_ROOT}/include"
)
find_library(LIBARCHIVE_LIBRARY
  NAMES archive
  PATHS "${MSYS2_ROOT}/lib"
)

qt_add_resources(APP_RESOURCES resources.qrc)


set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(QUAZIP_QT_MAJOR_VERSION 6 CACHE STRING "" FORCE)
add_subdirectory(src/thirdparty/quazip)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(log_viewer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        parser.h parser.cpp
        titlebar.h titlebar.cpp
        zipextractor.h zipextractor.cpp
        mainwindowuisetup.h mainwindowuisetup.cpp
        filemanager.h filemanager.cpp
        tabfactory.h tabfactory.cpp
        searchcontroller.h searchcontroller.cpp
        windowdragger.h windowdragger.cpp
        contextmenuhandler.h contextmenuhandler.cpp
        ramwatcher.h ramwatcher.cpp
        resources.qrc
        customdialog.h customdialog.cpp
        parser_exceptions.h
        ${APP_RESOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET log_viewer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(log_viewer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(log_viewer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(log_viewer
  PRIVATE
    ${LIBARCHIVE_INCLUDE_DIR}
)

target_link_libraries(log_viewer
  PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    QuaZip::QuaZip
    ${LIBARCHIVE_LIBRARY}
  )

# --- Added: Custom deploy command for Release builds (Windows) ---
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(TARGET log_viewer POST_BUILD
        COMMAND "${Qt6_DIR}/../../../bin/windeployqt" --release $<TARGET_FILE:log_viewer>
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Deploying Qt dependencies with windeployqt for Release"
    )
    add_custom_command(TARGET log_viewer POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
              "${MSYS2_ROOT}/bin/zlib1.dll"
              "$<TARGET_FILE_DIR:log_viewer>"
         COMMENT "Copying zlib1.dll into Release bundle"
    )
    add_custom_command(TARGET log_viewer POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libarchive-13.dll"
        "$<TARGET_FILE_DIR:log_viewer>"
      COMMENT "Copying libarchive-13.dll into Release bundle"
    )
    add_custom_command(TARGET log_viewer POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/zlib1.dll"
        "$<TARGET_FILE_DIR:log_viewer>"
    )
    add_custom_command(TARGET log_viewer POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libbz2-1.dll"
        "$<TARGET_FILE_DIR:log_viewer>"
    )
    add_custom_command(TARGET log_viewer POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libb2-1.dll"
        "$<TARGET_FILE_DIR:log_viewer>"
    )
    add_custom_command(TARGET log_viewer POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/liblz4.dll"
        "$<TARGET_FILE_DIR:log_viewer>"
    )
    add_custom_command(TARGET log_viewer POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/liblzma-5.dll"
        "$<TARGET_FILE_DIR:log_viewer>"
    )
    add_custom_command(TARGET log_viewer POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libcrypto-3-x64.dll"
        "$<TARGET_FILE_DIR:log_viewer>"
    )
    add_custom_command(TARGET log_viewer POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libexpat-1.dll"
        "$<TARGET_FILE_DIR:log_viewer>"
    )
    add_custom_command(TARGET log_viewer POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libiconv-2.dll"
        "$<TARGET_FILE_DIR:log_viewer>"
    )
    add_custom_command(TARGET log_viewer POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libzstd.dll"
        "$<TARGET_FILE_DIR:log_viewer>"
    )
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.log_viewer)
endif()
set_target_properties(log_viewer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS log_viewer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(log_viewer)
endif()

