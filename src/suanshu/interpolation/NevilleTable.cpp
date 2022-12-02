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

#include "NevilleTable.h"

using namespace suanshu;

NevilleTable::NevilleTable(const int n, const OrderedPairs& f) {
    int size = n;
    if (size < f.size()) {
        size = (int)(f.size() * kAllowance);
    }

    m_x.resize(size);
    m_table.resize(size);
    for (auto& row : m_table) row.resize(size);

    addData(f);
}

NevilleTable::NevilleTable(const OrderedPairs& f) : NevilleTable(0, f) {}

NevilleTable::NevilleTable(const int n) {
    m_x.resize(n);
    m_table.resize(n);
    for (auto& row : m_table) row.resize(n);
}

void NevilleTable::addData(const OrderedPairs& f) {
    const auto& fx = f.x();
    const auto& fy = f.y();

    const int dsize = fx.size();
    if (m_N + dsize > m_table.size()) {  // expand table
        const int newLength = (int)((m_N + dsize) * kAllowance);

        m_x.resize(newLength);

        m_table.resize(newLength);
        for (auto& row : m_table) row.resize(newLength);
    }

    for (int i = m_N, j = 0; j < dsize; ++i, ++j) {  // add the new points
        m_x[i] = fx[j];
        m_table[i][i] = fy[j];
    }

    m_N += dsize;
}

int NevilleTable::N() const { return m_N; }

dvec2 NevilleTable::getTable() const {
    dvec2 copy(m_N);
    for (int i = 0; i < m_N; ++i) copy[i] = m_table[i];
    return copy;
}

dvec NevilleTable::x() const { return m_x; }

double NevilleTable::get(const int i, const int j) const {
    aassert(j >= i, "only the upper triangular part of the table is used");
    return m_table[i][j];
}

double NevilleTable::evaluate(const double x) {
    aassert(m_N >= 2, "not enough data points for interpolation");

    if (m_N0 == (m_N - 1) && m_lastX == x) {
        return m_table[0][m_N0];  // return last saved value
    }

    /*
     * starting from the first column after the last computed column if evaluate
     * for the same x; otherwise, redo the computation from scratch
     */
    for (int j = (m_lastX == x ? m_N0 : 1); j < m_N; ++j) {  // Col
        for (int i = j - 1; i >= 0; --i) {                   // Row
            const double pij = (x - m_x[j]) * m_table[i][j - 1] +
                               (m_x[i] - x) * m_table[i + 1][j];

            const double divisor = m_x[i] - m_x[j];
            if (divisor == 0) {
                throw std::logic_error("xi and xj are identical");
            }

            m_table[i][j] = pij / divisor;
        }
    }

    // save the states
    m_lastX = x;
    m_N0 = m_N - 1;

    return m_table[0][m_N0];
}