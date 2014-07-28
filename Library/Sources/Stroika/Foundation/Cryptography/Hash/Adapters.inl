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
#include    "Algorithms/Jenkins.h"

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
                }



                template    <typename HASHER_TYPE, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE>
                HASH_RETURN_TYPE  HashAdapter (TYPE_TO_COMPUTE_HASH_OF data2Hash)
                {
                    Memory::BLOB    blob = Private_::SerializeForHash_ (data2Hash);
                    return static_cast<HASH_RETURN_TYPE> (HASHER_TYPE::Hash (blob.begin (), blob.end ()));
                }


#if 0
                template    <typename HASHER_TYPE, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE>
                HASH_RETURN_TYPE  HashAdapter (TYPE_TO_COMPUTE_HASH_OF data2Hash)
                {
                    return static_cast<HASH_RETURN_TYPE> (HASHER_TYPE::Hash (reinterpret_cast<const Byte*> (&data2Hash), reinterpret_cast<const Byte*> (&data2Hash + 1)));
                }



                template    <typename HASHER_TYPE>
                template    <>
                typename HASHER_TYPE::ReturnType  Adapapter<HASHER_TYPE>::Hash (const Characters::String& data2Hash)
                {
                    return 1;
                }
#endif

            }
        }
    }
}
#endif  /*_Stroika_Foundation_Cryptography_Hash_Adapters_inl_*/
