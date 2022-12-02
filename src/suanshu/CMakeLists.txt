add_library(suanshu STATIC

    arima/fitting/BurgYule.cpp
    arima/fitting/BurgYule.h

    arima/fitting/ConditionalSumOfSquares.cpp
    arima/fitting/conditionalSumOfSquares.h

    arima/ARIMAModel.cpp
    arima/ARIMAModel.h
    arima/ARIMAXModel.cpp
    arima/ARIMAXModel.h
    arima/ARMAFitting.h
    arima/ARMAModel.cpp
    arima/ARMAModel.h
    arima/ARMAXModel.cpp
    arima/ARMAXModel.h

    function/Differentiation.h
    function/FiniteDifference.cpp
    function/FiniteDifference.h
    function/Function.h
    function/GradientFunction.cpp
    function/HessianFunction.cpp
    function/OrderedPairs.h

    integration/Integrator.h
    integration/IterativeIntegrator.h
    integration/NewtonCotes.cpp
    integration/NewtonCotes.h
    integration/Riemann.cpp
    integration/Riemann.h
    integration/Romberg.cpp
    integration/Romberg.h
    integration/Simpson.cpp
    integration/Simpson.h
    integration/Trapezoidal.cpp
    integration/Trapezoidal.h

    interpolation/NevilleTable.cpp
    interpolation/NevilleTable.h

    optimization/problem/C2OptimProblem.cpp
    optimization/problem/OptimProblem.h
    optimization/unconstrained/NelderMead.cpp
    optimization/unconstrained/NelderMead.h
)

target_link_libraries(suanshu PUBLIC Eigen3::Eigen)

target_compile_definitions(suanshu PRIVATE _USE_MATH_DEFINES)

target_include_directories(suanshu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})