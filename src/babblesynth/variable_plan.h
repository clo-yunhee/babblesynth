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


#ifndef BABBLESYNTH_VARIABLE_PLAN_H
#define BABBLESYNTH_VARIABLE_PLAN_H

#include <vector>

namespace babblesynth {

class variable_plan {
public:
    enum transition {
        TransitionStep,
        TransitionLinear,
        TransitionCubic,
    };

    explicit variable_plan(bool piecewiseMonotonic = false, double initialValue = 0);
    variable_plan(const variable_plan& orig);
    ~variable_plan();

    variable_plan& operator=(const variable_plan& orig);

    variable_plan& stepToValueAtTime(double value, double time);
    variable_plan& linearToValueAtTime(double value, double time);
    variable_plan& cubicToValueAtTime(double value, double time);
    
    variable_plan& reset(double initialValue);

    double evaluateAtTime(double time) const;

    double duration() const;

private:
    void addPoint(double time, double value, transition trans);
    void updateSpline();

    double interpolateStep(int index, double time) const;
    double interpolateLinear(int index, double time) const;
    double interpolateCubic(int index, double time) const;

    bool m_isPiecewiseMonotonic;

    std::vector<double> m_times;
    std::vector<double> m_values;
    std::vector<transition> m_transitions;

    // tk::spline m_spline; // Moved to the cpp file because anonymous namespace
    void *m_spline;
};

} // babblesynth

#endif // BABBLESYNTH_VARIABLE_PLAN_H