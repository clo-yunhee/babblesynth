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

#include "fit.h"

#include <iostream>

#include "utils.h"

using namespace babblesynth;
using namespace babblesynth::arma;

inline Eigen::Map<const VectorXd> map(const std::vector<double>& vec) {
    return Eigen::Map<const VectorXd>(vec.data(), vec.size());
}

inline std::vector<double> map(const VectorXd& vec) {
    return std::vector<double>(vec.begin(), vec.end());
}

inline VectorXd sumBetaArg(const VectorXd& beta, const MatrixXd& arg) {
    // arg -> (N, terms)
    // beta -> (terms, 1)

    // std::cout << "arg = " << arg.rows() << "x" << arg.cols() << std::endl;
    // std::cout << "beta = " << beta.rows() << "x" << beta.cols() << std::endl;

    // beta.T -> (1, terms)

    // (N, terms)

    return (arg.array().rowwise() * beta.transpose().array())
        .rowwise()
        .sum()
        .matrix();
}

arma::model arma::fit(const std::vector<double>& vx, const int arTerms,
                      const int maTerms, const double epsLimit,
                      const int maxSteps) {
    VectorXd ar(arTerms);
    VectorXd ma(maTerms);

    if (arTerms == 0 && maTerms == 0) {
        // Pass.
    } else if (arTerms == 0) {
        ma = map(fitMaOnly(vx, maTerms, epsLimit, maxSteps));
    } else if (maTerms == 0) {
        ar = map(fitArOnly(vx, arTerms));
    } else {
        auto x = map(vx);
        const int N = x.size();

        MatrixXd arg, arg2;
        MatrixXd argAll(N, arTerms + maTerms);
        VectorXd initialBeta;
        VectorXd beta;
        VectorXd eps;
        double q0, q1;

        arg = shiftAndStack(x, arTerms);
        initialBeta = ols(arg, x);
        eps = x - sumBetaArg(initialBeta, arg);
        q0 = msqe(eps);

        for (int i = 0; i < maxSteps; ++i) {
            arg = shiftAndStack(x, arTerms);
            arg2 = shiftAndStack(eps, maTerms);
            argAll << arg, arg2;
            beta = ols(argAll, x);
            eps = x - sumBetaArg(beta, argAll);
            q1 = msqe(eps);
            if (epsLimit > std::abs(q1 - q0)) {
                break;
            }
            q0 = q1;
            if (i == maxSteps - 1) {
                std::cout << "Warning: Failed to converge within " << maxSteps
                          << " steps" << std::endl;
            }
        }

        ar = beta.head(arTerms).reverse();
        ma = beta.tail(maTerms).reverse();
    }

    VectorXd arPoly(arTerms + 1);
    arPoly(0) = 1;
    arPoly.tail(arTerms) = ar;

    VectorXd maPoly(maTerms + 1);
    maPoly(0) = 1;
    maPoly.tail(maTerms) = ma;

    return {map(arPoly), map(maPoly)};
}

std::vector<double> arma::fitMaOnly(const std::vector<double>& vx,
                                    const int maTerms, const double epsLimit,
                                    const int maxSteps) {
    auto x = map(vx);

    MatrixXd arg;
    VectorXd beta;
    VectorXd eps;
    double q0, q1;

    eps = x;
    q0 = msqe(eps);

    for (int i = 0; i < maxSteps; ++i) {
        arg = shiftAndStack(eps, maTerms);
        beta = ols(arg, x);
        eps = x - sumBetaArg(beta, arg);
        q1 = msqe(eps);
        if (epsLimit > std::abs(q1 - q0)) {
            break;
        }
        q0 = q1;
        if (i == maxSteps - 1) {
            std::cout << "Warning: Failed to converge within " << maxSteps
                      << " steps" << std::endl;
        }
    }

    return map(beta);
}

std::vector<double> arma::fitArOnly(const std::vector<double>& vx,
                                    const int arTerms) {
    auto x = map(vx);

    // VectorXd coefs = autoCorr(x, arTerms);
    VectorXd coefs = burg(x, arTerms);

    return map(coefs);
}