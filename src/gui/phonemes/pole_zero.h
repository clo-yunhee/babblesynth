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

#ifndef BABBLESYNTH_PHONEMES_POLE_ZERO_H
#define BABBLESYNTH_PHONEMES_POLE_ZERO_H

#include <complex>

namespace babblesynth {
namespace gui {
namespace phonemes {

struct PoleZero {
    double frequency;
    double quality;

    std::complex<double> toComplex(double fs);
};

}  // namespace phonemes
}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_PHONEMES_POLE_ZERO_H