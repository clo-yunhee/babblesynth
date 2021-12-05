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

#include "formant_filter.h"

#include <cmath>
#include <complex>
#include <numeric>

#include "../generator/noise.h"
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
    addParameter("F1 plan", variable_plan(true, 1000));
    addParameter("F2 plan", variable_plan(true, 1300));
    addParameter("F3 plan", variable_plan(true, 2400));
    addParameter("F4 plan", variable_plan(true, 2900));
    addParameter("F5 plan", variable_plan(true, 3800));
}

std::vector<double> formant_filter::generateFrom(
    const std::vector<double>& input,
    const std::vector<std::pair<int, int>>& periods, double Oq) {
    const int samples = input.size();

    std::vector<double> output(samples, 0);

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

        sosfilt(m_filter, input, output, startIndex, gci - 1, m_filterState);

        const double F1c = m_F1.evaluateAtTime(closedTime) + 100;
        const double F2c = m_F2.evaluateAtTime(closedTime) + 150;
        const double F3c = m_F3.evaluateAtTime(closedTime) + 200;
        const double F4c = m_F4.evaluateAtTime(closedTime) + 250;
        const double F5c = m_F5.evaluateAtTime(closedTime) + 300;

        designFilter({F1c, F2c, F3c, F4c, F5c});

        sosfilt(m_filter, input, output, gci, endIndex, m_filterState);
    }

    std::vector<double> outputFilt(output);
    std::vector<double> intZ(1, 0);

    lfilter({1, 0}, {1, 0.99}, output, outputFilt, 0, samples - 1, intZ);

    double maxAmplitude = 1e-10;

    for (int i = 0; i < samples; ++i) {
        const double xa = std::abs(outputFilt[i]);
        if (xa > maxAmplitude) {
            maxAmplitude = xa;
        }
    }

    for (int i = 0; i < samples; ++i) {
        outputFilt[i] /= maxAmplitude;
    }

    return outputFilt;
}

void formant_filter::designFilter(const std::vector<double>& freqs) {
    constexpr std::array bandwidths{90, 130, 160, 140, 180};

    std::vector<std::array<double, 6>> sos;
    std::vector<double> b, a;

    for (int i = 0; i < freqs.size(); ++i) {
        designFilterSection(freqs[i], bandwidths[i], b, a);
        sos.push_back({b[0], b[1], b[2], a[0], a[1], a[2]});
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

    // Auto-fill up to ten total formants.
    double freq = freqs.back() + avg;

    while (sos.size() < 10) {
        const double bandwidth = 0.3 * freq;

        designFilterSection(freq, bandwidth, b, a);
        sos.push_back({b[0], b[1], b[2], a[0], a[1], a[2]});

        freq += avg;
    }

    std::reverse(sos.begin(), sos.end());

    m_filter = sos;
    m_filterState.resize(sos.size(), {0.0, 0.0});
}

double formant_filter::designFilterSection(double f, double bw,
                                           std::vector<double>& b,
                                           std::vector<double>& a) {
    const double R = exp(-M_PI * bw / m_sampleRate);
    const double theta = 2 * M_PI * f / m_sampleRate;

    const double b0 = (1 - R) * sqrt(1 - 2 * R * cos(2 * theta) + (R * R));

    b = {b0, b0, 0};
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