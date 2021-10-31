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

#include "variable.h"

using namespace babblesynth;

variable::variable(bool isTrulyContinuous)
    : m_isTrulyContinuous(isTrulyContinuous), m_actualValue(0), m_plan(0) {}

void variable::setPlan(const variable_plan &plan) { m_plan = plan; }

void variable::update(double time) {
    m_actualValue = m_plan.evaluateAtTime(time);
}

double variable::evaluateAtTime(double time) const {
    if (m_isTrulyContinuous) {
        return m_plan.evaluateAtTime(time);
    } else {
        return m_actualValue;
    }
}

double variable::maxTime() const { return m_plan.duration(); }