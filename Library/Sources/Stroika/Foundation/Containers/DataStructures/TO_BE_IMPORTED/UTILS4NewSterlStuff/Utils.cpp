#include "../../Shared/Headers/Utils.h"


std::mt19937&   GetRandomNumberEngine ()
{
    static  std::mt19937 sEngine;
    static  bool    sFirstTime = true;
    if (sFirstTime) {
        sFirstTime = false;
        sEngine.seed (static_cast<unsigned int> (time (NULL)));
    }
    return sEngine;
}

size_t RandomSize_t (size_t first, size_t last)
{
    std::mt19937& eng = GetRandomNumberEngine ();
    Assert (eng.min () <= first);
// Assert (eng.max () >= last);    // g++ has 8 byte size_t in 64 bit??

    std::uniform_int_distribution<size_t> unif (first, last);
    size_t result = unif (eng);

    Ensure (result >= first);
    Ensure (result <= last);
    return result;
}


size_t  NormallyDistributedRandomNumber (size_t first, size_t last)
{
    Require (last > first);

    double  range = (last - first);

    std::mt19937& eng = GetRandomNumberEngine ();
    Assert (eng.min () <= 0);
    Assert (eng.max () >= range);


    std::normal_distribution<double> nDist (range / 2, range / 6);
    double result = nDist (eng);

    while (result > range or result < 0) {  // about 1% of the time we will get a number outside of our range, given the standard deviation we set
        result = nDist (eng);
    }
    result += first;
    size_t results = size_t (result) + first;

    Ensure (result >= first);
    Ensure (result <= last);
    return results;
}
