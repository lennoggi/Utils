#include <cassert>
#include <array>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;


// Grid parameters
#define N     21
#define XMIN  0.
#define XMAX  10.

// Point onto which the interpolation will be performed
#define X 8.32

// Degree of the Lagrange interpolating polynomial
#define P 5
#if (P & 1)
#define P_ODD true
#else
#define P_ODD false
#endif



// Function to be interpolated
double f(const double &x) {
    return sin(x);
}



int main() {
    // Sanity checks
    assert(X >= XMIN and X <= XMAX);
    assert(P < N);


    // Build the grid
    array<double, N> x;
    const double dx = (XMAX - XMIN)/(N - 1);

    for (int n = 0; n < N; ++n) {
        x.at(n) = XMIN + n*dx;
    }

    cout << endl
         << "xmin    = " << XMIN << endl
         << "xmax    = " << XMAX << endl
         << "#points = " << N    << endl
         << endl
         << "Interpolation order: " << P << endl
         << endl;


    // Locate the interpolation point inside the grid via binary search
    int nlow = 0;
    int nup  = N - 1;
    int nhalf;

    while (nlow < nup - 1) {
        nhalf = (nlow + nup)/2;  // **INTEGER** division
        (X >= x.at(nhalf)) ? nlow = nhalf : nup = nhalf;
    }

    assert(nup == nlow + 1);

    cout << "Target point X = " << X << " lies between "
         << "x[" << nlow << "] = " << x.at(nlow) << " and "
         << "x[" << nup  << "] = " << x.at(nup) << endl
         << endl;


    /* Select a window of P+1 points centered around X (or off-centered if X is
     * too close to the boundaries) and perform a Lagrange interpolation with a
     * P-th order polynomial in that window                                     */
    const int Phalf = P/2;  // **INTEGER** division
    array<double, P+1> xwindow;

    #if (P_ODD)
    if (nlow < Phalf) {
    #else
    if (nup  < Phalf) {
    #endif
        for (int i = 0; i <= P; ++i) {
            xwindow.at(i) = x.at(i);
        }
    }

    else if (nup >= N-Phalf) {
        const int offset_end = N-P-1;
        for (int i = 0; i <= P; ++i) {
            xwindow.at(i) = x.at(i + offset_end);
        }
    }

    else {
        for (int i = 0; i <= P; ++i) {
            #if (P_ODD)
            xwindow.at(i) = x.at(i + nlow - Phalf);
            #else
            xwindow.at(i) = x.at(i + nup  - Phalf);
            #endif
        }
    }

    cout << "Interpolation window (" << P+1 << " points):" << endl;
    for (int i = 0; i <= P; ++i) {
        cout << xwindow.at(i) << endl;
    }


    // Interpolate
    double fX = 0.;
    for (int i = 0; i <= P; ++i) {
        double pi = f(xwindow.at(i));
        for (int j = 0; j <=P; ++j) {
            if (j != i) {
                pi *= (X - xwindow.at(j))/(xwindow.at(i) - xwindow.at(j));
            }
        }

        fX += pi;
    }

    cout << endl << "Interpolated value:  f_int("
                 << setprecision(6)  << X << ") = "
                 << setprecision(16) << fX
         << endl << "Expected value:          f("
                 << setprecision(6)  << X << ") = "
                 << setprecision(16) << f(X)
         << endl << endl;

    return 0;
}
