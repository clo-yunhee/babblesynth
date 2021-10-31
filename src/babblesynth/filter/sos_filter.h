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

#ifndef BABBLESYNTH_SOS_FILTER
#define BABBLESYNTH_SOS_FILTER

#include <array>
#include <complex>
#include <vector>

namespace babblesynth {
namespace filter {

std::vector<std::complex<double>> solveRoots(const std::vector<double>& P);

double tf2zpk(const std::vector<double>& b, const std::vector<double>& a,
              std::vector<std::complex<double>>& z,
              std::vector<std::complex<double>>& p);

std::vector<std::array<double, 6>> zpk2sos(
    const std::vector<std::complex<double>>& z,
    const std::vector<std::complex<double>>& p, double k);

void sosfilt(const std::vector<std::array<double, 6>>& sos,
             const std::vector<double>& x, std::vector<double>& y, int start,
             int end, std::vector<std::array<double, 2>>& zi);

std::vector<std::array<double, 2>> sosfilt_zi(
    const std::vector<std::array<double, 6>>& sos);

}  // namespace filter
}  // namespace babblesynth

#endif  // BABBLESYNTH_SOS_FILTER