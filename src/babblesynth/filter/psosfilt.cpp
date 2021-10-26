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


#include "sos_filter.h"

using namespace babblesynth;

void filter::psosfilt(
    const std::vector<std::array<double, 6>>& sos,
    const std::vector<double>& x, std::vector<double>& y,
    int start, int end,
    std::vector<std::array<double, 2>>& zi)
{
    for (int k = start; k <= end; ++k) {
        double sum = 0;
        double x_cur = x[k];
        double x_new;
        for (int s = 0; s < sos.size(); ++s) {
            x_new    = sos[s][0] * x_cur + zi[s][0];
            zi[s][0] = sos[s][1] * x_cur - sos[s][4] * x_new + zi[s][1];
            zi[s][1] = sos[s][2] * x_cur - sos[s][5] * x_new;
            sum     += x_new;
        }
        y[k] = sum / sos.size();
    }
}

static std::array<double, 2> lfilter3_zi(const std::array<double, 3>& b, const std::array<double, 3>& a)
{
    std::array<double, 2> zi;
 
    zi[0] = (a[1] + a[2]) / (b[1] + b[2] - b[0] * (a[1] + a[2]));
    zi[1] = 1 + a[1] * zi[0] - b[1] + a[1] * b[0];

    return zi;
}

std::vector<std::array<double, 2>> filter::psosfilt_zi(const std::vector<std::array<double, 6>>& sos)
{
    const int nsecs = sos.size();

    std::vector<std::array<double, 2>> zi(nsecs);

    for (int section = 0; section < nsecs; ++section) {
        const auto& [b0, b1, b2, a0, a1, a2] = sos[section];

        std::array b {b0, b1, b2};
        std::array a {a0, a1, a2};

        if (a0 != 1) {
            for (auto& x : b)
                x /= a0;
            for (auto& x : a)
                x /= a0;
        }

        zi[section] = lfilter3_zi(b, a);
    }

    return zi;
}