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


#include <stdexcept>

#include "variable_plan.h"
#include "spline.h"

using namespace babblesynth;

variable_plan::variable_plan(double initialValue)
    : m_spline(new tk::spline)
{
    m_times.push_back(0);
    m_values.push_back(initialValue);
}

variable_plan::variable_plan(const variable_plan& orig)
    : m_times(orig.m_times),
      m_values(orig.m_values),
      m_transitions(orig.m_transitions),
      m_spline(new tk::spline)
{
    ((tk::spline *) m_spline)->set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0);
    ((tk::spline *) m_spline)->set_points(m_times, m_values, tk::spline::cspline_hermite);
}

variable_plan::~variable_plan()
{
    delete (tk::spline *) m_spline;
}

variable_plan& variable_plan::operator=(const variable_plan& orig)
{
    delete (tk::spline *) m_spline;

    m_times = orig.m_times;
    m_values = orig.m_values;
    m_transitions = orig.m_transitions;
    m_spline = new tk::spline;

    ((tk::spline *) m_spline)->set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0);
    ((tk::spline *) m_spline)->set_points(m_times, m_values, tk::spline::cspline_hermite);

    return *this;
}

variable_plan& variable_plan::stepToValueAtTime(double value, double time)
{
    addPoint(time, value, TransitionStep);
    return *this;
}

variable_plan& variable_plan::linearToValueAtTime(double value, double time)
{
    addPoint(time, value, TransitionLinear);
    return *this;
}

variable_plan& variable_plan::cubicToValueAtTime(double value, double time)
{
    addPoint(time, value, TransitionCubic);
    return *this;
}

variable_plan& variable_plan::reset(double initialValue)
{
    m_times.clear();
    m_values.clear();
    m_transitions.clear();
    m_times.push_back(0);
    m_values.push_back(initialValue);
    return *this;
}

double variable_plan::evaluateAtTime(double time) const
{
    int leftIndex = 0;
    while (m_times[leftIndex] < time) {
        ++leftIndex;
    }
    leftIndex -= 1;

    if (leftIndex < 0) {
        return m_values.front();
    }
    else if (leftIndex >= m_values.size() - 1) {
        return m_values.back();
    }

    switch (m_transitions[leftIndex]) {
    case TransitionStep:
        return interpolateStep(leftIndex, time);
    case TransitionLinear:
        return interpolateLinear(leftIndex, time);
    case TransitionCubic:
        return interpolateCubic(leftIndex, time);
    default:
        throw std::invalid_argument("unknown transition type for variable plan");
    }
}

double variable_plan::duration() const
{
    return m_times.back();
}

void variable_plan::addPoint(double time, double value, transition trans)
{
    m_times.push_back(time);
    m_values.push_back(value);
    m_transitions.push_back(trans);
    ((tk::spline *) m_spline)->set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0);
    ((tk::spline *) m_spline)->set_points(m_times, m_values, tk::spline::cspline_hermite);
    //m_spline.make_monotonic();
}

double variable_plan::interpolateStep(int index, double time) const
{
    return m_values[index + 1];
}

double variable_plan::interpolateLinear(int index, double time) const
{
    const double T0 = m_times[index];
    const double V0 = m_values[index];

    const double T1 = m_times[index + 1];
    const double V1 = m_values[index + 1];

    return V0 + (V1 - V0) * (time - T0) / (T1 - T0);
}

double variable_plan::interpolateCubic(int index, double time) const
{
    return (*(tk::spline *) m_spline)(time);
}