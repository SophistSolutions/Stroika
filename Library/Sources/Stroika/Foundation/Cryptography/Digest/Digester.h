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
                 *
                 *  EXAMPLE USAGE:
                 *      string digestStr = Format (Digester<Result128BitType, Algorithm::MD5>::ComputeDigest (s, e));
                 *
                 *  EXAMPLE USAGE:
                 *      SourceDefinition    tmp;    // some struct which defines ostream operator>>
                 *      string  digestStr = Format (
                 *          Digester<Result128BitType, Algorithm::MD5>::ComputeDigest (
                 *                  Streams::iostream::SerializeItemToBLOB (tmp).As<Streams::BinaryInputStream> ()
                 *              )
                 *          );
                 *      // NB: SOON WE CAN address issue https://github.com/SophistSolutions/Stroika/issues/88 we can lsoe the As<Streams...> stuff
                 *
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
