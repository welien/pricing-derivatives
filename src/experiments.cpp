#include <strategies/black-scholes.hpp>
#include <strategies/binomial.hpp>
#include <simulation/experiments.hpp>
#include <iostream>

namespace ex {
    // simulation must know:
    // * what call/put function we're using
    // * whether it's constrained by run time or number of calculations
    // * random number of days will be taken into consideration
    // * random number of k from st will be considered -up until maxFuture
    // * random number of puts and calls will be taken into consideration
    // * need random distribution functions for this
    double BSExperiment(std::vector<double> columnReturns, std::vector<double> columnPrice, int historyLength, int maxFuture, double r) {
        // probability of skipping the day;
        //3.70121e+14
        //3.70121e+14
        float p_day = 0.0;
        float p_future = 0.0;
        float p_strike = 0.0;
        int maxOptions = 10;

        std::vector<float> strikes({0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5});

        double total = 0;
        double variance = 0;
        double sigma = 0;
        size_t size = columnReturns.size();

        //double sigma2;
        //MovingVolatility mv = MovingVolatility(columnReturns, historyLength);
        //sigma2 = mv.getVolatility();
        // for days = st
        for (int i = historyLength; i + maxFuture < size; i++) {
            if (i % 200 == 0) {
                std::cout << "Cumulative profit at " << i << " is " << total << std::endl;
            }

            /*if (i!=historyLength)
                sigma2 = mv.step();*/
            // compute standard deviation from the past x days
            sigma = bs::vectorSTD(columnReturns, i-historyLength, i+1);
                
            // with some probability p skip this day
            if (rand() < p_day) {
                continue;
            }
            // for futures tau
            for (int j = 1; j <= maxFuture; j++) {
                // with probability p skip this day T
                if (rand() < p_future) {
                    continue;
                }

                for (float s : strikes) {
                    // with probability p skip this strike price
                    if (rand() < p_strike) {
                        continue;
                    }

                    double spot = columnPrice.at(i);
                    double strike = spot * s;
                    // tau = j
                    // how much we make selling puts/calls
                    std::pair<double, double> callPut = bs::getCallPut(j, sigma, spot, strike, r);
                    double putPrice = callPut.second;
                    double callPrice = callPut.first;
                    //double putPrice = bs::put(i, j, r, historyLength, spot, strike, columnReturns);
                    //double callPrice = bs::call(i, j, r, historyLength, spot, strike, columnReturns);
                    //std::cout << " call price " << callPrice << std::endl;
                    //std::cout << " put price " << putPrice << std::endl;
                    // how much we lose on people executing puts/calls
                    double putLoss = 0;
                    double callLoss = 0;
                    double actualPriceAtStrikeTime = columnPrice[(i+j)];
                    if (actualPriceAtStrikeTime < strike) {
                        // put options are in the money
                        // must buy the asset at strike price and sell it at market price
                        putLoss = strike - actualPriceAtStrikeTime;
                        //std::cout << "Losing per put " << putLoss << std::endl;
                    } else if (actualPriceAtStrikeTime > strike) {
                        // call options are in the money
                        // this happens if I buy the asset at strike time
                        callLoss = actualPriceAtStrikeTime - strike;
                        // this happens if I buy the aset at the spot
                        // callLoss = columnPrice.at(i) - strike;
                    }
                    // if it's equal, nothing happens - executing the option is no different from buying/selling from/to the market directly

                    int nCalls = rand() % maxOptions;
                    int nPuts = rand() % maxOptions;
                    //std::cout << "Selling calls and puts: " << nCalls << " " << nPuts << std::endl;

                    // now to calculate how much money market maker made selling these options
                    // and how much money market maker lost selling/buying the underlying asset based on theseoptions
                    double result = (putPrice - putLoss) * nPuts + (callPrice - callLoss) * nCalls;
                    total += result;
        
                    //std::cout << "St " << i << " K " << (i+j) << " made " << total << std::endl;
                    }

            }
            
        }
        std::cout << "Total result is " << total << std::endl;
        return total;
    }

    double binomialExperiment(std::vector<double> columnReturns, std::vector<double> columnPrice, int historyLength, int maxFuture, double r) {
        size_t size = columnReturns.size();
        double sigma = 0;
        std::vector<float> strikes({0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5});
        int steps = 50;
        double total = 0;

        for (int i = historyLength; i + maxFuture < size; i++) {
            sigma = bs::vectorSTD(columnReturns, i-historyLength, i+1);

            if (i % 200 == 0) {
                std::cout << "Cumulative profit at " << i << " is " << total << std::endl;
            }

            for (int j = 1; j <= maxFuture; j++) {
                for (float s : strikes) {
                    double spot = columnPrice.at(i);
                    double strike = spot * s;

                    std::pair<double, double> callPut = bin::getCallPut(j, sigma, spot, strike, r, steps);
                    std::pair<double, double> callPutBS = bs::getCallPut(j, sigma, spot, strike, r);
                    //std::cout << "BS call/put " << callPutBS.first << "/" << callPutBS.second;
                    //std::cout << "Bin call/put" << callPut.first << "/" << callPut.second << std::endl;
                    double putPrice = callPut.second;
                    double callPrice = callPut.first;

                    double putLoss = 0;
                    double callLoss = 0;
                    double actualPriceAtStrikeTime = columnPrice[(i+j)];
                    if (actualPriceAtStrikeTime < strike) {
                        putLoss = strike - actualPriceAtStrikeTime;
                    } else if (actualPriceAtStrikeTime > strike) {
                        callLoss = actualPriceAtStrikeTime - strike;
                    }

                    int nCalls = 1;
                    int nPuts = 1;

                    // now to calculate how much money market maker made selling these options
                    // and how much money market maker lost selling/buying the underlying asset based on theseoptions
                    double result = (putPrice - putLoss) * nPuts + (callPrice - callLoss) * nCalls;
                    total += result;
                }
            }
        }
        return 0;
    }
}

MovingVolatility::MovingVolatility(std::vector<double> v, int intervalLength) : data(v.data()), dataSize(v.size()) {
    sizeX = intervalLength;
    indexOfFirst = 0;
    indexOfLast = intervalLength;
    sumOfX = 0;
    squaredSumOfX = 0;
    varianceOfX = 0;

    // indexOfLast not included
    for (int i = indexOfFirst; i < indexOfLast; i++) {
        sumOfX += data[i];
        squaredSumOfX += pow(data[i], 2);
    }

    meanOfX = sumOfX / sizeX;

    // compute variance
    double term2 = sizeX * meanOfX;
    varianceOfX = squaredSumOfX / sizeX - pow(meanOfX, 2);
    //std::cout << "term1 " << squaredSumOfX;
    //std::cout << " term2 " << term2 << std::endl;
}

double MovingVolatility::step() {
    // if reached end of data, return nothing
    if (indexOfLast==dataSize-1) {
        return 0;
    }

    // update sum
    double newSum = sumOfX - data[indexOfFirst] + data[indexOfLast];
    std::cout << "new sum " << newSum;
    // update mean
    double newMean = newSum / sizeX;
    std::cout << " new mean " << newMean;
    double newSquaredSum = squaredSumOfX - pow(data[indexOfFirst],2) + pow(data[indexOfLast],2);
    std::cout << " new SquaredSum " << newSquaredSum;
    double newVariance = newSquaredSum / sizeX - pow(newMean, 2);

    meanOfX = newMean;
    sumOfX = newSum;
    squaredSumOfX = newSquaredSum;
    varianceOfX = newVariance;

    indexOfFirst++;
    indexOfLast++;

    std::cout << " new variance " << newVariance << std::endl;

    return sqrt(varianceOfX);
}

double MovingVolatility::getVolatility() {
    return sqrt(varianceOfX);
}

MovingVolatility::~MovingVolatility()
{
}
