/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_inl_
#define _Stroika_Foundation_Cryptography_Hash_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <type_traits>

#include "Format.h"

namespace Stroika::Foundation::Cryptography {

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

    namespace Private_ {
        template <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE>
        inline HASH_RETURN_TYPE Hash_SimpleHash_ (TYPE_TO_COMPUTE_HASH_OF data2Hash, enable_if_t<is_arithmetic_v<TYPE_TO_COMPUTE_HASH_OF>>* = nullptr)
        {
            // Just pass in pointers directly, and don't make a BLOB memory object (speed hack)
            // note - no need to optimize mkReturnType_ is already optimizable way for common case of arithmetic types
            return Private_::mkReturnType_<HASH_RETURN_TYPE> (DIGESTER::ComputeDigest (reinterpret_cast<const byte*> (&data2Hash), reinterpret_cast<const byte*> (&data2Hash) + sizeof (data2Hash)));
        }
        template <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE>
        HASH_RETURN_TYPE Hash_SimpleHash_ (TYPE_TO_COMPUTE_HASH_OF data2Hash, enable_if_t<not is_arithmetic_v<TYPE_TO_COMPUTE_HASH_OF>>* = nullptr)
        {
            Memory::BLOB blob = Private_::SerializeForHash_ (data2Hash);
            return Private_::mkReturnType_<HASH_RETURN_TYPE> (DIGESTER::ComputeDigest (blob.begin (), blob.end ()));
        }
    }

    /*
     ********************************************************************************
     ************************************** Hash ************************************
     ********************************************************************************
     */
    template <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE>
    inline HASH_RETURN_TYPE Hash (TYPE_TO_COMPUTE_HASH_OF data2Hash)
    {
        return Private_::Hash_SimpleHash_<DIGESTER, TYPE_TO_COMPUTE_HASH_OF, HASH_RETURN_TYPE> (data2Hash);
    }
    template <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE>
    HASH_RETURN_TYPE Hash (TYPE_TO_COMPUTE_HASH_OF data2Hash, const Memory::BLOB& salt)
    {
        Memory::BLOB blob = Private_::SerializeForHash_ (data2Hash) + salt;
        return Private_::mkReturnType_<HASH_RETURN_TYPE> (DIGESTER::ComputeDigest (blob.begin (), blob.end ()));
    }
    template <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE>
    HASH_RETURN_TYPE Hash (TYPE_TO_COMPUTE_HASH_OF data2Hash, TYPE_TO_COMPUTE_HASH_OF salt)
    {
        Memory::BLOB blob = Private_::SerializeForHash_ (data2Hash) + Private_::SerializeForHash_ (salt);
        return Private_::mkReturnType_<HASH_RETURN_TYPE> (DIGESTER::ComputeDigest (blob.begin (), blob.end ()));
    }

}
#endif /*_Stroika_Foundation_Cryptography_Hash_inl_*/
