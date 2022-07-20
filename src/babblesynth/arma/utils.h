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

#ifndef BABBLESYNTH_ARMA_UTILS_H
#define BABBLESYNTH_ARMA_UTILS_H

#include <Eigen/Dense>

namespace babblesynth {
namespace arma {

using Eigen::MatrixXd;
using Eigen::Ref;
using Eigen::VectorXd;

double msqe(const Ref<const VectorXd> &eps);

VectorXd ols(const Ref<const MatrixXd> &X, const Ref<const VectorXd> &y);

VectorXd shift(const Ref<const VectorXd> &array, int lags, double fill = 0.0);

MatrixXd shiftAndStack(const Ref<const VectorXd> &x, int numLags);

VectorXd autoCorr(const Ref<const VectorXd> &x, const int m);

VectorXd burg(const Ref<const VectorXd> &x, const int m);

}  // namespace arma
}  // namespace babblesynth

#endif  // BABBLESYNTH_ARMA_UTILS_H
