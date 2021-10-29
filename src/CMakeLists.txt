if(MSVC)
    set(CMAKE_CXX_FLAGS "/MD /EHsc")
    set(CMAKE_CXX_FLAGS_RELEASE "/Ob2 /Oi /Os /Oy /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS "-ftree-vectorize -fno-omit-frame-pointer -fno-math-errno -ffast-math")
endif()

string(TOUPPER ${CMAKE_BUILD_TYPE} FLAGS_SUFFIX)
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS_${FLAGS_SUFFIX}} ${CMAKE_CXX_FLAGS}")

add_subdirectory(babblesynth)
add_subdirectory(dr_libs)
add_subdirectory(cli)
add_subdirectory(gui)