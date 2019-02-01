/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <set>

#include "StroikaConfig.h"

#include "../Characters/Format.h"
#include "../Characters/SDKString.h"
#include "../Characters/String_Constant.h"
#include "../Execution/Exceptions.h"
#include "../Execution/StringException.h"

#include "Locale.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Configuration;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 *********** Configuration::UsePlatformDefaultLocaleAsDefaultLocale *************
 ********************************************************************************
 */
void Configuration::UsePlatformDefaultLocaleAsDefaultLocale ()
{
    locale::global (GetPlatformDefaultLocale ());
}

#if !qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy
/*
 ********************************************************************************
 ************************* Configuration::GetAvailableLocales *******************
 ********************************************************************************
 */
#if 0
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
vector<Characters::String> Configuration::GetAvailableLocales ()
{
    // @todo
    // horrible!!!! - see TOOD
    vector<Characters::String> result;
    result.reserve (10);
    IgnoreExceptionsForCall (result.push_back (FindLocaleName (L"en"sv, L"us"sv)));
    return result;
}
#endif

/*
 ********************************************************************************
 *************************** Configuration::FindLocaleName **********************
 ********************************************************************************
 */
#if !qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy
Characters::String Configuration::FindLocaleName (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode)
{
    using namespace Characters;
    Require (iso2LetterLanguageCode.length () == 2);
    Require (iso2LetterTerritoryCode.length () == 2); // may lift this in the future and make it optional

#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx ("Configuration::FindLocaleName");
    DbgTrace (L"(%s,%s)", iso2LetterLanguageCode.c_str (), iso2LetterTerritoryCode.c_str ());
#endif

    // This is a HORRIBLE way - but I know of no better (especially no better portable way).
    // See todo for planned fixes
    //  --LGP 2013-03-02
    //
    // Could make less heinous with memoizing, but not currently used much, and I plan todo much better impl...
    set<String> part1{
        iso2LetterLanguageCode,
        iso2LetterLanguageCode.ToLowerCase (),
        iso2LetterLanguageCode.ToUpperCase (),
    };
    static const set<String> part2
    {
#if qCompilerAndStdLib_process_init_constructor_array_Buggy
        {L"-"},
            {L"_"},
            {L"."},
            {L" "},
#else
        L"-"sv,
            L"_"sv,
            L"."sv,
            L" "sv,
#endif
    };
    set<String> part3{
        iso2LetterTerritoryCode,
        iso2LetterTerritoryCode.ToLowerCase (),
        iso2LetterTerritoryCode.ToUpperCase (),
    };
    static const set<String> part4
    {
#if qCompilerAndStdLib_process_init_constructor_array_Buggy
        {L""},
            {L".utf8"},
#else
        String{},
            L".utf8"sv,
#endif
    };
    for (auto&& i1 : part1) {
        for (auto&& i2 : part2) {
            for (auto&& i3 : part3) {
                for (auto&& i4 : part4) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"***trying locale (i1 + i2 + i3 + i4).AsUTF8 ().c_str ()=%s", (i1 + i2 + i3 + i4).c_str ());
#endif
                    IgnoreExceptionsForCall (return String::FromNarrowSDKString (locale ((i1 + i2 + i3 + i4).AsUTF8 ().c_str ()).name ()));
                }
            }
        }
    }
    Execution::Throw (Execution::Exception (Characters::Format (L"Locale (%s-%s) not found", iso2LetterLanguageCode.c_str (), iso2LetterTerritoryCode.c_str ())));
}

/*
 ********************************************************************************
 *************************** Configuration::FindNamedLocale *********************
 ********************************************************************************
 */
locale Configuration::FindNamedLocale (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode)
{
    return locale (FindLocaleName (iso2LetterLanguageCode, iso2LetterTerritoryCode).AsNarrowSDKString ().c_str ());
}
#endif