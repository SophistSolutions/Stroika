/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
     ********************** Cache::BloomFilter<T>::Statistics ***********************
     ********************************************************************************
     */
    template <typename T>
    inline double BloomFilter<T>::Statistics::GetFractionFull () const
    {
        return static_cast<double> (fBitsSet) / static_cast<double> (fBitCount);
    }
    template <typename T>
    inline double BloomFilter<T>::Statistics::ProbabilityOfFalsePositive () const
    {
        return BloomFilter<T>::ProbabilityOfFalsePositive (fHashFunctions, fBitCount, fApparentlyDistinctAddCalls);
    }
    template <typename T>
    Characters::String BloomFilter<T>::Statistics::ToString () const
    {
        Characters::StringBuilder sb;
        sb += L"{";
        sb += L"fHashFunctions: " + Characters::ToString (fHashFunctions) + L", ";
        sb += L"fBitCount: " + Characters::ToString (fBitCount) + L", ";
        sb += L"fBitsSet: " + Characters::ToString (fBitsSet) + L", ";
        sb += L"fActualAddCalls: " + Characters::ToString (fActualAddCalls) + L", ";
        sb += L"fApparentlyDistinctAddCalls: " + Characters::ToString (fApparentlyDistinctAddCalls) + L", ";
        sb += L"ProbabilityOfFalsePositive: " + Characters::ToString (ProbabilityOfFalsePositive ());
        sb += L"}";
        return sb.str ();
    }

    /*
     ********************************************************************************
     ****************************** Cache::BloomFilter ******************************
     ********************************************************************************
     */
    template <typename T>
    inline BloomFilter<T>::BloomFilter (const Containers::Sequence<HashFunctionType>& hashFunctions, size_t bitCount)
        : fHashFunctions_{hashFunctions.template As<vector<HashFunctionType>> ()}
    {
        Require (fHashFunctions_.size () >= 1);
        Require (bitCount >= 1);
        fBits_.resize (bitCount, false);
    }
    template <typename T>
    BloomFilter<T>::BloomFilter (size_t expectedMaxSetSize, const HashFunctionType& defaultHashFunction, double desiredFalsePositivityRate)
    {
        size_t bitCount        = OptimizeBitSize (expectedMaxSetSize, desiredFalsePositivityRate);
        size_t hashFunctionCnt = OptimizeNumberOfHashFunctions (expectedMaxSetSize, bitCount);
        fBits_.resize (bitCount, false);
        fHashFunctions_ = DeriveIndependentHashFunctions (defaultHashFunction, hashFunctionCnt).template As<vector<HashFunctionType>> ();
    }
    template <typename T>
    inline unsigned int BloomFilter<T>::OptimizeBitSize (size_t nElements, double desiredFalsePositiveProbability)
    {
        // based on https://en.wikipedia.org/wiki/Bloom_filter (approximate)
        return static_cast<unsigned int> (::ceil (-static_cast<double> (nElements) * log (desiredFalsePositiveProbability) / log (2) * log (2)));
    }
    template <typename T>
    inline unsigned int BloomFilter<T>::OptimizeNumberOfHashFunctions (size_t setSize, size_t bitSize)
    {
        // based on https://en.wikipedia.org/wiki/Bloom_filter - (m/n)*ln(2)
        return static_cast<unsigned int> (::ceil ((double (bitSize) / setSize) * log (2)));
    }
    template <typename T>
    inline double BloomFilter<T>::ProbabilityOfFalsePositive (size_t hashFunctionCount, size_t bitCount, size_t nElementsInsertedSoFar)
    {
        // From https://en.wikipedia.org/wiki/Bloom_filter
        return ::pow (1 - ::exp (-static_cast<double> (hashFunctionCount * nElementsInsertedSoFar) / bitCount), hashFunctionCount);
    }
    template <typename T>
    inline double BloomFilter<T>::ProbabilityOfFalsePositive (size_t nElementsInsertedSoFar) const
    {
        return ProbabilityOfFalsePositive (fHashFunctions_.size (), fBits_.size (), nElementsInsertedSoFar);
    }
    template <typename T>
    auto BloomFilter<T>::DeriveIndependentHashFunctions (const HashFunctionType& h, size_t repeatCount) -> Containers::Sequence<HashFunctionType>
    {
        Require (repeatCount >= 1);
        /**
         *  From https://en.wikipedia.org/wiki/Bloom_filter
         *
         *      The requirement of designing k different independent hash functions can be prohibitive
         *      for large k. For a good hash function with a wide output, there should be little if any 
         *      correlation between different bit-fields of such a hash, so this type of hash can be 
         *      used to generate multiple "different" hash functions by slicing its output into multiple 
         *      bit fields. Alternatively, one can pass k different initial values (such as 0, 1, ..., k − 1) 
         *      to a hash function that takes an initial value; or add (or append) these values to the key
         *
         *  This trick here - is something similar to the suggestions in the wiki article - deriving
         *  a hash function by combining a unique seed (by hashing a constant) and combining that with the
         *  has result of the original function.
         */
        Containers::Sequence<HashFunctionType> result{h};
        for (size_t i = 1; i < repeatCount; ++i) {
            HashResultType seed = Cryptography::Digest::Hash<HashResultType>{}(i);
            result += [=] (const T& t) { return Cryptography::Digest::HashValueCombine (h (t), seed); };
        }
        return result;
    }
    template <typename T>
    bool BloomFilter<T>::Add (Configuration::ArgByValueType<T> elt)
    {
        size_t sz{fBits_.size ()};
        bool   apparentlyNewAddition = false;
        for (const HashFunctionType& f : fHashFunctions_) {
            size_t idx = static_cast<size_t> (f (elt) % sz);
            if (not fBits_[idx]) {
                apparentlyNewAddition = true;
            }
            fBits_[idx] = true;
        }
        ++fActualAddCalls_;
        if (apparentlyNewAddition) {
            ++fApparentlyDistinctAddCalls_;
        }
        return apparentlyNewAddition;
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
    template <typename T>
    auto BloomFilter<T>::GetStatistics () const -> Statistics
    {
        size_t nTrue{};
        for (bool i : fBits_) {
            if (i) {
                ++nTrue;
            }
        }
        return Statistics{fHashFunctions_.size (), fBits_.size (), nTrue, fActualAddCalls_, fApparentlyDistinctAddCalls_};
    }

}

#endif /*_Stroika_Foundation_Cache_BloomFilter_inl_*/
