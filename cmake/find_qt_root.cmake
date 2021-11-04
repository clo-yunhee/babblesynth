find_package(Qt REQUIRED NAMES Qt5)

set(_qt_root "${Qt_DIR}/../../..")

message(STATUS "Looking for Qt paths")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(QT_RUNTIME_DIR ${_qt_root}/lib)
    set(QT_PLUGIN_DIR ${_qt_root}/plugins)

elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Check if the default QT_ROOT is already correct. (If standalone Qt installation) 
    if(EXISTS ${_qt_root}/plugins)
        set(QT_RUNTIME_DIR ${_qt_root}/../..)
        set(QT_PLUGIN_DIR ${_qt_root}/plugins)

    else()
        # This instead of lib/ subfolder because it can be in lib64.
        set(QT_RUNTIME_DIR ${Qt_DIR}/../..)

        find_path(_qt_prefix
            NAMES qt5
            HINTS
                /usr/lib
                /usr/lib64
                /usr/libexec
                /usr/local/lib
                /usr/local/lib64
                /usr/local/libexec
                /opt/lib
                /opt/lib64
                /opt/libexec
            PATH_SUFFIXES
                mkspecs
                plugins
                qml
        )

        set(QT_PLUGIN_DIR ${_qt_prefix}/qt5/plugins)
    endif()
else()
    message(AUTHOR_WARNING "  There is no known Qt 5 installation directory structure for this platform (${CMAKE_SYSTEM_NAME}). Will default to the Windows behaviour.")
endif()

get_filename_component(QT_RUNTIME_DIR ${QT_RUNTIME_DIR} ABSOLUTE)
get_filename_component(QT_PLUGIN_DIR ${QT_PLUGIN_DIR} ABSOLUTE)

message(STATUS "  Qt runtime path: ${QT_RUNTIME_DIR}")
message(STATUS "  Qt plugin path: ${QT_PLUGIN_DIR}")
