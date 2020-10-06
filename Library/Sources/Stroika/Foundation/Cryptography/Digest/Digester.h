/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Digester_h_
#define _Stroika_Foundation_Cryptography_Digest_Digester_h_ 1

#include "../../StroikaPreComp.h"

#include <cstdint>

#include "../../Configuration/Common.h"
#include "../../Memory/BLOB.h"
#include "../../Streams/InputStream.h"

/*
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *      @todo -- RETHINK IF RESULTS SB SAME REGARDLESS OF ENDIAN - NOT CONSISTENT!!!! --LGP 2015-08-26
 *
 */

namespace Stroika::Foundation::Cryptography::Digest {

    using Memory::BLOB;

    namespace Algorithm {

        template <typename ALGORITHM>
        struct DigesterDefaultTraitsForAlgorithm {
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
     *  \note Endianness - these algorithms logically operate on bytes, so if you use RETURN_TYPE=uin32_t (or anything but byte array) -
     *          expect the actual numerical value will depend on endianness.
     *
     *  \par Example Usage
     *      \code
     *          string digestStr = Format (Digester<Algorithm::MD5>::ComputeDigest (s, e));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          SourceDefinition    tmp;    // some struct which defines ostream operator>>
     *          string  digestStr = Format (Digester<Algorithm::MD5>{} (Streams::iostream::SerializeItemToBLOB (tmp)));
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          return Format<String> (Digester<Algorithm::MD5>{} (Memory::BLOB::Raw (sb.AsUTF8 ())));
     *      \endcode
     *
     *  @see  DigestDataToString ()
     *
     *  \par Example Usage
     *      \code
     *          SourceDefinition    tmp;    // some struct which defines ostream operator>>
     *          string  digestStr = DigestDataToString<Digester<Algorithm::MD5>> (tmp);
     *      \endcode
     *
     *  @see  Hash ()
     *
     */
    template <typename ALGORITHM, typename RETURN_TYPE = typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>
    struct Digester {
        using ReturnType = RETURN_TYPE;

        [[deprecated ("Since Stroika 2.1b6 - use instance of Digester and call operator()")]] static ReturnType ComputeDigest (const Streams::InputStream<std::byte>::Ptr& from);
        [[deprecated ("Since Stroika 2.1b6 - use instance of Digester and call operator()")]] static ReturnType ComputeDigest (const std::byte* from, const std::byte* to);
        [[deprecated ("Since Stroika 2.1b6 - use instance of Digester and call operator()")]] static ReturnType ComputeDigest (const BLOB& from);
        /**
         */
        ReturnType operator() (const Streams::InputStream<std::byte>::Ptr& from) const;
        ReturnType operator() (const std::byte* from, const std::byte* to) const;
        ReturnType operator() (const BLOB& from) const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Digester.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_Digester_h_*/
