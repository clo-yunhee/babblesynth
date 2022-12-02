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

#ifndef SUANSHU_INTEGRATION_ITERATIVE_INTEGRATOR_H
#define SUANSHU_INTEGRATION_ITERATIVE_INTEGRATOR_H

#include "Integrator.h"

namespace suanshu {

class IterativeIntegrator : public Integrator {
   public:
    // get the discretization size for the current iteration
    virtual double h() = 0;

    // compute a refined sum for the integral
    virtual double next(int iteration, const UnivariateRealFunction& f,
                        double a, double b, double sum0) = 0;

    // get the maximum number of iterations for this iterative procedure
    virtual int getMaxIterations() const = 0;
};

}  // namespace suanshu

#endif  // SUANSHU_INTEGRATION_ITERATIVE_INTEGRATOR_H