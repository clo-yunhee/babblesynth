include(FetchContent)

add_subdirectory(dr_libs)

FetchContent_Declare(
    fftw3
    URL      https://www.fftw.org/fftw-3.3.10.tar.gz
    URL_HASH SHA256=56c932549852cddcfafdab3820b0200c7742675be92179e59e6215b340e26467
)

FetchContent_Declare(
    xerces-c
    URL      https://dlcdn.apache.org//xerces/c/3/sources/xerces-c-3.2.4.tar.gz
    URL_HASH SHA256=3d8ec1c7f94e38fee0e4ca5ad1e1d9db23cbf3a10bba626f6b4afa2dedafe5ab
)

FetchContent_Declare(
    libsndfile
    GIT_REPOSITORY https://github.com/libsndfile/libsndfile.git
    GIT_TAG        a4f1387ab8084fac7f0301b18e01011be9381672 # latest (because Lame dependency search is broken on 1.1.0)
)

FetchContent_Declare(
    libsamplerate
    URL      https://github.com/libsndfile/libsamplerate/releases/download/0.2.2/libsamplerate-0.2.2.tar.xz
    URL_HASH SHA256=3258DA280511D24B49D6B08615BBE824D0CACC9842B0E4CAF11C52CF2B043893
)

FetchContent_Declare(
    Eigen
    URL      https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.bz2
    URL_HASH SHA256=b4c198460eba6f28d34894e3a5710998818515104d6e74e5cc331ce31e46e626
)

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "" FORCE)
set(BUILD_TESTING OFF CACHE INTERNAL "" FORCE)
set(BUILD_PROGRAMS OFF CACHE INTERNAL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(EIGEN_BUILD_DOC OFF CACHE INTERNAL "" FORCE)
set(EIGEN_BUILD_PKGCONFIG OFF CACHE INTERNAL "" FORCE)
set(xmlch-type "char16_t" CACHE INTERNAL "" FORCE)

set(BUILD_TESTS OFF CACHE INTERNAL "")
set(ENABLE_THREADS OFF CACHE INTERNAL "")
set(ENABLE_SSE ON CACHE INTERNAL "")

FetchContent_MakeAvailable(fftw3 xerces-c libsndfile libsamplerate Eigen)

target_include_directories(fftw3 INTERFACE $<BUILD_INTERFACE:${fftw3_SOURCE_DIR}/api>)

target_include_directories(xerces-c INTERFACE
    $<BUILD_INTERFACE:${xerces-c_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${xerces-c_BINARY_DIR}/src>)
