## What is this?

I wanted to learn how european options are priced while at the same time work on a project in C++. Mainly because C++ offers very good performance when it comes to large amount of calculations that can't be easily abstracted into matrix operations (otherwise I would be able to easily use some Python library with C++ implementation)

## Data

My first idea was to generate the data from lognormal distribution. I'm sure that could work fine with pricing options using Black-Scholes, not only fine - it would fulfill the assumptions of Black-Scholes. Though I am not convinced that taking the assumptions of lognormal distribution of returns for granted reflects real market behaviour closely enough. And so I decided to use historical data of S&P index. I am using adjusted close price data to avoid adding more confounding effects into the model. Dealing with stock splits and dividends would add unnecessary complexity to my task.

Unfortunately I don't have access to data that would show size of demand for specific options at different times so I need to settle on uniformly random demand for either of the two types of options. Obviously if there are some news that could influence price in the future, market participants will be more likely to buy one or the other type of options. My model won't take into account these possible fluctuations.

## How to use cmake and conan:

1. Install cmake and conan
2. `mkdir build` (in repo root)
3. `cd build`
4. `conan install ..` (this makes conan install and create cmake script based on conanfile.txt in root)
5. `cmake ..` (this makes cmake generate makefiles)
6. `cd ..` (go back to root)
7. `cmake --build build` (this builds the project and puts the binary into /build/bin/)
8. `./build/bin/app` (to run the binary)

## Common issues:

* if you accidentally use `cmake .` inside root, CMakeCache.txt gets created which will make further use of cmake futile. Just delete the cache file and try again. Also you won't do wrong if you delete /build directory before trying again.
* if the program runs slow and you tried to profile it in the past, be sure to disable the `-pg` flag with cmake

## What I learned:

* how cmake and conan work, how to write CMakeLists
* benefits of initialization lists in C++
* how to implement Black-Scholes formula in C++
* profiling C++ code to improve performance

## Profiling

1. `cmake -DCMAKE_CXX_FLAGS=-pg -DCMAKE_EXE_LINKER_FLAGS=-pg -DCMAKE_SHARED_LINKER_FLAGS=-pg --build build` (set flags to compile binary that outputs profiling info)
2. `cmake --build build` (to compile the project)
3. `./build/bin/app SPY.csv` (to run the binary, when the program returns normally, profiling results will be in gmon.out)
4. `gprof build/bin/app gmon.out >> analysis.txt` (to process profiling information into readable format)
5. `cmake -DCMAKE_CXX_FLAGS="" -DCMAKE_EXE_LINKER_FLAGS="" -DCMAKE_SHARED_LINKER_FLAGS="" --build build` (to remove profiling flags)

## Progress

After implementing black-scholes formula according to its specification on [Wikipedia](https://en.wikipedia.org/wiki/Black%E2%80%93Scholes_model#Black%E2%80%93Scholes_formula) I verified its validity by comparing results from my program to those of [Economic Research Institute](https://www.erieri.com/blackscholes). It works. And after running monte carlo experiment with volatility calculated from 1000 past observations (days), continuously compounded daily risk free rate of 0.0055% (2% annual) and possible dates of maturity 1 to 100 I measured the program required 43.9 seconds to perform this simulation on 500 consecutive days.

To run better comparative simulations I need to optimize the code. My first thought is to parallelize it but before that I think I should focus on making it as efficient as possible on single thread. I am using profiler distributed with gcc compiler to see which functions consume most processing time. This is the most important part of the profiler's output:

```
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 20.64      3.15     3.15 2204420836     0.00     0.00  std::vector<double, std::allocator<double> >::at(unsigned long)
 18.31      5.95     2.80 2204420836     0.00     0.00  std::vector<double, std::allocator<double> >::operator[](unsigned long)
 16.97      8.54     2.59 2204421256     0.00     0.00  std::vector<double, std::allocator<double> >::size() const
 15.49     10.90     2.37 2204420836     0.00     0.00  std::vector<double, std::allocator<double> >::_M_range_check(unsigned long) const
 15.20     13.23     2.32  1100000     0.00     0.00  bs::vectorSTD(std::vector<double, std::allocator<double> >&, int, int)
 ```

 Clearly most of the time is spent accessing returns on individual days to compute 1000-day moving volatility. The problem seems to be that I compute volatility each time I am pricing an option with different spot price, maturity day or strike price. The first step will be to make it compute volatility only once for each spot price.

 Then I'm thinking that since I am calculating price of call option twice - once for itself and another time to price its respective put equivalent the required calculations are basically doubled. I will create a new function that computes both at the same time and returns the tuple.

 Finally I think I can speed up calculation of volatility. Instead of calculating it anew for every day from the past X observations, I could simply subtract the contribution of the oldest day and add volatilty contribution for the next day.

 The less I access elements of the `std::vector` using `.at` method, the fewer calls to range check there will be. Perhaps I could rely on my knowledge of the vector size and access the elements directly via `[]`.