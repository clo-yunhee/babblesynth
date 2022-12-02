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

#ifndef SUANSHU_INTERPOLATION_NEVILLE_TABLE_H
#define SUANSHU_INTERPOLATION_NEVILLE_TABLE_H

#include "defs.h"
#include "function/OrderedPairs.h"

namespace suanshu {

class NevilleTable {
   public:
    NevilleTable(int n, const OrderedPairs& f);
    NevilleTable(const OrderedPairs& f);
    NevilleTable(int n = 0);

    void addData(const OrderedPairs& f);

    int N() const;
    dvec2 getTable() const;
    dvec x() const;

    double get(int i, int j) const;

    double evaluate(double x);

   private:
    dvec m_x;  // the abscissae
    dvec2 m_table;
    double m_lastX;  // last x evaluated
    int m_N;         // number of last data points
    int m_N0;        // last N before adding new data

    static constexpr double kAllowance = 1.2;
};

}  // namespace suanshu

#endif  // SUANSHU_INTERPOLATION_NEVILLE_TABLE_H