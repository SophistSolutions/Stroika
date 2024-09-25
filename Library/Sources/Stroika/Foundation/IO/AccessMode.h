/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_AccessMode_h_
#define _Stroika_Foundation_IO_AccessMode_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Enumeration.h"

namespace Stroika::Foundation::IO {

    /**
     * First draft of access-mode support. Much better stuff in PHRDB permissions security logic.
     * But this will do for now...
     *      -- LGP 2009-08-15
     *
     *  \note   Common::DefaultNames<> supported
     */
    enum class AccessMode : uint8_t {
        eNoAccess,
        eRead  = 0x1,
        eWrite = 0x2,

        // composite values
        eReadWrite = eRead | eWrite,

        Stroika_Define_Enum_Bounds (eNoAccess, eReadWrite)
    };

    constexpr AccessMode operator& (AccessMode l, AccessMode r);
    constexpr AccessMode operator| (AccessMode l, AccessMode r);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "AccessMode.inl"

#endif /*_Stroika_Foundation_IO_AccessMode_h_*/
