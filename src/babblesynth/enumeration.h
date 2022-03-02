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

#ifndef BABBLESYNTH_ENUMERATION_H
#define BABBLESYNTH_ENUMERATION_H

#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "utility.h"

namespace babblesynth {

class enumeration_value {
   public:
    explicit enumeration_value(const int index, const std::string& name)
        : m_index(index), m_name(name) {}

    int index() const { return m_index; }

    const std::string& name() const { return m_name; }

   private:
    int m_index;
    std::string m_name;
};

class enumeration {
   public:
    using value_type = enumeration_value;

    template <class... T>
    explicit enumeration(const T&... values) : m_count(sizeof...(T)) {
        const std::array names = {std::string(values...)};

        m_values.reserve(m_count);
        m_indices.reserve(m_count);

        for (int i = 0; i < m_count; ++i) {
            m_values.emplace_back(i, names[i]);
            m_indices.emplace(names[i], i);
        }
    }

    const int count() { return m_count; }

    const enumeration_value& valueOf(int index) { return m_values.at(index); }

    const std::string& nameOf(int index) { return valueOf(index).name(); }

    const int indexOf(const std::string& name) { return m_indices.at(name); }

    const enumeration_value& valueOf(const std::string& name) {
        return valueOf(indexOf(name));
    }

    const std::vector<enumeration_value>& values() { return m_values; }

   private:
    int m_count;
    std::vector<enumeration_value> m_values;
    std::unordered_map<std::string, int> m_indices;
};

}  // namespace babblesynth

#endif  // BABBLESYNTH_ENUMERATION_H
