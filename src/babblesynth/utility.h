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

#ifndef BABBLESYNTH_UTILITY_H
#define BABBLESYNTH_UTILITY_H

#include <type_traits>
#include <variant>

namespace babblesynth {
namespace util {

template <typename T, typename... Rest>
struct is_any : std::false_type {};

template <typename T, typename First>
struct is_any<T, First> : std::is_same<T, First> {};

template <typename T, typename First, typename... Rest>
struct is_any<T, First, Rest...>
    : std::integral_constant<bool, std::is_same<T, First>::value ||
                                       is_any<T, Rest...>::value> {};

template <typename T, typename... Rest>
inline constexpr bool is_any_v = is_any<T, Rest...>::value;

template <typename T, typename... Types>
T get_with_numeric_conversion(const std::variant<Types...>& variant) {
    // Automatically convert between int and double (truncate from double to
    // int)
    if constexpr (std::is_same_v<T, int>) {
        if (std::holds_alternative<double>(variant)) {
            return std::get<double>(variant);
        }
    } else if constexpr (std::is_same_v<T, double>) {
        if (std::holds_alternative<int>(variant)) {
            return std::get<int>(variant);
        }
    }
    return std::get<T>(variant);
}

}  // namespace util
}  // namespace babblesynth

#endif  // BABBLESYNTH_UTILITY_H
