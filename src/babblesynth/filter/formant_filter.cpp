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

#include "formant_filter.h"

#include <cmath>
#include <complex>
#include <numeric>

#include "../generator/noise.h"
#include "filters.h"

using namespace babblesynth::filter;

formant_filter::formant_filter(int sampleRate)
    : parameter_holder(),
      m_F1(true),
      m_F2(true),
      m_F3(true),
      m_F4(true),
      m_F5(true),
      m_B1(true),
      m_B2(true),
      m_B3(true),
      m_B4(true),
      m_B5(true),
      m_Z1(true),
      m_Z2(true),
      m_A1(true),
      m_A2(true),
      m_sampleRate(sampleRate) {
    addParameter("F1 plan", variable_plan(true, 1000));
    addParameter("F2 plan", variable_plan(true, 1300));
    addParameter("F3 plan", variable_plan(true, 2400));
    addParameter("F4 plan", variable_plan(true, 2900));
    addParameter("F5 plan", variable_plan(true, 3800));

    addParameter("B1 plan", variable_plan(false, 90));
    addParameter("B2 plan", variable_plan(false, 130));
    addParameter("B3 plan", variable_plan(false, 160));
    addParameter("B4 plan", variable_plan(false, 140));
    addParameter("B5 plan", variable_plan(false, 180));

    addParameter("AF1 plan", variable_plan(true, 400));
    addParameter("AF2 plan", variable_plan(true, 1200));

    addParameter("AB1 plan", variable_plan(false, 90));
    addParameter("AB2 plan", variable_plan(false, 100));
}

std::vector<double> formant_filter::generateFrom(
    const std::vector<double>& input,
    const std::vector<std::pair<int, int>>& periods, double Oq) {
    const int samples = input.size();

    std::vector<double> output(samples, 0);

    m_filterState.clear();

    constexpr double fffAmp = 0.065;
    constexpr double fbfAmp = 0.03;

    constexpr double affAmp = 0.02;
    constexpr double abfAmp = 0.01;

    for (const auto& [startIndex, endIndex] : periods) {
        const double gci = startIndex + Oq * (endIndex - startIndex);

        const double openTime = ((startIndex + gci) / 2) / double(m_sampleRate);
        const double closedTime = ((gci + endIndex) / 2) / double(m_sampleRate);

        const double openFlutter =
            (sin(24.1 * M_PI * openTime) + sin(12.7 * M_PI * openTime) +
             sin(7.1 * M_PI * openTime) + sin(4.7 * M_PI * openTime)) /
            4;

        const double F1o = m_F1.evaluateAtTime(openTime);
        const double F2o = m_F2.evaluateAtTime(openTime);
        const double F3o = m_F3.evaluateAtTime(openTime);
        const double F4o = m_F4.evaluateAtTime(openTime);
        const double F5o = m_F5.evaluateAtTime(openTime);

        const double B1o =
            m_B1.evaluateAtTime(openTime) * (1 + fbfAmp * openFlutter);
        const double B2o =
            m_B2.evaluateAtTime(openTime) * (1 + fbfAmp * openFlutter);
        const double B3o =
            m_B3.evaluateAtTime(openTime) * (1 + fbfAmp * openFlutter);
        const double B4o =
            m_B4.evaluateAtTime(openTime) * (1 + fbfAmp * openFlutter);
        const double B5o =
            m_B5.evaluateAtTime(openTime) * (1 + fbfAmp * openFlutter);

        const double Z1o = m_Z1.evaluateAtTime(openTime);
        const double Z2o = m_Z2.evaluateAtTime(openTime);

        const double A1o =
            m_A1.evaluateAtTime(openTime) * (1 + abfAmp * openFlutter);
        const double A2o =
            m_A2.evaluateAtTime(openTime) * (1 + abfAmp * openFlutter);

        designFilter({F1o, F2o, F3o, F4o, F5o}, {B1o, B2o, B3o, B4o, B5o},
                     {Z1o, Z2o}, {A1o, A2o});

        sosfilt(m_filter, input, output, startIndex, gci - 1, m_filterState);

        const double gciTime = gci / m_sampleRate;
        const double gciFlutter =
            (sin(24.1 * M_PI * gciTime) + sin(12.7 * M_PI * gciTime) +
             sin(7.1 * M_PI * gciTime) + sin(4.7 * M_PI * gciTime)) /
            4;

        const double F1c =
            m_F1.evaluateAtTime(closedTime) * (1 + fffAmp * gciFlutter);
        const double F2c =
            m_F2.evaluateAtTime(closedTime) * (1 + fffAmp * gciFlutter);
        const double F3c =
            m_F3.evaluateAtTime(closedTime) * (1 + fffAmp * gciFlutter);
        const double F4c =
            m_F4.evaluateAtTime(closedTime) * (1 + fffAmp * gciFlutter);
        const double F5c =
            m_F5.evaluateAtTime(closedTime) * (1 + fffAmp * gciFlutter);

        const double B1c =
            m_B1.evaluateAtTime(closedTime) * (1 + fbfAmp * gciFlutter);
        const double B2c =
            m_B2.evaluateAtTime(closedTime) * (1 + fbfAmp * gciFlutter);
        const double B3c =
            m_B3.evaluateAtTime(closedTime) * (1 + fbfAmp * gciFlutter);
        const double B4c =
            m_B4.evaluateAtTime(closedTime) * (1 + fbfAmp * gciFlutter);
        const double B5c =
            m_B5.evaluateAtTime(closedTime) * (1 + fbfAmp * gciFlutter);

        const double Z1c =
            m_Z1.evaluateAtTime(closedTime) * (1 + affAmp * gciFlutter);
        const double Z2c =
            m_Z2.evaluateAtTime(closedTime) * (1 + affAmp * gciFlutter);

        const double A1c =
            m_A1.evaluateAtTime(closedTime) * (1 + abfAmp * gciFlutter);
        const double A2c =
            m_A2.evaluateAtTime(closedTime) * (1 + abfAmp * gciFlutter);

        designFilter({F1c, F2c, F3c, F4c, F5c}, {B1c, B2c, B3c, B4c, B5c},
                     {Z1c, Z2c}, {A1c, A2c});

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

void formant_filter::designFilter(const std::vector<double>& resF,
                                  const std::vector<double>& resB,
                                  const std::vector<double>& antiF,
                                  const std::vector<double>& antiB) {
    std::vector<std::array<double, 6>> sos;
    std::vector<double> b, a;

    for (int i = 0; i < resF.size(); ++i) {
        designResonance(resF[i], resB[i], b, a);
        sos.push_back({b[0], b[1], b[2], a[0], a[1], a[2]});
    }

    for (int i = 0; i < antiF.size(); ++i) {
        designAntiresonance(antiF[i], antiB[i], b, a);
        sos.push_back({b[0], b[1], b[2], a[0], a[1], a[2]});
    }

    // Take the normalized average of 3rd formant and above.
    double avg;
    if (resF.size() > 2) {
        double sum = 0;
        for (int i = 2; i < resF.size(); ++i) {
            sum += resF[i] / (i + 1);
        }
        avg = sum / (resF.size() - 2);
    } else {
        avg = (resF[0] + resF[1]) / 2;
    }
    avg += 300;

    // Auto-fill up to ten total formants.
    double freq = resF.back() + avg;

    while (sos.size() < 10) {
        const double bandwidth = 0.3 * freq;

        designResonance(freq, bandwidth, b, a);
        sos.push_back({b[0], b[1], b[2], a[0], a[1], a[2]});

        freq += avg;
    }

    std::reverse(sos.begin(), sos.end());

    m_filter = sos;
    m_filterState.resize(sos.size(), {0.0, 0.0});
}

double formant_filter::designResonance(double f, double bw,
                                       std::vector<double>& b,
                                       std::vector<double>& a) {
    const double R = exp(-M_PI * bw / m_sampleRate);
    const double theta = 2 * M_PI * f / m_sampleRate;

    const double b0 = (1 - R) * sqrt(1 - 2 * R * cos(2 * theta) + (R * R));

    b = {b0, b0, 0};
    a = {1, -2 * R * cos(theta), R * R};

    return R;
}

double formant_filter::designAntiresonance(double f, double bw,
                                           std::vector<double>& b,
                                           std::vector<double>& a) {
    const double R = exp(-M_PI * bw / m_sampleRate);
    const double theta = 2 * M_PI * f / m_sampleRate;

    // const double b0 = (1 - R) * sqrt(1 - 2 * R * cos(2 * theta) + (R * R));

    // b = {b0, b0, 0};

    b = {1, 0, theta * theta};
    a = {1, theta / (f / bw), theta * theta};

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
    } else if (param.name() == "B1 plan") {
        m_B1.setPlan(param.value<variable_plan>());
    } else if (param.name() == "B2 plan") {
        m_B2.setPlan(param.value<variable_plan>());
    } else if (param.name() == "B3 plan") {
        m_B3.setPlan(param.value<variable_plan>());
    } else if (param.name() == "B4 plan") {
        m_B4.setPlan(param.value<variable_plan>());
    } else if (param.name() == "B5 plan") {
        m_B5.setPlan(param.value<variable_plan>());
    } else if (param.name() == "AF1 plan") {
        m_Z1.setPlan(param.value<variable_plan>());
    } else if (param.name() == "AF2 plan") {
        m_Z2.setPlan(param.value<variable_plan>());
    } else if (param.name() == "AB1 plan") {
        m_A1.setPlan(param.value<variable_plan>());
    } else if (param.name() == "AB2 plan") {
        m_A2.setPlan(param.value<variable_plan>());
    }

    return true;
}