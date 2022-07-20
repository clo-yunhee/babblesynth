add_library(suanshu STATIC

    arima/fitting/conditional_sum_of_squares.cpp
    arima/fitting/conditional_sum_of_squares.h

    arima/arima_model.cpp
    arima/arima_model.h
    arima/arimax_model.cpp
    arima/arimax_model.h
    arima/arma_fitting.h
    arima/arma_model.cpp
    arima/arma_model.h
    arima/armax_model.cpp
    arima/armax_model.h

    function/function.h

    integration/integrator.h
    integration/iterative_integrator.h
    integration/newton_cotes.cpp
    integration/newton_cotes.h
    integration/riemann.cpp
    integration/riemann.h
    integration/simpson.cpp
    integration/simpson.h
    integration/trapezoidal.cpp
    integration/trapezoidal.h
)

target_link_libraries(suanshu PRIVATE Eigen3::Eigen)

target_compile_definitions(suanshu PRIVATE _USE_MATH_DEFINES)

target_include_directories(suanshu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})