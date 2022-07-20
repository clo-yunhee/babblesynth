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

#ifndef SUANSHU_ARIMA_ARMAX_MODEL_H
#define SUANSHU_ARIMA_ARMAX_MODEL_H

#include "arimax_model.h"

namespace suanshu {

class ARMAXModel : public ARIMAXModel {
   public:
    ARMAXModel(double mu, const dvec& AR, const dvec& MA, const dvec& psi,
               double sigma = 1);

    ARMAXModel(const dvec& AR, const dvec& MA, const dvec& psi,
               double sigma = 1);

    ARMAXModel(const ARMAXModel&) = default;

    // compute the zero-intercept (mu) ARMAX conditional mean
    double armaxMeanNoIntercept(const dvec& arLags, const dvec& maLags,
                                const dvec& exVar) const;

    // compute the ARMAX conditional mean
    double armaxMean(const dvec& arLags, const dvec& maLags,
                     const dvec& exVar) const;
};

}  // namespace suanshu

#endif  // SUANSHU_ARIMA_ARMAX_MODEL_H