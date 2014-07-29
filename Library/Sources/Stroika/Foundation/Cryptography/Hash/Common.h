/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Hash_Common_h_
#define _Stroika_Foundation_Cryptography_Hash_Common_h_  1

#include    "../../StroikaPreComp.h"

#include    <array>
#include    <cstdint>

#include    "../../Configuration/Common.h"
#include    "../../Streams/BinaryInputStream.h"




/*
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Hash {


                using   Memory::Byte;


                using   HashResult128BitType = array<uint8_t, 16>; // 16 bytes, but 128 bits
                using   HashResult160BitType = array<uint8_t, 20>; // 20 bytes, but 160 bits


                /**
                 *  RETURN_TYPE is typically uint32_t, uint64_t, or HashResult128BitType, HashResult160BitTypem etc,
                 *  but could in principle be anything.
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
