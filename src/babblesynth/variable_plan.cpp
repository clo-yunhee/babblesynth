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

#include "variable_plan.h"

#include <numeric>
#include <stdexcept>

#include "spline.h"

using namespace babblesynth;

variable_plan::variable_plan(bool piecewiseMonotonic, double initialValue)
    : m_isPiecewiseMonotonic(piecewiseMonotonic),
      m_times({0.0}),
      m_values({initialValue}),
      m_transitions({TransitionLinear, TransitionLinear}),
      m_spline(nullptr) {
    updateSpline();
}

variable_plan::variable_plan(const variable_plan& orig)
    : m_isPiecewiseMonotonic(orig.m_isPiecewiseMonotonic),
      m_times(orig.m_times),
      m_values(orig.m_values),
      m_transitions(orig.m_transitions),
      m_spline(nullptr) {
    updateSpline();
}

variable_plan::~variable_plan() {
    if (m_spline != nullptr) {
        delete (tk::spline*)m_spline;
    }
}

variable_plan& variable_plan::operator=(const variable_plan& orig) {
    if (m_spline != nullptr) {
        delete (tk::spline*)m_spline;
        m_spline = nullptr;
    }

    m_isPiecewiseMonotonic = orig.m_isPiecewiseMonotonic;
    m_times = orig.m_times;
    m_values = orig.m_values;
    m_transitions = orig.m_transitions;
    updateSpline();
    return *this;
}

variable_plan& variable_plan::stepToValueAtTime(double value, double time) {
    addPoint(time, value, TransitionStep);
    return *this;
}

variable_plan& variable_plan::linearToValueAtTime(double value, double time) {
    addPoint(time, value, TransitionLinear);
    return *this;
}

variable_plan& variable_plan::cubicToValueAtTime(double value, double time) {
    addPoint(time, value, TransitionCubic);
    return *this;
}

variable_plan& variable_plan::reset(double initialValue) {
    m_times.clear();
    m_values.clear();
    m_transitions.clear();
    m_times.push_back(0);
    m_values.push_back(initialValue);
    updateSpline();
    return *this;
}

double variable_plan::evaluateAtTime(double time) const {
    int leftIndex = 0;
    while (leftIndex < m_times.size() && m_times[leftIndex] < time) {
        ++leftIndex;
    }
    leftIndex -= 1;

    double value = 0;

    if (leftIndex < 0) {
        value = m_values.front();
    } else if (leftIndex >= m_values.size() - 1) {
        value = m_values.back();
    } else {
        switch (m_transitions[leftIndex]) {
            case TransitionStep:
                value = interpolateStep(leftIndex, time);
                break;
            case TransitionLinear:
                value = interpolateLinear(leftIndex, time);
                break;
            case TransitionCubic:
                value = interpolateCubic(leftIndex, time);
                break;
            default:
                throw std::invalid_argument(
                    "unknown transition type for variable plan");
        }
    }

    if (value <= 0) {
        value = 1e-6;
    }

    return value;
}

double variable_plan::duration() const { return m_times.back(); }

void variable_plan::addPoint(double time, double value, transition trans) {
    m_times.push_back(time);
    m_values.push_back(value);
    m_transitions.push_back(trans);
    updateSpline();
}

void variable_plan::updateSpline() {
    if (m_spline != nullptr) {
        delete (tk::spline*)m_spline;
    }

    if (m_times.size() >= 3) {
        m_spline =
            new tk::spline(m_times, m_values, tk::spline::cspline_hermite,
                           m_isPiecewiseMonotonic, tk::spline::first_deriv, 0.0,
                           tk::spline::first_deriv, 0.0);
    } else {
        m_spline = nullptr;
    }
}

double variable_plan::interpolateStep(int index, double time) const {
    return m_values[index + 1];
}

double variable_plan::interpolateLinear(int index, double time) const {
    const double T0 = m_times[index];
    const double V0 = m_values[index];

    const double T1 = m_times[index + 1];
    const double V1 = m_values[index + 1];

    return V0 + (V1 - V0) * (time - T0) / (T1 - T0);
}

double variable_plan::interpolateCubic(int index, double time) const {
    if (m_spline != nullptr) {
        return (*(tk::spline*)m_spline)(time);
    } else {
        return interpolateLinear(index, time);
    }
}