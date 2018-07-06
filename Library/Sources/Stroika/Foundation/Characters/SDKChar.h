/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_SDKChar_h_
#define _Stroika_Foundation_Characters_SDKChar_h_ 1

#include "../StroikaPreComp.h"

#include <type_traits>

#if qPlatform_Windows
#include <tchar.h>
#endif

#include "../Configuration/Common.h"

/**
 *  Each platform SDK has its own policy for representing characters. Some use narrow characters (char),
 *  and a predefined code page (often configured via locale), and others use wide-characters (wchar_t unicode).
 *
 *  SDKChar is the underlying represenation of the SDK's characters - whether it be narrow or wide.
 */

namespace Stroika::Foundation {
    namespace Characters {

        /**
         *  qTargetPlatformSDKUseswchar_t
         *
         *  Defines if we use wchar_t or char for most platform interfaces (mostly applicable/useful for windows)
         */
#if !defined(qTargetPlatformSDKUseswchar_t)
#error "qTargetPlatformSDKUseswchar_t should normally be defined indirectly by StroikaConfig.h"
#endif

        /**
         *  SDKChar is the kind of character passed to most/default platform SDK APIs.
         */
        using SDKChar = conditional_t<qTargetPlatformSDKUseswchar_t, wchar_t, char>;
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "SDKChar.inl"

#endif /*_Stroika_Foundation_Characters_SDKChar_h_*/
