/*
 * BabbleSynth
 * Copyright (C) 2022  Clo Yun-Hee Dufour
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Romberg.h"

using namespace suanshu;

Romberg::Romberg(IterativeIntegrator* integrator) : m_integrator(integrator) {
    aassert(integrator->getMaxIterations() >= 2,
            "the number of iterations must be > 2 for extrapolation");
}

double Romberg::integrate(const UnivariateRealFunction& f, double a, double b) {
    double sum(0);
    double sum0(0), sum1(0);

    for (int i = 1; i <= m_integrator->getMaxIterations(); ++i) {
        sum = m_integrator->next(i, f, a, b, sum);
        const double h = m_integrator->h();

        sum1 = addPointToExtrapolation(h, sum);
        sum0 = getLastExtrapolationPoint();

        if ((i > 3)  // avoid spurious convergence by doing at least a few iters
            && relativeError(sum1, sum0) < getPrecision()) {
            break;  // converged
        }
    }

    return sum1;
}

double Romberg::getPrecision() const { return m_integrator->getPrecision(); }

double Romberg::addPointToExtrapolation(const double h, const double sum) {
    m_neville.addData({{h}, {sum}});
    if (m_neville.N() < 2) return std::numeric_limits<double>::quiet_NaN();
    return m_neville.evaluate(0);
}

double Romberg::getLastExtrapolationPoint() {
    const int N = m_neville.N();
    if (N < 2) return std::numeric_limits<double>::quiet_NaN();
    return m_neville.get(0, N - 2);
}