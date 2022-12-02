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

#include "app_state.h"

#include <stdexcept>

#include "filter/formant_filter.h"
#include "generator/source_generator.h"
#include "variable_plan.h"

using namespace babblesynth::gui;

constexpr int nF = 5;
constexpr int nZ = 2;

AppState::AppState(int sampleRate)
    : m_pitchPlan(false),
      m_amplitudePlan(false),
      m_formantFrequencyPlans(nF),
      m_formantBandwidthPlans(nF),
      m_antiformantFrequencyPlans(nZ),
      m_antiformantBandwidthPlans(nZ) {
    setSampleRate(sampleRate);
    m_pitchPlan.reset(140).stepToValueAtTime(140, 1.0);
    m_amplitudePlan.reset(1).stepToValueAtTime(1, 0.95).cubicToValueAtTime(0,
                                                                           1.0);
    formantFrequencyPlan(2)->reset(2400);
    formantFrequencyPlan(3)->reset(2900);
    formantFrequencyPlan(4)->reset(4200);

    for (int i = 0; i < nF; ++i) {
        formantBandwidthPlan(i)->reset(80 + i * 15);
    }

    antiformantFrequencyPlan(0)->reset(400);
    antiformantBandwidthPlan(0)->reset(100);

    antiformantFrequencyPlan(1)->reset(1200);
    antiformantBandwidthPlan(1)->reset(200);
}

void AppState::setSampleRate(int sampleRate) {
    m_sampleRate = sampleRate;
    m_sourceGenerator.reset(new generator::source_generator(sampleRate));
    m_formantFilter.reset(new filter::formant_filter(sampleRate));
}

babblesynth::generator::source_generator *AppState::source() {
    return m_sourceGenerator.get();
}

babblesynth::variable_plan *AppState::pitchPlan() { return &m_pitchPlan; }

babblesynth::variable_plan *AppState::amplitudePlan() {
    return &m_amplitudePlan;
}

babblesynth::filter::formant_filter *AppState::formantFilter() {
    return m_formantFilter.get();
}

babblesynth::variable_plan *AppState::formantFrequencyPlan(int n) {
    if (n < 0 || n >= m_formantFrequencyPlans.size()) {
        throw std::invalid_argument("invalid formant plan number");
    }

    return &m_formantFrequencyPlans[n];
}

babblesynth::variable_plan *AppState::formantBandwidthPlan(int n) {
    if (n < 0 || n >= m_formantBandwidthPlans.size()) {
        throw std::invalid_argument("invalid formant plan number");
    }

    return &m_formantBandwidthPlans[n];
}

babblesynth::variable_plan *AppState::antiformantFrequencyPlan(int n) {
    if (n < 0 || n >= m_antiformantFrequencyPlans.size()) {
        throw std::invalid_argument("invalid antiformant plan number");
    }

    return &m_antiformantFrequencyPlans[n];
}

babblesynth::variable_plan *AppState::antiformantBandwidthPlan(int n) {
    if (n < 0 || n >= m_antiformantBandwidthPlans.size()) {
        throw std::invalid_argument("invalid antiformant plan number");
    }

    return &m_antiformantBandwidthPlans[n];
}

void AppState::updatePlans() {
    m_sourceGenerator->getParameter("Pitch plan").setValue(m_pitchPlan);
    m_sourceGenerator->getParameter("Amplitude plan").setValue(m_amplitudePlan);

    for (int n = 0; n < m_formantFrequencyPlans.size(); ++n) {
        std::string nameF = "F" + std::to_string(n + 1) + " plan";
        std::string nameB = "B" + std::to_string(n + 1) + " plan";

        m_formantFilter->getParameter(nameF).setValue(
            m_formantFrequencyPlans[n]);

        m_formantFilter->getParameter(nameB).setValue(
            m_formantBandwidthPlans[n]);
    }

    for (int n = 0; n < m_antiformantFrequencyPlans.size(); ++n) {
        std::string nameF = "AF" + std::to_string(n + 1) + " plan";
        std::string nameB = "AB" + std::to_string(n + 1) + " plan";

        m_formantFilter->getParameter(nameF).setValue(
            m_antiformantFrequencyPlans[n]);

        m_formantFilter->getParameter(nameB).setValue(
            m_antiformantBandwidthPlans[n]);
    }
}