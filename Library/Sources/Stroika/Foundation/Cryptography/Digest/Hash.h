/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Hash_h_
#define _Stroika_Foundation_Cryptography_Digest_Hash_h_ 1

#include "../../StroikaPreComp.h"

#include <cstdint>

#include "../../Characters/String.h"

#include "Algorithm/SuperFastHash.h"

#include "Digester.h"

/*
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 */

namespace Stroika::Foundation::Cryptography::Digest {

    /**
     *  Mimic the behavior of std::hash<> - except hopefully pick a better default algorithm than gcc did.
     *  (https://news.ycombinator.com/item?id=13745383)
     * 
     *  Generally replaces the same types 'T' as defined https://en.cppreference.com/w/cpp/utility/hash specialized,
     *  but just in case, the default algorithm / implementation just defaults to hash<T>{} ();
     */
    template <typename T>
    struct Hash {
        size_t operator() (const T& t) const
        {
            return hash<T>{}(t);
        }
    };

    template <typename RESULT_TYPE>
    inline RESULT_TYPE HashCombine (RESULT_TYPE lhs, RESULT_TYPE rhs)
    {
        // inspired by https://en.cppreference.com/w/cpp/utility/hash - return h1 ^ (h2 << 1); // or use boost::hash_combine
        return lhs ^ (rhs << 1); // don't simply XOR, I think, because this would be symetric and produce zero if lhs==rhs
    }

    /*
     * === DEFINE Hash<> specializations ===
     */

    /// @todo SUPPORT RESULT_TYPE
    namespace Private_ {
        template <typename T, enable_if<is_integral_v<T>>* = nullptr>
        struct Hash {
            size_t operator() (const T& t) const
            {
                using DIGESTER = Digester<Algorithm::SuperFastHash>; // pick arbitrarily which algorithm to use for now -- err on the side of quick and dirty
                return DIGESTER::ComputeDigest (reinterpret_cast<const byte*> (&t), reinterpret_cast<const byte*> (&t + 1));
            }
        };
    }
    template <>
    struct Hash<char> : Private_::Hash<char> {
    };
    template <>
    struct Hash<unsigned char> : Private_::Hash<unsigned char> {
    };
    template <>
    struct Hash<short> : Private_::Hash<short> {
    };
    template <>
    struct Hash<unsigned short> : Private_::Hash<unsigned short> {
    };
    template <>
    struct Hash<int> : Private_::Hash<int> {
    };
    template <>
    struct Hash<unsigned int> : Private_::Hash<unsigned int> {
    };
    template <>
    struct Hash<long> : Private_::Hash<long> {
    };
    template <>
    struct Hash<unsigned long> : Private_::Hash<unsigned long> {
    };
    template <>
    struct Hash<long long> : Private_::Hash<long long> {
    };
    template <>
    struct Hash<unsigned long long> : Private_::Hash<unsigned long long> {
    };

    template <>
    struct Hash<Characters::String> {
        size_t operator() (const Characters::String& t) const
        {
            using DIGESTER = Digester<Algorithm::SuperFastHash>; // pick arbitrarily which algorithm to use for now -- err on the side of quick and dirty
            auto p         = t.GetData<wchar_t> ();
            return DIGESTER::ComputeDigest (reinterpret_cast<const std::byte*> (p.first), reinterpret_cast<const std::byte*> (p.second));
        }
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Hash.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_Hash_h_*/
