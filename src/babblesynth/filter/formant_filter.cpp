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

#include <cmath>
#include <complex>
// #include <iostream>
#include <numeric>

#include "../generator/noise.h"
#include "formant_filter.h"
#include "sos_filter.h"
#include "tf_filter.h"

using namespace babblesynth::filter;

formant_filter::formant_filter(int sampleRate)
    : parameter_holder(),
      m_F1(true),
      m_F2(true),
      m_F3(true),
      m_F4(true),
      m_F5(true),
      m_sampleRate(sampleRate) {
    addParameter("F1 plan", variable_plan(1000));
    addParameter("F2 plan", variable_plan(1300));
    addParameter("F3 plan", variable_plan(2400));
    addParameter("F4 plan", variable_plan(2900));
    addParameter("F5 plan", variable_plan(3800));
}

/*std::vector<double> formant_filter::generateFrom(
    const std::vector<double>& input,
    const std::vector<std::pair<int, int>>& periods) {
    const int samples = input.size();

    std::vector<double> lfilt_output = input;
    std::vector<double> sosfilt_output = input;

    m_filterState.clear();

    for (const auto& [startIndex, endIndex] : periods) {
        const double startTime = startIndex / double(m_sampleRate);
        const double endTime = endIndex / double(m_sampleRate);

        const double midTime = (startTime + endTime) / 2;

        const double F1 = m_F1.evaluateAtTime(midTime);
        const double F2 = m_F2.evaluateAtTime(midTime);
        const double F3 = m_F3.evaluateAtTime(midTime);
        const double F4 = m_F4.evaluateAtTime(midTime);
        const double F5 = m_F5.evaluateAtTime(midTime);

        designFilter({F1, F2, F3, F4, F5});

        lfilter(m_B, m_A, input, lfilt_output, startIndex, endIndex, m_Z);
        sosfilt(m_filter, input, sosfilt_output, startIndex, endIndex,
                m_filterState);
    }

    for (int i = 0; i < samples; ++i) {
        if (std::isnan(lfilt_output[i]) || std::isinf(lfilt_output[i])) {
            // std::cout << "lfilter overflowed, substituting with sosfilt
            // result" << std::endl;
            lfilt_output[i] = sosfilt_output[i];
        }
    }

    double maxAmplitude = 1e-10;

    for (int i = 0; i < samples; ++i) {
        const double xa = std::abs(lfilt_output[i]);
        if (xa > maxAmplitude) {
            maxAmplitude = xa;
        }
    }

    for (int i = 0; i < samples; ++i) {
        lfilt_output[i] /= maxAmplitude;
    }

    return lfilt_output;
}*/

std::vector<double> formant_filter::generateFrom(
    const std::vector<double>& input,
    const std::vector<std::pair<int, int>>& periods, double Oq) {
    const int samples = input.size();

    std::vector<double> output = input;

    m_filterState.clear();

    for (const auto& [startIndex, endIndex] : periods) {
        const double gci = startIndex + Oq * (endIndex - startIndex);

        const double openTime = ((startIndex + gci) / 2) / double(m_sampleRate);
        const double closedTime = ((gci + endIndex) / 2) / double(m_sampleRate);

        const double F1o = m_F1.evaluateAtTime(openTime);
        const double F2o = m_F2.evaluateAtTime(openTime);
        const double F3o = m_F3.evaluateAtTime(openTime);
        const double F4o = m_F4.evaluateAtTime(openTime);
        const double F5o = m_F5.evaluateAtTime(openTime);

        designFilter({F1o, F2o, F3o, F4o, F5o});

        lfilter(m_B, m_A, input, output, startIndex, gci - 1, m_Z);
        // sosfilt(m_filter, input, output, startIndex, gci - 1, m_filterState);

        const double F1c = m_F1.evaluateAtTime(closedTime);
        const double F2c = m_F2.evaluateAtTime(closedTime);
        const double F3c = m_F3.evaluateAtTime(closedTime);
        const double F4c = m_F4.evaluateAtTime(closedTime);
        const double F5c = m_F5.evaluateAtTime(closedTime);  // + 50;

        designFilter({F1c, F2c, F3c, F4c, F5c});

        lfilter(m_B, m_A, input, output, gci, endIndex, m_Z);
        // sosfilt(m_filter, input, output, gci, endIndex, m_filterState);
    }

    double maxAmplitude = 1e-10;

    for (int i = 0; i < samples; ++i) {
        const double xa = std::abs(output[i]);
        if (xa > maxAmplitude) {
            maxAmplitude = xa;
        }
    }

    for (int i = 0; i < samples; ++i) {
        output[i] /= maxAmplitude;
    }

    return output;
}

static std::vector<double> conv(const std::vector<double>& a,
                                const std::vector<double>& b) {
    const int na = a.size();
    const int nb = b.size();
    const int ny = na + nb - 1;

    std::vector<double> y(ny);

    for (int i = 0; i < ny; ++i) {
        double sum = 0;
        double c = 0;

        for (int j = 0; j <= i; ++j) {
            if (j < na && i - j < nb) {
                double x = (a[j] * b[i - j]) - c;
                double t = sum + x;

                c = (t - sum) - x;
                sum = t;
            }
        }

        y[i] = sum;
    }

    return y;
}

void formant_filter::designFilter(const std::vector<double>& freqs) {
    constexpr std::array bandwidths{90, 130, 160, 140, 180};

    std::vector<std::vector<double>> Bs;
    std::vector<std::vector<double>> As;
    std::vector<double> Rs;

    std::vector<double> b, a;
    double r;

    for (int i = 0; i < freqs.size(); ++i) {
        r = designFilterSection(freqs[i], bandwidths[i], b, a);

        Bs.push_back(b);
        As.push_back(a);
        Rs.push_back(r);
    }

    // Take the normalized average of 3rd formant and above.
    double avg;
    if (freqs.size() > 2) {
        double sum = 0;
        for (int i = 2; i < freqs.size(); ++i) {
            sum += freqs[i] / (i + 1);
        }
        avg = sum / (freqs.size() - 2);
    } else {
        avg = (freqs[0] + freqs[1]) / 2;
    }
    avg += 300;

    // Auto-fill up to eight total formants.
    double freq = freqs.back() + avg;

    while (Bs.size() < 10) {
        const double bandwidth = 0.3 * freq;

        r = designFilterSection(freq, bandwidth, b, a);

        Bs.push_back(b);
        As.push_back(a);
        Rs.push_back(r);

        freq += avg;
    }

    // Sort the sections in increasing R.
    std::vector<int> idx(Rs.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(),
              [&Rs](int i, int j) { return Rs[i] < Rs[j]; });

    std::vector<double> B = Bs[idx[0]];
    std::vector<double> A = As[idx[0]];

    for (int i = 0; i < Bs.size(); ++i) {
        B = conv(B, Bs[idx[i]]);
        A = conv(A, As[idx[i]]);
    }

    if (A[0] != 1) {
        const double a0 = A[0];
        for (auto& x : B) x /= a0;
        for (auto& x : A) x /= a0;
    }

    m_B = B;
    m_A = A;
    m_Z.resize(B.size() - 1, 0);

    m_filter.resize(Bs.size());
    for (int i = 0; i < Bs.size(); ++i) {
        m_filter[i][0] = Bs[i][0];
        m_filter[i][1] = Bs[i][1];
        m_filter[i][2] = Bs[i][2];
        m_filter[i][3] = As[i][0];
        m_filter[i][4] = As[i][1];
        m_filter[i][5] = As[i][2];
    }

    m_filterState.resize(Bs.size(), {0.0, 0.0});
}

double formant_filter::designFilterSection(double f, double bw,
                                           std::vector<double>& b,
                                           std::vector<double>& a) {
    const double R = exp(-M_PI * bw / m_sampleRate);
    const double theta = 2 * M_PI * f / m_sampleRate;

    const double b0 = (1 - R) * sqrt(1 - 2 * R * cos(2 * theta) + (R * R));

    b = {b0, 0, 0};
    a = {1, -2 * R * cos(theta), R * R};

    return R;
}

bool formant_filter::onParameterChange(const parameter& param) {
    if (param.name() == "F1 plan") {
        m_F1.setPlan(param.value<variable_plan>());
    } else if (param.name() == "F2 plan") {
        m_F2.setPlan(param.value<variable_plan>());
    } else if (param.name() == "F3 plan") {
        m_F3.setPlan(param.value<variable_plan>());
    } else if (param.name() == "F4 plan") {
        m_F4.setPlan(param.value<variable_plan>());
    } else if (param.name() == "F5 plan") {
        m_F5.setPlan(param.value<variable_plan>());
    }

    return true;
}