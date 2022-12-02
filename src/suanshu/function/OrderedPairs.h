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

#ifndef SUANSHU_FUNCTION_ORDERED_PAIRS_H
#define SUANSHU_FUNCTION_ORDERED_PAIRS_H

#include <utility>
#include <vector>

#include "defs.h"

namespace suanshu {

class OrderedPairs {
   public:
    using storage_type = std::pair<std::vector<double>, std::vector<double>>;

    OrderedPairs(const storage_type& data) : m_data(data) {
        aassert(data.first.size() == data.second.size(),
                "ordered pairs unequal number");
    }

    OrderedPairs(std::initializer_list<double> left,
                 std::initializer_list<double> right)
        : OrderedPairs(std::make_pair(left, right)) {}

    const std::vector<double>& x() const { return m_data.first; }
    const std::vector<double>& y() const { return m_data.second; }
    int size() const { return m_data.first.size(); }

   private:
    storage_type m_data;
};

}  // namespace suanshu

#endif  // SUANSHU_FUNCTION_ORDERED_PAIRS_H