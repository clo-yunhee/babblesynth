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

#include "utils.h"

using namespace babblesynth;
using namespace babblesynth::arma;

using Eigen::last;
using Eigen::lastp1;
using Eigen::seq;

double arma::msqe(const Ref<const VectorXd>& eps) {
    return eps.cwiseAbs2().mean();
}

VectorXd arma::ols(const Ref<const MatrixXd>& X, const Ref<const VectorXd>& y) {
    return X.completeOrthogonalDecomposition().solve(y);
}

VectorXd arma::shift(const Ref<const VectorXd>& array, const int lags,
                     const double fill) {
    VectorXd result = VectorXd::Constant(array.size(), fill);

    if (lags > 0) {
        const int subLength = result.size() - lags;
        result.tail(subLength) = array.head(subLength);
    } else if (lags < 0) {
        const int subLength = -lags;
        result.head(subLength) = array.tail(lags);
    } else {
        result = array;
    }

    return result;
}

MatrixXd arma::shiftAndStack(const Ref<const VectorXd>& x, const int numLags) {
    MatrixXd result = MatrixXd::Zero(x.size(), numLags);

    for (int i = 0; i < numLags; ++i) {
        result.col(i) = shift(x, i + 1);
    }

    return result;
}

VectorXd arma::autoCorr(const Ref<const VectorXd>& x, const int m) {
    const int n = x.size();

    VectorXd aut(m + 1);
    VectorXd lpc(m);
    double error;
    double epsilon;
    int i, j;

    /* autocorrelation, p+1 lag coefficients */
    j = m + 1;
    while (j--) {
        double d = 0; /*double needed for accumulator depth*/
        for (i = j; i < n; ++i) d += x(i) * x(i - j);
        aut(j) = d;
    }

    /* Generate lpc coefficients from autocorr values */

    /* set our noise floor to about -100dB */
    error = aut(0) * (1 + 1e-10);
    epsilon = 1e-9 * aut(0) + 1e-10;

    for (i = 0; i < m; ++i) {
        double r = -aut(i + 1);

        if (error < epsilon) {
            lpc.tail(m - i).setZero();
            break;
        }

        for (j = 0; j < i; ++j) r -= lpc(j) * aut(i - j);
        r /= error;

        /* Update LPC coefficients and total error */

        lpc(i) = r;
        for (j = 0; j < i / 2; ++j) {
            double tmp = lpc(j);

            lpc(j) += r * lpc(i - 1 - j);
            lpc(i - 1 - j) += r * tmp;
        }
        if (i & 1) lpc(j) += lpc(j) * r;

        error *= 1 - r * r;
    }

    double g = 0.99;
    double damp = g;
    for (j = 0; j < m; ++j) {
        lpc[j] *= damp;
        damp *= g;
    }

    return lpc;
}

static VectorXd detailBurg(const Ref<const VectorXd>& x, const int m,
                           double* xms) {
    const int n = x.size();

    VectorXd b1(n + 1);
    VectorXd b2(n + 1);
    VectorXd aa(m + 1);

    VectorXd a(m + 1);

    int i, j;

    double p = 0.0;
    for (j = 1; j <= n; ++j) p += x(j - 1) * x(j - 1);

    *xms = p / n;
    if (*xms <= 0.0) {
        return a;
    }

    b1(1) = x(0);
    b2(n - 1) = x(n - 1);
    for (j = 2; j <= n - 1; ++j) b1(j) = b2(j - 1) = x(j - 1);

    for (i = 1; i <= m; ++i) {
        double num = 0.0, denum = 0.0;
        for (j = 1; j <= n - i; ++j) {
            num += b1(j) * b2(j);
            denum += b1(j) * b1(j) + b2(j) * b2(j);
        }

        if (denum <= 0.0) {
            *xms = 0.0;
            return a;
        }

        a(i) = 2.0 * num / denum;

        *xms *= 1.0 - a(i) * a(i);

        for (j = 1; j <= i - 1; ++j) a(j) = aa(j) - a(i) * aa(i - j);

        if (i < m) {
            for (j = 1; j <= i; ++j) aa(j) = a(j);
            for (j = 1; j <= n - i - 1; ++j) {
                b1(j) -= aa(i) * b2(j);
                b2(j) = b2(j + 1) - aa(i) * b1(j + 1);
            }
        }
    }

    return a;
}

VectorXd arma::burg(const Ref<const VectorXd>& x, const int m) {
    double xms;
    VectorXd lpca = detailBurg(x, m, &xms);

    if (xms <= 0) {
        lpca.setZero();
    }

    return -lpca.tail(m);
}