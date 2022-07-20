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

#ifndef SUANSHU_FUNCTION_FUNCTION_H
#define SUANSHU_FUNCTION_FUNCTION_H

#include <functional>

#include "defs.h"

namespace suanshu {

template <typename D, typename R>
class Function {
   public:
    using fn_type = std::function<D(R)>;

    template <typename F>
    Function(int nD, int nR, F fn) : m_nD(nD), m_nR(nR), m_fn(fn) {}

    R evaluate(D x) const { return m_fn(x); }
    int dimensionOfDomain() const { return m_nD; }
    int dimensionOfRange() const { return m_nR; }

   private:
    int m_nD;
    int m_nR;
    fn_type m_fn;
};

using RealScalarFunction = Function<Vector, double>;
using UnivariateRealFunction = Function<double, double>;

}  // namespace suanshu

#endif  // SUANSHU_FUNCTION_FUNCTION_H