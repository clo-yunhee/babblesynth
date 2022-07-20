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

#ifndef BABBLESYNTH_PHONEMES_XMLWSTR_H
#define BABBLESYNTH_PHONEMES_XMLWSTR_H

#include <QString>
#include <memory>
#include <ostream>
#include <string_view>
#include <xercesc/util/XMLString.hpp>

class XMLWStr {
   public:
    XMLWStr();
    XMLWStr(const XMLCh *);
    XMLWStr(const char *);
    XMLWStr(const std::string &);
    XMLWStr(const QString &);
    virtual ~XMLWStr();

    XMLWStr(const XMLWStr &);
    XMLWStr &operator=(const XMLWStr &);

    XMLWStr(XMLWStr &&) = default;
    XMLWStr &operator=(XMLWStr &&) = default;

    int length() const;

    bool startsWith(const XMLWStr &prefix, int offset = 0) const;

    XMLWStr substring(int start, int end) const;
    int indexOf(const XMLWStr &pattern) const;

    XMLWStr replace(const XMLWStr &pattern, const XMLWStr &replacement) const;

    template <typename T>
    inline bool operator==(const T &other) const {
        static_assert(
            std::is_same_v<XMLWStr, T> || std::is_constructible_v<XMLWStr, T>,
            "RHS must be XMLWStr-constructible");

        if constexpr (std::is_same_v<XMLWStr, T>) {
            return xercesc::XMLString::equals(m_string.get(),
                                              other.m_string.get());
        } else if constexpr (std::is_constructible_v<XMLWStr, T>) {
            return *this == XMLWStr(other);
        }
    }

    template <typename T>
    inline XMLWStr operator+(const T &rhs) const {
        static_assert(
            std::is_same_v<XMLWStr, T> || std::is_constructible_v<XMLWStr, T>,
            "RHS must be XMLWStr-constructible");

        if constexpr (std::is_same_v<XMLWStr, T>) {
            XMLWStr result(length() + rhs.length());
            xercesc::XMLString::copyString(result.m_string.get(),
                                           m_string.get());
            xercesc::XMLString::catString(result.m_string.get(),
                                          rhs.m_string.get());
            return result;
        } else if constexpr (std::is_constructible_v<XMLWStr, T>) {
            return *this + XMLWStr(rhs);
        }
    }

    template <typename T>
    inline bool operator!=(const T &other) const {
        return !(*this == other);
    }

    inline const XMLCh *xmlch() const { return m_string.get(); }

    inline operator const XMLCh *() const { return xmlch(); }

    inline operator QString() const {
        return QString::fromUtf16(m_string.get());
    }

    friend std::ostream &operator<<(std::ostream &os, const XMLWStr &);

   private:
    XMLWStr(int length);

    std::unique_ptr<XMLCh[]> m_string;
    int m_length;
};

/* String literal. */
inline XMLWStr operator"" _x(const char *string, size_t) { return string; }

/* Hashing and comparison overloads. */
namespace std {
template <>
struct hash<XMLWStr> {
    // Use the std::hash implementation with basic_string_view
    inline size_t operator()(const XMLWStr &string) const {
        return m_hash(string.xmlch());
    }

   private:
    std::hash<std::basic_string_view<XMLCh>> m_hash;
};

template <>
struct less<XMLWStr> {
    inline bool operator()(const XMLWStr &lhs, const XMLWStr &rhs) const {
        return m_less(lhs.xmlch(), rhs.xmlch());
    }

   private:
    std::less<std::basic_string_view<XMLCh>> m_less;
};
}  // namespace std

#endif  // BABBLESYNTH_PHONEMES_XMLWSTR_H
