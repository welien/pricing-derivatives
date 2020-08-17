// https://www.gormanalysis.com/blog/reading-and-writing-csv-files-with-cpp/
#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include <stdexcept>
#include <sstream>

/**
 * Objects created by this class are basically tables with columns and rows.
*/
class Dataset{
    private:
        std::vector<std::pair<std::string, std::vector<double>>> read_csv(std::string filename);
        std::vector<std::pair<std::string, std::vector<double>>> ds;
    public:
        /**
         * @param path Path to csv file to be loaded.
        */
        Dataset(std::string path);
        ~Dataset();
        std::vector<double> *getColumn(std::string colName);
        void printRows(int n_rows);
        void addReturns(std::string colName, std::string asCol);
};
    