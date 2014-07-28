/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_Adapters_inl_
#define _Stroika_Foundation_Cryptography_Hash_Adapters_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Hash {


                namespace Private_ {
                    // for POD_TYPE ONLY
                    template    <typename TYPE_TO_COMPUTE_HASH_OF>
                    Memory::BLOB    SerializeForHash_ (TYPE_TO_COMPUTE_HASH_OF data2Hash)
                    {
                        return Memory::BLOB (reinterpret_cast<const Byte*> (&data2Hash), reinterpret_cast<const Byte*> (&data2Hash + 1));
                    }
                    template    <>
                    Memory::BLOB    SerializeForHash_ (Characters::String data2Hash)
                    {
                        string  utf8    =   data2Hash.AsUTF8 ();    // unwise approach because costly
                        return Memory::BLOB (reinterpret_cast<const Byte*> (utf8.c_str ()), reinterpret_cast<const Byte*> (utf8.c_str () + utf8.length ()));
#if 0
                        // bad idea cuz value diffs across endian/size wchar_t...
                        const wchar_t*  p   =   data2Hash.c_str ();
                        return Memory::BLOB (reinterpret_cast<const Byte*> (p), reinterpret_cast<const Byte*> (p + data2Hash.length ()));
#endif
                    }
                    template    <>
                    Memory::BLOB    SerializeForHash_ (const char* data2Hash)
                    {
                        return Memory::BLOB (reinterpret_cast<const Byte*> (data2Hash), reinterpret_cast<const Byte*> (data2Hash + ::strlen (data2Hash)));
                    }
                    template    <>
                    Memory::BLOB    SerializeForHash_ (string data2Hash)
                    {
                        return Memory::BLOB (reinterpret_cast<const Byte*> (data2Hash.c_str ()), reinterpret_cast<const Byte*> (data2Hash.c_str () + data2Hash.length ()));
                    }
                }


                /*
                 ********************************************************************************
                 ************************************** Adapter *********************************
                 ********************************************************************************
                 */
                template    <typename HASHER_TYPE, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE>
                HASH_RETURN_TYPE  Adapter (TYPE_TO_COMPUTE_HASH_OF data2Hash)
                {
                    Memory::BLOB    blob = Private_::SerializeForHash_ (data2Hash);
                    return static_cast<HASH_RETURN_TYPE> (HASHER_TYPE::Hash (blob.begin (), blob.end ()));
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Cryptography_Hash_Adapters_inl_*/
