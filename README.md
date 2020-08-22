## What is this?

I wanted to learn how european options are priced while at the same time work on a project in C++. Mainly because C++ offers very good performance when it comes to large amount of calculations that can't be easily abstracted into matrix operations (otherwise I would be able to easily use some Python library with C++ implementation).

## Data

My first idea was to generate the data from lognormal distribution. I'm sure that could work fine for pricing options using Black-Scholes, not only fine - it would fulfill the assumptions of Black-Scholes. Though I am not convinced that taking the assumptions of lognormal distribution of returns for granted reflects real market behaviour closely enough. And so I decided to use historical data of S&P index. I am using adjusted close price data to avoid adding more confounding effects into the model. Dealing with stock splits and dividends would add unnecessary complexity to my task.

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

### Black-Scholes

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

 After calculating 1000-day moving volatility only once for every spot price day and calculating price of puts and calls simultaneously, the same simulation was executed in ~0.11 seconds. That's nearly 400-fold speedup.

 ```
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 60.03      0.03     0.03   550000     0.00     0.00  bs::getCallPut(int, double, double, double, double)
 20.01      0.04     0.01  2121836     0.00     0.00  std::vector<double, std::allocator<double> >::operator[](unsigned long)
 20.01      0.05     0.01        1    10.00    49.92  mc::experiment(std::vector<double, std::allocator<double> >, std::vector<double, std::allocator<double> >, int, int, double)
  0.00      0.05     0.00  1200000     0.00     0.00  __gnu_cxx::__normal_iterator<float*, std::vector<float, std::allocator<float> > >::base() const
```

There is still room for further improvement as can be seen from the number of accesses to `std::vector`. So I implement the moving mean/volatility optimization. It is based on the fact that:

<img src="https://render.githubusercontent.com/render/math?math=\Huge{Var(x)=\sum{(x_{i}-\mu)^{2}}}">

Which can be expanded into:

<img src="https://render.githubusercontent.com/render/math?math=\Huge{Var(x)={\frac{{\sum}x_i^2}{N}}-\mu^2}">

And so for every new consecutive volatility calculation I subtract the first square component from a variable containing the sum of all squares and add the new square component. Similarly I subtract weighted component of the first element in the mean variable and add weighted component of the next element. Then I simply compute variance by the formula above.

Unfortunately this method doesn't reliably work on computers. When very small numbers are subtracted from substantially larger numbers of float or double type, due to how these types are defined the operations create errors. Errors so significant that in my case only after a few iterations variance becomes negative.

I thought I would implement a function for volatility that would calculate sums and squared sums in one loop and then using the formula above compute standard deviation. There should be N fewer arithmetic operations than if I first summed up all the elements to get the mean and then loop and subtract the mean from each element and square the result.

```
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 29.28      0.12     0.12  6405300     0.00     0.00  bs::getCallPut(int, double, double, double, double)
 21.96      0.21     0.09        1    90.04   410.09  mc::experiment(std::vector<double, std::allocator<double> 
  7.32      0.32     0.03     5823     0.01     0.01  bs::vectorSTDfaster(std::vector<double, 
  2.44      0.40     0.01     5823     0.00     0.01  bs::vectorSTD(std::vector<double, std::allocator<double> >&, int, int)
  ```

  But apparently it's three times slower. Which I am not able to explain. At least the program with the same parameters when run on the entire dataset finishes within a few seconds. Optimization was successful.
  
  ### Binomial Model
  
  Next I want to implement binomial pricing model for the same options. I am using Cox-Ross-Rubinstein method to find the probability of price going up or down by some factor and the factor itself considering the assumption that the asset price behaves in the same way as it would in risk neutral world.
  
  With the knowledge of these three variables I can explore all price paths and the payoff of the option will simply be the expected payoff of all the options in all the paths.
  
  To skip the need to compute the whole binary tree of possible outcomes I decided to use the binomial expansion formula by employing "n choose k" formula to compute the probability coefficients of all outcomes. I ran an experiment to price options using binomial model with 50 steps (depth of the tree). But for some reason option values calculate using this method were very off - even NEGATIVE. Options can't have negative values.
  
  I used CLion's GDB debugger to step through the computations to see where the option valuation turned negative. Eventually I found out that it was the factorial function in the formula for calculation of binomial coefficients. With 50 steps (= values on 50th row of Pascal's triangle) the factorial function would quickly overflow the int type. Even with the optimization of calculating reduced factorials the multiplication would still overflow the int type.
  
  After researching a bit how different types in C are implemented and what precision they offer I decided to use double floating point type instead. I could have used long long int but it has only range about +-2 * 10^19 while double has range about +-2 * 10^308. This also means that the double calculations will be more imprecise with growing values. The algorithm should now be able to compute option values with binomial models up till depth about 170. Then the same problem arises and it must be implemented differently.
  
  There is another way that these calculations could be done efficiently with any depth. Calculating new coefficients inductively from past coefficients according to the formula:
  
<img src="https://render.githubusercontent.com/render/math?math=\Huge{Combinations(x)={Combinations(x-1)\times\frac{depth-i{%2b}1}{i}}}">


This way one could have a vector of size depth/2 where all partial results are stored and the coefficient will multiply the probabilities one partial result at the time. Perhaps I will implement this later.

When compared to the Black-Scholes formula, experiments show that binomial model also makes profitable valuation of options. Although not as profitable as options priced using B-S. Side by side comparison of individual options priced using binomial model and B-S formula show that they are usually very similar and sometimes even identical. With sufficient depth of binomial model I'm sure we would eventually converge.

### Monte Carlo