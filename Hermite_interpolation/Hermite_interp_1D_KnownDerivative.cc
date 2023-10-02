#include <iostream>
#include <iomanip>
#include <array>

#define N      4
#define POISON 1.e+300

using namespace std;


double Hermite_interp_1D(const array<double, N> &x,
                         const array<double, N> &f,
                         const array<double, N> &fder,
                         const double &target) {
    array<double, 2*N>             z;
    array<array<double, 2*N>, 2*N> Q;

    // Poison Q initially
    for (int i = 0; i < 2*N; ++i) {
        for (int j = 0; j < 2*N; ++j) {
            Q.at(i).at(j) = POISON;
        }
    }


    // Fill z and the first two columns of Q
    for (int i = 0; i < N; ++i) {
        const auto &xi  = x.at(i);
        const auto &fi  = f.at(i);

        z.at(2*i)     = xi;
        z.at(2*i + 1) = xi;

        Q.at(2*i).at(0)     = fi;
        Q.at(2*i + 1).at(0) = fi;
        Q.at(2*i + 1).at(1) = fder.at(i);

        if (i > 0) {
            Q.at(2*i).at(1) = ( Q.at(2*i).at(0) - Q.at(2*i - 1).at(0) ) / ( z.at(2*i) - z.at(2*i - 1) );
        }
    }


    // Fill all the other columns of Q
    for (int i = 2; i < 2*N; ++i) {
        for (int j = 2; j <= i; ++j) {
            Q.at(i).at(j) = (Q.at(i).at(j-1) - Q.at(i-1).at(j-1)) / (z.at(i) - z.at(i-j));
        }
    }


    // Print the divided differences table and the Hermite polynomial coefficients
    cout << endl << "Divided differences table"
         << endl << "-------------------------"
         << endl;


    for (int i = 0; i < 2*N; ++i) {
        cout << setw(3) << z.at(i);
        for (int j = 0; j < 2*N; ++j) {
            cout << setw(10) << Q.at(i).at(j) << " ";
        }

        cout << endl;
    }

    cout << endl << "Coefficients: ";
    for (int i = 0; i < 2*N; ++i) {
        cout << Q.at(i).at(i) << " ";
    }
    cout << endl;


    // Compute the Hermite polynomial at the target point
    double interp_val = Q.at(0).at(0);

    for (int i = 1; i < 2*N; ++i) {
        double prod = 1.;

        for (int k = 0; k < i; ++k) {
            prod *= (target - z.at(k));
        }

        interp_val += Q.at(i).at(i)*prod;
    }

    return interp_val;
}





int main() {
    const array<double, N> x    = {1., 2., 4., 5.};
    const array<double, N> f    = {2., 1., 3., 6.};
    const array<double, N> fder = {-1., -3., 4., 5.};

    const double target     = 4.38;
    const auto   interp_val = Hermite_interp_1D(x, f, fder, target);

    cout << endl << "Interpolated value: " << interp_val << endl << endl;

    return 0;
}
