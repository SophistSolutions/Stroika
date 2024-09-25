/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <set>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/Throw.h"

#include "Locale.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Common;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

/*
 ********************************************************************************
 ********************* Common::LocaleNotFoundException **************************
 ********************************************************************************
 */
LocaleNotFoundException::LocaleNotFoundException (const optional<String>& iso2LetterLanguageCode, const optional<String>& iso2LetterTerritoryCode)
    : RuntimeErrorException{(iso2LetterLanguageCode.has_value () or iso2LetterTerritoryCode.has_value ())
                                ? Characters::Format ("Locale ({}-{}) not found"_f, iso2LetterLanguageCode.value_or (""sv),
                                                      iso2LetterTerritoryCode.value_or (""sv))
                                : "Locale not found"_k}
{
}

/*
 ********************************************************************************
 *************** Common::UsePlatformDefaultLocaleAsDefaultLocale ****************
 ********************************************************************************
 */
void Common::UsePlatformDefaultLocaleAsDefaultLocale ()
{
    locale::global (GetPlatformDefaultLocale ());
}

/*
 ********************************************************************************
 *********************** Common::GetAvailableLocales *********************
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
vector<Characters::String> Common::GetAvailableLocales ()
{
    // @todo
    // horrible!!!! - see TOOD
    vector<Characters::String> result;
    result.reserve (10);
    IgnoreExceptionsForCall (result.push_back (FindLocaleName ("en"sv, "us"sv)));
    return result;
}

/*
 ********************************************************************************
 ************************* Common::FindLocaleName ************************
 ********************************************************************************
 */
Characters::String Common::FindLocaleName (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode)
{
    if (auto r = FindLocaleNameQuietly (iso2LetterLanguageCode, iso2LetterTerritoryCode)) {
        return *r;
    }
    Execution::Throw (LocaleNotFoundException{iso2LetterLanguageCode, iso2LetterTerritoryCode});
}

optional<Characters::String> Common::FindLocaleNameQuietly (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode)
{
    using namespace Characters;
    Require (iso2LetterLanguageCode.length () == 2);
    Require (iso2LetterTerritoryCode.length () == 2); // may lift this in the future and make it optional

#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{"Common::FindLocaleName", "{},{}"_f, iso2LetterLanguageCode, iso2LetterTerritoryCode};
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
    static const set<String> part2{
        "-"sv,
        "_"sv,
        "."sv,
        " "sv,
    };
    set<String> part3{
        iso2LetterTerritoryCode,
        iso2LetterTerritoryCode.ToLowerCase (),
        iso2LetterTerritoryCode.ToUpperCase (),
    };
    static const set<String> part4{
        String{},
        ".utf8"sv,
    };
    for (const auto& i1 : part1) {
        for (const auto& i2 : part2) {
            for (const auto& i3 : part3) {
                for (const auto& i4 : part4) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("***trying locale (i1 + i2 + i3 + i4)={}"_f, i1 + i2 + i3 + i4);
#endif
                    IgnoreExceptionsForCall (return String::FromNarrowSDKString (locale{(i1 + i2 + i3 + i4).AsNarrowSDKString ().c_str ()}.name ()));
                }
            }
        }
    }
    return nullopt;
}

/*
 ********************************************************************************
 *************************** Common::FindNamedLocale *********************
 ********************************************************************************
 */
locale Common::FindNamedLocale (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode)
{
    return locale{FindLocaleName (iso2LetterLanguageCode, iso2LetterTerritoryCode).AsNarrowSDKString ().c_str ()};
}

/*
 ********************************************************************************
 ******************** Common::FindNamedLocaleQuietly *********************
 ********************************************************************************
 */
optional<locale> Common::FindNamedLocaleQuietly (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode)
{
    if (auto o = FindLocaleNameQuietly (iso2LetterLanguageCode, iso2LetterTerritoryCode)) {
        return locale{o->AsNarrowSDKString ().c_str ()};
    }
    return nullopt;
}
