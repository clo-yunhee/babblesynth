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

#ifndef BABBLESYNTH_PHONEMES_PHONEME_H
#define BABBLESYNTH_PHONEMES_PHONEME_H

#include <vector>
#include <xercesc/util/XMLString.hpp>

#include "pole_zero.h"
#include "xmlwstr.h"

namespace babblesynth {
namespace gui {

class PhonemeEditor;

namespace phonemes {

class Phoneme {
   public:
    Phoneme() = default;
    Phoneme(const XMLWStr& name);

    Phoneme(const Phoneme& other);
    Phoneme(Phoneme&& other);
    Phoneme& operator=(const Phoneme& other) = default;

    const XMLWStr& name() const;

    template <typename PoleFn, typename ZeroFn>
    void updatePlansWith(const PoleFn& fnPole, const ZeroFn& fnZero) const {
        for (const auto& pole : m_poles) {
            fnPole(pole.i, pole.frequency, pole.bandwidth);
        }
        for (const auto& zero : m_zeros) {
            fnZero(zero.i, zero.frequency, zero.bandwidth);
        }
    }

   private:
    XMLWStr m_name;
    std::vector<PoleZero> m_poles;
    std::vector<PoleZero> m_zeros;

    void addPole(double frequency, double bandwidth, int i = -1);
    void addZero(double frequency, double bandwidth, int i = -1);

    friend class PhonemeDictionary;
    friend class gui::PhonemeEditor;
};

}  // namespace phonemes
}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_PHONEMES_PHONEME_H