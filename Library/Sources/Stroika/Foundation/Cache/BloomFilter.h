/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_BloomFilter_h_
#define _Stroika_Foundation_Cache_BloomFilter_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/ToString.h"
#include "../Configuration/Common.h"
#include "../Configuration/TypeHints.h"
#include "../Containers/Sequence.h"
#include "../Containers/Set.h"

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
        using HashFunctionType = function<size_t (T)>;

    public:
        using HashResultType = invoke_result_t (HashFunctionType);

    public:
        static inline constexpr double kDefaultDesiredFalsePositivityRate = 0.1;

    public:
        /**
         *  The constructor with an explicit set of hash functions and bitCount uses those hash functions and bitCount.
         *  The construcotr taking  expectedMaxSetSize calls OptimizeBitSize and OptimizeNumberOfHashFunctions to calculate
         *  optimimum numbers of hash functions and bitstorage for the given expected set size. It also uses DeriveIndependentHashFunctions ()
         *  to automatically create (semi) independent hash functions from the original argument one (this maybe could use tuning/work).
         */
        BloomFilter (const Containers::Sequence<HashFunctionType>& hashFunctions, size_t bitCount);
        BloomFilter (size_t expectedMaxSetSize, const HashFunctionType& defaultHashFunction = hash<T>{}, double desiredFalsePositivityRate = kDefaultDesiredFalsePositivityRate);
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
        static double     ProbabilityOfFalsePositive (int hashFunctionCount, int bitCount, int nElementsInsertedSoFar);
        nonvirtual double ProbabilityOfFalsePositive (int nElementsInsertedSoFar) const;

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
