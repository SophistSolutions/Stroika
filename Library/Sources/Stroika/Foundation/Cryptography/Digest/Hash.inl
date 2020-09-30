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
#include <type_traits>

#include "../Format.h"

namespace Stroika::Foundation::Cryptography::Digest {

    /*
     ********************************************************************************
     ***************************** SystemHashDigester<T> ****************************
     ********************************************************************************
     */
    template <typename T>
    size_t SystemHashDigester<T>::operator() (const Streams::InputStream<std::byte>::Ptr& from) const
    {
        Memory::BLOB b = from.ReadAll ();
        return this->operator() (b.begin (), b.end ());
    }
    template <typename T>
    inline size_t SystemHashDigester<T>::operator() (const std::byte* from, const std::byte* to) const
    {
        Require (to - from == sizeof (T));
        return hash<T>{}(*reinterpret_cast<const T*> (from));
    }
    template <typename T>
    inline size_t SystemHashDigester<T>::operator() (const BLOB& from) const
    {
        return this->operator() (from.begin (), from.end ());
    }

    namespace Private_ {
        // Adapt the digest return type to the return type declared by the Hasher (often the same)
        template <typename ADAPTER_RETURN_TYPE, typename HASHER_RETURN_TYPE>
        inline ADAPTER_RETURN_TYPE mkReturnType1_ (HASHER_RETURN_TYPE hashVal, enable_if_t<is_arithmetic_v<ADAPTER_RETURN_TYPE>, void>* = nullptr)
        {
            return static_cast<ADAPTER_RETURN_TYPE> (hashVal);
        }
        template <typename ADAPTER_RETURN_TYPE, typename HASHER_RETURN_TYPE>
        inline ADAPTER_RETURN_TYPE mkReturnType1_ (HASHER_RETURN_TYPE hashVal, enable_if_t<is_same_v<ADAPTER_RETURN_TYPE, string>>* = nullptr)
        {
            return Format (hashVal);
        }
        template <typename ADAPTER_RETURN_TYPE, typename HASHER_RETURN_TYPE>
        inline ADAPTER_RETURN_TYPE mkReturnType_ (HASHER_RETURN_TYPE hashVal)
        {
            return mkReturnType1_<ADAPTER_RETURN_TYPE> (hashVal);
        }
    }

    /*
     ********************************************************************************
     ********************** Cryptography::Digest::Hash<T> ***************************
     ********************************************************************************
     */
    template <typename T, typename DIGESTER, typename HASH_RETURN_TYPE, typename SERIALIZER>
    constexpr inline Hash<T, DIGESTER, HASH_RETURN_TYPE, SERIALIZER>::Hash (const T& seed)
        : fSeed{nullopt}
    {
        // NOTE - if not for support of SystemHashDigester<> - which combines hashing with digesting in one -
        // we could do fSeed{DIGESTER{}(CONVERT_T_2_DIGEST_ARG{}(seed))} and skip this re-assignment
        fSeed = (*this) (seed); // OK to call now with no seed set
    }
    template <typename T, typename DIGESTER, typename HASH_RETURN_TYPE, typename SERIALIZER>
    inline HASH_RETURN_TYPE Hash<T, DIGESTER, HASH_RETURN_TYPE, SERIALIZER>::operator() (const T& t) const
    {
        HASH_RETURN_TYPE result;
        if constexpr (is_same_v<DIGESTER, SystemHashDigester<T>> or is_same_v<DIGESTER, hash<T>>) {
            result = hash<T>{}(t);
        }
        else {
            result = Private_::mkReturnType_<HASH_RETURN_TYPE> (DIGESTER{}(SERIALIZER{}(t)));
        }
        if (fSeed) {
            result = HashValueCombine (*fSeed, result);
        }
        return result;
    }

    /*  ========================== DEFINE Hash<> specializations ==========================  */

    namespace Private_ {
        template <typename T, typename DIGESTER, typename HASH_RETURN_TYPE, enable_if<is_standard_layout_v<T>>* = nullptr>
        struct Hash_POD_Impl_ {
            constexpr Hash_POD_Impl_ () = default;
            constexpr Hash_POD_Impl_ (const T& seed)
                : fSeed{nullopt}
            {
                fSeed = (*this) (seed); // OK to call now with no seed set
            }
            size_t operator() (const T& t) const
            {
                auto result = DIGESTER{}(reinterpret_cast<const byte*> (&t), reinterpret_cast<const byte*> (&t + 1));
                if (fSeed) {
                    result = HashValueCombine (*fSeed, result);
                }
                return result;
            }
            optional<HASH_RETURN_TYPE> fSeed;
        };
    }

    template <>
    struct Hash<bool, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<bool, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<bool, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<char, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<char, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<char, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<signed char, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<signed char, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<signed char, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<unsigned char, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<unsigned char, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<unsigned char, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
#if __cpp_char8_t >= 201811L
    template <>
    struct Hash<char8_t, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<char8_t, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<char8_t, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
#endif
    template <>
    struct Hash<char16_t, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<char16_t, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<char16_t, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<char32_t, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<char32_t, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<char32_t, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<wchar_t, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<wchar_t, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<wchar_t, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<short, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<short, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<short, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<unsigned short, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<unsigned short, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<unsigned short, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<int, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<int, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<int, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<unsigned int, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<unsigned int, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<unsigned int, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<long, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<long, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<long, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<unsigned long, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<unsigned long, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<unsigned long, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<long long, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<long long, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<long long, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<unsigned long long, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<unsigned long long, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<unsigned long long, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<float, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<float, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<float, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<double, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<double, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<double, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<long double, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<long double, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<long double, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
    };
    template <>
    struct Hash<nullptr_t, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash_POD_Impl_<nullptr_t, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash_POD_Impl_<nullptr_t, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash_POD_Impl_;
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
    template <>
    inline string HashValueCombine (string lhs, string rhs)
    {
        // @todo sb some better way to combine, but this should work for now...
        return lhs + rhs;
    }

}

#endif /*_Stroika_Foundation_Cryptography_Digest_Hash_inl_*/
