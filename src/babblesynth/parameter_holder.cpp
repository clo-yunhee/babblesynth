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


#include <functional>
#include <stdexcept>

#include "parameter_holder.h"

using namespace babblesynth;

const std::vector<std::string> parameter_holder::getParameterNames() const
{
    std::vector<std::string> names;
    names.reserve(m_parameters.size());

    for (const auto& parameter : m_parameters) {
        names.push_back(parameter.name());
    }
    return names;
}

parameter_holder::parameter_holder(bool isQueued)
    : m_isQueued(isQueued)
{}

parameter& parameter_holder::getParameter(int index)
{
    return m_parameters.at(index);
}

parameter& parameter_holder::getParameter(const std::string& name)
{
    for (auto& parameter : m_parameters) {
        if (parameter.name() == name) {
            return parameter;
        }
    }
    throw std::invalid_argument("No parameter found with that name");
}

const parameter& parameter_holder::getParameter(int index) const
{
    return m_parameters.at(index);
}

const parameter& parameter_holder::getParameter(const std::string& name) const
{
    for (const auto& parameter : m_parameters) {
        if (parameter.name() == name) {
            return parameter;
        }
    }
    throw std::invalid_argument("No parameter found with that name");
}

parameter& parameter_holder::addParameter(const parameter& newParam)
{
    // Expect 10 parameters at most.
    if (m_parameters.capacity() < maxNumberOfParameters) {
        m_parameters.reserve(maxNumberOfParameters);
    }

    if (m_parameters.size() == maxNumberOfParameters) {
        throw std::length_error("maximum number of parameters reached");
    }

    m_parameters.push_back(newParam);

    parameter& param = m_parameters.back();
    param.addObserver(std::bind(&parameter_holder::internalObserver, this, std::placeholders::_1));

    // Trigger an immediate event for init.
    onParameterChange(param);

    return param;
}

void parameter_holder::processEvents()
{
    if (!m_isQueued) {
        throw std::logic_error("can't process event on a non-queued parameter holder");
    }
     
    for (const auto updatedParam : m_updatedParameters) {
        onParameterChange(*updatedParam);
    }
    m_updatedParameters.clear();
}

void parameter_holder::internalObserver(const parameter& updatedParam)
{
    if (m_isQueued) {
        m_updatedParameters.emplace(&updatedParam);
    }
    else {
        onParameterChange(updatedParam);
    }
}