/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Digest_ResultTypes_h_
#define _Stroika_Foundation_Cryptography_Digest_ResultTypes_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <array>
#include <cstdint>

#include "Stroika/Foundation/Configuration/Common.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Cryptography::Digest {

    /**
     *  16 bytes, but 128 bits
     */
    using Result128BitType = array<uint8_t, 16>;

    /**
     *  20 bytes, but 160 bits
     */
    using Result160BitType = array<uint8_t, 20>;

    /**
     *  Convert the various 'hash' / 'digest' output formats amongst each other. NOT ALL
     *  combinations are supported, but many common ones are, and this provides a convenient extention point
     *  to provide additional conversions.
     * 
     *  This allows Digester<> and Hash<> to be used and convert to types which are directly supported by
     *  the base algorithm (either throwing away bits or sometimes (zero?) filling with extra bits.
     */
    template <typename OUT_RESULT, typename IN_RESULT>
    constexpr OUT_RESULT ConvertResult (IN_RESULT inResult);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ResultTypes.inl"

#endif /*_Stroika_Foundation_Cryptography_Digest_ResultTypes_h_*/
