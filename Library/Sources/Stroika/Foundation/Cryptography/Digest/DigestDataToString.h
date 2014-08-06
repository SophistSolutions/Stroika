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


                template    <typename DIGESTER, typename T>
                string  DigestDataToString (const T& t);


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
