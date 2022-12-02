#include "FiniteDifference.h"

using namespace suanshu;

FiniteDifference::FiniteDifference(const RealScalarFunction& f,
                                   const std::vector<int>& varIdx)
    : m_f(f), m_varIdx(varIdx) {
    for (int i = 0; i < varIdx.size(); ++i) {
        aassert(
            varIdx[i] >= 0 && varIdx[i] < f.dimensionOfDomain(),
            "invalid variable specification; order specifies variable indices");
    }
}

double FiniteDifference::evaluate(const Vector& x) {
    const double h = pow(std::numeric_limits<double>::epsilon(),
                         1.0 / (m_varIdx.size() + 1)) *
                     std::max(1e-1, x.norm());

    return evaluate(x, h);
}

double FiniteDifference::evaluate(const Vector& x, const double h) {
    return evaluateByRecursion(m_varIdx.size(), h, x);
}

double FiniteDifference::evaluateByRecursion(const int n, const double h,
                                             const Vector& z) {
    Vector x(z);

    // the base case; order == 1
    if (n == 1) {
        const int i = m_varIdx[0];
        x(i) = z(i) + h;
        const double fx_p = m_f.evaluate(x);  // f(x+h)
        x(i) -= 2 * h;
        const double fx_n = m_f.evaluate(x);        // f(x-h)
        const double dfdx = (fx_p - fx_n) / 2 / h;  // central difference
        return dfdx;
    }

    // order >= 2
    const int i = m_varIdx[n - 1];
    x(i) = z(i) + h;
    const double fx_p = evaluateByRecursion(n - 1, h, x);  // d(n-1){f(x+h)}
    x(i) -= 2 * h;
    const double fx_n = evaluateByRecursion(n - 1, h, x);  // d(n-1){f(x-h)}
    const double dfdx = (fx_p - fx_n) / 2 / h;             // central difference
    return dfdx;
}