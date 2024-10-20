/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_Format_h_
#define _Stroika_Foundation_Cryptography_Format_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <array>
#include <cstdint>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 *  TODO:
 *      @todo   Consider renaming this module FormatResult
 */

namespace Stroika::Foundation::Cryptography {

    /**
     *  Generally, this Format function is used to take a BLOB and render it as a hex string.
     *
     *  CRYTO_RESULT_TO_FORMAT_TYPE can be
     *      o   any of the uintN types (e.g. uint16_t, etc)
     *      o   any of the defined std::array<> types defined in Digest/ResultTypes.h.
     *      o   Memory::BLOB
     *
     *  AS_RESULT_TYPE can be:
     *      o   Common::GUID
     *      o   Characters::String
     *      o   std::string
     */
    template <typename AS_RESULT_TYPE, typename CRYTO_RESULT_TO_FORMAT_TYPE>
    AS_RESULT_TYPE Format (const CRYTO_RESULT_TO_FORMAT_TYPE& digestResult);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Format.inl"

#endif /*_Stroika_Foundation_Cryptography_Format_h_*/
