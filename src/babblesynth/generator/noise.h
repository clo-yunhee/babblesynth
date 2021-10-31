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

#ifndef BABBLESYNTH_NOISE_H
#define BABBLESYNTH_NOISE_H

#include <random>
#include <vector>

namespace babblesynth {
namespace generator {

namespace noise {
std::vector<double> white(int length);
std::vector<double> colored(int length, double alpha = 2);

inline std::vector<double> brown(int length) { return colored(length, 2); }

extern std::random_device rd;
extern std::mt19937 gen;
extern std::uniform_real_distribution<> dis;
}  // namespace noise

}  // namespace generator
}  // namespace babblesynth

#endif  // BABBLESYNTH_NOISE_H