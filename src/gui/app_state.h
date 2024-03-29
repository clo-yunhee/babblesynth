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

#ifndef BABBLESYNTH_APP_STATE_H
#define BABBLESYNTH_APP_STATE_H

#include <babblesynth.h>

#include <memory>

namespace babblesynth {
namespace gui {

class AppState {
   public:
    AppState(int sampleRate = 48000);

    void setSampleRate(int sampleRate);

    babblesynth::generator::source_generator *source();
    babblesynth::variable_plan *pitchPlan();
    babblesynth::variable_plan *amplitudePlan();

    babblesynth::filter::formant_filter *formantFilter();
    babblesynth::variable_plan *formantFrequencyPlan(int n);
    babblesynth::variable_plan *formantBandwidthPlan(int n);
    babblesynth::variable_plan *antiformantFrequencyPlan(int n);
    babblesynth::variable_plan *antiformantBandwidthPlan(int n);

    void updatePlans();

   private:
    int m_sampleRate;

    std::unique_ptr<babblesynth::generator::source_generator> m_sourceGenerator;
    babblesynth::variable_plan m_pitchPlan;
    babblesynth::variable_plan m_amplitudePlan;

    std::unique_ptr<babblesynth::filter::formant_filter> m_formantFilter;
    std::vector<babblesynth::variable_plan> m_formantFrequencyPlans;
    std::vector<babblesynth::variable_plan> m_formantBandwidthPlans;
    std::vector<babblesynth::variable_plan> m_antiformantFrequencyPlans;
    std::vector<babblesynth::variable_plan> m_antiformantBandwidthPlans;
};

extern std::shared_ptr<AppState> appState;

}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_APP_STATE_H