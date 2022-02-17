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

#include "pole_zero.h"

#include <cmath>

using namespace babblesynth::gui::phonemes;

std::complex<double> PoleZero::toComplex(double fs) {
    const double bandwidth = frequency / quality;

    const double mag = exp(-M_PI * bandwidth / fs);
    const double phase = 2 * M_PI * frequency / fs;

    return std::polar(mag, phase);
}