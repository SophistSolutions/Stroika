/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_ResultTypes_h_
#define _Stroika_Foundation_Cryptography_Digest_ResultTypes_h_  1

#include    "../../StroikaPreComp.h"

#include    <array>
#include    <cstdint>

#include    "../../Configuration/Common.h"




/*
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Digest {


                using   Result128BitType = array<uint8_t, 16>; // 16 bytes, but 128 bits
                using   Result160BitType = array<uint8_t, 20>; // 20 bytes, but 160 bits


            }
        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ResultTypes.inl"

#endif  /*_Stroika_Foundation_Cryptography_Digest_ResultTypes_h_*/
