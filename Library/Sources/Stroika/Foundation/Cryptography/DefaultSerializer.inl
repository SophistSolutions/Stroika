/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_DefaultSerializer_inl_
#define _Stroika_Foundation_Cryptography_DefaultSerializer_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <type_traits>

namespace Stroika::Foundation::Cryptography {

    namespace Private_ {
        using std::byte;
        template <typename TYPE_TO_COMPUTE_HASH_OF>
        Memory::BLOB SerializeForHash_ (TYPE_TO_COMPUTE_HASH_OF data2Hash, enable_if_t<is_trivially_copy_constructible_v<TYPE_TO_COMPUTE_HASH_OF>, void>* = nullptr)
        {
            // From https://en.cppreference.com/w/cpp/types/is_trivially_copyable:
            //      Objects of trivially-copyable types that are not potentially-overlapping subobjects are the
            //      only C++ objects that may be safely copied with std::memcpy or serialized to/from binary files
            //      with std::ofstream::write()/std::ifstream::read().
            return Memory::BLOB (reinterpret_cast<const byte*> (&data2Hash), reinterpret_cast<const byte*> (&data2Hash + 1));
        }
        template <typename TYPE_TO_COMPUTE_HASH_OF>
        inline Memory::BLOB SerializeForHash_ (TYPE_TO_COMPUTE_HASH_OF data2Hash, enable_if_t<is_same_v<typename remove_cv<TYPE_TO_COMPUTE_HASH_OF>::type, Memory::BLOB>>* = nullptr)
        {
            return data2Hash;
        }
        inline Memory::BLOB SerializeForHash_ (const char* data2Hash)
        {
            return Memory::BLOB (reinterpret_cast<const byte*> (data2Hash), reinterpret_cast<const byte*> (data2Hash + ::strlen (data2Hash)));
        }
        inline Memory::BLOB SerializeForHash_ (const string& data2Hash)
        {
            return Memory::BLOB (reinterpret_cast<const byte*> (data2Hash.c_str ()), reinterpret_cast<const byte*> (data2Hash.c_str () + data2Hash.length ()));
        }
    }

    /*
     ********************************************************************************
     ************************ Cryptography::DefaultSerializer<T> ********************
     ********************************************************************************
     */
    template <typename T>
    inline Memory::BLOB DefaultSerializer<T>::operator() (const T& t) const
    {
        return Private_::SerializeForHash_ (t);
    }

}

#endif /*_Stroika_Foundation_Cryptography_DefaultSerializer_inl_*/
