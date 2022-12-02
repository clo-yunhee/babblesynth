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

#ifndef SUANSHU_FUNCTION_FINITE_DIFFERENCE_H
#define SUANSHU_FUNCTION_FINITE_DIFFERENCE_H

#include "Function.h"

namespace suanshu {

class FiniteDifference {
   public:
    FiniteDifference(const RealScalarFunction& f,
                     const std::vector<int>& varIdx);

    double evaluate(const Vector& x);
    double evaluate(const Vector& x, double h);

   private:
    double evaluateByRecursion(int n, double h, const Vector& z);

    RealScalarFunction m_f;
    std::vector<int> m_varIdx;
};

}  // namespace suanshu

#endif  // SUANSHU_FUNCTION_FINITE_DIFFERENCE_H