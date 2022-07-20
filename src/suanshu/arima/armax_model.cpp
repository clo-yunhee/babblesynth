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

#include "armax_model.h"

using namespace suanshu;

ARMAXModel::ARMAXModel(const double mu, const dvec& AR, const dvec& MA,
                       const dvec& psi, const double sigma)
    : ARIMAXModel(mu, AR, 0, MA, psi, sigma) {}

ARMAXModel::ARMAXModel(const dvec& AR, const dvec& MA, const dvec& psi,
                       const double sigma)
    : ARMAXModel(0, AR, MA, psi, sigma) {}

double ARMAXModel::armaxMeanNoIntercept(const dvec& arLags, const dvec& maLags,
                                        const dvec& exVar) const {
    return dotProduct(m_AR, arLags) + dotProduct(m_MA, maLags) +
           dotProduct(m_psi, exVar);
}

double ARMAXModel::armaxMean(const dvec& arLags, const dvec& maLags,
                             const dvec& exVar) const {
    return m_mu + armaxMeanNoIntercept(arLags, maLags, exVar);
}