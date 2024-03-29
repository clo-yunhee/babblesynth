﻿/*
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

#include "source_generator.h"

#include <algorithm>
#include <cmath>

#include "../filter/butterworth.h"
#include "../filter/filters.h"
#include "../source/abstract_source.h"
#include "../source/lf.h"
#include "noise.h"

using namespace babblesynth::generator;

source_generator::source_generator(int sampleRate)
    : parameter_holder(),
      m_pitch(true),
      m_amplitude(false),
      m_sampleRate(sampleRate),
      m_antialiasFilter(filter::butterworth::lowPass(
          8, double(sampleRate) / 2 - 2000, sampleRate)) {
    addParameter("Source type", source::sources.valueOf("LF"));
    addParameter("Pitch plan",
                 variable_plan(false, 220).stepToValueAtTime(220, 1.0));
    addParameter("Amplitude plan",
                 variable_plan(false, 1).stepToValueAtTime(1, 1.0));
    addParameter("Jitter", 0.03).setMin(0).setMax(1);
    addParameter("Aspiration", 0.10).setMin(0).setMax(1);
    addParameter("Flutter", 0.005).setMin(0).setMax(0.9);
}

bool source_generator::onParameterChange(const parameter& param) {
    if (param.name() == "Source type") {
        const auto sourceName = param.value<enumeration_value>().name();
        if (sourceName == "LF") {
            m_source = std::make_unique<source::lf>();
        } else {
            return false;
        }
    } else if (param.name() == "Pitch plan") {
        m_pitch.setPlan(param.value<variable_plan>());
    } else if (param.name() == "Amplitude plan") {
        m_amplitude.setPlan(param.value<variable_plan>());
    } else if (param.name() == "Aspiration") {
        m_aspirationPercentage = param.value<double>();
    } else if (param.name() == "Jitter") {
        m_jitterPercentage = param.value<double>();
    } else if (param.name() == "Flutter") {
        m_flutterAmplitude = param.value<double>();
    }

    return true;
}

babblesynth::source::abstract_source* source_generator::getSource() {
    return m_source.get();
}

std::vector<double> source_generator::generate(
    std::vector<std::pair<int, int>>& periods, double* Oq) {
    const double duration = m_pitch.maxTime();
    const int samples = std::ceil(duration * m_sampleRate);

    std::vector<double> output(samples, 0);
    std::vector<double> noise = noise::colored(samples, -4);

    double noiseAmplitude =
        std::max(-*std::min_element(noise.begin(), noise.end()),
                 +*std::max_element(noise.begin(), noise.end()));

    *Oq = m_source->getParameter("Oq").value<double>();

    double phase = 0;
    double c = 0;  // running compensation for lost low-order bits

    double lastNoise = noise[0] / noiseAmplitude;

    int periodStart = 0;

    double maxAmplitude = 1e-10;

    auto aafiltz = std::vector<std::array<double, 2>>(m_antialiasFilter.size(),
                                                      {0.0, 0.0});

    for (int index = 0; index < samples; ++index) {
        const double time = index / double(m_sampleRate);

        const double f0 = m_pitch.evaluateAtTime(time);

        const double jitterHz = f0 * m_jitterPercentage * lastNoise / 2;

        const double flutter =
            (sin(24.1 * M_PI * time) + sin(12.7 * M_PI * time) +
             sin(7.1 * M_PI * time) + sin(4.7 * M_PI * time)) /
            4;

        const double phaseDelta =
            2 * M_PI * (f0 * (1 + m_flutterAmplitude * flutter) + jitterHz) /
            m_sampleRate;

        m_amplitude.update(time);

        output[index] = m_source->evaluateAtPhase(phase);

        // Only add aspiration noise during the open phase.
        if (phase / 2 * M_PI < *Oq) {
            output[index] +=
                m_aspirationPercentage * noise[index] / noiseAmplitude;
        }

        output[index] *= m_amplitude.evaluateAtTime(time);

        if (std::abs(output[index]) > maxAmplitude) {
            maxAmplitude = std::abs(output[index]);
        }

        // Kahan summation algorithm for the phase variable.
        const double y = phaseDelta - c;
        const double t = phase + y;
        c = (t - phase) - y;
        phase = t;

        // modulo 2*pi
        if (phase > 2 * M_PI) {
            phase -= 2 * M_PI;
            lastNoise = noise[index] / noiseAmplitude;
            periods.emplace_back(periodStart, index);
            periodStart = index + 1;
        }
    }

    // Remove the last partial period.
    output.resize(periodStart);

    auto antialiasedOutput = output;
    filter::sosfilt(m_antialiasFilter, output, antialiasedOutput, 0,
                    output.size() - 1, aafiltz);
    return antialiasedOutput;
}
