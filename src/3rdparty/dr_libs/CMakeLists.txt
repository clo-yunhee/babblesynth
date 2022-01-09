add_library(dr_libs OBJECT
    dr_flac.h
    dr_mp3.h
    dr_wav.h
    dr_libs.cpp
)

target_include_directories(dr_libs INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
