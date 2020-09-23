/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_BloomFilter_h_
#define _Stroika_Foundation_Cache_BloomFilter_h_ 1

#include "../StroikaPreComp.h"

#include <type_traits>

#include "../Characters/ToString.h"
#include "../Configuration/Common.h"
#include "../Configuration/TypeHints.h"
#include "../Containers/Sequence.h"
#include "../Containers/Set.h"
#include "../Cryptography/Digest/Hash.h"

/**
 *
 *
 */
namespace Stroika::Foundation::Cache {

    /**
     * \brief a Bloom filter is a probablistic set, which returns either "probably in set" or "definitely not in set"
     *
     * Description:
     *      http://en.wikipedia.org/wiki/Bloom_filter
     *          fHashFunction.size () corresponds to 'k'
     *          fBitCount corresponds to 'm'
     */
    template <typename T>
    class BloomFilter {
    public:
        using HashResultType = uint64_t; // often something smaller will be used, but best to pick a wider type here, and @todo maybe make it a template parameter

    public:
        using HashFunctionType = function<HashResultType (T)>;

    public:
        static inline constexpr double kDefaultDesiredFalsePositivityRate = 0.1;

    public:
        /**
         *  The constructor with an explicit set of hash functions and bitCount uses those hash functions and bitCount.
         *  The constructor taking expectedMaxSetSize calls OptimizeBitSize and OptimizeNumberOfHashFunctions to calculate
         *  optimimum numbers of hash functions and bitstorage for the given expected set size. It also uses DeriveIndependentHashFunctions ()
         *  to automatically create (semi) independent hash functions from the original argument one (this maybe could use tuning/work).
         * 
         *  \note - for the hash functions, you can simple use std::hash<T>, but the hash implementation on gcc (libstdc++) works quite badly
         *          as a crytographic hash for integer types. So the default is to use the Stroika hash function.
         */
        BloomFilter (const Containers::Sequence<HashFunctionType>& hashFunctions, size_t bitCount);
        BloomFilter (size_t expectedMaxSetSize, const HashFunctionType& defaultHashFunction = Cryptography::Digest::Hash<T>{}, double desiredFalsePositivityRate = kDefaultDesiredFalsePositivityRate);
        BloomFilter (const BloomFilter& src) = default;
        BloomFilter& operator= (const BloomFilter& src) = default;

    public:
        static unsigned int OptimizeBitSize (size_t nElements, double desiredFalsePositiveProbability = kDefaultDesiredFalsePositivityRate);

    public:
        static unsigned int OptimizeNumberOfHashFunctions (size_t setSize, size_t bitSize);

    public:
        /**
         * From http://en.wikipedia.org/wiki/Bloom_filter: 
         *      (1-exp(-kn/m))^^k
         * 
         *      n elements inserted so far (other letter/vars see struct docs above)
         */
        static double     ProbabilityOfFalsePositive (size_t hashFunctionCount, size_t bitCount, size_t nElementsInsertedSoFar);
        nonvirtual double ProbabilityOfFalsePositive (size_t nElementsInsertedSoFar) const;

    public:
        static Containers::Sequence<HashFunctionType> DeriveIndependentHashFunctions (const HashFunctionType& h, size_t repeatCount);

    public:
        /**
         */
        nonvirtual void Add (Configuration::ArgByValueType<T> elt);

    public:
        /**
         */
        nonvirtual void clear ();

    public:
        /**
         *  \brief returns true if this Contains (elt) - and probably false otherwise.
         *
         *  False positive retrieval results are possible, but false negatives are not;
         * 
         *  Call GetEffectiveOptions ().ProbabilityOfFalsePositive (nEltsAdded) to an estimate of the probability of false positives.
         */
        nonvirtual bool Contains (Configuration::ArgByValueType<T> elt) const;

    public:
        struct Statistics;

    public:
        /**
         */
        nonvirtual Statistics GetStatistics () const;

    private:
        vector<HashFunctionType> fHashFunctions_;
        vector<bool>             fBits_; // @todo use Set_Bitstring
    };

    /**
     */
    template <typename T>
    struct BloomFilter<T>::Statistics {
        size_t fHashFunctions;
        size_t fBitCount;
        size_t fBitsSet;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual Characters::String ToString () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BloomFilter.inl"

#endif /*_Stroika_Foundation_Cache_BloomFilter_h_ */
