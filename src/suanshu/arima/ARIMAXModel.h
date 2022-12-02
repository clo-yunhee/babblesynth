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

#ifndef SUANSHU_ARIMA_ARIMAX_MODEL_H
#define SUANSHU_ARIMA_ARIMAX_MODEL_H

#include <ostream>
#include <vector>

#include "defs.h"

namespace suanshu {

class ARMAXModel;

class ARIMAXModel {
   public:
    ARIMAXModel(double mu, const dvec& AR, int d, const dvec& MA,
                const dvec& psi, double sigma = 1);

    ARIMAXModel(const dvec& AR, int d, const dvec& MA, const dvec& psi,
                double sigma = 1);

    ARIMAXModel(const ARIMAXModel&) = default;

    // the intercept (constant) term
    double mu() const { return m_mu; }

    // the i-th AR coefficient; AR(0) = 1
    double AR(const int i) const { return i == 0 ? 1 : m_AR.at(i - 1); }

    // the AR coefficients, excluding the initial 1
    const dvec& AR() const { return m_AR; }

    // the i-th MA coefficient; MA(0) = 1
    double MA(const int i) const { return i == 0 ? 1 : m_MA.at(i - 1); }

    // the MA coefficients, excluding the initial 1
    const dvec& MA() const { return m_MA; }

    // the coefficients of the deterministic terms
    const dvec& psi() const { return m_psi; }

    // the order of integration
    int d() const { return m_d; }

    // the number of AR terms
    int p() const { return m_AR.size(); }

    // the number of MA terms
    int q() const { return m_MA.size(); }

    // the white noise variance
    double sigma() const { return m_sigma; }

    // get the ARMAX specification of this ARIMAX model,
    // essentially ignoring the differencing.
    ARMAXModel* getArmax() const;

    // get a string representation of the model.
    std::string toString() const;

   protected:
    // the intercept (constant) term
    double m_mu;

    // the AR coefficients, excluding the initial 1
    dvec m_AR;

    // the MA coefficients, excluding the initial 1
    dvec m_MA;

    // the coefficients of the deterministic terms
    // (excluding the intercept term)
    dvec m_psi;

    // the order of integration
    int m_d;

    // the white noise variance
    double m_sigma;
};

}  // namespace suanshu

std::ostream& operator<<(std::ostream&, const suanshu::ARIMAXModel&);

#endif  // SUANSHU_ARIMA_ARIMAX_MODEL_H