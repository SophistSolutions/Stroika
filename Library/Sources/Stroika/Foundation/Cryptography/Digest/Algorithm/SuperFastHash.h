/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_SuperFastHash_h_
#define _Stroika_Foundation_Cryptography_Digest_SuperFastHash_h_ 1

#include "../../../StroikaPreComp.h"

#include <cstdint>

#include "../Digester.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  @see    http://www.azillionmonkeys.com/qed/hash.html
 *
 * TODO:
 *      @todo   See if we need to wrap this whole file in #ifndef for some 'license' issue, since I'm not sure
 *              this associated license is OK...
 *
 */

namespace Stroika::Foundation::Cryptography::Digest {

    namespace Algorithm {
        /*
         *  Implementation based on text from http://www.azillionmonkeys.com/qed/hash.html on 2014-07-28
         *
         *  NOTE - I tried to implement in a way that could be windowed, just reading it bits at a time,
         *  but the trouble is that the initial value of the hash is the length, and since the BinaryInputStream
         *  isn't necessarily seekable, we cannot compute its length.
         * 
         *  \note HOWEVER, being Windowed is very important, and being 'result-compatible' with the code in http://www.azillionmonkeys.com/qed/hash.html isn't.
         *        So instead, I start with a value of 0.
         * 
         */
        // Just a name to select template implementation
        struct SuperFastHash {
        };
        template <>
        struct DigesterDefaultTraitsForAlgorithm<SuperFastHash> {
            using ReturnType = uint32_t;
        };



        template <>
        struct DigesterAlgorithm<SuperFastHash> : public IDigestAlgorithm<DigesterDefaultTraitsForAlgorithm<SuperFastHash>::ReturnType> {
            using ReturnType = DigesterDefaultTraitsForAlgorithm<SuperFastHash>::ReturnType;

        public:
            DigesterAlgorithm () = default ;

        public:
            virtual void Write (const std::byte* start, const std::byte* end) override;

        public:
            virtual ReturnType Complete () override;

        private:
            uint32_t fHash_{0}; // original algorithm set this to len, but we cannot and be windowed
            int      rem{};
            array<byte, 3> finalBytes{};
        };

    }

    template <>
    struct Digester<Algorithm::SuperFastHash, uint32_t> {
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
#include "SuperFastHash.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_SuperFastHash_h_*/
