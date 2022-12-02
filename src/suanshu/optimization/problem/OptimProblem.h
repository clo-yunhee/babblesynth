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

#ifndef SUANSHU_OPTIMIZATION_PROBLEM_OPTIM_PROBLEM_H
#define SUANSHU_OPTIMIZATION_PROBLEM_OPTIM_PROBLEM_H

#include "function/Differentiation.h"

namespace suanshu {

class OptimProblem {
   public:
    virtual int dimension() const = 0;
    virtual RealScalarFunction f() const = 0;
};

class C2OptimProblem : public OptimProblem, public C2 {
   public:
    C2OptimProblem(const RealScalarFunction& f, const RealVectorFunction& g,
                   const RntoMatrix& H);

    C2OptimProblem(const RealScalarFunction& f, const RealVectorFunction& g);

    C2OptimProblem(const RealScalarFunction& f);

    int dimension() const override;

    RealScalarFunction f() const override;
    RealVectorFunction g() const override;
    RntoMatrix H() const override;

   private:
    RealScalarFunction m_f;
    RealVectorFunction m_g;
    RntoMatrix m_H;
};

}  // namespace suanshu

#endif  // SUANSHU_OPTIMIZATION_PROBLEM_OPTIM_PROBLEM_H