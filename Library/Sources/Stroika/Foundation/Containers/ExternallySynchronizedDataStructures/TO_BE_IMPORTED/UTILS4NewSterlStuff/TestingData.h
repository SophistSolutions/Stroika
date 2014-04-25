#pragma once

#include "Utils.h"



typedef enum DataDistribution {
    eUniformDist,
    eNormalizedDist,
    eZipfDist
} DataDistribution;

extern  const char* DisplayDistribution (DataDistribution d);



// fill in vector indices with indexes into vector v, based on datadistribution dd
template <typename T>
void    GetDistributedIndex (const std::vector<T>& v, std::vector<size_t>* indices, DataDistribution dd);

class   NormalDistribution {
public:
    NormalDistribution (double range) :
        fDistribution (range / 2, range / 6)
    {
    }

    template<class Engine>
    double operator()(Engine& eng)
    {
        return fDistribution(eng);
    }

private:
    std::normal_distribution<double> fDistribution;
};

class ZipfDistribution {
public:
    ZipfDistribution (size_t range) :
        fDistribution (range, 1, range, CalcDistribution)
    {
    }

    ~ZipfDistribution ()
    {
    }


    template<class Engine>
    size_t operator()(Engine& eng)
    {
        return fDistribution(eng);
    }

private:
    std::discrete_distribution<size_t> fDistribution;

    static  double  CalcDistribution (double k)
    {
        // our input is a not discrete, but somewhere "in the middle" of our range, which doesn't make sense for indexes
        k = (size_t) k;
        double result = std::pow (k, -1);   // in full Mandelbrot generality this is std::pow (k+shift, -exp), but for classic Zipf shift = 0 and exp = 1
        return result;
    }
};

template <typename T>
class   TestSet {
public:
    TestSet (size_t elementsToAdd, bool scrambled);

    const std::vector<T>&   GetData () const;
    const std::vector<T>&   GetMissing () const ;

    const std::vector<size_t>&  GetUniformIndices ();
    const std::vector<size_t>&  GetNormalIndices ();
    const std::vector<size_t>&  GetZipfIndices ();

    const std::vector<size_t>&  GetIndices (DataDistribution dd);

    const std::vector<size_t>&  GetMissingIndices ();

    // when doing timings, want everything created ahead of time
    void    PreLoad ();

private:
    std::vector<T>  fData;
    std::vector<T>  fMissing;

    std::vector<size_t> fUniformIndices;
    std::vector<size_t> fNormalIndices;
    std::vector<size_t> fZipfIndices;
    std::vector<size_t> fMissingIndices;    // for now, just one to missing and that with uniform distribution
};

/*
    Helper function for FillTestData. To support a new data type for test data, you specialize FillTestData.
    Routine should fill vector data with elementsToAdd elements, ensuring that each element has a different value. In addition,
    it should account for offset in such a way that any element added to a vector with offset true, should not be included in
    a vector built with offset false. This lets TestData create the needed Data and Missing vectors.
    Prebuild for case of size_t and string.
*/
template    <typename T>
void    FillTestData (std::vector<T>& data, size_t elementsToAdd, bool offset)
{
    AssertNotReached ();
}

inline  size_t  ComputeUniqueValue (size_t index, bool offset)
{
    return index * 2 + ((offset) ? 1 : 0);
}


#include "TestingData.inl"
