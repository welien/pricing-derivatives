#include <vector>

namespace bin {
    std::pair<double, double> getCallPut(int tau, double sigma, double st, double k, double r, int steps);
    double nChooseK(int n, int k);
    int factorial(int n);
    double permutationUntil(int n, int k);
    double permutation(int k);
}