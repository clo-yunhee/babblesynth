add_library(babblesynth OBJECT
    filter/butterworth.cpp
    filter/butterworth.h
    filter/formant_filter.cpp
    filter/formant_filter.h
    filter/lfilter.cpp
    filter/sos_filter.h
    filter/sosfilt.cpp
    filter/tf_filter.h
    filter/zpk2sos.cpp
    generator/noise.cpp
    generator/noise.h
    generator/source_generator.cpp
    generator/source_generator.h
    source/abstract_source.cpp
    source/abstract_source.h
    source/lf.cpp
    source/lf.h
    babblesynth.h
    enumeration.h
    parameter_holder.cpp
    parameter_holder.h
    parameter.h
    spline.h
    utility.h
    variable_plan.cpp
    variable_plan.h
    variable.cpp
    variable.h
)

target_compile_definitions(babblesynth PRIVATE _USE_MATH_DEFINES)

target_include_directories(babblesynth INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

#add_subdirectory(fast-akima-interpolation)
#target_link_libraries(babblesynth PRIVATE fast_akima)