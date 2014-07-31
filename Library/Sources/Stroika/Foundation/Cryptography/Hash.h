/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_h_
#define _Stroika_Foundation_Cryptography_Hash_h_  1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Memory/BLOB.h"

#include    "Digest/Digester.h"



/*
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {


            /**
             *  This function applies any (argument) Hash function (DIGESTER given data type, and
             *  returns the argument hash value.
             *
             *  Hash algorithms work on BLOBs, and generate return (often longish) integers, often encoded
             *  as strings and such.
             *
             *  This Adpater takes care of the general part of mapping the inputs and outputs to/from
             *  common forms, and then makes generic the actual hash computing algorithm.
             *
             *  EXAMPLE USAGE:
             *      using   USE_DIGESTER_     =   Digester<uint32_t, Algorithms::Jenkins>;
             *      VerifyTestResult (Hash<USE_DIGESTER_> (1) == 10338022);
             *      VerifyTestResult (Hash<USE_DIGESTER_> ("1") == 2154528969);
             *      VerifyTestResult (Hash<USE_DIGESTER_> (String (L"1")) == 2154528969);
             *
             *  If you use the overload with 'salt', if the SALT is not a BLOB, it will be turned into a BLOB with the same
             *  normalizing function used on data2Hash. The resulting BLOB salt will be combined with the serialized data2Hash
             *  producing a different output hash value. This can be used for cryptographic salt (as with passwords) or
             *  with rehashing, for example.
             *
             *  For now, this works with TYPE_TO_COMPUTE_HASH_OF:
             *      o   is_arithmetic (e.g. int, float, uint32_t, etc)
             *      o   const char*
             *      o   std::string
             *      o   String (or anything promotable to string)
             *      o   Memory::BLOB (just passed throgh, not adpated)
             *
             *  Other types should generate compile-time error.
             *
             *  Supported values for HASH_RETURN_TYPE, depend on the DIGESTER::ReturnType. This can be any type
             *  cast convertible into HASH_RETURN_TYPE (typically an unsigned int), or std::string, or Characters::String.
             */
            template    <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE = typename DIGESTER::ReturnType>
            HASH_RETURN_TYPE  Hash (TYPE_TO_COMPUTE_HASH_OF data2Hash);
            template    <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE = typename DIGESTER::ReturnType>
            HASH_RETURN_TYPE  Hash (TYPE_TO_COMPUTE_HASH_OF data2Hash, const Memory::BLOB& salt);
            template    <typename DIGESTER, typename TYPE_TO_COMPUTE_HASH_OF, typename HASH_RETURN_TYPE = typename DIGESTER::ReturnType>
            HASH_RETURN_TYPE  Hash (TYPE_TO_COMPUTE_HASH_OF data2Hash, TYPE_TO_COMPUTE_HASH_OF salt);


        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Hash.inl"

#endif  /*_Stroika_Foundation_Cryptography_Hash_h_*/
