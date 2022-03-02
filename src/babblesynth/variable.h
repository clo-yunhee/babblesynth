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

#ifndef BABBLESYNTH_VARIABLE_H
#define BABBLESYNTH_VARIABLE_H

#include "variable_plan.h"

namespace babblesynth {

class variable {
   public:
    explicit variable(bool isTrulyContinuous = true);

    void setPlan(const variable_plan& plan);

    void update(double time);  // used in the ^isTrulyContinuous case to update
                               // the value

    double evaluateAtTime(double time) const;

    double maxTime() const;

   private:
    bool m_isTrulyContinuous;
    double m_actualValue;  // only used in the ^isTrulyContinuous case
    variable_plan m_plan;
};

}  // namespace babblesynth

#endif  // BABBLESYNTH_VARIABLE_H