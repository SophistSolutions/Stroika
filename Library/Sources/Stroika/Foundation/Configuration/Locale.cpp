/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <set>

#include    "StroikaConfig.h"

#include    "../Characters/SDKString.h"
#include    "../Characters/Format.h"
#include    "../Characters/String_Constant.h"
#include    "../Execution/Exceptions.h"
#include    "../Execution/StringException.h"

#include    "Locale.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Configuration;


using   Characters::String_Constant;



/*
********************************************************************************
*********** Configuration::UsePlatformDefaultLocaleAsDefaultLocale *************
********************************************************************************
*/
void    Configuration::UsePlatformDefaultLocaleAsDefaultLocale ()
{
    locale::global (GetPlatformDefaultLocale ());
}






/*
********************************************************************************
************************* Configuration::GetAvailableLocales *******************
********************************************************************************
*/
#if     0
EnumSystemLocales(EnumLocalesProc, LCID_INSTALLED);
// the enumeration callback function
BOOL CALLBACK EnumLocalesProc(LPTSTR lpLocaleString)
{
    LCID uiCurLocale;
    TCHAR szCurNam[STR_LEN];
    if (!lpLocaleString)
        return FALSE;
// This enumeration returns the LCID as a character string while
// we will be using numbers in other NLS calls.
    uiCurLocale = uiConvertStrToInt(lpLocaleString);
// Get the language name associated with this locale ID.
    GetLocaleInfo(uiCurLocale, LOCALE_SLANGUAGE, szCurName, STR_LEN);
    return TRUE;
}
#endif
vector<Characters::String>    Configuration::GetAvailableLocales ()
{
    // horrible!!!! - see TOOD
    vector<Characters::String>  result;
    result.reserve (10);
    IgnoreExceptionsForCall (result.push_back (FindLocaleName (String_Constant { L"en" }, String_Constant { L"us" })));
    return result;
}





/*
********************************************************************************
*************************** Configuration::FindLocaleName **********************
********************************************************************************
*/
Characters::String    Configuration::FindLocaleName (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode)
{
    using   Characters::String;
    Require (iso2LetterLanguageCode.length () == 2);
    Require (iso2LetterTerritoryCode.length () == 2);       // may lift this in teh future and make it optional

    // This is a HORRIBLE way - but I know of no better (especially no better portable way).
    // See todo for planned fixes
    //  --LGP 2013-03-02
    //
    // Could make less heinous with memoizing, but not currently used much, and I plan todo much better impl...
    static  set<String> part1 {
        iso2LetterLanguageCode,
        iso2LetterLanguageCode.ToLowerCase (),
        iso2LetterLanguageCode.ToUpperCase (),
    };
    static  const   set<String> part2 {
        String_Constant { L"-" },
        String_Constant { L"_" },
        String_Constant { L"." },
        String_Constant { L" " },
    };
    static  set<String> part3 {
        iso2LetterTerritoryCode,
        iso2LetterTerritoryCode.ToLowerCase (),
        iso2LetterTerritoryCode.ToUpperCase (),
    };
    static  const   set<String> part4 {
        String_Constant { L"" },
        String_Constant { L".utf8" },
    };
    for (String i1 : part1) {
        for (String i2 : part2) {
            for (String i3 : part3) {
                for (String i4 : part4) {
                    IgnoreExceptionsForCall (return String::FromNarrowSDKString (locale ((i1 + i2 + i3 + i4).AsUTF8 ().c_str ()).name ()));
                }
            }
        }
    }
    Execution::DoThrow (Execution::StringException (Characters::Format (L"Locale (%s-%s) not found", iso2LetterLanguageCode.c_str (), iso2LetterTerritoryCode.c_str ())));
}





/*
********************************************************************************
*************************** Configuration::FindNamedLocale *********************
********************************************************************************
*/
locale    Configuration::FindNamedLocale (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode)
{
    return locale (FindLocaleName (iso2LetterLanguageCode, iso2LetterTerritoryCode).AsNarrowSDKString ().c_str ());
}
