/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_SuperFastHash_h_
#define _Stroika_Foundation_Cryptography_Digest_SuperFastHash_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdint>

#include "Stroika/Foundation/Cryptography/Digest/Digester.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  @see    http://www.azillionmonkeys.com/qed/hash.html
 *
 * TODO:
 *      @todo   See if we need to wrap this whole file in #ifndef for some 'license' issue, since I'm not sure
 *              this associated license is OK...
 *
 */

namespace Stroika::Foundation::Cryptography::Digest::Algorithm {

    /**
     *  Algorithm 'type tag' indicating this particular algorithm.
     */
    struct SuperFastHash {};

    /**
     *  Traits for the SuperFastHash algorithm.
     */
    template <>
    struct DigesterDefaultTraitsForAlgorithm<SuperFastHash> {
        using ReturnType = uint32_t;
    };

    /*
     *  \brief Windowing digester (code to do the digest algorithm) for the SuperFastHash algorithm.
     *
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
    template <>
    class DigesterAlgorithm<SuperFastHash> : public IDigestAlgorithm<DigesterDefaultTraitsForAlgorithm<SuperFastHash>::ReturnType> {
    public:
        using ReturnType = DigesterDefaultTraitsForAlgorithm<SuperFastHash>::ReturnType;

    public:
        DigesterAlgorithm ()                             = default;
        DigesterAlgorithm (const DigesterAlgorithm& src) = default;

    public:
        nonvirtual DigesterAlgorithm& operator= (const DigesterAlgorithm& rhs) = default;

    public:
        virtual void Write (const byte* start, const byte* end) override;

    public:
        virtual ReturnType Complete () override;

    private:
        uint32_t       fHash_{0}; // original algorithm set this to len, but we cannot and be windowed
        unsigned int   fRemainder_{};
        array<byte, 3> fFinalBytes_{};
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
#include "SuperFastHash.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_SuperFastHash_h_*/
