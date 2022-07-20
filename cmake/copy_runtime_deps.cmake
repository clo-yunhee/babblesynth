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

get_filename_component(_target_dir ${TARGET_EXECUTABLE} DIRECTORY)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_CROSSCOMPILING)
        set(CMAKE_GET_RUNTIME_DEPENDENCIES_PLATFORM "windows+pe")
        set(CMAKE_GET_RUNTIME_DEPENDENCIES_TOOL "objdump")
        set(CMAKE_GET_RUNTIME_DEPENDENCIES_COMMAND "x86_64-w64-mingw32-objdump")
    endif()

    file(GET_RUNTIME_DEPENDENCIES
        EXECUTABLES ${TARGET_EXECUTABLE}
        RESOLVED_DEPENDENCIES_VAR _r_deps
        UNRESOLVED_DEPENDENCIES_VAR _u_deps
        PRE_EXCLUDE_REGEXES "api-ms-*" "ext-ms-*"
        POST_EXCLUDE_REGEXES ".*system32/.*\\.dll" "${_target_dir}/(?!fftw3).*\\.dll"
        DIRECTORIES ${QT_RUNTIME_DIR} ${FFTW_OUTPUT_DIRECTORY} ${DLL_DIRECTORY}
    )

    foreach(_file ${_r_deps})
        file(INSTALL ${_file} DESTINATION ${_target_dir})
    endforeach()

    foreach(_file ${_u_deps})
        message(WARNING "Runtime dependency ${_file} could not be resolved.")
    endforeach()
    
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" OR CMAKE_CXX_SIMULATE_ID MATCHES "MSVC")
        if(CMAKE_BUILD_TYPE STREQUAL Debug)
            set(d_ "d")
        else()
            set(d_ "")
        endif()

        file(INSTALL ${QT_PLUGIN_DIR}/platforms/qwindows${d_}.dll DESTINATION ${_target_dir}/plugins/platforms)
        file(INSTALL ${QT_PLUGIN_DIR}/platforms/qwindows${d_}.pdb DESTINATION ${_target_dir}/plugins/platforms)

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        file(INSTALL ${QT_PLUGIN_DIR}/platforms/qwindows.dll DESTINATION ${_target_dir}/plugins/platforms)
        file(INSTALL ${QT_PLUGIN_DIR}/platforms/qwindows.debug DESTINATION ${_target_dir}/plugins/platforms)

    endif()

elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    file(INSTALL ${QT_PLUGIN_DIR}/platforms/libqxcb.so DESTINATION ${_target_dir}/plugins/platforms)
endif()
