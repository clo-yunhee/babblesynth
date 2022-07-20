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

#include "arima_model.h"

#include "arma_model.h"

using namespace suanshu;

ARIMAModel::ARIMAModel(const double mu, const dvec& AR, const int d,
                       const dvec& MA, const double sigma)
    : ARIMAXModel(mu, AR, d, MA, {}, sigma) {}

ARIMAModel::ARIMAModel(const dvec& AR, const int d, const dvec& MA,
                       const double sigma)
    : ARIMAModel(0, AR, d, MA, sigma) {}

ARMAModel* ARIMAModel::getArma() const {
    return new ARMAModel(m_mu, m_AR, m_MA, m_sigma);
}