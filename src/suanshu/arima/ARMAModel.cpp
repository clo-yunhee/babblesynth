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

#include "ARMAModel.h"

using namespace suanshu;

ARMAModel::ARMAModel(const double mu, const dvec& AR, const dvec& MA,
                     const double sigma)
    : ARIMAModel(mu, AR, 0, MA, sigma) {}

ARMAModel::ARMAModel(const dvec& AR, const dvec& MA, const double sigma)
    : ARMAModel(0, AR, MA, sigma) {}

double ARMAModel::armaMeanNoIntercept(const dvec& arLags,
                                      const dvec& maLags) const {
    return dotProduct(m_AR, arLags) + dotProduct(m_MA, maLags);
}

double ARMAModel::armaMean(const dvec& arLags, const dvec& maLags) const {
    return m_mu + armaMeanNoIntercept(arLags, maLags);
}