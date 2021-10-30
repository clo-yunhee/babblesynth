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


#ifndef BABBLESYNTH_SOURCE_GENERATOR_H
#define BABBLESYNTH_SOURCE_GENERATOR_H

#include "../source/abstract_source.h"

namespace babblesynth {
namespace generator {

class source_generator : public parameter_holder {
public:
    explicit source_generator(int sampleRate);
    virtual ~source_generator() = default;

    std::vector<double> generate(std::vector<std::pair<int, int>>& periods);

    source::abstract_source *getSource();

private:
    void onParameterChange(const parameter& param) override;

    std::unique_ptr<source::abstract_source> m_source;

    variable m_pitch;
    variable m_amplitude;
    double m_jitterPercentage;
    double m_aspirationPercentage;

    int m_sampleRate;
};

} // generator
} // babblesynth

#endif // BABBLESYNTH_SOURCE_GENERATOR_H