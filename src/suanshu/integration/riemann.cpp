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

#include "riemann.h"

#include <stdexcept>

#include "iterative_integrator.h"
#include "romberg.h"
#include "simpson.h"

using namespace suanshu;

Riemann::Riemann(const double precision, const int maxIterations)
    : m_precision(precision), m_maxIterations(maxIterations) {}

double Riemann::integrate(const UnivariateRealFunction& f, const double a,
                          const double b) {
    double fa = f.evaluate(a);
    bool aOk = isNumber(fa);

    double fb = f.evaluate(b);
    bool bOk = isNumber(fb);

    IterativeIntegrator* integrator;
    if (aOk && bOk) {
        integrator = new Simpson(m_precision, std::min(m_maxIterations, 20));
    } else {
        throw std::invalid_argument(
            "cannot evaluate integral at endpoints; consider using change of "
            "variable");
    }

    Romberg romberg(integrator);
    const double result = romberg.integrate(f, a, b);
    delete integrator;

    return result;
}

double Riemann::getPrecision() const { return m_precision; }