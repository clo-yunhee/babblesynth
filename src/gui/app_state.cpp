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


#include <stdexcept>

#include "app_state.h"
#include "filter/formant_filter.h"
#include "generator/source_generator.h"
#include "variable_plan.h"

using namespace babblesynth::gui;

AppState::AppState(int sampleRate)
    : m_pitchPlan(true),
      m_amplitudePlan(true),
      m_F1_plan(false),
      m_F2_plan(false),
      m_F3_plan(false),
      m_F4_plan(false),
      m_F5_plan(false)
{
    setSampleRate(sampleRate);
    m_pitchPlan.reset(140).stepToValueAtTime(140, 1.0);
    m_amplitudePlan.reset(1).stepToValueAtTime(1, 0.95).cubicToValueAtTime(0, 1.0);
    m_F1_plan.reset(700).stepToValueAtTime(700, 0.35).cubicToValueAtTime(1100, 0.9);
    m_F2_plan.reset(1200).stepToValueAtTime(1200, 0.35).cubicToValueAtTime(1300, 0.9);
    m_F3_plan.reset(2400);
    m_F4_plan.reset(2900);
    m_F5_plan.reset(4200);
}

void AppState::setSampleRate(int sampleRate)
{
    m_sampleRate = sampleRate;
    m_sourceGenerator.reset(new generator::source_generator(sampleRate));
    m_formantFilter.reset(new filter::formant_filter(sampleRate));
}

babblesynth::generator::source_generator *AppState::source()
{
    return m_sourceGenerator.get();
}

babblesynth::variable_plan *AppState::pitchPlan()
{
    return &m_pitchPlan;
}

babblesynth::variable_plan *AppState::amplitudePlan()
{
    return &m_amplitudePlan;
}

babblesynth::filter::formant_filter *AppState::formantFilter()
{
    return m_formantFilter.get();
}

babblesynth::variable_plan *AppState::formantPlan(int n)
{
    switch (n) {
    case 0:
        return &m_F1_plan;
    case 1:
        return &m_F2_plan;
    case 2:
        return &m_F3_plan;
    case 3:
        return &m_F4_plan;
    case 4:
        return &m_F5_plan;
    default:
        throw std::invalid_argument("invalid formant plan number");
    }
}

void AppState::updatePlans()
{
    m_sourceGenerator->getParameter("Pitch plan").setValue(m_pitchPlan);
    m_sourceGenerator->getParameter("Amplitude plan").setValue(m_amplitudePlan);

    m_formantFilter->getParameter("F1 plan").setValue(m_F1_plan);
    m_formantFilter->getParameter("F2 plan").setValue(m_F2_plan);
    m_formantFilter->getParameter("F3 plan").setValue(m_F3_plan);
    m_formantFilter->getParameter("F4 plan").setValue(m_F4_plan);
    m_formantFilter->getParameter("F5 plan").setValue(m_F5_plan);
}