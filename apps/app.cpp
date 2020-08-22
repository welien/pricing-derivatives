#include <iostream>
#include <chrono> 

#include <dataloader/dataset.hpp>
#include <simulation/experiments.hpp>

int main(int argc, char** argv) {
    printf("Welcome to experiments in pricing vanilla European options.\n");

    if (argc == 1) {
        std::cout << "Need input file" << std::endl;
        return 0;
    }
    //std::cout << argc << " " << argv[1] << std::endl;
    Dataset *spy_csv = new Dataset(argv[1]);
    spy_csv->addReturns("Adj Close", "AC Returns");
    //std::cout << spy_csv.at(0).first << std::endl;
    spy_csv->printRows(10);

    auto colRefReturns = spy_csv->getColumn("AC Returns");
    auto colRefPrice = spy_csv->getColumn("Adj Close");
    if (colRefReturns==NULL || colRefPrice==NULL) {
        std::cout << "GOT NULL COLUMN!" << std::endl;
        return 0;
    }
    
    int historyLength = 100;
    int maxFuture = 10;
    double riskFreeDailyContinuousRate = 0.000054;
    //double riskFreeDailyContinuousRate = 0.2;
    
    //auto start = std::chrono::high_resolution_clock::now(); 
    std::cout << "Experiment with Black-Scholes options pricing:" << std::endl;
    ex::BSExperiment(*colRefReturns, *colRefPrice, historyLength, maxFuture, riskFreeDailyContinuousRate);
    //auto stop = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    //std::cout << "Duration: " << duration.count() << std::endl;
    std::cout << "Experiment with binomial model options pricing:" << std::endl;
    ex::binomialExperiment(*colRefReturns, *colRefPrice, historyLength, maxFuture, riskFreeDailyContinuousRate);
}