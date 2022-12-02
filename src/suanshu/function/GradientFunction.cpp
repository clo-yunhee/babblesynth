#include "Differentiation.h"

using namespace suanshu;

Vector suanshu::Gradient(const RealScalarFunction& f, const Vector& x) {
    const int N = f.dimensionOfDomain();

    Vector g(N);
    for (int i = 0; i < N; ++i) {
        g(i) = FiniteDifference(f, {i}).evaluate(x);
    }
    return g;
}

RealVectorFunction suanshu::GradientFunction(const RealScalarFunction& f) {
    return RealVectorFunction(f.dimensionOfDomain(), f.dimensionOfDomain(),
                              [&](const Vector& x) { return Gradient(f, x); });
}