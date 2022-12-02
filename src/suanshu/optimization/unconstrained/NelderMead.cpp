#include "NelderMead.h"

#include <numeric>

using namespace suanshu;

NelderMeadSolution::NelderMeadSolution(const NelderMead& solver,
                                       const C2OptimProblem& problem)
    : z(solver), f(problem.f()), N(f.dimensionOfDomain()), N1(N + 1) {}

double NelderMeadSolution::minimum() const { return fx[0]; }

Vector NelderMeadSolution::minimizer() const { return x[0]; }

Vector NelderMeadSolution::search(const std::vector<Vector>& simplex) {
    setInitials(simplex);

    double lastFx = std::numeric_limits<double>::infinity();
    for (int i = 0; i < z.maxIterations; ++i) {
        step();

        if (i > kMinimumNumberOfIterations &&
            fabs(fx[0] - lastFx) < z.epsilon) {
            break;
        }

        lastFx = fx[0];
    }

    return minimizer();
}

static std::vector<Vector> defaultSimplexInitials(const Vector& initial) {
    constexpr double scale = 0.1;

    double inc = 0;
    for (int j = 0; j < initial.size(); ++j) {
        if (inc < fabs(initial[j])) {
            inc = fabs(initial[j]);
        }
    }
    inc = (inc == 0 ? 1 : scale * inc);

    std::vector<Vector> simplex(initial.size() + 1);
    simplex[0] = initial;
    for (int i = 1; i <= initial.size(); ++i) {
        simplex[i] = initial;
        simplex[i][i - 1] += inc;
    }
    return simplex;
}

void NelderMeadSolution::setInitials(const std::vector<Vector>& simplex) {
    if (simplex.size() != N1) {
        setInitials(defaultSimplexInitials(simplex[0]));
        return;
    }

    for (int i = 0; i < simplex.size(); ++i) {
        aassert(simplex[i].size() == N,
                "the dimension of x does not match the degree of freedom of f");
    }

    x = simplex;
    fx.resize(N1);
    for (int i = 0; i < N1; ++i) {
        fx[i] = f.evaluate(simplex[i]);
    }

    sort();
}

void NelderMeadSolution::step() {
    Vector xo = Vector::Zero(N);  // centroid
    for (int i = 0; i < N; ++i) {
        xo += x[i];
    }
    xo /= N;

    // reflection
    Vector xr = xo + z.alpha * (xo - x[N]);
    double fxr = f.evaluate(xr);

    if (fx[0] <= fxr && fxr < fx[N - 1]) { /* reflected point is neither best
                                              nor worst in the new simplex */
        replaceWorst(xr, fxr);
    } else if (fxr < fx[0]) { /* reflected point is better than the current
                                 best; try to go farther along this direction */
        // expansion
        Vector xe = xo + z.gamma * (xo - x[N]);
        double fxe = f.evaluate(xe);
        if (fxe < fxr) {
            replaceWorst(xe, fxe);
        } else {
            replaceWorst(xr, fxr);
        }
    } else { /* fx[N-1] <= fxr, i.e. reflected point is still worse than the
                second worst */
        Vector xc;
        if (fxr < fx[N]) {
            // outer contraction
            xc = xo + z.rho * (xr - xo);
        } else {
            // inner contraction
            xc = xo + z.rho * (x[N] - xo);
        }

        double fxc = f.evaluate(xc);
        if ((fxr < fx[N] && fxc <= fxr) /* if the contracted point is better
                                           than the reflected point */
            || (fxr >= fx[N] - std::numeric_limits<double>::epsilon() &&
                fxc < fx[N])) { /* the contracted point is better than the worst
                                   point */
            replaceWorst(xc, fxc);
        } else {
            // reduction
            for (int i = 1; i < N1; ++i) {
                x[i] = x[0] + z.sigma * (x[i] - x[0]);
                fx[i] = f.evaluate(x[i]);
            }
        }
    }

    sort();
}

void NelderMeadSolution::sort() {
    std::vector<size_t> idx(N1);
    std::iota(idx.begin(), idx.end(), 0);

    std::stable_sort(idx.begin(), idx.end(),
                     [this](size_t i, size_t j) { return fx[i] < fx[j]; });

    std::vector<Vector> y(N1);
    for (int i = 0; i < N1; ++i) {
        y[i] = std::move(x[idx[i]]);
    }
    std::swap(x, y);

    std::vector<double> fy(N1);
    for (int i = 0; i < N1; ++i) {
        fy[i] = fx[idx[i]];
    }
    std::swap(fx, fy);
}

void NelderMeadSolution::replaceWorst(const Vector& px, const double pfx) {
    x[N] = px;
    fx[N] = pfx;
}