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

#ifndef SUANSHU_FUNCTION_DIFFERENTIATION_H
#define SUANSHU_FUNCTION_DIFFERENTIATION_H

#include "FiniteDifference.h"
#include "Function.h"

namespace suanshu {

class C1 {
   public:
    virtual RealVectorFunction g() const = 0;
};

class C2 : public C1 {
   public:
    virtual RntoMatrix H() const = 0;
};

Vector Gradient(const RealScalarFunction& f, const Vector& x);
Matrix Hessian(const RealScalarFunction& f, const Vector& x);

RealVectorFunction GradientFunction(const RealScalarFunction& f);
RntoMatrix HessianFunction(const RealScalarFunction& f);

}  // namespace suanshu

#endif  // SUANSHU_FUNCTION_DIFFERENTIATION_H