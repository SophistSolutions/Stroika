/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Locale_h_
#define _Stroika_Foundation_Common_Locale_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <locale>
#include <variant>
#include <vector>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   I think GetPlatformDefaultLocale() can be optimized to use a static copy of locale("") since
 *              I believe C++ now guarantees multithreaded safe static construction and safe read only
 *              access to objects (even from multiple reader threads).
 *
 *      @todo   verify and document if for windows this is LOCALE_USER_DEFAULT or LOCALE_SYSTEM_DEFAULT.
 *              SB USER!
 *
 *      @todo   reconsider  if GetPlatformDefaultLocale SB inlined, since calls inline function that seems relatively
 *              big and I think maybe OK to cache value (static).
 *
 *      @todo   Enhance FindLocaleName() to make second param - optional (territory)
 *
 *      @todo   Consider the idea of a thread-local current locale (locale()). This would be handy
 *              for server applications like HealthFrameWorks Server which might get a locale
 *              from the client connection, for use in generating reports etc.
 *
 *      @todo   rewrite GetAvailableLocales and FindLocaleName() to look at source code for
 *              locale -a, and see what it does on POSIX systems, and to use EnumSystemLocales()
 *              and mapping to locale names, on windows platform (or better strategy if I can find it).
 *
 */

namespace Stroika::Foundation::Common {

    /**
     *  Predefined locale designations - for wherever a locale is chosen: formatting or parsing numbers, say.
     */
    enum class PredefinedLocale {
        /**
         *  Use the hardwired 'C' locale (not exactly a real locale in the sense of localization. But a good choice
         *  for the 'locale' to use for non-localized code (e.g. much serverside code).
         */
        eUseCLocale,

        /**
         *  \note - this selects the current locale at the time the preference is used, whereas
         *          in Stroika v2.1, it used the current locale at the time the preference object was created.
         */
        eUseCurrentLocale
    };
    using PredefinedLocale::eUseCLocale;
    using PredefinedLocale::eUseCurrentLocale;

    /**
     *  A locale, as options give one: a predefined one, or a given std::locale.
     */
    using LocaleSpecifier = variant<PredefinedLocale, locale>;

    /**
     *  The locale l specifies - eUseCurrentLocale meaning the global locale at the time of the call.
     */
    locale ResolveLocale (const LocaleSpecifier& l);

    /**
     */
    class LocaleNotFoundException : public Execution::Exception<runtime_error> {
    public:
        LocaleNotFoundException (const optional<Characters::String>& iso2LetterLanguageCode  = {},
                                 const optional<Characters::String>& iso2LetterTerritoryCode = {});

    public:
        static const LocaleNotFoundException kThe;
    };
    inline const LocaleNotFoundException LocaleNotFoundException::kThe;

    /**
     *  In C++, the default locale is "C" (aka locale::classic ()), not the one
     *  inherited from the OS.
     *
     *  Its not hard to get/set the one from the OS, but I've found it not well documented,
     *  so this is intended to make it a little easier/more readable.
     */
    locale GetPlatformDefaultLocale ();

    /**
     *  \brief  Set the operating system locale into the current C++ locale used by locale
     *          functions (and most locale-dependent stroika functions).
     *
     *  In C++, the default locale is "C", not the one inherited from the OS.
     *  Its not hard to get/set the one from the OS, but I've found it not well documented,
     *  so this is intended to make it a little easier/more readable.
     *
     *  \note   This changes every locale category - not just the character encoding - in the C library too: locale::global ()
     *          of a named locale does setlocale (LC_ALL, ...) (C++ [locale.statics]). So LC_NUMERIC follows the user's locale
     *          as well: under a German or French one, C's printf ("%f") writes 3,14 and strtod no longer reads 3.14, and
     *          iostreams created afterwards format numbers that way too. That can quietly corrupt machine-readable output
     *          (JSON, CSV, configuration files) from code - third-party code included - that does not name its locale; code
     *          writing such formats should use locale::classic () explicitly.
     *
     *  \note   Where LANG and LC_* are unset (cron, systemd services, many docker images), the platform default locale is
     *          typically just "C" again.
     *
     *  \note   It is not needed just to read text from the system - arguments (Execution::CommandLine), file names, environment
     *          variables: those are SDK strings, and in the "C" locale an SDK string is UTF-8 (see Characters::SDKChar). Call it
     *          for the user's conventions - formatting, collation - or for a locale whose encoding is not UTF-8.
     */
    void UsePlatformDefaultLocaleAsDefaultLocale ();

    /**
     *  \brief  List all installed locale names (names which can be passed to std::locale::CTOR)
     *
     *  I'm quite surprised this appears so hard to to in stdC++. I must be missing something...
     *
     *  @todo NYI really - hacked
     */
    vector<Characters::String> GetAvailableLocales ();

    /**
     *  \brief  Not all systems appear to follow the same naming conventions for locales, so help lookup
     *
     *  Not all systems appear to follow the same naming conventions for locales, so provide a handy
     *  lookup function.
     *
     *  This will throw an exception if no matching locale is found
     */
    Characters::String FindLocaleName (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode);

    /**
     *  \brief  Not all systems appear to follow the same naming conventions for locales, so help lookup
     *
     *  Not all systems appear to follow the same naming conventions for locales, so provide a handy
     *  lookup function.
     *
     *  This will return nullopt if no matching locale is found
     */
    optional<Characters::String> FindLocaleNameQuietly (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode);

    /**
     *  \brief  Find the locale matching these properties (for exception trying)
     *
     *  This will return a valid locale object with the prescribed properties, or it will raise
     *  an exception.
     */
    locale FindNamedLocale (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode);

    /**
     */
    optional<locale> FindNamedLocaleQuietly (const Characters::String& iso2LetterLanguageCode, const Characters::String& iso2LetterTerritoryCode);

    /**
     *  Temporarily use the given argument locale.
     *
     *      \code
     *          ScopedUseLocale useLocaleTmp{FindNamedLocaleQuietly("en", "us")};  // use the English US locale, if its available
     *          do stuff with us locale (if available)
     *      \endcode
     *
     *      \code
     *          ScopedUseLocale useLocaleTmp{FindNamedLocale("en", "us")};  // use the English US locale, or throw
     *          do stuff with us locale
     *      \endcode
     */
    class ScopedUseLocale {
    private:
        optional<locale> fPrev_;

    public:
        /**
         *  if locale l provided, its used for the lifetime of the ScopedUseLocale locale (and on destructor the old locale is restored)
         */
        ScopedUseLocale ()                       = delete;
        ScopedUseLocale (const ScopedUseLocale&) = delete;
        ScopedUseLocale (const optional<locale>& l);
        ~ScopedUseLocale ();
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Locale.inl"

#endif /*_Stroika_Foundation_Common_Locale_h_*/
