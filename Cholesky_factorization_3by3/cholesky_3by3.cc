/* ========================================================================
 * Perform the Cholesky decomposition A = L*L^T of a symmetric and positive
 * definite matrix A
 * ======================================================================== */
#include <cmath>
#include <array>
#include <iostream>
#include <iomanip>


int main() {
    /* -----------------------------------------.......-------------------------
     * Matrix `a` must be symmetric and positive definite (so det(a) > 0). If it
     * isn't, the sanity checks below will fail.
     * ------------------------------------------------------------------------- */
    constexpr std::array<double, 9> a {
         3.0,  -1.0,  1.0,
        -1.0,   4.0,  1.0,
         1.0,   1.0,  5.0
    };

    std::cout << "***** Matrix A *****" << std::endl;

    for (int i = 0; i < 3; ++i) {
        const int idx_i = i*3;

        for (int j = 0; j < 3; ++j) {
            std::cout << std::setprecision(6) << a[idx_i + j] << "\t";
        }

        std::cout << std::endl;
    }


    /* -------------------------------------------------------
     * Build the elements of the Cholesky decomposition matrix
     * ------------------------------------------------------- */
    if constexpr (a[0] < 0.0) {
        std::cout << "ERROR: a11 = " << a[0] << " < 0. Matrix A is not positive definite." << std::endl;
        return 1; 
    }

    constexpr double l11 = std::sqrt(a[0]);    // l11 = sqrt(a11)
    constexpr double l21 = a[3]/l11;           // l21 = a21/l11
    constexpr double l31 = a[6]/l11;           // l31 = a31/l11
    const double     l22_sq = a[4] - l21*l21;  // l22 = sqrt(l22 - l21^2)

    if (l22_sq < 0.0) {
        std::cout << "ERROR: l22_sq = " << l22_sq << " < 0. Matrix A is not positive definite." << std::endl;
        return 1; 
    }

    const double l22    = std::sqrt(l22_sq);
    const double l32    = (a[7] - l31*l21)/l22;      // l32 = (a32 - l31*l21) / l22
    const double l33_sq = a[8] - l31*l31 - l32*l32;  // l33 = sqrt(a33 - l31^2 - l21^2)

    if (l33_sq < 0.0) {
        std::cout << "ERROR: l33_sq = " << l33_sq << " < 0. Matrix A is not positive definite." << std::endl;
        return 1; 
    }

    const double l33 = std::sqrt(l33_sq);

    std::cout << std::endl << "***** Cholesky factorization: Matrix L s.t. A = L^T * L *****" << std::endl;

    std::cout << std::setprecision(6) << l11 <<           "\t\t0" <<         "\t\t0" << std::endl
                                      << l21 << "\t"   << l22     <<         "\t\t0" << std::endl
                                      << l31 << "\t\t" << l32     << "\t" << l33     << std::endl;


    /* --------------------------------------------------------
     * Sanity check: compute L * L^T and check that it equals A
     * -------------------------------------------------------- */
    std::cout << std::endl << "***** Matrix L^T * L : should equal A *****" << std::endl;

    const double a11 = l11*l11;
    const double a12 = l11*l21;
    const double a13 = l11*l31;
    const double a22 = l21*l21 + l22*l22;
    const double a23 = l21*l31 + l22*l32;
    const double a33 = l31*l31 + l32*l32 + l33*l33;

    std::cout << std::setprecision(6) << a11 << "\t" << a12 << "\t" << a13 << std::endl
                                      << a12 << "\t" << a22 << "\t" << a23 << std::endl
                                      << a13 << "\t" << a23 << "\t" << a33 << std::endl;
    return 0;
}
