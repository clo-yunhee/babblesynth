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

#ifndef SUANSHU_INTEGRATION_ROMBERG_H
#define SUANSHU_INTEGRATION_ROMBERG_H

#include "IterativeIntegrator.h"
#include "interpolation/NevilleTable.h"

namespace suanshu {

class Romberg : public Integrator {
   public:
    Romberg(IterativeIntegrator* integrator);

    double integrate(const UnivariateRealFunction& f, double a,
                     double b) override;

    double getPrecision() const override;

   private:
    double addPointToExtrapolation(double h, double sum);
    double getLastExtrapolationPoint();

    IterativeIntegrator* m_integrator;
    NevilleTable m_neville;
};

}  // namespace suanshu

#endif  // SUANSHU_INTEGRATION_ROMBERG_H