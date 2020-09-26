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

        using std::byte;

        template <typename TYPE_TO_COMPUTE_HASH_OF>
        Memory::BLOB SerializeForHash1_ (TYPE_TO_COMPUTE_HASH_OF data2Hash, enable_if_t<is_arithmetic_v<TYPE_TO_COMPUTE_HASH_OF>, void>* = nullptr)
        {
            return Memory::BLOB (reinterpret_cast<const byte*> (&data2Hash), reinterpret_cast<const byte*> (&data2Hash + 1));
        }
        template <typename TYPE_TO_COMPUTE_HASH_OF>
        inline Memory::BLOB SerializeForHash1_ (TYPE_TO_COMPUTE_HASH_OF data2Hash, enable_if_t<is_same_v<typename remove_cv<TYPE_TO_COMPUTE_HASH_OF>::type, Memory::BLOB>>* = nullptr)
        {
            return data2Hash;
        }
        inline Memory::BLOB SerializeForHash1_ (const Characters::String& data2Hash)
        {
            string utf8 = data2Hash.AsUTF8 (); // unwise approach because costly
            return Memory::BLOB (reinterpret_cast<const byte*> (utf8.c_str ()), reinterpret_cast<const byte*> (utf8.c_str () + utf8.length ()));
        }
        inline Memory::BLOB SerializeForHash1_ (const char* data2Hash)
        {
            return Memory::BLOB (reinterpret_cast<const byte*> (data2Hash), reinterpret_cast<const byte*> (data2Hash + ::strlen (data2Hash)));
        }
        inline Memory::BLOB SerializeForHash1_ (const string& data2Hash)
        {
            return Memory::BLOB (reinterpret_cast<const byte*> (data2Hash.c_str ()), reinterpret_cast<const byte*> (data2Hash.c_str () + data2Hash.length ()));
        }
        template <typename TYPE_TO_COMPUTE_HASH_OF>
        inline Memory::BLOB SerializeForHash_ (TYPE_TO_COMPUTE_HASH_OF data2Hash)
        {
            return SerializeForHash1_ (data2Hash);
        }
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
#if 0
    namespace Private_ {
        template <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE>
        inline HASH_RETURN_TYPE Hash_SimpleHash_ (TYPE_TO_COMPUTE_HASH_OF data2Hash, enable_if_t<is_arithmetic_v<TYPE_TO_COMPUTE_HASH_OF>>* = nullptr)
        {
            // Just pass in pointers directly, and don't make a BLOB memory object (speed hack)
            // note - no need to optimize mkReturnType_ is already optimizable way for common case of arithmetic types
            return Private_::mkReturnType_<HASH_RETURN_TYPE> (DIGESTER{}(reinterpret_cast<const byte*> (&data2Hash), reinterpret_cast<const byte*> (&data2Hash) + sizeof (data2Hash)));
        }
        template <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE>
        HASH_RETURN_TYPE Hash_SimpleHash_ (TYPE_TO_COMPUTE_HASH_OF data2Hash, enable_if_t<not is_arithmetic_v<TYPE_TO_COMPUTE_HASH_OF>>* = nullptr)
        {
            Memory::BLOB blob = Private_::SerializeForHash_ (data2Hash);
            return Private_::mkReturnType_<HASH_RETURN_TYPE> (DIGESTER{}(blob.begin (), blob.end ()));
        }
    }
#endif

#if 0

    /*
     ********************************************************************************
     ************************************** Hash ************************************
     ********************************************************************************
     */
    template <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE>
    inline HASH_RETURN_TYPE Hash_OldDeprecated (TYPE_TO_COMPUTE_HASH_OF data2Hash)
    {
        return Private_::Hash_SimpleHash_<DIGESTER, TYPE_TO_COMPUTE_HASH_OF, HASH_RETURN_TYPE> (data2Hash);
    }
    template <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE>
    HASH_RETURN_TYPE Hash_OldDeprecated (TYPE_TO_COMPUTE_HASH_OF data2Hash, const Memory::BLOB& salt)
    {
        Memory::BLOB blob = Private_::SerializeForHash_ (data2Hash) + salt;
        return Private_::mkReturnType_<HASH_RETURN_TYPE> (DIGESTER{}(blob.begin (), blob.end ()));
    }
    template <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE>
    HASH_RETURN_TYPE Hash_OldDeprecated (TYPE_TO_COMPUTE_HASH_OF data2Hash, TYPE_TO_COMPUTE_HASH_OF salt)
    {
        Memory::BLOB blob = Private_::SerializeForHash_ (data2Hash) + Private_::SerializeForHash_ (salt);
        return Private_::mkReturnType_<HASH_RETURN_TYPE> (DIGESTER{}(blob.begin (), blob.end ()));
    }
#endif

    /*
     ********************************************************************************
     ********************** Cryptography::Digest::Hash<T> ***************************
     ********************************************************************************
     */
    template <typename T, typename DIGESTER, typename HASH_RETURN_TYPE>
    constexpr inline Hash<T, DIGESTER, HASH_RETURN_TYPE>::Hash (const HASH_RETURN_TYPE& seed)
        : fSeed{seed}
    {
    }
    template <typename T, typename DIGESTER, typename HASH_RETURN_TYPE>
    inline HASH_RETURN_TYPE Hash<T, DIGESTER, HASH_RETURN_TYPE>::operator() (const T& t) const
    {
        HASH_RETURN_TYPE result;
        if constexpr (is_same_v<DIGESTER, SystemHashDigester<T>> or is_same_v<DIGESTER, hash<T>>) {
            result = hash<T>{}(t);
        }
        else {
            result = Private_::mkReturnType_<HASH_RETURN_TYPE> (DIGESTER{}(Private_::SerializeForHash_ (t)));
        }
        if (fSeed) {
            result = HashValueCombine (*fSeed, result);
        }
        return result;
    }

    /*  ========================== DEFINE Hash<> specializations ==========================  */

    namespace Private_ {
        template <typename T, typename DIGESTER, typename HASH_RETURN_TYPE, enable_if<is_arithmetic_v<T>>* = nullptr>
        struct Hash {
            constexpr Hash () = default;
            constexpr Hash (const HASH_RETURN_TYPE& seed)
                : fSeed{seed}
            {
            }
            size_t operator() (const T& t) const
            {
                return DIGESTER{}(reinterpret_cast<const byte*> (&t), reinterpret_cast<const byte*> (&t + 1));
            }
            optional<HASH_RETURN_TYPE> fSeed;
        };
    }

    template <>
    struct Hash<bool, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<bool, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<bool, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<char, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<char, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<char, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<signed char, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<signed char, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<signed char, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<unsigned char, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<unsigned char, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<unsigned char, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
#if __cpp_char8_t >= 201811L
    template <>
    struct Hash<char8_t, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<char8_t, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<char8_t, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
#endif
    template <>
    struct Hash<char16_t, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<char16_t, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<char16_t, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<char32_t, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<char32_t, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<char32_t, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<wchar_t, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<wchar_t, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<wchar_t, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<short, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<short, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<short, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<unsigned short, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<unsigned short, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<unsigned short, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<int, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<int, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<int, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<unsigned int, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<unsigned int, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<unsigned int, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<long, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<long, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<long, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<unsigned long, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<unsigned long, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<unsigned long, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<long long, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<long long, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<long long, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<unsigned long long, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<unsigned long long, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<unsigned long long, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<float, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<float, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<float, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<double, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<double, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<double, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<long double, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<long double, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<long double, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
    };
    template <>
    struct Hash<nullptr_t, DefaultHashDigester, DefaultHashDigester::ReturnType> : Private_::Hash<nullptr_t, DefaultHashDigester, DefaultHashDigester::ReturnType> {
        using Private_::Hash<nullptr_t, DefaultHashDigester, DefaultHashDigester::ReturnType>::Hash;
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
