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

if (WIN32)
    set(QT_ROOT ${Qt5_DIR}/../../..)
    get_filename_component(target_dir ${TARGET_EXECUTABLE} DIRECTORY)

    file(GET_RUNTIME_DEPENDENCIES
        EXECUTABLES ${TARGET_EXECUTABLE}
        RESOLVED_DEPENDENCIES_VAR _r_deps
        UNRESOLVED_DEPENDENCIES_VAR _u_deps
        PRE_EXCLUDE_REGEXES "api-ms-*" "ext-ms-*"
        POST_EXCLUDE_REGEXES ".*system32/.*\\.dll" "${target_dir}/.*\\.dll"
        DIRECTORIES ${QT_ROOT}/bin
    )

    foreach(_file ${_r_deps})
        file(INSTALL ${_file} DESTINATION ${target_dir})
    endforeach()

    foreach(_file ${_u_deps})
        message(WARNING "Runtime dependency ${_file} could not be resolved.")
    endforeach()

    file(INSTALL ${QT_ROOT}/plugins/platforms/qwindows.dll DESTINATION ${target_dir}/plugins/platforms)
    file(INSTALL ${QT_ROOT}/plugins/audio/qtaudio_windows.dll DESTINATION ${target_dir}/plugins/audio)
endif()