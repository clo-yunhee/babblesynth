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

#ifndef SUANSHU_ARIMA_ARMA_FITTING_H
#define SUANSHU_ARIMA_ARMA_FITTING_H

#include "ARMAModel.h"

namespace suanshu {

class ARMAFitting {
   public:
    // get the ARMA coefficients
    virtual ARMAModel getFittedARMA() const = 0;

    // get the variance of the white noise
    virtual double var() const = 0;

    // get the asymptotic standard errors of the estimators
    virtual Vector stdErr() const = 0;

    // get the asymptotic covariance matrix of the estimators
    virtual Matrix covariance() const = 0;

    // compute the AIC of model fitting
    virtual double AIC() const = 0;

    // compute the AICC of model fitting
    virtual double AICC() const = 0;

    // get string representation
    virtual std::string toString() const = 0;
};

}  // namespace suanshu

#endif  // SUANSHU_ARIMA_ARMA_FITTING_H