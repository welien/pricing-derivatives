#include <math.h>
#include <cmath>
#include <vector>
#include <iostream>

#include <strategies/black-scholes.hpp>

namespace bs {
    double call_opt(int tau, double sigma, double st, double k, double r) {
        double d1 = log(st/k) + (r + pow(sigma, 2)/2) * tau;
        d1 = d1 * (1 / (sigma * sqrt(tau)));
        double d2 = d1 - sigma * sqrt(tau);
        double pvk = k * exp(r * tau * -1);
        double call = normalCDF(d1) * st - normalCDF(d2) * pvk;
        /*std::cout << " tau: " << tau;
        std::cout << " sigma: " << sigma;
        std::cout << " spot price: " << st;
        std::cout << " strike price: " << k;
        std::cout << " risk free: " << r;
        std::cout << " d1: " << d1;
        std::cout << " d2: " << d2;
        std::cout << " pvk: " << pvk;
        std::cout << " call: " << call;
        std::cout << std::endl;*/
        return call;
    }

    double put_opt(int tau, double sigma, double st, double k, double r) {
        double call = call_opt(tau, sigma, st, k, r);
        double pvk = k * exp(r * tau * -1);
        double put = call - st + pvk;
        return put;
    }

    double normalCDF(double value) {
        return 0.5 * erfc(-value * M_SQRT1_2);
    }

    double call(int current, int tau, double r, int history, double st, double k, std::vector<double> &col) {
        // +1 so that change from "yesterday" to "today" is included
        double sigma = vectorSTD(col, current-history, current+1);
        return call_opt(tau, sigma, st, k, r);
    }

    double put(int current, int tau, double r, int history, double st, double k, std::vector<double> &col) {
        // +1 so that change from "yesterday" to "today" is included
        double sigma = vectorSTD(col, current-history, current+1);
        return put_opt(tau, sigma, st, k, r);
    }

    // [lowerBound; upperBound)
    double vectorSTD(std::vector<double> &data, int lowerBound, int upperBound) {
        double sum = 0.0, mean, standardDeviation = 0.0;

        int i;

        for(i = lowerBound; i < upperBound; i++)
        {
            sum += data[i];
        }

        mean = sum/(upperBound-lowerBound);

        for(i = lowerBound; i < upperBound; i++)
            standardDeviation += pow(data.at(i) - mean, 2);

        double correction = data.size() / (data.size() - 1);
        //double correction = 1.0;

        return sqrt(standardDeviation / (upperBound-lowerBound)) * correction;
    }

    // this function calculates both options at the same time - this should in theory cut computation in half
    std::pair<double, double> getCallPut(int tau, double sigma, double st, double k, double r) {
        double d1 = log(st/k) + (r + pow(sigma, 2)/2) * tau;
        d1 = d1 * (1 / (sigma * sqrt(tau)));
        double d2 = d1 - sigma * sqrt(tau);
        double pvk = k * exp(r * tau * -1);
        double call = normalCDF(d1) * st - normalCDF(d2) * pvk;
        double put = call - st + pvk;
        return {call,put};
    }
}