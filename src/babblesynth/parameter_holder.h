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

#ifndef BABBLESYNTH_PARAMETER_HOLDER_H
#define BABBLESYNTH_PARAMETER_HOLDER_H

#include <set>
#include <string>
#include <vector>

#include "parameter.h"

namespace babblesynth {

class parameter_holder {
   public:
    static constexpr int maxNumberOfParameters = 30;

    const std::vector<std::string> getParameterNames() const;

    parameter& getParameter(int index);
    parameter& getParameter(const std::string& name);

    const parameter& getParameter(int index) const;
    const parameter& getParameter(const std::string& name) const;

   protected:
    virtual ~parameter_holder() = default;

    parameter& addParameter(const parameter& param);

    template <typename... Ts>
    parameter& addParameter(Ts&&... args) {
        const parameter p = parameter(std::forward<Ts>(args)...);
        return addParameter(p);
    }

    virtual bool onParameterChange(const parameter& param) = 0;

   private:
    // Use a vector to preserve ordering.
    std::vector<parameter> m_parameters;
};

}  // namespace babblesynth

#endif  // BABBLESYNTH_PARAMETER_HOLDER_H