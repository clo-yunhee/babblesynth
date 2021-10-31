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

#ifndef BABBLESYNTH_PARAMETER_H
#define BABBLESYNTH_PARAMETER_H

#include <functional>
#include <optional>
#include <string>
#include <variant>

#include "enumeration.h"
#include "utility.h"
#include "variable_plan.h"

namespace babblesynth {

class parameter {
   public:
    using value_type =
        std::variant<std::monostate, int, double, bool, std::string,
                     enumeration_value, variable_plan>;
    using observer_type = std::function<bool(const parameter&)>;

    explicit parameter(const std::string& name) : m_name(name) {}

    template <typename T>
    explicit parameter(const std::string& name, const T& initialValue)
        : m_name(name), m_value(initialValue) {}

    const std::string& name() const { return m_name; }

    const char* type() const {
        if (std::holds_alternative<int>(m_value)) {
            return "int";
        } else if (std::holds_alternative<double>(m_value)) {
            return "double";
        } else if (std::holds_alternative<bool>(m_value)) {
            return "bool";
        } else if (std::holds_alternative<std::string>(m_value)) {
            return "string";
        } else if (std::holds_alternative<enumeration_value>(m_value)) {
            return "enum";
        } else if (std::holds_alternative<variable_plan>(m_value)) {
            return "var_plan";
        } else {
            return "<unknown type>";
        }
    }

    template <typename T>
    T value() const {
        return util::get_with_numeric_conversion<T>(m_value);
    }

    template <typename T>
    std::optional<T> setValue(const T& value) {
        auto old = m_value;
        m_value = value;

        for (const auto& observer : m_observers) {
            if (!observer(*this)) {
                T oldValue = util::get_with_numeric_conversion<T>(old);
                setValue(oldValue);
                return oldValue;
            }
        }

        return std::nullopt;
    }

    bool isRanged() const { return m_min.index() != 0 || m_max.index() != 0; }

    template <typename T>
    std::enable_if_t<util::is_any_v<T, int, double>, T> min() const {
        return util::get_with_numeric_conversion<T>(m_min);
    }

    template <typename T>
    std::enable_if_t<util::is_any_v<T, int, double>, parameter&> setMin(
        const T& min) {
        m_min = min;
        return *this;
    }

    template <typename T>
    std::enable_if_t<util::is_any_v<T, int, double>, T> max() const {
        return util::get_with_numeric_conversion<T>(m_max);
    }

    template <typename T>
    std::enable_if_t<util::is_any_v<T, int, double>, parameter&> setMax(
        const T& max) {
        m_max = max;
        return *this;
    }

    template <typename T>
    void addObserver(T observer) {
        m_observers.emplace_back(observer);
    }

   private:
    std::string m_name;
    value_type m_value;
    value_type m_min;
    value_type m_max;

    std::vector<observer_type> m_observers;
};

}  // namespace babblesynth

#endif  // BABBLESYNTH_PARAMETER_H
