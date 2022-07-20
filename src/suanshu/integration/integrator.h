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

#ifndef SUANSHU_INTEGRATION_INTEGRATOR_H
#define SUANSHU_INTEGRATION_INTEGRATOR_H

#include "function/function.h"

namespace suanshu {

class Integrator {
   public:
    // integrate f from a to b
    virtual double integrate(const UnivariateRealFunction& f, double a,
                             double b) = 0;

    // get the convergence threshold
    virtual double getPrecision() const = 0;
};

}  // namespace suanshu

#endif  // SUANSHU_INTEGRATION_INTEGRATOR_H