// https://www.gormanalysis.com/blog/reading-and-writing-csv-files-with-cpp/
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <dataloader/dataset.hpp>

// now using initialization list
Dataset::Dataset(std::string path): ds(read_csv(path)) {};

Dataset::~Dataset() {
    
}

std::vector<std::pair<std::string, std::vector<double>>> Dataset::read_csv(std::string filename){
    std::vector<std::pair<std::string, std::vector<double>>> result;

    std::ifstream myFile(filename);
    if(!myFile.is_open()) throw std::runtime_error("Could not open file");

    std::string line, colname;
    double val;

    if(myFile.good())
    {
        std::getline(myFile, line);
        std::stringstream ss(line);

        while(std::getline(ss, colname, ',')){
            result.push_back({colname, std::vector<double> {}});
        }
    }

    while(std::getline(myFile, line))
    {
        std::stringstream ss(line);
        int colIdx = 0;
        while(ss >> val){
            result.at(colIdx).second.push_back(val);
            if(ss.peek() == ',') ss.ignore();
            colIdx++;
        }
    }

    myFile.close();

    return result;
}

std::vector<double> *Dataset::getColumn(std::string colName) {
    size_t size = this->ds.size();
    for (int i = 0; i < size; i++) {
        if (this->ds.at(i).first.compare(colName)==0) {
            return &(this->ds.at(i).second);
        }
    }

    return NULL;
}

void Dataset::printRows(int n_rows) {
    int counter = 0;
    std::cout << std::setw(14) << "ID";
    for (auto column : this->ds) {
        std::cout << std::setw(14) << column.first;
    }
    std::cout << std::endl;
    for (int i = 0; i < n_rows; i++) {
        std::cout << std::setw(14) << counter;
        counter++;
        for (auto column : ds) {
            std::cout << std::setw(14) << column.second.at(i);
        }
        std::cout << std::endl;
    }
}

// creates a new column that contains returns of another column
void Dataset::addReturns(std::string colName, std::string asCol) {
    auto colRef = getColumn(colName);
    if (colRef==NULL)
        return;
    auto col = *colRef;
    size_t size = col.size();
    std::vector<double> returns(size);
    
    for (int i = 1; i < size; i++) {
        returns.at(i) = (col.at(i)/col.at(i-1));
    }
    std::pair<std::string, std::vector<double>> pair = {asCol, returns};
    ds.push_back(pair);
}