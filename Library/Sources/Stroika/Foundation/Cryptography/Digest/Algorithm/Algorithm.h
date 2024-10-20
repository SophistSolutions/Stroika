/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Algorithm_Algorithm_h_
#define _Stroika_Foundation_Cryptography_Digest_Algorithm_Algorithm_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdint>

#include "Stroika/Foundation/Configuration/Common.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 * 
 *  TODO:
 *      @todo   Consider losing IDigester, and IDigestAlgorithm - not REALLY used. We just want to have something like
 *              concepts here - to validate that the algorithms comply to this pattern.
 */

namespace Stroika::Foundation::Cryptography::Digest::Algorithm {

    /**
     *  No definition provided. It must be specialized for each algorithm.
     */
    template <typename ALGORITHM>
    struct DigesterDefaultTraitsForAlgorithm;

    /**
     *  @todo we may not really need base class, nor virtuals, just just concepts
     */
    class IDigester {
    public:
        virtual ~IDigester () = default;

    public:
        virtual void Write (const byte* start, const byte* end) = 0;
    };

    /**
     *  @todo we may not really need base class, nor virtuals, just just concepts
     */
    template <typename RESULT_TYPE>
    class IDigestAlgorithm : public IDigester {
    public:
        virtual RESULT_TYPE Complete () = 0;
    };

    /**
     *  \brief DigesterAlgorithm is specialized for each algorithm; generally don't use this directly, but use the wrapper Digest::IncrementalDigester<> (or Digest::Digester or Digest::ComputeDigest)
     *
     *      No definition provided. It must be specialized for each algorithm.
     */
    template <typename ALGORITHM, typename RETURN_TYPE = typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>
    class DigesterAlgorithm;

// @todo add concepts or some such to enforce DigesterAlgorithm follows this pattern.
#if 0
    template <typename ALGORITHM, typename RETURN_TYPE = typename Algorithm::DigesterDefaultTraitsForAlgorithm<ALGORITHM>::ReturnType>
    class DigesterAlgorithm : public IDigestAlgorithm<RETURN_TYPE> {
        using ReturnType = RETURN_TYPE;

        // static_assert (false, "must specialize this ");

    public:
        DigesterAlgorithm ()                             = default;
        DigesterAlgorithm (const DigesterAlgorithm& src) = default;

    public:
        virtual void Write (const byte* start, const byte* end) override;

    public:
        virtual ReturnType Complete () override;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include "Algorithm.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_Algorithm_Algorithm_h_*/
