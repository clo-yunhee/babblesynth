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

#include "OptimProblem.h"

using namespace suanshu;

C2OptimProblem::C2OptimProblem(const RealScalarFunction& f,
                               const RealVectorFunction& g, const RntoMatrix& H)
    : m_f(f), m_g(g), m_H(H) {
    aassert(f.dimensionOfDomain() == g.dimensionOfDomain(),
            "objective function and gradient must have the same domain "
            "dimension");
    aassert(f.dimensionOfDomain() == H.dimensionOfDomain(),
            "objective function and Hessian must have the same domain "
            "dimension");
}

C2OptimProblem::C2OptimProblem(const RealScalarFunction& f,
                               const RealVectorFunction& g)
    : C2OptimProblem(f, g, HessianFunction(f)) {}

C2OptimProblem::C2OptimProblem(const RealScalarFunction& f)
    : C2OptimProblem(f, GradientFunction(f)) {}

int C2OptimProblem::dimension() const { return m_f.dimensionOfDomain(); }

RealScalarFunction C2OptimProblem::f() const { return m_f; }

RealVectorFunction C2OptimProblem::g() const { return m_g; }

RntoMatrix C2OptimProblem::H() const { return m_H; }