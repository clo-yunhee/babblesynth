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

#ifndef BABBLESYNTH_FZERO_H
#define BABBLESYNTH_FZERO_H

#include <array>
#include <cmath>
#include <limits>

#ifdef __has_cpp_attribute
#if __has_cpp_attribute(const)
#define ATTR_CONST __attribute__((const))
#endif
#endif
#ifndef ATTR_CONST
#define ATTR_CONST
#endif

namespace babblesynth {

template <typename T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

template <typename Func>
double fzero(double a, double b, ATTR_CONST Func f,
             double tol = std::numeric_limits<double>::epsilon()) {
    double fa, fb, fc, fs;
    double c, s, d;
    bool mflag;

    fa = f(a);
    fb = f(b);
    fs = fa;

    if (sgn(fa) * sgn(fb) >= 0) {
        // Exit function because the root is not bracketed.
        return NAN;
    }

    if (std::abs(fa) < std::abs(fb)) {
        std::swap(a, b);
        std::swap(fa, fb);
    }

    c = a;
    fc = fa;
    mflag = true;
    // while (fb != 0 && fs != 0 && std::abs(b - a) >= tol) {
    //     if (fa != fc && fb != fc) {
    while (std::abs(fb) >= tol && std::abs(fs) >= tol &&
           std::abs(b - a) >= tol) {
        if (std::abs(fa - fc) < tol * std::abs(fc) &&
            std::abs(fb - fc) < tol * std::abs(fc)) {
            // Inverse quadratic interpolation
            s = (a * fb * fc) / ((fa - fb) * (fa - fc)) +
                (b * fa * fc) / ((fb - fa) * (fb - fc)) +
                (c * fa * fb) / ((fc - fa) * (fc - fb));
        } else {
            // Secant method
            s = b - (fb * (b - a)) / (fb - fa);
        }

        if (!((3 * a + b) / 4 <= s && s <= b) ||
            (mflag && std::abs(s - b) >= std::abs(b - c) / 2) ||
            (!mflag && std::abs(s - b) >= std::abs(c - d) / 2) ||
            (mflag && std::abs(b - c) < tol) ||
            (!mflag && std::abs(c - d) < tol)) {
            // Bisection method
            s = (a + b) / 2;
            mflag = true;
        } else {
            mflag = false;
        }
        fs = f(s);
        // d is assigned for the first time here; it won't be used above on
        // the first iteration between mflag is set
        d = c;
        c = b;
        fc = fb;
        if (sgn(fa) * sgn(fs) < 0) {
            b = s;
            fb = fs;
        } else {
            a = s;
            fa = fs;
        }
        if (std::abs(fa) < std::abs(fb)) {
            std::swap(a, b);
            std::swap(fa, fb);
        }
    }

    return (std::abs(fb) < std::abs(fs)) ? b : s;
}

template <typename Func>
double fzero(double a, ATTR_CONST Func f,
             double tol = std::numeric_limits<double>::epsilon()) {
    // Try to find a value for b which brackets a zero-crossing.
    const double fa = f(a);

    // For very small values, switch to std::absolute rather than relative
    // search
    const double aa =
        (std::abs(a) < 0.001) ? ((a == 0) ? 0.1 : sgn(a) * 0.1) : a;

    // Search in an ever-widening range around the initial point.
    std::array deltas{-.01, +.025, -.05, +.10, -.25,  +.50,  -1.,
                      +2.5, -5.,   +10., -50., +100., -500., +1000.};

    double b;
    double fb;
    for (double srch : deltas) {
        b = aa + aa * srch;
        fb = f(b);
        if (sgn(fa) * sgn(fb) <= 0) {
            break;
        }
    }

    return fzero(a, b, f, tol);
}

}  // namespace babblesynth

#endif  // BABBLESYNTH_FZERO_H
