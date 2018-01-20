/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_SDKString_h_
#define _Stroika_Foundation_Configuration_SDKString_h_ 1

#include "../StroikaPreComp.h"

#include <string>

#include "SDKChar.h"

/**
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Characters {

            /**
             *  This is the kind of String passed to most platform APIs.
             *
             *  @see SDKChar
             */
            using SDKString = basic_string<SDKChar>;

/**
             *  SDKSTR is a macro to wrap constant string literals to get const SDKChar*
             */
#if qTargetPlatformSDKUseswchar_t
#define SDKSTR(x) L##x
#else
#define SDKSTR(x) x
#endif

            /**
             * Even when the platform sdk uses wide strings (but more especailly when it does not) it will typically have
             * a prefered code-page for mapping narrow characters to/from wide ones (part of locale?). WideStringToNarrowSDKString will
             * perform that mapping.
             */
            string WideStringToNarrowSDKString (const wstring& ws);

            /**
             * Even when the platform sdk uses wide strings (but more especailly when it does not) it will typically have
             * a prefered code-page for mapping narrow characters to/from wide ones (part of locale?). NarrowSDKStringToWide will
             * perform that mapping.
             */
            wstring NarrowSDKStringToWide (const string& s);

            string    SDKString2NarrowSDK (const SDKString& s);
            SDKString NarrowSDK2SDKString (const string& s);

            wstring   SDKString2Wide (const SDKString& s);
            SDKString Wide2SDKString (const wstring& s);

            SDKString ToSDKString (const string& s);
            SDKString ToSDKString (const wstring& s);
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SDKString.inl"

#endif /*_Stroika_Foundation_Configuration_SDKString_h_*/
