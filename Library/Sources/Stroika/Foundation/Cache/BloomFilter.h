/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_BloomFilter_h_
#define _Stroika_Foundation_Cache_BloomFilter_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
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

    public:
        static unsigned int OptimizeBitSize (size_t nElements, float desiredFalsePositiveProbability = 0.1);
        static unsigned int OptimizeNumberOfHashFunctions (size_t setSize, optional<size_t> bitSize = nullopt);

    public:
        nonvirtual BloomFilterOptions Optimize () const;
    };

    /**
     * \brief a Bloom filter is a probablistic set, which returns either "possibly in set" or "definitely not in set"
     *
     * Description:
     *      http://en.wikipedia.org/wiki/Bloom_filter
     */
    template <typename T>
    class BloomFilter {
    public:
        using HashFunctionType = function<size_t (T)>;

    public:
        static constexpr inline HashFunctionType kDefaultHashFunction = hash<T>;

    public:
        /**
         *  nHashFunctions corresponds to 'k' in http://en.wikipedia.org/wiki/Bloom_filter
         *  expectedSetSize corresponds to 'm' in http://en.wikipedia.org/wiki/Bloom_filter
         */
        BloomFilter (Containers::Sequence<HashFunctionType> hashFunctions, size_t expectedSetSize);
        BloomFilter (size_t expectedSetSize);
        BloomFilter (const BloomFilter& src) = default;
        BloomFilter& operator= (const BloomFilter& src) = default;

    public:
        /**
         */
        nonvirtual void Add (ArgByValueType<T> elt);

    public:
        /**
         */
        nonvirtual void clear ();

    public:
        /**
         *  False positive retrieval results are possible, but false negatives are not;
         */
        nonvirtual void IsPresent (ArgByValueType<T> elt) const;

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
