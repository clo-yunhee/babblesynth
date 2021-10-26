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


#ifndef BABBLESYNTH_BABBLESYNTH_H
#define BABBLESYNTH_BABBLESYNTH_H

// Contains general C/C++ utility constructs.
#include "utility.h"

// Defines a custom enumeration type to allow for run-time introspection.
#include "enumeration.h"

// Defines a parameter type that holds a name, a value, and an optional value range (min/max).
#include "parameter.h"

// Defines a variable with special behaviour for continuous variation.
#include "variable.h"

// Defines a transition for a variable, with start and end points.
#include "variable.h"

// Defines a plan for a variable, with a sequence of transitions.
#include "variable_plan.h"

// Defines glottal source types.
#include "source/abstract_source.h"

// Defines a glottal source generator.
#include "generator/source_generator.h"

// Defines a formant filter.
#include "filter/formant_filter.h"

#endif // BABBLESYNTH_BABBLESYNTH_H
