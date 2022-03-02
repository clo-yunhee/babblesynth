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

#include "filters.h"

using namespace babblesynth;

void filter::sosfilt(const std::vector<std::array<double, 6>>& sos,
                     const std::vector<double>& x, std::vector<double>& y,
                     int start, int end,
                     std::vector<std::array<double, 2>>& zi) {
    for (int k = start; k <= end; ++k) {
        double x_cur = x[k];
        double x_new;
        for (int s = 0; s < sos.size(); ++s) {
            x_new = sos[s][0] * x_cur + zi[s][0];
            zi[s][0] = sos[s][1] * x_cur - sos[s][4] * x_new + zi[s][1];
            zi[s][1] = sos[s][2] * x_cur - sos[s][5] * x_new;
            x_cur = x_new;
        }
        y[k] = x_cur;
    }
}
