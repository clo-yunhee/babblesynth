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

#include "ARIMAModel.h"

namespace suanshu {

class ARMAModel : public ARIMAModel {
   public:
    ARMAModel(double mu, const dvec& AR, const dvec& MA, double sigma = 1);

    ARMAModel(const dvec& AR, const dvec& MA, double sigma = 1);

    ARMAModel(const ARMAModel&) = default;

    // compute the zero-intercept (mu) ARMA conditional mean
    double armaMeanNoIntercept(const dvec& arLags, const dvec& maLags) const;

    // compute the ARMA conditional mean
    double armaMean(const dvec& arLags, const dvec& maLags) const;
};

}  // namespace suanshu

#endif  // SUANSHU_ARIMA_ARMAX_MODEL_H