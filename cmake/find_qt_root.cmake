#[[

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>

]]

find_package(Qt REQUIRED NAMES Qt6)

set(_qt_root "${Qt_DIR}/../../..")

message(STATUS "Looking for Qt paths")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(NOT CMAKE_CROSSCOMPILING)
        set(QT_RUNTIME_DIR ${_qt_root}/bin)
        set(QT_LIBRARY_DIR ${_qt_root}/lib)
        set(QT_PLUGIN_DIR ${_qt_root}/plugins)
    else()
        # FIXME.
        set(QT_RUNTIME_DIR ${Qt_DIR}/../..)

        find_path(_qt_prefix
            NAMES qt6/plugins
            PATHS
                ${Qt_DIR}/../..
            PATH_SUFFIXES
                platforms
                audio
        )

        set(QT_PLUGIN_DIR ${_qt_prefix}/qt5/plugins)
    endif()

elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Check if the default QT_ROOT is already correct. (If standalone Qt installation) 
    if(EXISTS ${_qt_root}/plugins)
        set(QT_RUNTIME_DIR ${_qt_root}/../..)
        set(QT_LIBRARY_DIR ${_qt_root}/../..)
        set(QT_PLUGIN_DIR ${_qt_root}/plugins)

    else()
        # This instead of lib/ subfolder because it can be in lib64.
        set(QT_RUNTIME_DIR ${Qt_DIR}/../..)
        set(QT_LIBRARY_DIR ${Qt_DIR}/../..)

        find_path(_qt_prefix
            NAMES qt6/plugins
            PATHS
                /usr/lib64
                /usr/lib
                /usr/libexec
                /usr/local/lib64
                /usr/local/lib
                /usr/local/libexec
                /opt/lib64
                /opt/lib
                /opt/libexec
            PATH_SUFFIXES
                platforms
                audio
        )

        set(QT_PLUGIN_DIR ${_qt_prefix}/qt5/plugins)
    endif()
else()
    message(AUTHOR_WARNING "  There is no known Qt 6 installation directory structure for this platform (${CMAKE_SYSTEM_NAME}). Will default to the Windows behaviour.")
endif()

get_filename_component(QT_RUNTIME_DIR ${QT_RUNTIME_DIR} ABSOLUTE)
get_filename_component(QT_LIBRARY_DIR ${QT_LIBRARY_DIR} ABSOLUTE)
get_filename_component(QT_PLUGIN_DIR ${QT_PLUGIN_DIR} ABSOLUTE)

message(STATUS "  Qt runtime path: ${QT_RUNTIME_DIR}")
message(STATUS "  Qt library path: ${QT_LIBRARY_DIR}")
message(STATUS "  Qt plugin path: ${QT_PLUGIN_DIR}")
