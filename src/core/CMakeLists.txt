add_library(core STATIC
        filemanager.cpp
        filemanager.h
        parser.cpp
        parser.h
        parser_exceptions.h
        ramwatcher.cpp
        ramwatcher.h
        zipextractor.cpp
        zipextractor.h
        #more .cpp-files placed here
)

target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${LIBARCHIVE_INCLUDE_DIR}
)

target_compile_features(core
    PUBLIC cxx_std_17
)

target_link_libraries(core
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
        QuaZip::QuaZip
    PRIVATE
        ${LIBARCHIVE_LIBRARY}
)
