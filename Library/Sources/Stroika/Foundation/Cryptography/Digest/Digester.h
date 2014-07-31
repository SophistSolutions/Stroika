/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Digester_h_
#define _Stroika_Foundation_Cryptography_Digest_Digester_h_  1

#include    "../../StroikaPreComp.h"

#include    <cstdint>

#include    "../../Configuration/Common.h"
#include    "../../Streams/BinaryInputStream.h"




/*
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Digest {


                using   Memory::Byte;


                /**
                 *  RETURN_TYPE is typically uint32_t, uint64_t, or Result128BitType, Result128BitType etc,
                 *  but could in principle be anything.
                 */
                template    <typename RETURN_TYPE, typename ALGORITHM>
                struct  Digester {
                    using   ReturnType      =   RETURN_TYPE;

                    static  ReturnType  ComputeDigest (const Streams::BinaryInputStream& from);
                    static  ReturnType  ComputeDigest (const Byte* from, const Byte* to);
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
#include    "Digester.inl"

#endif  /*_Stroika_Foundation_Cryptography_Digest_Digester_h_*/
