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
#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Cache {

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
