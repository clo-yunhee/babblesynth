/*
 * BabbleSynth
 * Copyright (C) 2021  Clo Yun-Hee Dufour
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

using namespace babblesynth::source;

template<typename Func, typename Func2>
static double fzero(const Func& f, const Func2& df, double x0,
                const double tol = 1e-7,
                const double eps = 1e-13,
                const int maxIter = 50)
{
    for (int iter = 0; iter < maxIter; ++iter) {
        double y = f(x0);
        double dy = df(x0);
        if (std::abs(dy) < eps)
            return x0;
        double x1 = x0 - y / dy;
        if (std::abs(x1 - x0) <= tol)
            return x1;
        x0 = x1;
    }
    return x0;
}

lf::lf()
    : abstract_source(),
      Oq(0.6),
      am(0.7),
      Qa(0.1)
{
    addParameter("Oq", 0.6).setMin(0.1).setMax(0.9);
    addParameter("am", 0.7).setMin(0.55).setMax(0.9);
    addParameter("Qa", 0.1).setMin(0.01).setMax(1);
    calculateModelParameters();
}

double lf::evaluateAtPhase(double theta)
{
    const double t = theta / (2 * M_PI);

    if (t <= Te) {
        return -Ee * exp(alpha * (t - Te)) * sin(M_PI * t / Tp) / sin(M_PI * Te / Tp);
    }
    else {
        return -Ee / (epsilon * Ta) * (exp(-epsilon * (t - Te)) - exp(-epsilon * (T0 - Te)));
    }
}

void lf::calculateModelParameters()
{
    Ee = E;
    Te = Oq * T0;
    Tp = am * Oq * T0;
    Ta = Qa * (1 - Oq) * T0;

    const double wg = M_PI / Tp;

    // e is expressed by an implicit equation
    const auto fb = [=](double e) {
        return 1.0 - std::exp(-e * (T0 - Te)) - e * Ta;
    };
    const auto dfb = [=](double e) {
        return (T0 - Te) * std::exp(-e * (T0 - Te)) - Ta;
    };
    const double e = fzero(fb, dfb, 1.0 / (Ta + 1e-13));

    // a is expressed by another implicit equation
    // integral{0, T0} ULF(t) dt, where ULF(t) is the LF model equation
    const double A = (1.0 - std::exp(-e * (T0 - Te))) / (e * e * Ta) - (T0 - Te) * std::exp(-e * (T0 - Te)) / (e * Ta);
    const auto fa = [=](double a) {
        return (a * a + wg * wg) * std::sin(wg * Te) * A + wg * std::exp(-a * Te) + a * std::sin(wg * Te) - wg * std::cos(wg * Te);
    };
    const auto dfa = [=](double a) {
        return (2 * A * a + 1) * std::sin(wg * Te) - wg * Te * std::exp(-a * Te);
    };
    const double a = fzero(fa, dfa, 4.42);

    epsilon = e;
    alpha = a;
}

void lf::onParameterChange(const parameter& param)
{
    if (param.name() == "Oq") {
        Oq = param.value<double>();
    }
    else if (param.name() == "am") {
        am = param.value<double>();
    }
    else if (param.name() == "Qa") {
        Qa = param.value<double>();
    }
    calculateModelParameters();
}
