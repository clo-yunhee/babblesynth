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

#include "butterworth.h"

#include "filters.h"

using namespace babblesynth::filter;

std::vector<std::array<double, 6>> butterworth::lowPass(int N, double fc,
                                                        double fs) {
    const double Wn = fc / (fs / 2.0);
    const double Wo = std::tan(Wn * M_PI / 2.0);

    std::vector<std::complex<double>> p;

    // Step 1. Get Butterworth analog lowpass prototype.
    for (int i = 2 + N - 1; i <= 3 * N - 1; i += 2) {
        p.push_back(std::polar<double>(1, (M_PI * i) / (2.0 * N)));
    }

    // Step 2. Transform to low pass filter.
    std::complex<double> Sg = 1.0, prodSp = 1.0;

    std::vector<std::complex<double>> Sp(p.size()), Sz(0);

    for (int i = 0; i < p.size(); ++i) {
        Sg *= Wo;
        Sp[i] = Wo * p[i];
        prodSp *= (1.0 - Sp[i]);
    }

    // Step 3. Transform to digital filter.
    std::vector<std::complex<double>> P(Sp.size()), Z(Sp.size(), -1);

    double G = std::real(Sg / prodSp);

    for (int i = 0; i < Sp.size(); ++i) {
        P[i] = (1.0 + Sp[i]) / (1.0 - Sp[i]);
    }

    // Step 6. Convert to SOS.

    return zpk2sos(Z, P, G);
}

std::vector<std::array<double, 6>> butterworth::highPass(int N, double fc,
                                                         double fs) {
    const double Wn = fc / (fs / 2.0);
    const double Wo = std::tan(Wn * M_PI / 2.0);

    std::vector<std::complex<double>> p;

    // Step 1. Get Butterworth analog lowpass prototype.
    for (int i = 2 + N - 1; i <= 3 * N - 1; i += 2) {
        p.push_back(std::polar<double>(1, (M_PI * i) / (2.0 * N)));
    }

    // Step 2. Transform to high pass filter.
    std::complex<double> Sg = 1.0, prodSp = 1.0, prodSz = 1.0;

    std::vector<std::complex<double>> Sp(p.size()), Sz(p.size());

    for (int i = 0; i < p.size(); ++i) {
        Sg *= -p[i];
        Sp[i] = Wo / p[i];
        Sz[i] = 0.0;
        prodSp *= (1.0 - Sp[i]);
        prodSz *= (1.0 - Sz[i]);
    }
    Sg = 1.0 / Sg;

    // Step 3. Transform to digital filter.
    std::vector<std::complex<double>> P(Sp.size()), Z(Sp.size());

    double G = std::real(Sg * prodSz / prodSp);

    for (int i = 0; i < Sp.size(); ++i) {
        P[i] = (1.0 + Sp[i]) / (1.0 - Sp[i]);
        Z[i] = (1.0 + Sz[i]) / (1.0 - Sz[i]);
    }

    // Step 6. Convert to SOS.

    return zpk2sos(Z, P, G);
}