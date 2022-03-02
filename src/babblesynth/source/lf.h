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

#ifndef BABBLESYNTH_LF_H
#define BABBLESYNTH_LF_H

#include "abstract_source.h"

namespace babblesynth {
namespace source {

class lf : public abstract_source {
   public:
    lf();

    double evaluateAtPhase(double theta) override;

   private:
    bool calculateModelParameters();

    bool onParameterChange(const parameter& param) override;

    static constexpr double T0 = 1;
    static constexpr double E = 1;

    double Oq;
    double am;
    double Qa;

    double Ee;  // = E
    double Te;  // = Oq * T0
    double Tp;  // = am * Oq * T0
    double Ta;  // = Qa * (1 - Oq) * T0

    double alpha;
    double epsilon;
};

}  // namespace source
}  // namespace babblesynth

#endif  // BABBLESYNTH_LF_H