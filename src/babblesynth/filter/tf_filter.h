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


#ifndef BABBLESYNTH_TF_FILTER
#define BABBLESYNTH_TF_FILTER

#include <array>
#include <complex>
#include <vector>

namespace babblesynth {
namespace filter {

void lfilter(const std::vector<double>& b,
             const std::vector<double>& a,
             const std::vector<double>& x,
             std::vector<double>& y,
             int start, int end,
             std::vector<double>& z);

std::vector<double> lfiltic(const std::vector<double>& b,
                            const std::vector<double>& a,
                            const std::vector<double>& x,
                            const std::vector<double>& y,
                            int end);

} // filter
} // babblesynth

#endif // BABBLESYNTH_TF_FILTER