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


#include <algorithm>

#include "sos_filter.h"

using namespace babblesynth;

static decltype(auto) cplxpair(const std::vector<std::complex<double>>& v_, double tol = 100.0 * std::numeric_limits<double>::epsilon())
{
    const int l = v_.size();
    auto v = v_;

    std::vector<std::complex<double>> y(l, 0);

    // Find real values and put them (sorted) at the end of y
    std::vector<double> reals;
    for (auto it = v.begin(); it != v.end(); ++it) {
        const auto& z = *it;
        if (std::abs(z.imag()) <= tol * std::abs(z)) {
            reals.push_back(z.real());
            it = v.erase(it);
        }
        else {
            ++it;
        }
    }
    if (!reals.empty()) {
        std::sort(reals.begin(), reals.end());
        std::copy(reals.begin(), reals.end(), std::prev(y.end(), reals.size()));
    }

    // remaining values are all complex, if any
    const int nv = v.size();
    if (nv > 0) {
        if (nv % 2 == 1) {
            goto error;
        }

        // Sort v based on real part
        std::sort(v.begin(), v.end(), [](auto x, auto y) { return x.real() < y.real(); });
        
        // Check if real parts occur in pairs. If not: error
        for (int i = 0; i < v.size() / 2; ++i) {
            if (std::abs(v[2 * i].real() - v[2 * i + 1].real()) > tol * std::abs(v[2 * i])) {
                goto error;
            }
        }

        // Check if imag part of real part pairs are conjugates
        int yix = 0;
        while (v.size() > 0) {

            // Find all real parts equal to real(v[0])
            std::vector<int> idx;
            for (int i = 0; i < v.size(); ++i) {
                if (std::abs(v[i].real() - v[0].real()) <= tol * std::abs(v[i].real())) {
                    idx.push_back(i);
                }
            }
            const int nn = idx.size();
            if (nn <= 1) {
                goto error;
            }

            // Sort the imag parts of those values
            std::sort(idx.begin(), idx.end(), [&v](int i, int j) { return v[i].imag() < v[j].imag(); });
            std::vector<double> si;
            std::vector<std::complex<double>> q;
            for (int i : idx) {
                si.push_back(v[i].imag());
                q.push_back(v[i]); 
            }
            const int lq = q.size();

            // Verify conjugate-pairing of imag parts
            for (int i = 0; i < lq; ++i) {
                if (std::abs(si[i] + si[lq - 1 - i]) > tol * std::abs(q[i])) {
                    goto error;
                }
            }

            // Keep value with positive imag part, and compute conjugate
            // Value with smallest neg imag part first, then its conj

            y.resize(y.size() + nn);
            for (int i = lq - 1; i >= nn / 2; --i) {
                y[yix + i] = std::conj(q[i]);
                y[yix + nn / 2 + i] = q[i];
            }

            yix += nn;

            std::vector<std::complex<double>> newv;
            for (int i = 0; i < v.size(); ++i) {
                if (std::find(idx.begin(), idx.end(), i) == idx.end()) {
                    newv.push_back(v[i]);
                }
            }
            v = newv;
        }
    }

    return y;

error:
    throw std::invalid_argument("cplxpair: could not pair all complex numbers");
}

static std::pair<std::vector<std::complex<double>>, std::vector<double>> cplxreal(const std::vector<std::complex<double>>& z, double tol = 100.0 * std::numeric_limits<double>::epsilon())
{
    auto y = cplxpair(z, tol);

    std::vector<std::complex<double>> zc;
    std::vector<double> zr;

    for (const auto& z : y) {
        if (std::abs(z.imag()) <= tol * std::abs(z)) {
            zr.push_back(z.real());
        }
        else {
            // only pos imag numbers
            if (z.imag() >= 0) {
                zc.push_back(z);
            }
        }
    }

    return std::make_pair(zc, zr);
}

std::vector<std::array<double, 6>> filter::zpk2sos(
    const std::vector<std::complex<double>> &z,
    const std::vector<std::complex<double>> &p,
    double k)
{
    auto [zc, zr] = cplxreal(z, 1e-6);
    auto [pc, pr] = cplxreal(p, 1e-6);

    const int nzc = zc.size();
    const int npc = pc.size();

    int nzr = zr.size();
    int npr = pr.size();

    // Pair up real zeroes.
    int nzrsec = 0;
    std::vector<double> zrms, zrp;
    if (nzr > 0) {
        if (nzr % 2 == 1) {
            zr.push_back(0);
            nzr++;
        }
        nzrsec = nzr / 2;
        zrms.resize(nzrsec);
        zrp.resize(nzrsec);
        for (int i = 0; i < nzr - 1; i += 2) {
            zrms[i / 2] = -zr[i] - zr[i + 1];
            zrp[i / 2]  =  zr[i] * zr[i + 1];
        }
    }

    // Pair up real poles.
    int nprsec = 0;
    std::vector<double> prms, prp;
    if (npr > 0) {
        if (npr % 2 == 1) {
            pr.push_back(0);
            npr++;
        }
        nprsec = npr / 2;
        prms.resize(nprsec);
        prp.resize(nprsec);
        for (int i = 0; i < npr - 1; i += 2) {
            prms[i / 2] = -pr[i] - pr[i + 1];
            prp[i / 2]  =  pr[i] * pr[i + 1];
        }
    }
    else {
        nprsec = 0;
    }

    int nsecs = std::max(nzc + nzrsec, npc + nprsec);
    if (nsecs <= 0) nsecs = 1;

    std::vector<double> zcm2r(nzc), zca2(nzc);
    for (int i = 0; i < nzc; ++i) {
        zcm2r[i] = -2.0 * zc[i].real();
        double a = std::abs(zc[i]);
        zca2[i] = a * a;
    }

    std::vector<double> pcm2r(npc), pca2(npc);
    for (int i = 0; i < npc; ++i) {
        pcm2r[i] = -2.0 * pc[i].real();
        double a = std::abs(pc[i]);
        pca2[i] = a * a;
    }

    std::vector<std::array<double, 6>> sos(nsecs);
    
    for (int i = 0; i < nsecs; ++i) {
        sos[i][0] = 1.0;
        sos[i][3] = 1.0;
    }

    const int nzrl = nzc + nzrsec;
    const int nprl = npc + nprsec;

    for (int i = 0; i < nsecs; ++i) {
        if (i < nzc) {       // lay down a complex zero pair:
            sos[i][1] = zcm2r[i];
            sos[i][2] = zca2[i];
        }
        else if (i < nzrl) { // lay down a pair of real zeros:
            sos[i][1] = zrms[i - nzc];
            sos[i][2] = zrp[i - nzc];
        }
        else {
            sos[i][1] = sos[i][2] = 0.0;
        }

        if (i < npc) {       // lay down a complex pole pair:
            sos[i][4] = pcm2r[i];
            sos[i][5] = pca2[i];
        }
        else if (i < nprl) { // lay down a pair of real poles:
            sos[i][4] = prms[i - npc];
            sos[i][5] = prp[i - npc];
        }
        else {
            sos[i][4] = sos[i][5] = 0.0;
        }
    }

    std::reverse(sos.begin(), sos.end());

    if (sos.size() > 0) {
        for (int i = 0; i < 3; ++i) {
            sos[0][i] *= k;
        }
    }

    return sos;
}
