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

namespace Stroika::Foundation::Cryptography::Digest::Algorithm {

    /**
     *  Algorithm 'type tag' indicating this particular algorithm.
     */
    struct MD5 {
    };

    /**
     *  Traits for the MD5 algorithm.
     */
    template <>
    struct DigesterDefaultTraitsForAlgorithm<MD5> {
        using ReturnType = Result128BitType;
    };

    /**
     *  \brief Windowing digester (code to do the digest algorithm) for the MD5 algorithm.
     */
    template <>
    struct DigesterAlgorithm<MD5> : public IDigestAlgorithm<DigesterDefaultTraitsForAlgorithm<MD5>::ReturnType> {
        using ReturnType = DigesterDefaultTraitsForAlgorithm<MD5>::ReturnType;

    public:
        DigesterAlgorithm ();

    public:
        virtual void Write (const std::byte* start, const std::byte* end) override;

    public:
        virtual ReturnType Complete () override;

    private:
        //   public: //tmphack til we get rid of other class
        using UINT4 = uint32_t;
        struct MD5_CTX {
            UINT4         i[2];       /* number of _bits_ handled mod 2^64 */
            UINT4         buf[4];     /* scratch buffer */
            unsigned char in[64];     /* input buffer */
            unsigned char digest[16]; /* actual digest after MD5Final call */
        };
        MD5_CTX fCtx_{};

    private:
        // public: //tmphack til we get rid of other class
        static void MD5Init_ (MD5_CTX* mdContext);

        static void MD5Update_ (MD5_CTX* mdContext, const unsigned char* inBuf, unsigned int inLen);
        static void MD5Final_ (MD5_CTX* mdContext);
        static void Transform (UINT4* buf, UINT4* in);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MD5.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_MD5_h_*/
