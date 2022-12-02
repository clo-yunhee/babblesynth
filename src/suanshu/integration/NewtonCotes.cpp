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

#include "NewtonCotes.h"

using namespace suanshu;

NewtonCotes::NewtonCotes(const int rate, const Type type,
                         const double precision, const int maxIterations)
    : m_nAbcissas(1) {}

double NewtonCotes::integrate(const UnivariateRealFunction& f, const double a,
                              const double b) {
    double sum0, sum1(std::numeric_limits<double>::quiet_NaN());
    for (int iter = 1; iter <= m_maxIterations; ++iter) {
        sum0 = sum1;
        sum1 = next(iter, f, a, b, sum0);

        if ((iter > 3) && relativeError(sum1, sum0) < m_precision) {
            break;  // converged
        }
    }

    return sum1;
}

double NewtonCotes::next(const int iter, const UnivariateRealFunction& f,
                         const double a, const double b, const double sum0) {
    aassert(iter > 0, "iteration count must be positive");

    double sum = sum0;

    if (iter == 1) {
        m_h = (b - a);
        m_nAbcissas = 1;

        switch (m_type) {
            case CLOSED:  // use the two endpoints
                sum = 0.5 * m_h * (f.evaluate(a) + f.evaluate(b));
                break;
            case OPEN:  // use the midpoint
                sum = m_h * (f.evaluate((a + b) / 2));
                break;
        }
        return sum;
    } else {
        // summing up the interior abscissas
        m_h /= m_rate;
        m_nAbcissas *= m_rate;

        double addition = 0;
        double x = a;
        for (int i = 1; i < m_nAbcissas; ++i) {
            x += m_h;
            if ((i % m_rate) == 0) {
                continue;  // this abscissa is already computed in a previous
                           // iteration
            }

            addition += f.evaluate(x);
        }

        sum = sum0 / m_rate + m_h * addition;
    }

    return sum;
}

double NewtonCotes::h() { return m_h; }

int NewtonCotes::getMaxIterations() const { return m_maxIterations; }

double NewtonCotes::getPrecision() const { return m_precision; }