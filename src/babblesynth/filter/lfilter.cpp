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

void filter::lfilter(const std::vector<double>& b, const std::vector<double>& a,
                     const std::vector<double>& x, std::vector<double>& y,
                     int start, int end, std::vector<double>& z) {
    const int len_b = b.size();
    // const int len_a = a.size();

    // const double a0 = a[0];

    int k;  // Index along x and y
    int j;  // Index along b and a
    int m;  // Index along z
    int n;  // Index for iterating middle delays

    // Assume the filter is already normalized.

    for (k = start; k <= end; ++k) {
        j = 0;

        if (len_b > 1) {
            m = 0;
            // First delay
            y[k] = z[m] + b[j] * x[k];
            j++;
            // Middle delays
            for (n = 0; n < len_b - 2; ++n) {
                z[m] = z[m + 1] + x[k] * b[j] - y[k] * a[j];
                j++;
                m++;
            }
            // Last delay
            z[m] = x[k] * b[j] - y[k] * a[j];
        } else {
            y[k] = x[k] * b[j];
        }
    }
}
