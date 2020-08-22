#include <vector>
#include <cmath>
#include <iostream>

#include <strategies/binomial.hpp>

namespace bin {
    std::pair<double, double> getCallPut(int tau, double sigma, double st, double k, double r, int steps) {
        // Cox-Ross-Rubinstein for p, u, d
        // means we're working with the assumption that asset behaves
        // like it would in a risk-neutral world p* u + (1-p)d = e^(rΔt)
        int depth = steps;
        double dt = ((double)tau) / depth;
        // calculated to ensure that variance matches
        double u = exp(sigma * sqrt(dt));
        double d = exp(-sigma * sqrt(dt));
        double p = (exp(r * dt) - d) / (u - d);
        // accumulator for put and call
        double accP = 0;
        double accC = 0;
        // sum of binomial products
        for (int i = 1; i <= depth; i++) {
            // price of asset at time n
            double sn = st * pow(u, depth-i) * pow(d, i-1);
            // if the payoff is negative, price of option is zero
            double callNprice = sn - k;
            callNprice = (callNprice < 0) ? 0 : callNprice;
            // same goes for put option
            double putNprice = k - sn;
            putNprice = (putNprice < 0) ? 0 : putNprice;
            // calculated using binomial expansion - may be optimized
            double binCoeff = nChooseK(depth-1, i-1);
            double probability = binCoeff * pow(p, depth-i) * pow(1-p, i-1);
            accC += callNprice * probability;
            accP += putNprice * probability;
        }
        double discount = exp(-tau * r);
        double callDiscounted = accC * discount;
        double putDiscounted = accP * discount;
        return {callDiscounted,putDiscounted};
    }

    double nChooseK(int n, int k) {
        // combinatorics
        //std::cout << "Calculating nChooseK" << std::endl;
        //return (double)factorial(n)/ (double)(factorial(k) * factorial(n-k));
        double allPermutations = permutationUntil(n, k);
        double permutationsOfChosen = permutation(k);
        return (allPermutations / permutationsOfChosen);
        
    }

    double permutation(int k) {
        double result = 1.0;
        for (int i = k; i > 0; i--) {
            result *= i;
        }
        return result;
    }

    int factorial(int n) {
        if (n < 2) {
            return 1;
        } else {
            return n * factorial(n-1);
        }
    }

    double permutationUntil(int n, int k) {
        double result = 1.0;
        for (int i = n; i > n-k; i--) {
            result *= i;
        }
        return result;
    }
}
