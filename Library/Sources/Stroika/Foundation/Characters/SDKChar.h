/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_SDKChar_h_
#define _Stroika_Foundation_Characters_SDKChar_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <type_traits>

#if qPlatform_Windows
#include <tchar.h>
#endif

#include "Stroika/Foundation/Configuration/Common.h"

/**
 *  Each platform SDK has its own policy for representing characters. Some use narrow characters (char),
 *  and a predefined code page (often configured via locale), and others use wide-characters (wchar_t unicode).
 *
 *  SDKChar is the underlying representation of the SDK's characters - whether it be narrow or wide.
 */

namespace Stroika::Foundation::Characters {

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
     * 
     *  Platform-Specific Meaning:
     *      o   Windows
     *          Typically this is wchar_t, which is char16_t-ish. Windows SDK also supports an older "A" API (active Code page single byte)
     *          which Stroika probably still supports, but this has not been tested in a while (not very useful, not used much anymore).
     * 
     *      o   Unix
     *          There is no standard. This could be locale-dependent (often EUC based multibyte character sets).
     *          Or could be UTF-8. These aren't totally incompatible possibilities.
     * 
     *      o   MacOS
     *          Same as 'Unix' above, but most typically UTF-8. So Stroika assumes UTF-8.
     *          See <https://stackoverflow.com/questions/3071377/saner-way-to-get-character-encoding-of-the-cli-in-mac-os-x>
     * 
     *      o   Linux
     *          Same as 'Unix' above - default to assume locale{} based.
     */
    using SDKChar = conditional_t<qTargetPlatformSDKUseswchar_t, wchar_t, char>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "SDKChar.inl"

#endif /*_Stroika_Foundation_Characters_SDKChar_h_*/
