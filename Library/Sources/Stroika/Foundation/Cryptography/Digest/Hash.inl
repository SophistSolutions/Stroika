/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Hash_inl_
#define _Stroika_Foundation_Cryptography_Digest_Hash_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "Algorithm/SuperFastHash.h"

namespace Stroika::Foundation::Cryptography::Digest {

    /*
     ********************************************************************************
     ********************** Cryptography::Digest::Hash<T> ***************************
     ********************************************************************************
     */
    template <typename T>
    inline size_t Hash<T>::operator() (const T& t) const
    {
        return hash<T>{}(t);
    }

    /*  === DEFINE Hash<> specializations ===     */

    namespace Private_ {
        template <typename T, enable_if<is_integral_v<T>>* = nullptr>
        struct Hash {
            size_t operator() (const T& t) const
            {
                using DIGESTER = Digester<Algorithm::SuperFastHash>; // pick arbitrarily which algorithm to use for now -- err on the side of quick and dirty
                return DIGESTER{}(reinterpret_cast<const byte*> (&t), reinterpret_cast<const byte*> (&t + 1));
            }
        };
    }
    template <>
    struct Hash<bool> : Private_::Hash<bool> {
    };
    template <>
    struct Hash<char> : Private_::Hash<char> {
    };
    template <>
    struct Hash<signed char> : Private_::Hash<signed char> {
    };
    template <>
    struct Hash<unsigned char> : Private_::Hash<unsigned char> {
    };
#if __cpp_char8_t >= 201811L
    template <>
    struct Hash<char8_t> : Private_::Hash<char8_t> {
    };
#endif
    template <>
    struct Hash<char16_t> : Private_::Hash<char16_t> {
    };
    template <>
    struct Hash<char32_t> : Private_::Hash<char32_t> {
    };
    template <>
    struct Hash<wchar_t> : Private_::Hash<wchar_t> {
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
    struct Hash<float> : Private_::Hash<float> {
    };
    template <>
    struct Hash<double> : Private_::Hash<double> {
    };
    template <>
    struct Hash<long double> : Private_::Hash<long double> {
    };
    template <>
    struct Hash<nullptr_t> : Private_::Hash<nullptr_t> {
    };

    /*
     ********************************************************************************
     ****************** Cryptography::Digest::HashValueCombine **********************
     ********************************************************************************
     */
    template <typename RESULT_TYPE>
    inline RESULT_TYPE HashValueCombine (RESULT_TYPE lhs, RESULT_TYPE rhs)
    {
        // inspired by https://en.cppreference.com/w/cpp/utility/hash - return h1 ^ (h2 << 1); // or use boost::hash_combine
        return lhs ^ (rhs << 1); // don't simply XOR, I think, because this would be symetric and produce zero if lhs==rhs
    }

}

#endif /*_Stroika_Foundation_Cryptography_Digest_Hash_inl_*/
