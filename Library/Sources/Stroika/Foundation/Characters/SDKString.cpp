/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <climits>

#include "Stroika/Foundation/Characters/CodeCvt.h"

#include "SDKString.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

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
    return CodeCvt<wchar_t>{locale{}}.Bytes2String<wstring> (as_bytes (s));
#endif
}
wstring Characters::SDK2Wide (span<const SDKChar> s, AllowMissingCharacterErrorsFlag)
{
    constexpr auto kOptions_ = CodeCvt<wchar_t>::Options{.fInvalidCharacterReplacement = UTFConvert::Options::kDefaultMissingReplacementCharacter};
#if qStroika_Foundation_Common_Platform_MacOS
    static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8, kOptions_};
    return kCvt_.Bytes2String<wstring> (as_bytes (s));
#else
    // decoding can always produce the replacement, whether or not the locale can encode it
    return CodeCvt<wchar_t>{locale{}, kOptions_}.Bytes2String<wstring> (as_bytes (s));
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
    return CodeCvt<wchar_t>{locale{}}.String2Bytes<SDKString> (s);
#endif
}
SDKString Characters::Wide2SDK (span<const wchar_t> s, AllowMissingCharacterErrorsFlag)
{
    constexpr auto kOptions_ = CodeCvt<wchar_t>::Options{.fInvalidCharacterReplacement = UTFConvert::Options::kDefaultMissingReplacementCharacter};
#if qStroika_Foundation_Common_Platform_MacOS
    static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8, kOptions_};
    return kCvt_.String2Bytes<SDKString> (s);
#else
    // encoding needs a replacement the locale can represent - U+FFFD if it can (the "C" locale cannot), else '?', which
    // every locale can
    locale l{};
    auto   o = kOptions_;
    if (not LocaleCanEncode_ (l, *o.fInvalidCharacterReplacement)) {
        o.fInvalidCharacterReplacement = '?';
    }
    return CodeCvt<wchar_t>{l, o}.String2Bytes<SDKString> (s);
#endif
}
#endif
