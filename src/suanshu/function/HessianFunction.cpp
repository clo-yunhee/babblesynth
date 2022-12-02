#include "Differentiation.h"

using namespace suanshu;

Matrix suanshu::Hessian(const RealScalarFunction& f, const Vector& x) {
    const int N = f.dimensionOfDomain();

    Matrix g(N, N);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            g(i, j) = FiniteDifference(f, {i, j}).evaluate(x);
        }
    }
    return g;
}

RntoMatrix suanshu::HessianFunction(const RealScalarFunction& f) {
    return RntoMatrix(f.dimensionOfDomain(), 1,
                      [&](const Vector& x) { return Hessian(f, x); });
}