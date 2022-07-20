/*
 * BabbleSynth
 * Copyright (C) 2022  Clo Yun-Hee Dufour
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

#include "resample.h"

#include <samplerate.h>

std::vector<double> babblesynth::resample(const std::vector<double>& inDbl,
                                          const double fsIn,
                                          const double fsOut) {
    std::vector<float> in(inDbl.begin(), inDbl.end());

    const int outExpectedLen = (int)((double)in.size() * fsOut / fsIn + 0.5);
    std::vector<float> out(outExpectedLen);

    SRC_DATA data;
    memset(&data, 0, sizeof(data));
    data.data_in = in.data();
    data.data_out = out.data();
    data.input_frames = in.size();
    data.output_frames = out.size();
    data.src_ratio = fsOut / fsIn;

    src_simple(&data, SRC_SINC_BEST_QUALITY, 1);

    out.resize(data.output_frames_gen);

    return std::vector<double>(out.begin(), out.end());
}