/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_CRC32_h_
#define _Stroika_Foundation_Cryptography_Digest_CRC32_h_ 1

#include "../../../StroikaPreComp.h"

#include <cstdint>

#include "../Digester.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  @see    http://en.wikipedia.org/wiki/Cyclic_redundancy_check
 *  @see    http://www.riccibitti.com/crcguide.htm
 *
 *
 */

namespace Stroika::Foundation::Cryptography::Digest {

    namespace Algorithm {
        struct CRC32 {
        };
        template <>
        struct DigesterDefaultTraitsForAlgorithm<CRC32> {
            using ReturnType = uint32_t;
        };
    }

    template <>
    struct Digester<Algorithm::CRC32, uint32_t> {
        using ReturnType = uint32_t;

        static ReturnType ComputeDigest (const Streams::InputStream<Byte>::Ptr& from);
        static ReturnType ComputeDigest (const Byte* from, const Byte* to);
        static ReturnType ComputeDigest (const BLOB& from);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CRC32.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_CRC32_h_*/
