/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Jenkins_h_
#define _Stroika_Foundation_Cryptography_Digest_Jenkins_h_ 1

#include "../../../StroikaPreComp.h"

#include <cstdint>

#include "../Digester.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  @see    http://en.wikipedia.org/wiki/Jenkins_hash_function
 *
 *
 * TODO:
 *      @todo   Add other hashes - listed on http://en.wikipedia.org/wiki/Jenkins_hash_function - like - SPOOKY???
 *
 *
 */

namespace Stroika::Foundation::Cryptography::Digest {

    namespace Algorithm {
        // Just a name to select template implementation
        struct Jenkins {
        };
        template <>
        struct DigesterDefaultTraitsForAlgorithm<Jenkins> {
            using ReturnType = uint32_t;
        };



        template <>
        struct DigesterAlgorithm<Jenkins> : public IDigestAlgorithm<DigesterDefaultTraitsForAlgorithm<Jenkins>::ReturnType> {
            using ReturnType = DigesterDefaultTraitsForAlgorithm<Jenkins>::ReturnType;

        public:
            DigesterAlgorithm () = default;

        public:
            virtual void Write (const std::byte* start, const std::byte* end) override;

        public:
            virtual ReturnType Complete () override;

        private:
            uint32_t fData_{0};
        };

    }

    template <>
    struct Digester<Algorithm::Jenkins, uint32_t> {
        using ReturnType = uint32_t;

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
#include "Jenkins.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_Jenkins_h_*/
