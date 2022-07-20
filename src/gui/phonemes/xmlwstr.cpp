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

#include "xmlwstr.h"

#include <xercesc/util/XMLUniDefs.hpp>

using namespace xercesc;

inline std::unique_ptr<XMLCh[]> copy(const XMLCh *string) {
    const int length = XMLString::stringLen(string);
    std::unique_ptr<XMLCh[]> ptr(new XMLCh[length + 1]);
    XMLString::copyString(ptr.get(), string);
    return std::move(ptr);
}

inline int strLength(const XMLCh *string) {
    return XMLString::stringLen(string);
}

XMLWStr::XMLWStr() : XMLWStr("") {}

XMLWStr::XMLWStr(const XMLCh *string)
    : m_string(copy(string)), m_length(strLength(m_string.get())) {}

XMLWStr::XMLWStr(const char *string) {
    XMLCh *trans = XMLString::transcode(string);
    m_string = copy(trans);
    m_length = strLength(m_string.get());
    XMLString::release(&trans);
}

XMLWStr::XMLWStr(const std::string &string) : XMLWStr(string.c_str()) {}

XMLWStr::XMLWStr(const QString &string)
    : m_string(copy(reinterpret_cast<const XMLCh *>(string.utf16()))),
      m_length(strLength(m_string.get())) {}

XMLWStr::XMLWStr(const XMLWStr &other)
    : m_string(copy(other.m_string.get())),
      m_length(strLength(m_string.get())) {}

XMLWStr::XMLWStr(const int length)
    : m_string(new XMLCh[length + 1]), m_length(length) {
    // Default fill to spaces
    for (int i = 0; i < length; ++i) m_string[i] = chSpace;
    m_string[length] = 0;
}

XMLWStr::~XMLWStr() {}

XMLWStr &XMLWStr::operator=(const XMLWStr &other) {
    m_string = copy(other.m_string.get());
    return *this;
}

int XMLWStr::length() const { return m_length; }

bool XMLWStr::startsWith(const XMLWStr &prefix, const int offset) const {
    return XMLString::startsWithI(m_string.get() + offset,
                                  prefix.m_string.get());
}

XMLWStr XMLWStr::substring(const int start, const int end) const {
    XMLWStr sub(end - start);
    XMLString::subString(sub.m_string.get(), m_string.get(), start, end);
    return sub;
}

int XMLWStr::indexOf(const XMLWStr &pattern) const {
    return XMLString::patternMatch(m_string.get(), pattern.m_string.get());
}

XMLWStr XMLWStr::replace(const XMLWStr &pattern,
                         const XMLWStr &replacement) const {
    const int startIndex = indexOf(pattern);
    if (startIndex < 0) return *this;

    const int len = length();
    const int patLen = pattern.length();
    const int repLen = replacement.length();

    XMLWStr result(len - patLen + repLen);

    if (startIndex > 0) {
        std::copy(&m_string[0], &m_string[startIndex - 1], &result.m_string[0]);
    }

    std::copy(&replacement.m_string[0], &replacement.m_string[repLen - 1],
              &result.m_string[startIndex]);

    if (startIndex + patLen < len - 1) {
        std::copy(&m_string[startIndex + patLen], &m_string[len - 1],
                  &result.m_string[startIndex + repLen]);
    }

    m_string[len] = 0;

    return result;
}

std::ostream &operator<<(std::ostream &os, const XMLWStr &string) {
    char *cstr = XMLString::transcode(string.m_string.get());
    os << cstr;
    XMLString::release(&cstr);
    return os;
}
