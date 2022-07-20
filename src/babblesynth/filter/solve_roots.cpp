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

#include <iostream>
#include <random>

#include "filters.h"

static std::random_device rd;
static std::mt19937 gen(rd());

using namespace babblesynth;

static std::pair<double, double> upperLowerBounds(
    const std::vector<double>& P) {
    const int degree = (int)P.size() - 1;

    double maxUpper = 0;
    double maxLower = 0;
    for (int i = 0; i < degree + 1; ++i) {
        const double absPi = std::abs(P[i]);
        if (i >= 1 && absPi > maxUpper) maxUpper = absPi;
        if (i < degree && absPi > maxLower) maxLower = absPi;
    }

    const double upper = 1 + 1 / std::abs(P[0]) * maxUpper;
    const double lower = std::abs(P[degree]) / (std::abs(P[degree]) + maxLower);

    return std::make_pair(upper, lower);
}

static std::vector<std::complex<double>> initRoots(
    const std::vector<double>& P) {
    const int degree = (int)P.size() - 1;
    const auto [upper, lower] = upperLowerBounds(P);

    static std::uniform_real_distribution<> radius(lower, upper);
    static std::uniform_real_distribution<> angle(0, 2 * M_PI);

    std::vector<std::complex<double>> roots;
    for (int i = 0; i < degree; ++i) {
        roots.push_back(std::polar(radius(gen), angle(gen)));
    }

    return roots;
}

static std::vector<std::complex<double>> evaluatePolyDer(
    const std::vector<double>& P, const std::complex<double>& x,
    int numberOfDerivatives) {
    const int degree = (int)P.size() - 1;
    std::vector<std::complex<double>> derivatives(numberOfDerivatives + 1);
    numberOfDerivatives =
        numberOfDerivatives > degree ? degree : numberOfDerivatives;

    derivatives[0] = P[0];

    for (int i = 1; i <= degree; ++i) {
        const int n = numberOfDerivatives < i ? numberOfDerivatives : i;
        for (int j = n; j >= 1; --j) {
            derivatives[j] = derivatives[j] * x + derivatives[j - 1];
        }
        derivatives[0] = derivatives[0] * x + P[i];
    }

    double fact = 1.0;
    for (int j = 2; j <= numberOfDerivatives; ++j) {
        fact *= j;
        derivatives[j] *= fact;
    }
    return derivatives;
}

static void aberthIterate(const std::vector<double>& P,
                          std::vector<std::complex<double>>& roots) {
    int iteration = 0;

    while (true) {
        int valid = 0;
        for (int k = 0; k < (int)roots.size(); ++k) {
            auto y = evaluatePolyDer(P, roots[k], 1);
            auto ratio = y[0] / y[1];

            std::complex<double> sum = 0.0;
            for (int j = 0; j < (int)roots.size(); ++j) {
                if (j != k) {
                    sum += 1.0 / (roots[k] - roots[j]);
                }
            }

            auto offset = ratio / (1.0 - ratio * sum);
            if (std::abs(offset.real()) < 1e-10 &&
                std::abs(offset.imag()) < 1e-10) {
                valid++;
            }
            roots[k] -= offset;
        }
        if (valid == (int)roots.size()) {
            break;
        }
        if (iteration > 50) {
            std::cout << "aberth: didn't converge fast enough" << std::endl;
            break;
        }
        iteration++;
    }
}

std::vector<std::complex<double>> filter::solveRoots(
    const std::vector<double>& P) {
    if (P.size() == 1) {
        return {};
    }

    auto roots = initRoots(P);

    aberthIterate(P, roots);

    return roots;
}