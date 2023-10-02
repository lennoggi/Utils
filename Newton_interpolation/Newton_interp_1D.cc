#include <iostream>
#include <iomanip>
#include <array>

#define N      4
#define POISON 1.e+300

using namespace std;


double Newton_interp_1D(const array<double, N> &x,
                        const array<double, N> &f,
                        const double &target) {
    static_assert(N >= 2);
    array<array<double, N>, N> Q;

    for (int i = 0; i < N; ++i) {
        Q.at(i).at(0) = f.at(i);
        for (int j = 1; j < N; ++j) {
            Q.at(i).at(j) = POISON;
        }
    }

    for (int i = 1; i < N; ++i) {
        for (int j = 1; j <= i; ++j) {
            Q.at(i).at(j) = (Q.at(i).at(j-1) - Q.at(i-1).at(j-1)) / (x.at(i) - x.at(i-j));
        }
    }


    cout << endl << "Divided differences table"
         << endl << "-------------------------"
         << endl;

    for (int i = 0; i < N; ++i) {
        cout << setw(3) << x.at(i);
        for (int j = 0; j < N; ++j) {
            cout << setw(10) << Q.at(i).at(j) << " ";
        }

        cout << endl;
    }

    cout << endl << "Coefficients: ";
    for (int i = 0; i < N; ++i) {
        cout << Q.at(i).at(i) << " ";
    }
    cout << endl;


    /*double interp_val = Q.at(0).at(0);

    for (int i = 1; i < N; ++i) {
        double prod = 1.;

        for (int k = 0; k < i; ++k) {
            prod *= (target - x.at(k));
        }

        interp_val += Q.at(i).at(i)*prod;
    }*/


    double interp_val = Q.at(N-1).at(N-1);

    for (int k = 1; k < N; ++k) {
        interp_val = Q.at(N-1-k).at(N-1-k) + (target - x.at(N-1-k))*interp_val;
    }

    return interp_val;
}





int main() {
    const array<double, N> x    = {1., 2., 4., 5.};
    const array<double, N> f    = {2., 1., 3., 6.};

    const double target     = 4.38;
    const auto   interp_val = Newton_interp_1D(x, f, target);

    cout << endl << "Interpolated value: " << interp_val << endl << endl;

    return 0;
}
