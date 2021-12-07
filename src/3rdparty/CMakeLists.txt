include(FetchContent)

add_subdirectory(dr_libs)

FetchContent_Declare(
    fftw3
    URL      https://www.fftw.org/fftw-3.3.10.tar.gz
    URL_HASH SHA256=56c932549852cddcfafdab3820b0200c7742675be92179e59e6215b340e26467
)

FetchContent_Declare(
    xerces-c
    URL      https://dlcdn.apache.org//xerces/c/3/sources/xerces-c-3.2.3.tar.gz
    URL_HASH SHA256=fb96fc49b1fb892d1e64e53a6ada8accf6f0e6d30ce0937956ec68d39bd72c7e
)

FetchContent_MakeAvailable(fftw3 xerces-c)

target_include_directories(fftw3 INTERFACE $<BUILD_INTERFACE:${fftw3_SOURCE_DIR}/api>)

target_include_directories(xerces-c INTERFACE
    $<BUILD_INTERFACE:${xerces-c_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${xerces-c_BINARY_DIR}/src>)