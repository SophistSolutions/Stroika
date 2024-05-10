/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Locale_h_
#define _Stroika_Foundation_Configuration_Locale_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <locale>
#include <vector>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
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

namespace Stroika::Foundation::Configuration {

    /**
     */
    class LocaleNotFoundException : public Execution::RuntimeErrorException<> {
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
     *          ScopedUseLocale useLocaleTmp{FindNamedLocaleQuietly("en", "us")};  // use the english US locale, if its available
     *          do stuff with us locale (if available)
     *      \endcode
     *
     *      \code
     *          ScopedUseLocale useLocaleTmp{FindNamedLocale("en", "us")};  // use the english US locale, or throw
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

#endif /*_Stroika_Foundation_Configuration_Locale_h_*/
