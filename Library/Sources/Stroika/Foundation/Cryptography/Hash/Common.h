/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_Common_h_
#define _Stroika_Foundation_Cryptography_Hash_Common_h_  1

#include    "../../StroikaPreComp.h"

#include    <string>
#include    <vector>

#include    "../../Configuration/Common.h"
#include    "../../Streams/BinaryInputStream.h"




/*
 * STATUS *****************DRAFT ..... ROUHGH OUTLINE
 *
 * TODO:
 *      o
 *
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Hash {


                using   Memory::Byte;

                /**
                 *  RETURN_TYPE is typically uint32_t, uint64_t, or array<uint8_t,NBYTES>, but could in principle be
                 *  anything.
                 */
                template    <typename RETURN_TYPE, typename ALGORITHM>
                struct Hasher {
                    using   ReturnType      =   RETURN_TYPE;

                    static  ReturnType  Hash (const Streams::BinaryInputStream& from);
                    static  ReturnType  Hash (const Byte* from, const Byte* to);
                };


            }
        }
    }
}


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Common.inl"

#endif  /*_Stroika_Foundation_Cryptography_Hash_Common_h_*/
