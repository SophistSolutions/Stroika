/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_TString_h_
#define _Stroika_Foundation_Characters_TString_h_   1


#include    "../StroikaPreComp.h"

#include    <string>

#include    "TChar.h"
#include    "SDKString.h"


/*
 *  MODULE OBSOLETE - TO BE RETIRED SOON
 *      -- LGP 2013-09-28
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            ///// FILE TYPE ETC ALL OBSOLETE - USE SDKString
            typedef SDKString    TString;


            ///// FILE TYPE ETC ALL OBSOLETE - USE SDKString
            inline  string  TString2NarrowSDK (const TString& s)
            {
                return SDKString2NarrowSDK (s);
            }
            ///// FILE TYPE ETC ALL OBSOLETE - USE SDKString
            inline  TString NarrowSDK2TString (const string& s)
            {
                return NarrowSDK2SDKString (s);
            }
            ///// FILE TYPE ETC ALL OBSOLETE - USE SDKString
            inline  wstring TString2Wide (const TString& s)
            {
                return SDKString2Wide (s);
            }
            ///// FILE TYPE ETC ALL OBSOLETE - USE SDKString
            inline  TString Wide2TString (const wstring& s)
            {
                return Wide2SDKString (s);
            }
            ///// FILE TYPE ETC ALL OBSOLETE - USE SDKString
            inline  TString ToTString (const string& s)
            {
                return ToSDKString (s);
            }
            ///// FILE TYPE ETC ALL OBSOLETE - USE SDKString
            inline  TString ToTString (const wstring& s)
            {
                return ToSDKString (s);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Characters_TString_h_*/
