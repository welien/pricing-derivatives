#include <strategies/black-scholes.hpp>

namespace mc {
    double experiment(std::vector<double> columnReturns, std::vector<double> columnPrice, int historyLength, int maxFuture, double r);
}