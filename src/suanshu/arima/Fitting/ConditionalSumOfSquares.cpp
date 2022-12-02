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

#include "ConditionalSumOfSquares.h"

#include "integration/Riemann.h"
#include "optimization/unconstrained/NelderMead.h"

using namespace suanshu;

ConditionalSumOfSquares::ConditionalSumOfSquares(const dvec& xt, const int p,
                                                 const int d, const int q) {
    n = xt.size();

    // make stationary (ARMA) by differencing when d > 0
    dvec dxt0 = xt;
    if (d > 0) {
        dxt0 = diff(dxt0, 1, d);
    }

    // calculate mean
    mu = 0;
    for (int i = 0; i < dxt0.size(); ++i) {
        mu += dxt0[i];
    }
    mu /= dxt0.size();

    // demean
    dvec dxt1(dxt0.size());
    for (int i = 0; i < dxt0.size(); ++i) {
        dxt1[i] = dxt0[i] - mu;
    }

    RealScalarFunction nL = nLogLikelihood(p, q, dxt1);

    NelderMead optim(0, 500);
    NelderMeadSolution soln = optim.solve(C2OptimProblem(nL));

    Vector xmin = Vector::Zero(p + q + 1);  // initial AR, MA are 0
    xmin(p + q) = 0.01;                     // initial var is very small
    xmin = soln.search({xmin});

    maxLikelihood = -1 * nL.evaluate(xmin);
    estimators = Estimators(xmin, p, q);
}

ARMAModel ConditionalSumOfSquares::getFittedARMA() const {
    double intercept = 1;
    for (const auto& x : estimators.phi) {
        intercept -= x;
    }
    intercept *= mu;

    return ARMAModel(intercept, estimators.phi, estimators.theta,
                     estimators.var);
}

double ConditionalSumOfSquares::var() const { return estimators.var; }

Matrix ConditionalSumOfSquares::covariance() const {
    const int p = estimators.p();
    const int q = estimators.q();
    const int pq = p + q;

    Riemann I;
    Matrix cov(pq, pq);

    for (int j = 0; j < pq; ++j) {
        for (int k = j; k < pq; ++k) {
            const auto dgdj = dlogg(j);
            const auto dgdk = dlogg(k);

            UnivariateRealFunction integrand(
                1, 1, [&dgdj, &dgdk](double x) -> double {
                    return dgdj.evaluate(x) * dgdk.evaluate(x);
                });

            const double Wjk = I.integrate(integrand, -M_PI, M_PI) / (4 * M_PI);

            cov(j, k) = Wjk;
            cov(k, j) = Wjk;
        }
    }

    return cov.fullPivLu().inverse() / n;
}

Vector ConditionalSumOfSquares::stdErr() const {
    int pq = estimators.p() + estimators.q();

    Matrix cov = covariance();
    Vector stdErr(pq);

    for (int i = 0; i < pq; ++i) {
        stdErr(i) = sqrt(cov(i, i));
    }
    return stdErr;
}

int ConditionalSumOfSquares::nParams() const {
    return estimators.p() + estimators.q() + 1;
}

double ConditionalSumOfSquares::AIC() const {
    const double logL = log(maxLikelihood);
    return -2.0 * logL + 2.0 * nParams();
}

double ConditionalSumOfSquares::AICC() const {
    const double logL = log(maxLikelihood);
    return -2.0 * log(maxLikelihood) +
           2.0 * nParams() * n / (n - nParams() - 1);
}

std::string ConditionalSumOfSquares::toString() const {
    return getFittedARMA().toString();
}

RealScalarFunction ConditionalSumOfSquares::nLogLikelihood(const int p,
                                                           const int q,
                                                           const dvec& arma) {
    const int maxPQ = std::max(p, q);
    const int length = arma.size();

    // CSS assumes that the unobserved past observations are 0 with size = maxPQ
    dvec xtExtended(length + maxPQ, 0.0);
    std::copy(arma.begin(), arma.end(), xtExtended.begin() + maxPQ);

    const int lengthExtended = xtExtended.size();

    return RealScalarFunction(p + q + 1, 1, [=](Vector params) -> double {
        Estimators estimators(params, p, q);
        const double var = estimators.var;
        if (var < 0) {
            return DBL_MAX;
        }

        // compute the random error terms / innovations / white noise
        dvec Z(lengthExtended);
        for (int i = 0; i < length; ++i) {
            int t = maxPQ + i;

            dvec x_lag(p);
            dvec z_lag(q);

            for (int j = 0; j < p; ++j) {
                x_lag[j] = xtExtended[t - j - 1];
            }

            for (int j = 0; j < q; ++j) {
                z_lag[j] = Z[t - j - 1];
            }

            const double Xt_hat = dotProduct(estimators.phi, x_lag) +
                                  dotProduct(estimators.theta, z_lag);
            Z[t] = xtExtended[t] - Xt_hat;
        }

        const double CSS = dotProduct(Z, Z);  // the conditional sum of squares
        const double nL = length * log(var) + CSS / var;
        return nL;
    });
}

UnivariateRealFunction ConditionalSumOfSquares::dlogg(const int j) const {
    const dvec& coeff = j < estimators.p() ? estimators.phi : estimators.theta;

    return UnivariateRealFunction(1, 1, [=](double x) -> double {
        double numerator = 2.0 * cos((j + 1) * x);
        for (int r = 1; r <= coeff.size(); ++r) {
            numerator -= 2.0 * coeff[r - 1] * cos((j + 1) * x - x);
        }

        double denominator = 1;
        for (int r = 0; r < coeff.size(); ++r) {
            denominator += coeff[r] * coeff[r];
            denominator -= 2.0 * coeff[r] * cos(x);
        }

        for (int r = 1; r <= coeff.size(); ++r) {
            for (int s = r + 1; s <= coeff.size(); ++s) {
                denominator +=
                    2.0 * coeff[r - 1] * coeff[s - 1] * cos((r - s) * s);
            }
        }

        return numerator / denominator;
    });
}