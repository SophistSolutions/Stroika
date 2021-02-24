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

namespace Stroika::Foundation::Cryptography::Digest::Algorithm {

    /**
     *  Algorithm 'type tag' indicating this particular algorithm.
     */
    struct CRC32 {
    };

    /**
     *  Traits for the CRC32 algorithm.
     */
    template <>
    struct DigesterDefaultTraitsForAlgorithm<CRC32> {
        using ReturnType = uint32_t;
    };

    /**
     *  \brief Windowing digester (code to do the digest algorithm) for the CRC32 algorithm.
     */
    template <>
    struct DigesterAlgorithm<CRC32> : public IDigestAlgorithm<DigesterDefaultTraitsForAlgorithm<CRC32>::ReturnType> {
        using ReturnType = DigesterDefaultTraitsForAlgorithm<CRC32>::ReturnType;

    public:
        DigesterAlgorithm () = default;

    public:
        virtual void Write (const std::byte* start, const std::byte* end) override;

    public:
        virtual ReturnType Complete () override;

    private:
        uint32_t fData_{0xFFFFFFFF};
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CRC32.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_CRC32_h_*/
