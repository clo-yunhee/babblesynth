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


#include <array>

#include "noise.h"

using namespace babblesynth::generator;

std::random_device               noise::rd;
std::mt19937                     noise::gen(rd());
std::uniform_real_distribution<> noise::dis(-1.0, 1.0);

std::vector<double> noise::white(int length)
{
    std::vector<double> out(length);
    for (int i = 0; i < length; ++i)
        out[i] = dis(gen);
    return out;
}

std::vector<double> noise::colored(int length, double alpha)
{
    std::array<double, 64> filter;
    filter[0] = 1.0;
    for (int k = 1; k < 64; ++k) {
        filter[k] = (k - 1.0 - alpha / 2.0) * filter[k - 1] / double(k);
    }

    auto noise = white(length + filter.size());
    std::vector<double> filtered(length + filter.size());
    for (int i = 0; i < length + filter.size(); ++i) {
        filtered[i] = 0.0;
        for (int j = 0; j < filter.size(); ++j)
            if (i - j >= 0)
                filtered[i] += filter[j] * noise[i - j];
    }

    std::vector<double> out(length);
    for (int i = 0; i < length; ++i) {
        out[i] = filtered[filter.size() + i];
    }
    return out;
}