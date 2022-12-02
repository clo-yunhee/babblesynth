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

#ifndef SUANSHU_ARIMA_FITTING_CONDITIONAL_SUM_OF_SQUARES_H
#define SUANSHU_ARIMA_FITTING_CONDITIONAL_SUM_OF_SQUARES_H

#include "arima/ARMAFitting.h"
#include "function/Function.h"

namespace suanshu {

class ConditionalSumOfSquares : public ARMAFitting {
   public:
    ConditionalSumOfSquares(const dvec& xt, int p, int d, int q);

    ARMAModel getFittedARMA() const override;

    double var() const override;
    Vector stdErr() const;
    Matrix covariance() const override;

    double AIC() const override;
    double AICC() const override;
    std::string toString() const override;

    int nParams() const;

   private:
    struct Estimators {
        dvec phi;
        dvec theta;
        double var;

        template <typename Container>
        Estimators(const Container& coefficients, int p, int q)
            : phi(coefficients.begin(), coefficients.begin() + p),
              theta(coefficients.begin() + p, coefficients.begin() + p + q),
              var(coefficients[p + q]) {}

        Estimators() : var(std::numeric_limits<double>::quiet_NaN()) {}

        int p() const { return phi.size(); }
        int q() const { return theta.size(); }
    };

    // the estimated mean of the model
    double mu;

    // the estimated ARMA model and coefficients
    // and variance for the white noise
    Estimators estimators;

    // the length of the observations
    int n;

    // the maximum likelihood
    double maxLikelihood;

    static RealScalarFunction nLogLikelihood(int p, int q, const dvec& arma);

    UnivariateRealFunction dlogg(int j) const;
};

}  // namespace suanshu

#endif  // SUANSHU_ARIMA_FITTING_CONDITIONAL_SUM_OF_SQUARES_H