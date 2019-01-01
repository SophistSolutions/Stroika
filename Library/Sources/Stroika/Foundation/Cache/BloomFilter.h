/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_BloomFilter_h_
#define _Stroika_Foundation_Cache_BloomFilter_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "../Memory/SharedByValue.h"

/**
 *
 * Description:
 *      http://en.wikipedia.org/wiki/Bloom_filter
 *
 * TODO:
 *      @todo   Biggest question is whether this is just a single implemeentaiton or virtual
 *              abstraction with different backend implementations (probably the latter).
 *
 *      @todo   interesting if we want to have an AsIterable<T>() method - so you can iterate
 *              over something in the filter? But probably NOT since its not clear what you could
 *              return (due to nature of bloom filter you don't store the stuff, but the hashes)
 *
 */

namespace Stroika::Foundation::Cache {

    /**
     *  @todo   very rough initial draft of API
     */
    template <typename T>
    class BloomFilter<T> {
    public:
        nonvirtual void Add (ArgByValueType<T> elt);

    public:
        nonvirtual void Remove (ArgByValueType<T> elt);

    public:
        /**
         *  False positive retrieval results are possible, but false negatives are not;
         */
        nonvirtual void IsPresent (T elt) const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cache_BloomFilter_h_ */
