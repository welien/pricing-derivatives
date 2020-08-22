#include <vector>
namespace ex {
    double BSExperiment(std::vector<double> columnReturns, std::vector<double> columnPrice, int historyLength, int maxFuture, double r);
    double binomialExperiment(std::vector<double> columnReturns, std::vector<double> columnPrice, int historyLength, int maxFuture, double r);
}

class MovingVolatility
{
private:
    double *data;
    double sumOfX;
    double squaredSumOfX;
    double meanOfX;
    double varianceOfX;
    int sizeX;
    int indexOfFirst;
    int indexOfLast;
    int dataSize;
public:
    double std;
    MovingVolatility(std::vector<double> v, int intervalLength);
    ~MovingVolatility();
    double step();
    double getVolatility();
};