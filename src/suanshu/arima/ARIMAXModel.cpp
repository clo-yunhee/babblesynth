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

#include "ARIMAXModel.h"

#include <sstream>

#include "ARMAXModel.h"

using namespace suanshu;

ARIMAXModel::ARIMAXModel(const double mu, const dvec& AR, const int d,
                         const dvec& MA, const dvec& psi, const double sigma) {
    aassert(!(AR.empty() && MA.empty()),
            "At least one of AR or MA terms must be present");
    aassert(d >= 0, "d >= 0");
    aassert(sigma >= 0, "sigma >= 0");

    m_mu = mu;
    m_AR = AR;
    m_MA = MA;
    m_psi = psi;
    m_d = d;
    m_sigma = sigma;
}

ARIMAXModel::ARIMAXModel(const dvec& AR, const int d, const dvec& MA,
                         const dvec& psi, const double sigma)
    : ARIMAXModel(0, AR, d, MA, psi, sigma) {}

std::string ARIMAXModel::toString() const {
    std::stringstream ss;

    ss << "Xt = " << m_mu;

    ss << " + (";
    for (int i = 0; i < m_AR.size(); ++i) {
        ss << m_AR[i] << "*X_{t-" << (i + 1) << "}";
    }
    ss << ") ";

    ss << " + (";
    for (int i = 1; i < m_MA.size(); ++i) {
        ss << m_MA[i] << "*e_{t-" << (i + 1) << "}";
    }
    ss << ") ";

    ss << " + (";
    for (int i = 0; i < m_psi.size(); ++i) {
        ss << m_psi[i] << "*D_{t-" << i << "}";
    }
    ss << ") ";

    ss << " + e_t";

    ss << "; var(e_t) = " << m_sigma << ", d = " << m_d;

    return ss.str();
}

ARMAXModel* ARIMAXModel::getArmax() const {
    return new ARMAXModel(m_mu, m_AR, m_MA, m_psi, m_sigma);
}

std::ostream& operator<<(std::ostream& os, const ARIMAXModel& model) {
    return os << model.toString();
}