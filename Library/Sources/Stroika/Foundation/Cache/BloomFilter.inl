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

#include "../Characters/StringBuilder.h"
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
        return static_cast<unsigned int> (::ceil ((double (useBitSize) / setSize) * log (2)));
    }
    inline double BloomFilterOptions::ProbabilityOfFalsePositive (int hashFunctionCount, int bitCount, int nElementsInsertedSoFar)
    {
        // From https://en.wikipedia.org/wiki/Bloom_filter
        return ::pow (1 - ::exp (-static_cast<double> (hashFunctionCount * nElementsInsertedSoFar) / bitCount), hashFunctionCount);
    }
    inline double BloomFilterOptions::ProbabilityOfFalsePositive (int nElementsInsertedSoFar) const
    {
        Require (fHashFunctionCount);
        Require (fBitCount);
        return ProbabilityOfFalsePositive (*fHashFunctionCount, *fBitCount, nElementsInsertedSoFar);
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
    inline Characters::String BloomFilterOptions::ToString () const
    {
        Characters::StringBuilder sb;
        sb += L"{";
        sb += L"fExpectedMaxSetSize: " + Characters::ToString (fExpectedMaxSetSize) + L", ";
        sb += L"}";
        sb += L"{";
        sb += L"fBitCount: " + Characters::ToString (fBitCount) + L", ";
        sb += L"}";
        sb += L"{";
        sb += L"fHashFunctionCount: " + Characters::ToString (fHashFunctionCount) + L", ";
        sb += L"}";
        sb += L"{";
        sb += L"fDesiredFalsePositivityRate: " + Characters::ToString (fDesiredFalsePositivityRate) + L", ";
        sb += L"}";
        return sb.str ();
    }

    /*
     ********************************************************************************
     ************************* Cache::BloomFilter<T>::Options ***********************
     ********************************************************************************
     */
    template <typename T>
    BloomFilter<T>::Options::Options (const BloomFilterOptions& o)
        : BloomFilterOptions{o}
        , fHashFunctions{Containers::Sequence<HashFunctionType>{hash<T>{}}}
    {
    }
    template <typename T>
    BloomFilter<T>::Options::Options (size_t expectedMaxSetSize, const HashFunctionType& defaultHashFunction)
        : BloomFilterOptions{expectedMaxSetSize}
        , fHashFunctions{defaultHashFunction}
    {
    }
    template <typename T>
    BloomFilter<T>::Options::Options (const Containers::Sequence<HashFunctionType>& hashFunctions, size_t bitCount)
        : BloomFilterOptions{nullopt, bitCount, hashFunctions.GetLength ()}
        , fHashFunctions{hashFunctions}
    {
    }
    template <typename T>
    Characters::String BloomFilter<T>::Options::ToString () const
    {
        Characters::StringBuilder sb{BloomFilterOptions::ToString ().SubString (0, -1)};
        sb += L", fHashFunctions.length: " + Characters::ToString (fHashFunctions.GetLength ());
        sb += L"}";
        return sb.str ();
    }

    /*
     ********************************************************************************
     ********************************** Cache::BloomFilter **************************
     ********************************************************************************
     */
    template <typename T>
    inline BloomFilter<T>::BloomFilter (const Containers::Sequence<HashFunctionType>& hashFunctions, const BloomFilterOptions& options)
        : fBits_{}
    {
        Require (hashFunctions.size () >= 1);
        BloomFilterOptions o = options.Optimize ();
        // @todo only repeat enuf to get at least desired number of hash functions
        fHashFunctions_ = hashFunctions.Repeat (*o.fHashFunctionCount).template As<vector<HashFunctionType>> ();
        if (*o.fHashFunctionCount < fHashFunctions_.size ()) {
            fHashFunctions_.erase (fHashFunctions_.begin () + *o.fHashFunctionCount, fHashFunctions_.end ());
        }
        Ensure (fHashFunctions_.size () >= 1);
        Ensure (fHashFunctions_.size () == *o.fHashFunctionCount);
        fBits_.resize (*o.fBitCount, false);
        Ensure (fBits_.size () >= 1);
    }
    template <typename T>
    inline BloomFilter<T>::BloomFilter (const Options& options)
        : BloomFilter{options.fHashFunctions, options}
    {
    }
    template <typename T>
    inline auto BloomFilter<T>::GetEffectiveOptions () const -> Options
    {
        return Options{Containers::Sequence<HashFunctionType> (fHashFunctions_), fBits_.size ()};
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
