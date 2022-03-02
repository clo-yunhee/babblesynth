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

#ifndef BABBLESYNTH_ABSTRACT_SOURCE_H
#define BABBLESYNTH_ABSTRACT_SOURCE_H

#include <cmath>

#include "../enumeration.h"
#include "../parameter_holder.h"
#include "../variable.h"

namespace babblesynth {
namespace source {

class abstract_source : public parameter_holder {
   public:
    virtual ~abstract_source() = default;

    virtual double evaluateAtPhase(double theta) = 0;

   protected:
    abstract_source();
};

extern enumeration sources;

}  // namespace source
}  // namespace babblesynth

#endif  // BABBLESYNTH_ABSTRACT_SOURCE_H