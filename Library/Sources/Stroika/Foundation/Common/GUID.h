/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_GUID_h_
#define _Stroika_Foundation_Common_GUID_h_ 1

#include "../StroikaPreComp.h"

#if qPlatform_Windows
#include <guiddef.h>
#endif

#include "../Characters/ToString.h"
#include "../Configuration/Common.h"

/**
 */

namespace Stroika::Foundation::Common {

    /**
     *  
     */
    struct GUID {
        /**
         *  \note - when converting from a string, GUID allows the leading/trailing {} to be optionally provided.
         */
        GUID () = default;
#if qPlatform_Windows
        constexpr GUID (const ::GUID& src);
#endif
        GUID (const string& src);
        GUID (const Characters::String& src);

        uint32_t Data1{};
        uint16_t Data2{};
        uint16_t Data3{};
        uint8_t  Data4[8]{};

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual Characters::String ToString () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "GUID.inl"

#endif /*_Stroika_Foundation_Common_GUID_h_*/