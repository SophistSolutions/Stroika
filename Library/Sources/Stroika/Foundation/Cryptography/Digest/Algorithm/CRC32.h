/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_CRC32_h_
#define _Stroika_Foundation_Cryptography_Digest_CRC32_h_ 1

#include "../../../StroikaPreComp.h"

#include <cstdint>

#include "Algorithm.h"

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
    class DigesterAlgorithm<CRC32> : public IDigestAlgorithm<DigesterDefaultTraitsForAlgorithm<CRC32>::ReturnType> {
    public:
        using ReturnType = DigesterDefaultTraitsForAlgorithm<CRC32>::ReturnType;

    public:
        DigesterAlgorithm ()                             = default;
        DigesterAlgorithm (const DigesterAlgorithm& src) = default;

    public:
        nonvirtual DigesterAlgorithm& operator= (const DigesterAlgorithm& rhs) = default;

    public:
        virtual void Write (const std::byte* start, const std::byte* end) override;

    public:
        virtual ReturnType Complete () override;

    private:
        uint32_t fData_{0xFFFFFFFF};
#if qDebug
        bool fCompleted_{false};
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CRC32.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_CRC32_h_*/
