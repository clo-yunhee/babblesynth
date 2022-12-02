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

#include "BurgYule.h"

using namespace suanshu;

static Vector detailBurg(const Eigen::Ref<const Vector>& x, const int m,
                         double* xms) {
    const int n = x.size();

    Vector b1(n + 1);
    Vector b2(n + 1);
    Vector aa(m + 1);

    Vector a(m + 1);

    int i, j;

    double p = 0.0;
    for (j = 1; j <= n; ++j) p += x(j - 1) * x(j - 1);

    *xms = p / n;
    if (*xms <= 0.0) {
        return a;
    }

    b1(1) = x(0);
    b2(n - 1) = x(n - 1);
    for (j = 2; j <= n - 1; ++j) b1(j) = b2(j - 1) = x(j - 1);

    for (i = 1; i <= m; ++i) {
        double num = 0.0, denum = 0.0;
        for (j = 1; j <= n - i; ++j) {
            num += b1(j) * b2(j);
            denum += b1(j) * b1(j) + b2(j) * b2(j);
        }

        if (denum <= 0.0) {
            *xms = 0.0;
            return a;
        }

        a(i) = 2.0 * num / denum;

        *xms *= 1.0 - a(i) * a(i);

        for (j = 1; j <= i - 1; ++j) a(j) = aa(j) - a(i) * aa(i - j);

        if (i < m) {
            for (j = 1; j <= i; ++j) aa(j) = a(j);
            for (j = 1; j <= n - i - 1; ++j) {
                b1(j) -= aa(i) * b2(j);
                b2(j) = b2(j + 1) - aa(i) * b1(j + 1);
            }
        }
    }

    return a;
}

static Vector burg(const Eigen::Ref<const Vector>& x, const int m) {
    double xms;
    Vector lpca = detailBurg(x, m, &xms);

    if (xms <= 0) {
        lpca.setZero();
    }

    return -lpca.tail(m);
}

static Vector lfilter(const Eigen::Ref<const Vector>& b,
                      const Eigen::Ref<const Vector>& a,
                      const Eigen::Ref<const Vector>& x) {
    /* assume it's already normalized */

    const int lx = x.size();
    const int la = a.size();
    const int lb = b.size();
    const int lab = std::max(la, lb);

    Vector y(lx);
    Vector z = Vector::Zero(lab - 1);

    for (int k = 0; k < lx; ++k) {
        if (lab > 1) {
            y(k) = z(0) + b(0) * x(k);
            for (int n = 0; n < lab - 2; ++n) {
                z(n) = z(n + 1) + x(k) * b(n + 1) - y(k) * a(n + 1);
            }
            z(lab - 2) = x(k) * b(lab - 1) - y(k) * a(lab - 1);
        } else {
            y(k) = x(k) * b(0);
        }
    }

    return y;
}

/* mode = 'same', unbias = False */
static Vector maYuleWalker(const Eigen::Ref<const Vector>& x,
                           const int polesOrder, const int zerosOrder) {}

ARMAModel suanshu::FitBurgYule(const dvec& xv, const int polesOrder,
                               const int zerosOrder) {
    Vector x = Eigen::Map<const Vector>(xv.data(), xv.size());
    Vector a = burg(x, polesOrder);

    if (zerosOrder == 0) {
        return ARMAModel(stlVector(a), {});
    }

    // lfilter([1], a[1:], x)[poles_order:]
    Vector filtb = Vector::Zero(a.size());
    filtb(0) = 1;

    Vector noises = lfilter(filtb, -a, x).tail(x.size() - polesOrder);
    Vector b = burg(noises, 2 * zerosOrder);

    return ARMAModel(stlVector(a), stlVector(b));
}