/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_TString_h_
#define _Stroika_Foundation_Characters_TString_h_   1


#include    "../StroikaPreComp.h"

#include    <string>

#include    "TChar.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {

            typedef std::basic_string<TChar>    TString;

            /*
             * Even when the platform sdk uses wide strings (but more especailly when it does not) it will typically have
             * a prefered code-page for mapping narrow characters to/from wide ones (part of locale?). WideStringToNarrowSDKString will
             * perform that mapping.
             */
            string  WideStringToNarrowSDKString (const wstring& ws);

            /*
             * Even when the platform sdk uses wide strings (but more especailly when it does not) it will typically have
             * a prefered code-page for mapping narrow characters to/from wide ones (part of locale?). NarrowSDKStringToWide will
             * perform that mapping.
             */
            wstring NarrowSDKStringToWide (const string& s);

            string  TString2NarrowSDK (const TString& s);
            TString NarrowSDK2TString (const string& s);


            wstring TString2Wide (const TString& s);
            TString Wide2TString (const wstring& s);

            TString ToTString (const string& s);
            TString ToTString (const wstring& s);

        }
    }
}
#endif  /*_Stroika_Foundation_Characters_TString_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "TString.inl"



