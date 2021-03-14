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

namespace Stroika::Foundation::Cryptography::Digest::Algorithm {

    /**
     *  Algorithm 'type tag' indicating this particular algorithm.
     */
    struct Jenkins {
    };

    /**
     *  Traits for the Jenkins algorithm.
     */
    template <>
    struct DigesterDefaultTraitsForAlgorithm<Jenkins> {
        using ReturnType = uint32_t;
    };

    /**
     *  \brief Windowing digester (code to do the digest algorithm) for the Jenkins algorithm.
     */
    template <>
    class DigesterAlgorithm<Jenkins> : public IDigestAlgorithm<DigesterDefaultTraitsForAlgorithm<Jenkins>::ReturnType> {
        using ReturnType = DigesterDefaultTraitsForAlgorithm<Jenkins>::ReturnType;

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
        uint32_t fData_{0};
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
#include "Jenkins.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_Jenkins_h_*/
