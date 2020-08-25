/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_BloomFilter_inl_
#define _Stroika_Foundation_Cache_BloomFilter_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <cmath>

#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     ***************************** Cache::BloomFilterOptions ************************
     ********************************************************************************
     */
    inline unsigned int BloomFilterOptions ::OptimizeBitSize (size_t nElements, float desiredFalsePositiveProbability)
    {
        // based on https://en.wikipedia.org/wiki/Bloom_filter (approximate)
        return -nElements * log (desiredFalsePositiveProbability) / log (2) * log (2);
    }
    inline unsigned int BloomFilterOptions::OptimizeNumberOfHashFunctions (size_t setSize, optional<size_t> bitSize)
    {
        size_t useBitSize = bitSize.value_or (OptimizeBitSize (setSize));
        // (m/n)*ln(2)
        return round (double (setSize) / useBitSize) * log (2);
    }
    inline BloomFilterOptions BloomFilterOptions::Optimize () const
    {
        // Assert (fBitCount or fExpectedMaxSetSize);
        BloomFilterOptions tmp = *this;
#if 0
        if (not fBitCount) {
            fBitCount = OptimizeBitSize ()
        }
#endif
        return tmp;
    }

    /*
     ********************************************************************************
     ********************************** Cache::BloomFilter **************************
     ********************************************************************************
     */
    template <typename T>
    inline BloomFilter<T>::BloomFilter (Containers::Sequence<HashFunctionType> hashFunctions, size_t expectedSetSize)
        : fHashFunctions_{hashFunctions}
        , fBits_{}
    {
        fBits_.resize (expectedSetSize, false);
    }
    template <typename T>
    inline BloomFilter<T>::BloomFilter (size_t expectedSetSize)
        : BloomFilter{kDefaultHashFunction, expectedSetSize}
    {
    }
    template <typename T>
    void BloomFilter<T>::Add (ArgByValueType<T> elt)
    {
        size_t sz{fBits_.size ()};
        for (const HashFunctionType& f : fHashFunctions_) {
            fBits_[f (elt) % sz] = true;
        }
    }
    template <typename T>
    inline void BloomFilter<T>::clear ()
    {
        fBits_.clear ();
    }
    template <typename T>
    bool BloomFilter<T>::IsPresent (ArgByValueType<T> elt) const
    {
        size_t sz{fBits_.size ()};
        for (const HashFunctionType& f : fHashFunctions_) {
            if (not fBits_[f (elt) % sz]) {
                return false;
            }
        }
        return true;
    }

}

#endif /*_Stroika_Foundation_Cache_BloomFilter_inl_*/
