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

#include "lf.h"

#include "../fzero.h"

using namespace babblesynth::source;

template <typename Func, typename Func2>
static double oldfzero(const Func& f, const Func2& df, double x0,
                       const double tol = 1e-7, const double eps = 1e-13,
                       const int maxIter = 50) {
    for (int iter = 0; iter < maxIter; ++iter) {
        double y = f(x0);
        double dy = df(x0);
        if (std::abs(dy) < eps) return x0;
        double x1 = x0 - y / dy;
        if (std::abs(x1 - x0) <= tol) return x1;
        x0 = x1;
    }
    return x0;
}

lf::lf() : abstract_source(), Oq(0.6), am(0.7), Qa(0.1) {
    addParameter("Oq", 0.6);
    addParameter("am", 0.7).setMin(0.5).setMax(1);
    addParameter("Qa", 0.1);
    calculateModelParameters();
}

double lf::evaluateAtPhase(double theta) {
    const double t = theta / (2 * M_PI);

    if (t <= Te) {
        return -Ee * exp(alpha * (t - Te)) * sin(M_PI * t / Tp) /
               sin(M_PI * Te / Tp);
    } else {
        return -Ee / (epsilon * Ta) *
               (exp(-epsilon * (t - Te)) - exp(-epsilon * (T0 - Te)));
    }
}

bool lf::calculateModelParameters() {
    Ee = E;
    Te = Oq * T0;
    Tp = (am > 0.5 ? am : 0.5001) * Oq * T0;
    Ta = Qa * (1 - Oq) * T0;

    // epsilon is expressed by an implicit equation
    const auto fn_e = [=](double e) {
        return 1.0 - exp(-e * (T0 - Te)) - e * Ta;
    };
    epsilon = fzero(1.0 / (Ta + 1e-9), fn_e, 1e-10);

    // If epsilon did not solve to anything revert the param change.
    if (std::isnan(epsilon)) return false;

    // alpha is expressed by another implicit equation
    const double wg = M_PI / Tp;

    const auto fn_a = [=](double a) {
        return 1.0 / (a * a + wg * wg) *
                   (exp(-a * Te) * wg / sin(wg * Te) + a - wg / tan(wg * Te)) -
               ((T0 - Te) / (exp(epsilon * (T0 - Te)) - 1) - 1 / epsilon);
    };

    // Find the first interval with a zero crossing
    std::array ints{-1e20, -1e9, -1e8, -1e7, -1e6, -1e5, -1e4,
                    -1e3,  -1e2, -1e1, 0.0,  1e1,  1e2,  1e3,
                    1e4,   1e5,  1e6,  1e7,  1e8,  1e9,  1e20};
    double fa, fb;
    double a, b;

    for (int i = 0; i < ints.size() - 1; ++i) {
        a = ints[i];
        fa = fn_a(a);

        b = ints[i + 1];
        fb = fn_a(b);

        if (sgn(fa) * sgn(fb) <= 0) break;
    }

    b = ints[0];
    fb = fn_a(b);
    for (int i = 1; i < ints.size(); ++i) {
        a = b;
        fa = fb;

        b = ints[i];
        fb = fn_a(b);

        if (sgn(fa) * sgn(fb) <= 0) {
            break;
        }
    }

    alpha = fzero(a, b, fn_a, 1e-10);

    // If alpha did not solve to anything revert the param change.
    if (std::isnan(alpha)) return false;

    return true;
}

bool lf::onParameterChange(const parameter& param) {
    if (param.name() == "Oq") {
        Oq = param.value<double>();
    } else if (param.name() == "am") {
        am = param.value<double>();
    } else if (param.name() == "Qa") {
        Qa = param.value<double>();
    }
    return calculateModelParameters();
}
