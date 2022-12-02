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

#ifndef SUANSHU_OPTIMIZATION_UNCONSTRAINED_NELDER_MEAD_H
#define SUANSHU_OPTIMIZATION_UNCONSTRAINED_NELDER_MEAD_H

#include "function/Function.h"
#include "optimization/problem/OptimProblem.h"

namespace suanshu {

struct NelderMead;

class NelderMeadSolution {
   public:
    double minimum() const;
    Vector minimizer() const;

    Vector search(const std::vector<Vector>& simplex);

    void setInitials(const std::vector<Vector>& simplex);
    void step();

   private:
    NelderMeadSolution(const NelderMead& solver, const C2OptimProblem& problem);

    void sort();
    void replaceWorst(const Vector& px, double pfx);

    const NelderMead& z;
    std::vector<Vector> x;
    std::vector<double> fx;
    RealScalarFunction f;  // the objective function
    int N;                 // degree of freedom for (f)
    int N1;                // 1 + degree of freedom for (f)

    static constexpr int kMinimumNumberOfIterations = 50;

    friend struct NelderMead;
};

struct NelderMead {
    NelderMead(double alpha, double gamma, double rho, double sigma,
               double epsilon, int maxIterations)
        : alpha(alpha),
          gamma(gamma),
          rho(rho),
          sigma(sigma),
          epsilon(epsilon),
          maxIterations(maxIterations) {}

    NelderMead(double epsilon, int maxIterations)
        : NelderMead(1, 2, 0.5, 0.5, epsilon, maxIterations) {}

    NelderMeadSolution solve(const C2OptimProblem& problem) {
        return NelderMeadSolution(*this, problem);
    }

    double alpha;  // the reflection coefficient
    double gamma;  // the expansion coefficient
    double rho;    // the contraction coefficient
    double sigma;  // the shrink/reduction coefficient
    double epsilon;
    int maxIterations;
};

}  // namespace suanshu

#endif  // SUANSHU_OPTIMIZATION_UNCONSTRAINED_NELDER_MEAD_H
