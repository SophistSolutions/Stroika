/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_Algorithm_Algorithm_h_
#define _Stroika_Foundation_Cryptography_Digest_Algorithm_Algorithm_h_ 1

#include "../../../StroikaPreComp.h"

#include <cstdint>

#include "../../../Configuration/Common.h"

/*
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
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
        virtual void Write (const std::byte* start, const std::byte* end) = 0;
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
     *  No definition provided. It must be specialized for each algorithm.
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
        DigesterAlgorithm (const DigesterAlgorithm& src) = delete;

    public:
        virtual void Write (const std::byte* start, const std::byte* end) override;

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
