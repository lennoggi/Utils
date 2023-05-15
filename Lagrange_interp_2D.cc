#include <cassert>
#include <array>
#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace std;


// Grid parameters
#define NX 21
#define NY 41

#define XMIN 0.
#define XMAX 10.

#define YMIN 0.
#define YMAX 20.


// Point onto which the interpolation will be performed
#define X 8.32
#define Y 18.86


// Degrees of the Lagrange interpolating polynomials along x and y
#define PX 5
#if (PX & 1)
#define PX_ODD true
#else
#define PX_ODD false
#endif

#define PY 4
#if (PY & 1)
#define PY_ODD true
#else
#define PY_ODD false
#endif



/* ***************************
 * Function to be interpolated
 * *************************** */
double f(const double &x,
         const double &y) {
    const double r = sqrt(x*x + y*y);
    return sin(r)/r;
}



/* *********************
 * Routine to get a grid
 * ********************* */
vector<double> get_grid(const double &min,
                        const double &max,
                        const int    &npoints) {
    const double delta = (max - min)/(npoints - 1);
    vector<double> grid(npoints);

    for (int n = 0; n < npoints; ++n) {
        grid.at(n) = min + n*delta;
    }

    return grid;
}



/* ************************************************************
 * Routine to locate a point inside a 1D grid via binary search
 * ************************************************************ */
array<int, 2> locate(const double         &target,
                     const vector<double> &grid) {
    int nlow = 0;
    int nup  = grid.size() - 1;
    int nhalf;

    while (nlow < nup - 1) {
        nhalf = (nlow + nup)/2;  // **INTEGER** division
        (target >= grid.at(nhalf)) ? nlow = nhalf : nup = nhalf;
    }

    assert(nup == nlow + 1);

    const array<int, 2> bounds{nlow, nup};
    return bounds;
}



/* *****************************************************************************
 * Routine selecting a window of order+1 points -- 'order' is the order of the
 * interpolating polynomial -- centered around the target point, or off-centered
 * if the latter is too close to the boundaries
 * ******************************************************************************/
vector<double> get_window(const int &nlow,
                          const int &nup,
                          const int &order,
                          const vector<double> &grid) {
    const int npoints      = grid.size();
    const int order_half   = order/2;  // **INTEGER** division
    const int n_lowup = (order & 1) ? nlow : nup;
    vector<double> window(order + 1);

    if (n_lowup < order_half) {
        for (int i = 0; i <= order; ++i) {
            window.at(i) = grid.at(i);
        }
    }

    else if (nup >= npoints - order_half) {
        const int offset_end = npoints - order - 1;
        for (int i = 0; i <= order; ++i) {
            window.at(i) = grid.at(i + offset_end);
        }
    }

    else {
        const int n_grid_low = n_lowup - order_half;
        for (int i = 0; i <= order; ++i) {
            window.at(i) = grid.at(i + n_grid_low);
        }
    }

    return window;
}



/* ************************************************
 * Routine performing the 1D Lagrange interpolation
 * ************************************************ */
double Lagrange_interp_1d(const double         &target,
                          const vector<double> &window,
                          const vector<double> &fwindow) {
    const auto order_p1 = window.size();
    assert(fwindow.size() == order_p1);

    double interp_f = 0.;

    for (int i = 0; i < order_p1; ++i) {
        double fi = fwindow.at(i);
        for (int j = 0; j < order_p1; ++j) {
            if (j != i) {
                fi *= (target - window.at(j))/(window.at(i) - window.at(j));
            }
        }

        interp_f += fi;
    }

    return interp_f;
}





/* *************
 * Main function
 * ************* */
int main() {
    // Sanity checks
    assert(X >= XMIN and X <= XMAX);
    assert(Y >= YMIN and Y <= YMAX);

    assert(PX < NX);
    assert(PY < NY);


    // Build the grid
    const auto xgrid = get_grid(XMIN, XMAX, NX);
    const auto ygrid = get_grid(YMIN, YMAX, NY);


    // Locate the interpolation point inside the grid via binary search
    const auto xbounds = locate(X, xgrid);
    const auto &nx_low = xbounds.at(0);
    const auto &nx_up  = xbounds.at(1);

    const auto ybounds = locate(Y, ygrid);
    const auto &ny_low = ybounds.at(0);
    const auto &ny_up  = ybounds.at(1);


    // Get the interpolation windows along x and y
    const auto xwindow = get_window(nx_low, nx_up, PX, xgrid);
    const auto ywindow = get_window(ny_low, ny_up, PY, ygrid);


    // Interpolate
    const int nx_lowup    = (PX & 1) ? nx_low : nx_up;
    const int nx_grid_low = nx_lowup - PX/2;

    const int ny_lowup    = (PY & 1) ? ny_low : ny_up;
    const int ny_grid_low = ny_lowup - PY/2;

    vector<double> fywindow(PY + 1);
    for (int j = 0; j <= PY; ++j) {
        vector<double> fxwindow(PX + 1);
        for (int i = 0; i <= PX; ++i) {
            fxwindow.at(i) = f(xgrid.at(i + nx_grid_low), Y);
        }

        fywindow.at(j) = Lagrange_interp_1d(X, xwindow, fxwindow);
    }

    const double interp_f = Lagrange_interp_1d(Y, ywindow, fywindow);


    // Print some info
    cout << endl
         << "x direction"                   << endl
         << "-----------"                   << endl
         << "xmin:                " << XMIN << endl
         << "xmax:                " << XMAX << endl
         << "Number of points:    " << NX   << endl
         << "Interpolation order: " << PX   << endl
         << endl
         << "Target point X = " << X << " lies between "
         << "xgrid[" << nx_low << "] = " << xgrid.at(nx_low) << " and "
         << "xgrid[" << nx_up  << "] = " << xgrid.at(nx_up)  << endl
         << endl;
    cout << "Interpolation window (" << PX+1 << " points):" << endl;
    for (int i = 0; i <= PX; ++i) {
        cout << xwindow.at(i) << endl;
    }

    cout << endl << endl
         << "y direction"                   << endl
         << "-----------"                   << endl
         << "ymin:                " << YMIN << endl
         << "ymax:                " << YMAX << endl
         << "Number of points:    " << NY   << endl
         << "Interpolation order: " << PY   << endl
         << endl
         << "Target point Y = " << Y << " lies between "
         << "ygrid[" << ny_low << "] = " << ygrid.at(ny_low) << " and "
         << "ygrid[" << ny_up  << "] = " << ygrid.at(ny_up)  << endl
         << endl;
    cout << "Interpolation window (" << PY+1 << " points):" << endl;
    for (int j = 0; j <= PY; ++j) {
        cout << ywindow.at(j) << endl;
    }

    cout << endl
         << "Interpolated value:  f_int("
                 << setprecision(6)  << X << ", " << Y << ") = "
                 << setprecision(16) << interp_f << endl
         << "Expected value:          f("
                 << setprecision(6)  << X << ", " << Y << ") = "
                 << setprecision(16) << f(X, Y) << endl
         << endl;

    return 0;
}
