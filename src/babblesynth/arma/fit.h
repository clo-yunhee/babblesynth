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

#ifndef BABBLESYNTH_ARMA_FIT_H
#define BABBLESYNTH_ARMA_FIT_H

#include <vector>

namespace babblesynth {
namespace arma {

struct model {
    std::vector<double> ar;
    std::vector<double> ma;
};

model fit(const std::vector<double>& x, int arTerms, int maTerms,
          double epsLimit = 1e-3, int maxSteps = 30);

std::vector<double> fitMaOnly(const std::vector<double>& x, int maTerms,
                              double epsLimit = 1e-3, int maxSteps = 30);

std::vector<double> fitArOnly(const std::vector<double>& x, int arTerms);

}  // namespace arma
}  // namespace babblesynth

#endif  // BABBLESYNTH_ARMA_FIT_H
