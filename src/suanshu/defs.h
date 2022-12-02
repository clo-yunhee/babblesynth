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

#ifndef SUANSHU_DEFS_H
#define SUANSHU_DEFS_H

#include <Eigen/Dense>
#include <stdexcept>
#include <vector>

namespace suanshu {

using dvec = std::vector<double>;
using dvec2 = std::vector<dvec>;

using Vector = Eigen::VectorXd;
using Matrix = Eigen::MatrixXd;

constexpr double EPSILON = std::numeric_limits<double>::epsilon();

inline void aassert(const bool cond, const char* msg) {
#ifndef NDEBUG
    if (!cond) throw std::logic_error(msg);
#endif
}

inline std::vector<double> stlVector(const Vector& vec) {
    return std::vector<double>(vec.begin(), vec.end());
}

inline bool isNumber(const double x) {
    return !std::isinf(x) && !std::isnan(x);
}

inline double relativeError(const double x1, const double x0) {
    return fabs(x1 / x0 - 1);
}

template <typename T>
T dotProduct(const std::vector<T>& x, const std::vector<T>& y) {
    aassert(x.size() == y.size(), "unmatched vector lengths");

    T fx(0);
    for (int i = 0; i < x.size(); ++i) {
        fx += x[i] * y[i];
    }
    return fx;
}

template <typename T>
std::vector<T> diff(const std::vector<T>& arr, int lag = 1, int order = 1) {
    aassert(arr.size() > 0, "array length > 0");
    aassert(lag >= 1, "lag >= 1");
    aassert(order >= 1, "order >= 1");

    std::vector<T> result(arr.size() - lag);
    for (int j = 0, i = lag; i < arr.size(); ++j, ++i) {
        result[j] = arr[i] - arr[j];
    }

    if (order > 1) {  // recursion
        return diff(result, lag, order - 1);
    }
    return result;
}

}  // namespace suanshu

#endif  // SUANSHU_DEFS_H