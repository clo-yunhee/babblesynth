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

#ifndef BABBLESYNTH_FORMANT_FILTER_H
#define BABBLESYNTH_FORMANT_FILTER_H

#include "../parameter_holder.h"
#include "../variable.h"

namespace babblesynth {
namespace filter {

class formant_filter : public parameter_holder {
   public:
    explicit formant_filter(int sampleRate);
    virtual ~formant_filter() = default;

    std::vector<double> generateFrom(
        const std::vector<double>& input,
        const std::vector<std::pair<int, int>>& periods, double Oq);

   private:
    void designFilter(const std::vector<double>& resF,
                      const std::vector<double>& resB,
                      const std::vector<double>& antiF,
                      const std::vector<double>& antiB);

    double designResonance(double f, double bw, std::vector<double>& b,
                           std::vector<double>& a);

    double designAntiresonance(double f, double bw, std::vector<double>& b,
                               std::vector<double>& a);

    bool onParameterChange(const parameter& param) override;

    variable m_F1;
    variable m_F2;
    variable m_F3;
    variable m_F4;
    variable m_F5;

    variable m_B1;
    variable m_B2;
    variable m_B3;
    variable m_B4;
    variable m_B5;

    variable m_Z1;
    variable m_Z2;

    variable m_A1;
    variable m_A2;

    std::vector<std::array<double, 6>> m_filter;
    std::vector<std::array<double, 2>> m_filterState;

    int m_sampleRate;
};

}  // namespace filter
}  // namespace babblesynth

#endif  // BABBLESYNTH_FORMANT_FILTER_H