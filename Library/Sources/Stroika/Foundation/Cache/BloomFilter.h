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
    };

    /**
     * \brief a Bloom filter is a probablistic set, which returns either "probably in set" or "definitely not in set"
     *
     * Description:
     *      http://en.wikipedia.org/wiki/Bloom_filter
     */
    template <typename T>
    class BloomFilter {
    public:
        using HashFunctionType = function<size_t (T)>;

    public:
        static const inline HashFunctionType kDefaultHashFunction = hash<T>{};

    public:
        /**
         *  nHashFunctions corresponds to 'k' in http://en.wikipedia.org/wiki/Bloom_filter
         *  expectedSetSize corresponds to 'm' in http://en.wikipedia.org/wiki/Bloom_filter
         */
        BloomFilter (const Containers::Sequence<HashFunctionType>& hashFunctions, const BloomFilterOptions& options);
        BloomFilter (const BloomFilterOptions& options);
        BloomFilter (const BloomFilter& src) = default;
        BloomFilter& operator= (const BloomFilter& src) = default;

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
