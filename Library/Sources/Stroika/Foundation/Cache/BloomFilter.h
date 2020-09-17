/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_BloomFilter_h_
#define _Stroika_Foundation_Cache_BloomFilter_h_ 1

#include "../StroikaPreComp.h"

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
     * From http://en.wikipedia.org/wiki/Bloom_filter: 
     *      fHashFunctionCount corresponds to 'k'
     *      fBitCount corresponds to 'm'
     */
    struct BloomFilterOptions {
        optional<size_t>       fExpectedMaxSetSize;
        optional<size_t>       fBitCount; // allocated bit count
        optional<unsigned int> fHashFunctionCount;
        double                 fDesiredFalsePositivityRate{0.1};

    public:
        nonvirtual BloomFilterOptions Optimize () const;

    public:
        static unsigned int OptimizeBitSize (size_t nElements, double desiredFalsePositiveProbability = 0.1);
        static unsigned int OptimizeNumberOfHashFunctions (size_t setSize, optional<size_t> bitSize = nullopt);

    public:
        /**
         * From http://en.wikipedia.org/wiki/Bloom_filter: 
         *      (1-exp(-kn/m))^^k
         * 
         *      n elements inserted so far (other letter/vars see struct docs above)
         */
        static double     ProbabilityOfFalsePositive (int hashFunctionCount, int bitCount, int nElementsInsertedSoFar);
        nonvirtual double ProbabilityOfFalsePositive (int nElementsInsertedSoFar) const;
    };

    /**
     * \brief a Bloom filter is a probablistic set, which returns either "probably in set" or "definitely not in set"
     *
     * Description:
     *      http://en.wikipedia.org/wiki/Bloom_filter
     * 
     *  TODO:
     *      @todo MAYBE KEEP Options object in BloomFilter (readonly)
     *      @todo MAYBE subclass (in template) Options to take actual hash functions as OPTIONS arg with right types
     *      @todo CLARIFY if using exact given hash functions or number from options calculation
     *      @todo MAYBE redo options object so HAS FIELDS of actual choices but takes ARGS that say optional and 
     *            those get filled in with optimal values in CTOR.
     */
    template <typename T>
    class BloomFilter {
    public:
        using HashFunctionType = function<size_t (T)>;

    public:
        using Options = BloomFilterOptions;

    public:
        static const inline HashFunctionType kDefaultHashFunction = hash<T>{};

    public:
        /**
         */
        BloomFilter (const Containers::Sequence<HashFunctionType>& hashFunctions, const BloomFilterOptions& options);
        BloomFilter (const Options& options);
        BloomFilter (const BloomFilter& src) = default;
        BloomFilter& operator= (const BloomFilter& src) = default;

    public:
        /**
         *  Not necessarily exactly the constructor options but the effective values used.
         */
        nonvirtual Options GetEffectiveOptions () const;

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

    private:
        vector<HashFunctionType> fHashFunctions_;
        vector<bool>             fBits_; // @todo use Set_Bitstring
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BloomFilter.inl"

#endif /*_Stroika_Foundation_Cache_BloomFilter_h_ */
