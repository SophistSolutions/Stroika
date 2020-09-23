/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Hash_h_
#define _Stroika_Foundation_Cryptography_Digest_Hash_h_ 1

#include "../../StroikaPreComp.h"

#include <cstdint>

#include "../../Characters/String.h"

#include "Digester.h"

/*
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 */

namespace Stroika::Foundation::Cryptography::Digest {

    /**
     *  Mimic the behavior of std::hash<> - except hopefully pick a better default algorithm than gcc did.
     *  (https://news.ycombinator.com/item?id=13745383)
     * 
     *  Generally replaces the same types 'T' as defined https://en.cppreference.com/w/cpp/utility/hash specialized,
     *  but just in case, the default algorithm / implementation just defaults to hash<T>{} ();
     * 
     *  This template is NOT defined for all T, and must be explicitly specialized for many user types, because we dont
     *  know in general how to compute the digest of an arbitrary C++ type.
     * 
     *  This class is 'pre-specialized' for:
     *      o   all builtin numeric types, int, char, unsigned int, long etc...
     *      o   all types for which std::hash<T> is defined
     *      o   Characters::String
     */
    template <typename T>
    struct Hash {
        size_t operator() (const T& t) const;
    };

    /**
     *  \brief combine two hash values to produce a new hash value
     * 
     *  inspired by https://en.cppreference.com/w/cpp/utility/hash - return h1 ^ (h2 << 1); // or use boost::hash_combine
     */
    template <typename RESULT_TYPE>
    RESULT_TYPE HashValueCombine (RESULT_TYPE lhs, RESULT_TYPE rhs);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Hash.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_Hash_h_*/
