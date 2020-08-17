#include <strategies/black-scholes.hpp>
#include <simulation/montecarlo.hpp>
#include <iostream>

namespace mc {
    // simulation must know:
    // * what call/put function we're using
    // * whether it's constrained by run time or number of calculations
    // * random number of days will be taken into consideration
    // * random number of k from st will be considered -up until maxFuture
    // * random number of puts and calls will be taken into consideration
    // * need random distribution functions for this
    double experiment(std::vector<double> columnReturns, std::vector<double> columnPrice, int historyLength, int maxFuture, double r) {
        // probability of skipping the day;
        float p_day = 0.0;
        float p_future = 0.0;
        float p_strike = 0.0;
        float maxOptionsPDnK = 10;

        std::vector<float> strikes({0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5});

        double result = 0;
        size_t size = columnReturns.size();
        // for days = st
        for (int i = historyLength; i + maxFuture < size; i++) {
            if (i % 1500 == 0) {
                return result;
            }
            std::cout << "Cumulative profit at " << i << " is " << result << std::endl;
                
                //std::cout << "Cumulative profit at " << i << " is " << result << std::endl;
            // with certain probability p skip this day
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
                    double putPrice = bs::put(i, j, r, historyLength, spot, strike, columnReturns);
                    double callPrice = bs::call(i, j, r, historyLength, spot, strike, columnReturns);
                    //std::cout << " call price " << callPrice << std::endl;
                    //std::cout << " put price " << putPrice << std::endl;
                    // how much we lose on people executing puts/calls
                    double putLoss = 0;
                    double callLoss = 0;
                    double actualPriceAtStrikeTime = columnPrice.at(i+j);
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
                        /*std::cout << "Losing per call " << callLoss;
                        std::cout << " call price " << callPrice;
                        std::cout << " strike " << strike;
                        std::cout << " actual price " << actualPriceAtStrikeTime;
                        std::cout << std::endl;*/
                    }
                    // if it's equal, nothing happens - executing the option is no different from buying/selling from/to the market directly

                    int nCalls = rand() * maxOptionsPDnK;
                    int nPuts = rand() * maxOptionsPDnK;

                    // now to calculate how much money market maker made selling these options
                    // and how much money market maker lost selling/buying the underlying asset based on theseoptions
                    double total = (putPrice - putLoss) * nPuts + (callPrice - callLoss) * nCalls;
                    result += total;
        
                    //std::cout << "St " << i << " K " << (i+j) << " made " << total << std::endl;
                    }

            }
            
        }
        std::cout << "Total result is " << result << std::endl;
        return result;
    }
}