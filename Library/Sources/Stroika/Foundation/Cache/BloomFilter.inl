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
#include "../Execution/Exceptions.h"
#include "../Math/Common.h"

namespace Stroika::Foundation::Cache {

    /*
     ********************************************************************************
     ***************************** Cache::BloomFilterOptions ************************
     ********************************************************************************
     */
    inline unsigned int BloomFilterOptions::OptimizeBitSize (size_t nElements, double desiredFalsePositiveProbability)
    {
        // based on https://en.wikipedia.org/wiki/Bloom_filter (approximate)
        return static_cast<unsigned int> (::ceil (-static_cast<double> (nElements) * log (desiredFalsePositiveProbability) / log (2) * log (2)));
    }
    inline unsigned int BloomFilterOptions::OptimizeNumberOfHashFunctions (size_t setSize, optional<size_t> bitSize)
    {
        size_t useBitSize = bitSize.value_or (OptimizeBitSize (setSize));
        // based on https://en.wikipedia.org/wiki/Bloom_filter - (m/n)*ln(2)
        return static_cast<unsigned int> (::ceil ((double (setSize) / useBitSize) * log (2)));
    }
    inline BloomFilterOptions BloomFilterOptions::Optimize () const
    {
        Require (fBitCount or fExpectedMaxSetSize);
        Require (fHashFunctionCount or fExpectedMaxSetSize);
        BloomFilterOptions tmp = *this;
        if (not tmp.fBitCount) {
            if (tmp.fExpectedMaxSetSize) {
                tmp.fBitCount = OptimizeBitSize (*tmp.fExpectedMaxSetSize, tmp.fDesiredFalsePositivityRate);
            }
        }
        if (not tmp.fHashFunctionCount) {
            tmp.fHashFunctionCount = OptimizeNumberOfHashFunctions (*tmp.fExpectedMaxSetSize, *tmp.fBitCount);
        }
        Ensure (tmp.fBitCount);
        Ensure (tmp.fHashFunctionCount);
        return tmp;
    }

    /*
     ********************************************************************************
     ********************************** Cache::BloomFilter **************************
     ********************************************************************************
     */
    template <typename T>
    inline BloomFilter<T>::BloomFilter (const Containers::Sequence<HashFunctionType>& hashFunctions, const BloomFilterOptions& options)
        : fHashFunctions_{hashFunctions.As<vector<HashFunctionType>> ()}
        , fBits_{}
    {
        BloomFilterOptions o = options.Optimize ();
        if (*o.fHashFunctionCount != fHashFunctions_.size ()) {
            AssertNotReached (); // must manually create functions here
        }
        fBits_.resize (*o.fBitCount, false);
    }
    template <typename T>
    inline BloomFilter<T>::BloomFilter (const BloomFilterOptions& options)
        : BloomFilter{Containers::Sequence<HashFunctionType>{kDefaultHashFunction}, options}
    {
    }
    template <typename T>
    void BloomFilter<T>::Add (Configuration::ArgByValueType<T> elt)
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
    bool BloomFilter<T>::Contains (Configuration::ArgByValueType<T> elt) const
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
