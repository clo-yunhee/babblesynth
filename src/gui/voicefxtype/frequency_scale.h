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

#ifndef BABBLESYNTH_VOICEFX_FREQUENCY_SCALE_H
#define BABBLESYNTH_VOICEFX_FREQUENCY_SCALE_H

#include <cmath>

namespace babblesynth {
namespace gui {
namespace voicefx {

inline double hzToLog(double f) { return 100 * std::log2(f); }
inline double logToHz(double f) { return std::pow(2, f / 100); }
inline double hzToMel(double f) { return 1127 * std::log(1 + f / 700); }
inline double melToHz(double m) { return 700 * (exp(m / 1127) - 1); }
inline double hzToErb(double f) { return 24.7 * (4.37 * f + 1); }
inline double erbToHz(double m) { return (m / 24.7 - 1) / 4.37; }

}  // namespace voicefx
}  // namespace gui
}  // namespace babblesynth

#endif  // BABBLESYNTH_VOICEFX_FREQUENCY_SCALE_H