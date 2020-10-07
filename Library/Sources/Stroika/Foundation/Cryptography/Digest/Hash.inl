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
#include "ResultTypes.h"

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
            result = ConvertResult<HASH_RETURN_TYPE> (DIGESTER{}(SERIALIZER{}(t)));
        }
        if (fSeed) {
            result = HashValueCombine (*fSeed, result);
        }
        return result;
    }

    /*
     ********************************************************************************
     ****************** Cryptography::Digest::HashValueCombine **********************
     ********************************************************************************
     */
    namespace Private_ {
        // @todo VERY WEAK impl - of template matching - but good enuf for now - really want to use concept to see if << defined and returns same type
        template <typename RESULT_TYPE, enable_if_t<is_arithmetic_v<RESULT_TYPE> or is_same_v<RESULT_TYPE, std::byte>>* = nullptr>
        inline RESULT_TYPE HashValueCombine (RESULT_TYPE lhs, RESULT_TYPE rhs)
        {
            // inspired by https://en.cppreference.com/w/cpp/utility/hash - return h1 ^ (h2 << 1); // or use boost::hash_combine
            return lhs ^ (rhs << 1); // don't simply XOR, I think, because this would be symetric and produce zero if lhs==rhs
        }
        // @todo VERY WEAK impl - of template matching - but good enuf for now - really want to use concept to see if is container
        template <typename RESULT_TYPE, enable_if_t<not(is_arithmetic_v<RESULT_TYPE> or is_same_v<RESULT_TYPE, std::byte>)>* = nullptr>
        RESULT_TYPE HashValueCombine (RESULT_TYPE lhs, RESULT_TYPE rhs)
        {
            RESULT_TYPE result = lhs;
            auto        i      = result.begin ();
            auto        ri     = rhs.begin ();
            while (i != result.end () and ri != rhs.end ()) {
                *i = HashValueCombine (*i, *ri);
                ++i;
                ++ri;
            }
            // e.g. if lhs empty, result sb rhs, so we always look at all chars of both sides
            // @todo if constexpr check if has append!!!
            if constexpr (is_same_v<RESULT_TYPE, string>) {
                while (ri != rhs.end ()) {
                    result += *ri;
                    ++ri;
                }
            }
            return result;
        }
    }
    template <typename RESULT_TYPE>
    inline RESULT_TYPE HashValueCombine (RESULT_TYPE lhs, RESULT_TYPE rhs)
    {
        return Private_::HashValueCombine (lhs, rhs);
    }

}

#endif /*_Stroika_Foundation_Cryptography_Digest_Hash_inl_*/
