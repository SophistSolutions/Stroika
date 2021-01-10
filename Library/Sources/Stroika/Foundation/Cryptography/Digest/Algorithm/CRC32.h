/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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

    using std::byte;

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

        ReturnType                                                                                              operator() (const Streams::InputStream<std::byte>::Ptr& from) const;
        ReturnType                                                                                              operator() (const std::byte* from, const std::byte* to) const;
        ReturnType                                                                                              operator() (const BLOB& from) const;
        [[deprecated ("Since Stroika 2.1b6 - use instance of Digester and call operator()")]] static ReturnType ComputeDigest (const Streams::InputStream<std::byte>::Ptr& from)
        {
            return Digester{}(from);
        }
        [[deprecated ("Since Stroika 2.1b6 - use instance of Digester and call operator()")]] static ReturnType ComputeDigest (const std::byte* from, const std::byte* to)
        {
            return Digester{}(from, to);
        }
        [[deprecated ("Since Stroika 2.1b6 - use instance of Digester and call operator()")]] static ReturnType ComputeDigest (const BLOB& from)
        {
            return Digester{}(from);
        }
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CRC32.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_CRC32_h_*/
