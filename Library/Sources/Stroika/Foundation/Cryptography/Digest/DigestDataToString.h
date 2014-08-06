/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_DigestDataToString_h_
#define _Stroika_Foundation_Cryptography_Digest_DigestDataToString_h_  1

#include    "../../StroikaPreComp.h"

#include    <cstdint>

#include    "../../Configuration/Common.h"
#include    "../../Memory/BLOB.h"
#include    "../../Streams/BinaryInputStream.h"



/*
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Digest {


                /**
                 *  A simple helper function to make it easier (fewer characters) for the most common
                 *  case, taking a data object and using operator << to insert it into a stream, and then
                 *  taking THAT and digesting it, and taking its result to compute a string of the hash (hex).
                 */
                template    <typename DIGESTER, typename T, typename RESULT_TYPE = string>
                RESULT_TYPE  DigestDataToString (const T& t);


            }
        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "DigestDataToString.inl"

#endif  /*_Stroika_Foundation_Cryptography_Digest_DigestDataToString_h_*/
