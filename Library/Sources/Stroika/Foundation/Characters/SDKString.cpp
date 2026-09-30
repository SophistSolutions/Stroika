/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <climits>

#include "Stroika/Foundation/Characters/CodeCvt.h"

#include "SDKString.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

#if !qTargetPlatformSDKUseswchar_t
namespace {
    constexpr CodeCvt<wchar_t>::Options kAllowMissing_{.fInvalidCharacterReplacement = UTFConvert::Options::kDefaultMissingReplacementCharacter};
}
#endif

#if not qTargetPlatformSDKUseswchar_t && not qStroika_Foundation_Common_Platform_MacOS
namespace {
    // CodeCvt cannot say whether a replacement is encodable - encoding throws when one is needed and it is not - so ask the locale
    bool LocaleCanEncode_ (const locale& l, Character c)
    {
        static_assert (sizeof (wchar_t) == sizeof (char32_t)); // so one Character is one wchar_t
        using CVT        = codecvt<wchar_t, char, mbstate_t>;
        wchar_t        w = static_cast<wchar_t> (c.As<char32_t> ());
        mbstate_t      state{};
        const wchar_t* fromNext{};
        char           to[MB_LEN_MAX];
        char*          toNext{};
        return use_facet<CVT> (l).out (state, &w, &w + 1, fromNext, begin (to), end (to), toNext) == CVT::ok;
    }

    // The converters for one locale
    struct LocaleCodeCvts_ {
        locale           fLocale;
        CodeCvt<wchar_t> fStrict;             // throws on a missing character
        CodeCvt<wchar_t> fAllowMissingToWide; // U+FFFD - decoding can always produce it
        CodeCvt<wchar_t> fAllowMissingToSDK; // U+FFFD if the locale can encode it (the "C" locale cannot), else '?', which every locale can
    };
    LocaleCodeCvts_ MakeLocaleCodeCvts_ (const locale& l)
    {
        auto toSDK = kAllowMissing_;
        if (not LocaleCanEncode_ (l, *toSDK.fInvalidCharacterReplacement)) {
            toSDK.fInvalidCharacterReplacement = '?';
        }
        return LocaleCodeCvts_{l, CodeCvt<wchar_t>{l}, CodeCvt<wchar_t>{l, kAllowMissing_}, CodeCvt<wchar_t>{l, toSDK}};
    }

    // Set when this thread's cache (below) is destroyed. Trivially destructible, so still readable afterwards - while the
    // thread's other thread_locals are destroyed, and on the main thread while static destructors run.
    thread_local bool tLocaleCodeCvtsCacheGone_{false};

    // Constructing a CodeCvt from a locale is costly (it builds a codecvt_byname), so each thread keeps the ones for the
    // global locale until that changes. So a locale::global () takes effect at the next conversion, just as with no cache:
    // CodeCvt depends only on the locale's name, and that is what locale's operator== compares. Once the cache is gone,
    // each call builds its own.
    CodeCvt<wchar_t> GetCodeCvt_ (CodeCvt<wchar_t> LocaleCodeCvts_::* which)
    {
        locale l{};
        if (tLocaleCodeCvtsCacheGone_) [[unlikely]] {
            return MakeLocaleCodeCvts_ (l).*which;
        }
        struct Cache_ {
            optional<LocaleCodeCvts_> fEntry;
            ~Cache_ ()
            {
                tLocaleCodeCvtsCacheGone_ = true;
            }
        };
        static thread_local Cache_ tCache_;
        if (not tCache_.fEntry or tCache_.fEntry->fLocale != l) [[unlikely]] {
            tCache_.fEntry.emplace (MakeLocaleCodeCvts_ (l));
        }
        // a copy (it shares the rep), so it stays valid even if a nested call - a Throw () traced mid-conversion - finds
        // the global locale changed, and rebuilds the cache
        return (*tCache_.fEntry).*which;
    }
}
#endif

/*
 ********************************************************************************
 ******************************* Characters::SDK2Wide ***************************
 ********************************************************************************
 */
#if !qTargetPlatformSDKUseswchar_t
wstring Characters::SDK2Wide (span<const SDKChar> s)
{
#if qStroika_Foundation_Common_Platform_MacOS
    static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8};
    return kCvt_.Bytes2String<wstring> (as_bytes (s));
#else
    return GetCodeCvt_ (&LocaleCodeCvts_::fStrict).Bytes2String<wstring> (as_bytes (s));
#endif
}
wstring Characters::SDK2Wide (span<const SDKChar> s, AllowMissingCharacterErrorsFlag)
{
#if qStroika_Foundation_Common_Platform_MacOS
    static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8, kAllowMissing_};
    return kCvt_.Bytes2String<wstring> (as_bytes (s));
#else
    return GetCodeCvt_ (&LocaleCodeCvts_::fAllowMissingToWide).Bytes2String<wstring> (as_bytes (s));
#endif
}
#endif

/*
 ********************************************************************************
 ******************************* Characters::Wide2SDK ***************************
 ********************************************************************************
 */
#if !qTargetPlatformSDKUseswchar_t
SDKString Characters::Wide2SDK (span<const wchar_t> s)
{
#if qStroika_Foundation_Common_Platform_MacOS
    static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8};
    return kCvt_.String2Bytes<SDKString> (s);
#else
    return GetCodeCvt_ (&LocaleCodeCvts_::fStrict).String2Bytes<SDKString> (s);
#endif
}
SDKString Characters::Wide2SDK (span<const wchar_t> s, AllowMissingCharacterErrorsFlag)
{
#if qStroika_Foundation_Common_Platform_MacOS
    static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8, kAllowMissing_};
    return kCvt_.String2Bytes<SDKString> (s);
#else
    return GetCodeCvt_ (&LocaleCodeCvts_::fAllowMissingToSDK).String2Bytes<SDKString> (s);
#endif
}
#endif
