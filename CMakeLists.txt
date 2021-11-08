cmake_minimum_required(VERSION 3.21)
project(BabbleSynth LANGUAGES CXX VERSION 1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/find_qt_root.cmake)

add_subdirectory(src)

set(CPACK_PACKAGE_NAME "BabbleSynth")
set(CPACK_PACKAGE_VENDOR "Clo Yun-Hee Dufour")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A low-level parametric voice synthesizer")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "BabbleSynth")
set(CPACK_PACKACE_CHECKSUM SHA256)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt)
set(CPACK_MONOLITHIC_INSTALL TRUE)
set(CPACK_STRIP_FILES TRUE)
set(CPACK_THREADS 0)

set(_pkg_module_file ${CMAKE_CURRENT_SOURCE_DIR}/cmake/packaging/${CMAKE_SYSTEM_NAME}.cmake)
if(NOT EXISTS ${_pkg_module_file} OR IS_DIRECTORY ${_pkg_module_file})
    message(AUTHOR_WARNING "There are no packaging instructions script for this platform (${CMAKE_SYSTEM_NAME}). Will default to the Linux behaviour.")
    set(_pkg_module_file ${CMAKE_CURRENT_SOURCE_DIR}/cmake/packaging/Linux.cmake)
    message(STATUS "Configuring packaging for: Linux")
else()
    message(STATUS "Configuring packaging for: ${CMAKE_SYSTEM_NAME}")
endif()

include(${_pkg_module_file})

set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${LIB_DESTINATION})
include(InstallRequiredSystemLibraries)

install(FILES LICENSE.txt DESTINATION ${LICENSE_DESTINATION})
install(TARGETS babblesynth-gui
    RUNTIME_DEPENDENCY_SET _runtime_deps
    RUNTIME DESTINATION ${BIN_DESTINATION}
    LIBRARY DESTINATION ${LIB_DESTINATION}
    ARCHIVE DESTINATION ${LIB_DESTINATION}
)

foreach(_platform ${PLATFORM_PLUGINS})
    install(FILES ${QT_PLUGIN_DIR}/platforms/${CMAKE_SHARED_LIBRARY_PREFIX}${_platform}${CMAKE_SHARED_LIBRARY_SUFFIX}
            DESTINATION ${PLUGIN_DESTINATION}/platforms)
endforeach()

foreach(_audio ${AUDIO_PLUGINS})
    install(FILES ${QT_PLUGIN_DIR}/audio/${CMAKE_SHARED_LIBRARY_PREFIX}${_audio}${CMAKE_SHARED_LIBRARY_SUFFIX}
            DESTINATION ${PLUGIN_DESTINATION}/audio)
endforeach()

if(RUNTIME_DEPS)
    install(RUNTIME_DEPENDENCY_SET _runtime_deps
        PRE_EXCLUDE_REGEXES ${RUNTIME_DEPS_PRE_EX}
        POST_EXCLUDE_REGEXES ${RUNTIME_DEPS_POST_EX}
        DIRECTORIES ${RUNTIME_DEPS_DIRS}
        RUNTIME DESTINATION ${BIN_DESTINATION}
        LIBRARY DESTINATION ${LIB_DESTINATION}
    )
endif()

include(CPack)