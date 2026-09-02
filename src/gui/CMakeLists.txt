qt_add_resources(APP_RESOURCES ${CMAKE_SOURCE_DIR}/resources/resources.qrc)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(lettershark
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        parser_exceptions.h
        titlebar.h titlebar.cpp
        mainwindowuisetup.h mainwindowuisetup.cpp
        tabfactory.h tabfactory.cpp
        searchcontroller.h searchcontroller.cpp
        windowdragger.h windowdragger.cpp
        contextmenuhandler.h contextmenuhandler.cpp
        customdialog.h customdialog.cpp
        ${APP_RESOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET lettershark APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(lettershark SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(lettershark
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(lettershark
  PRIVATE
    core
    Qt${QT_VERSION_MAJOR}::Widgets
  )

# --- Added: Custom deploy command for Release builds (Windows) ---
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(TARGET lettershark POST_BUILD
        COMMAND "${Qt6_DIR}/../../../bin/windeployqt" --release $<TARGET_FILE:lettershark>
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Deploying Qt dependencies with windeployqt for Release"
    )
    add_custom_command(TARGET lettershark POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
              "${MSYS2_ROOT}/bin/zlib1.dll"
              "$<TARGET_FILE_DIR:lettershark>"
         COMMENT "Copying zlib1.dll into Release bundle"
    )
    add_custom_command(TARGET lettershark POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libarchive-13.dll"
        "$<TARGET_FILE_DIR:lettershark>"
      COMMENT "Copying libarchive-13.dll into Release bundle"
    )
    add_custom_command(TARGET lettershark POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/zlib1.dll"
        "$<TARGET_FILE_DIR:lettershark>"
    )
    add_custom_command(TARGET lettershark POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libbz2-1.dll"
        "$<TARGET_FILE_DIR:lettershark>"
    )
    add_custom_command(TARGET lettershark POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libb2-1.dll"
        "$<TARGET_FILE_DIR:lettershark>"
    )
    add_custom_command(TARGET lettershark POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/liblz4.dll"
        "$<TARGET_FILE_DIR:lettershark>"
    )
    add_custom_command(TARGET lettershark POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/liblzma-5.dll"
        "$<TARGET_FILE_DIR:lettershark>"
    )
    add_custom_command(TARGET lettershark POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libcrypto-3-x64.dll"
        "$<TARGET_FILE_DIR:lettershark>"
    )
    add_custom_command(TARGET lettershark POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libexpat-1.dll"
        "$<TARGET_FILE_DIR:lettershark>"
    )
    add_custom_command(TARGET lettershark POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libiconv-2.dll"
        "$<TARGET_FILE_DIR:lettershark>"
    )
    add_custom_command(TARGET lettershark POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MSYS2_ROOT}/bin/libzstd.dll"
        "$<TARGET_FILE_DIR:lettershark>"
    )
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.dksoftwaresolutions.lettershark)
endif()
set_target_properties(lettershark PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS lettershark
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(lettershark)
endif()
