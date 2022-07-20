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

#include "simpson.h"

using namespace suanshu;

Simpson::Simpson(const double precision, const int maxIterations)
    : m_trapezoidal(precision, maxIterations),
      m_precision(precision),
      m_maxIterations(maxIterations) {}

double Simpson::integrate(const UnivariateRealFunction& f, const double a,
                          const double b) {
    double sum0 = NAN;
    double sum1 = NAN;

    for (int iter = 1; iter <= m_maxIterations; ++iter) {
        sum0 = sum1;
        sum1 = next(iter, f, a, b, sum0);

        if ((iter > 3) && relativeError(sum1, sum0) < m_precision) {
            break;  // converged
        }
    }

    return sum1;
}

double Simpson::next(const int iteration, const UnivariateRealFunction& f,
                     const double a, const double b, const double sum) {
    if (iteration == 1) {
        m_t0 = m_trapezoidal.next(1, f, a, b, sum);
        m_t1 = m_trapezoidal.next(2, f, a, b, m_t0);
    } else {
        m_t0 = m_t1;
        m_t1 = m_trapezoidal.next(iteration + 1, f, a, b, m_t0);
    }

    const double result = (4 * m_t1 - m_t0) / 3;
    return result;
}

double Simpson::h() { return m_trapezoidal.h(); }

int Simpson::getMaxIterations() const { return m_maxIterations; }

double Simpson::getPrecision() const { return m_precision; }