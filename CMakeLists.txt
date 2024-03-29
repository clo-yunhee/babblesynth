cmake_minimum_required(VERSION 3.21)
project(BabbleSynth LANGUAGES C CXX)
set(PROJECT_VERSION 0.2-alpha.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/find_qt_root.cmake)

add_subdirectory(src)

set(CPACK_PACKAGE_NAME "BabbleSynth")
set(CPACK_PACKAGE_VENDOR "Clo Yun-Hee Dufour")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A low-level parametric voice synthesizer")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "BabbleSynth")
set(CPACK_PACKACE_CHECKSUM SHA256)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt)
set(CPACK_COMPONENTS_ALL BabbleSynth)
set(CPACK_COMPONENT_BABBLESYNTH_DISPLAY_NAME "Install BabbleSynth")
set(CPACK_COMPONENT_BABBLESYNTH_REQUIRED TRUE)
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
set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT BabbleSynth)
include(InstallRequiredSystemLibraries)

install(FILES LICENSE.txt DESTINATION ${LICENSE_DESTINATION} COMPONENT BabbleSynth)
install(TARGETS babblesynth-gui
    RUNTIME_DEPENDENCY_SET _runtime_deps
    RUNTIME DESTINATION ${BIN_DESTINATION} COMPONENT BabbleSynth
    LIBRARY DESTINATION ${LIB_DESTINATION} COMPONENT BabbleSynth
    ARCHIVE DESTINATION ${LIB_DESTINATION} COMPONENT BabbleSynth
)

foreach(_platform ${PLATFORM_PLUGINS})
    install(FILES ${QT_PLUGIN_DIR}/platforms/${CMAKE_SHARED_LIBRARY_PREFIX}${_platform}${CMAKE_SHARED_LIBRARY_SUFFIX}
            DESTINATION ${PLUGIN_DESTINATION}/platforms
            COMPONENT BabbleSynth)
endforeach()

foreach(_audio ${AUDIO_PLUGINS})
    install(FILES ${QT_PLUGIN_DIR}/audio/${CMAKE_SHARED_LIBRARY_PREFIX}${_audio}${CMAKE_SHARED_LIBRARY_SUFFIX}
            DESTINATION ${PLUGIN_DESTINATION}/audio
            COMPONENT BabbleSynth)
endforeach()

if(RUNTIME_DEPS)
    install(RUNTIME_DEPENDENCY_SET _runtime_deps
        PRE_EXCLUDE_REGEXES ${RUNTIME_DEPS_PRE_EX}
        POST_EXCLUDE_REGEXES ${RUNTIME_DEPS_POST_EX}
        DIRECTORIES ${RUNTIME_DEPS_DIRS}
        RUNTIME DESTINATION ${BIN_DESTINATION} COMPONENT BabbleSynth
        LIBRARY DESTINATION ${LIB_DESTINATION} COMPONENT BabbleSynth
    )
endif()

include(CPack)