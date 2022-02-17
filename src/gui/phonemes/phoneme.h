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

#ifndef BABBLESYNTH_PHONEMES_PHONEME_H
#define BABBLESYNTH_PHONEMES_PHONEME_H

#include <vector>
#include <xercesc/util/XMLString.hpp>

#include "pole_zero.h"

namespace babblesynth {
namespace gui {
namespace phonemes {

class Phoneme {
   public:
    Phoneme(const XMLCh *name);
    virtual ~Phoneme();

   private:
    XMLCh *m_name;
    std::vector<PoleZero> m_poles;
    std::vector<PoleZero> m_zeros;

    void addPole(double frequency, double quality);
    void addZero(double frequency, double quality);

    friend class PhonemeDictionary;
};

}  // namespace phonemes
}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_PHONEMES_PHONEME_H