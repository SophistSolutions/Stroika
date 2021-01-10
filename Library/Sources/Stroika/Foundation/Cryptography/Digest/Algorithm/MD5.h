/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_MD5_h_
#define _Stroika_Foundation_Cryptography_Digest_MD5_h_ 1

#include "../../../StroikaPreComp.h"

#include "../Digester.h"
#include "../ResultTypes.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo
 *
 */

namespace Stroika::Foundation::Cryptography::Digest {

    namespace Algorithm {
        // Just a name to select template implementation
        struct MD5 {
        };
        template <>
        struct DigesterDefaultTraitsForAlgorithm<MD5> {
            using ReturnType = Result128BitType;
        };
    }

    /**
     *  BLOB    blob2Hash   =   ...;
     *  BLOB    b           =   Digester<Algorithm::MD5>::ComputeDigest (blob2Hash);
     */
    template <>
    struct Digester<Algorithm::MD5, Result128BitType> {
        using ReturnType = Result128BitType;

        ReturnType operator() (const Streams::InputStream<std::byte>::Ptr& from) const;
        ReturnType operator() (const std::byte* from, const std::byte* to) const;
        ReturnType operator() (const BLOB& from) const;

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
#include "MD5.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_MD5_h_*/
