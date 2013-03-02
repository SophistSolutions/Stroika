/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <set>

#include    "StroikaConfig.h"

#include    "../Characters/TString.h"
#include    "../Execution/Exceptions.h"
#include    "../Execution/StringException.h"

#include    "Locale.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Configuration;





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
    IgnoreExceptionsForCall (result.push_back (FindLocaleName (L"en", L"us")));
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
    set<String> part1;
    part1.insert (iso2LetterLanguageCode);
    part1.insert (iso2LetterLanguageCode.ToLowerCase ());
    part1.insert (iso2LetterLanguageCode.ToUpperCase ());
    set<String> part2;
    part2.insert (L"-");
    part2.insert (L"_");
    part2.insert (L".");
    part2.insert (L" ");
    set<String> part3;
    part3.insert (iso2LetterTerritoryCode);
    part3.insert (iso2LetterTerritoryCode.ToLowerCase ());
    part3.insert (iso2LetterTerritoryCode.ToUpperCase ());
    set<String> part4;
    part4.insert (L"");
    part4.insert (L".utf8");
    for (String i1 : part1) {
        for (String i2 : part2) {
            for (String i3 : part3) {
                for (String i4 : part4) {
                    IgnoreExceptionsForCall (return Characters::NarrowSDKStringToWide (locale ((i1 + i2 + i3 + i4).AsUTF8 ().c_str ()).name ()));
                }
            }
        }
    }
    Execution::DoThrow<Execution::StringException> (Execution::StringException (L"Locale not found"));
}





/*
********************************************************************************
*************************** Configuration::FindNamedLocale *********************
********************************************************************************
*/
locale    Configuration::FindNamedLocale (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode)
{
    return locale (Characters::TString2NarrowSDK (FindLocaleName (iso2LetterLanguageCode, iso2LetterTerritoryCode).AsTString ()).c_str ());
}
