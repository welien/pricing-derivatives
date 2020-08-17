#include <math.h>
#include <cmath>
#include <vector>

namespace bs {
    /** pricing of european vanilla options using black-scholes formulas
     * tau is units of time until maturity
     * sigma is volatility of returns of given asset
     * st is current price of the asset
     * k is the strike price
     * r is risk free rate
    */
    double call_opt(int tau, double sigma, double st, double k, double r);
    double put_opt(int tau, double sigma, double st, double k, double r);
    double normalCDF(double value);
    double call(int current, int tau, double r, int history, double st, double k, std::vector<double> &col);
    double put(int current, int tau, double r, int history, double st, double k, std::vector<double> &col);
    double vectorSTD(std::vector<double> &data, int lowerBound, int upperBound);
}