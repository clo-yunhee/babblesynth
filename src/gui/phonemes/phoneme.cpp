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

#include "phoneme.h"

using namespace babblesynth::gui::phonemes;
using namespace xercesc;

Phoneme::Phoneme(const XMLCh* name) : m_name(XMLString::replicate(name)) {}

Phoneme::~Phoneme() { XMLString::release(&m_name); }

const XMLCh* Phoneme::name() const { return m_name; }

void Phoneme::addPole(const double frequency, const double bandwidth,
                      const int i) {
    const int ri = i >= 0 ? i : m_poles.size();
    m_poles.push_back({ri, frequency, bandwidth});
}

void Phoneme::addZero(const double frequency, const double bandwidth,
                      const int i) {
    const int ri = i >= 0 ? i : m_zeros.size();
    m_zeros.push_back({ri, frequency, bandwidth});
}