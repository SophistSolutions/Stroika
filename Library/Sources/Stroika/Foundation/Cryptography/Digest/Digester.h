/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Digester_h_
#define _Stroika_Foundation_Cryptography_Digest_Digester_h_  1

#include    "../../StroikaPreComp.h"

#include    <cstdint>

#include    "../../Configuration/Common.h"
#include    "../../Memory/BLOB.h"
#include    "../../Streams/InputStream.h"



/*
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Digest {


                using   Memory::Byte;
                using   Memory::BLOB;


                namespace   Algorithm {


                    template    <typename ALGORITHM>
                    struct  DigesterDefaultTraitsForAlgorithm {
                        // TODO - REPLACE
                    };


                }


                /**
                 *  A Digest is an algorithm that takes a stream of bytes and computes a series of bits
                 *  (can be thought of as a number, or string, or seqeunce of bits) which hopefully as
                 *  nearly as possible (given the length of the digest) uniquely identifies the input.
                 *
                 *  A digest is generally of fixed length - often 4, or 16, or 20 bytes long.
                 *
                 *  RETURN_TYPE is typically uint32_t, uint64_t, or Result128BitType, Result128BitType etc,
                 *  but could in principle be anything.
                 *
                 *  \par Example Usage
                 *      \code
                 *      string digestStr = Format (Digester<Algorithm::MD5>::ComputeDigest (s, e));
                 *      \endcode
                 *
                 *  \par Example Usage
                 *      \code
                 *      SourceDefinition    tmp;    // some struct which defines ostream operator>>
                 *      string  digestStr = Format (Digester<Algorithm::MD5>::ComputeDigest (Streams::iostream::SerializeItemToBLOB (tmp)));
                 *      \endcode
                 *
                 *  @see  DigestDataToString ()
                 *
                 *  \par Example Usage
                 *      \code
                 *      SourceDefinition    tmp;    // some struct which defines ostream operator>>
                 *      string  digestStr = DigestDataToString<Digester<Algorithm::MD5>> (tmp);
                 *      \endcode
                 */
                template    <typename ALGORITHM, typename RETURN_TYPE = typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>
                struct  Digester {
                    using   ReturnType      =   RETURN_TYPE;

                    static  ReturnType  ComputeDigest (const Streams::InputStream<Byte>& from);
                    static  ReturnType  ComputeDigest (const Byte* from, const Byte* to);
                    static  ReturnType  ComputeDigest (const BLOB& from);
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
