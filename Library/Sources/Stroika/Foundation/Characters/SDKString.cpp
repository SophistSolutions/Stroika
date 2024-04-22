/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/CodeCvt.h"

#include "SDKString.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

/*
 ********************************************************************************
 ******************************* Characters::SDK2Wide ***************************
 ********************************************************************************
 */
#if !qTargetPlatformSDKUseswchar_t
wstring Characters::SDK2Wide (span<const SDKChar> s)
{
#if qPlatform_MacOS
    static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8};
    return kCvt_.Bytes2String<wstring> (as_bytes (s));
#else
    return CodeCvt<wchar_t>{locale{}}.Bytes2String<wstring> (as_bytes (s));
#endif
}
wstring Characters::SDK2Wide (span<const SDKChar> s, AllowMissingCharacterErrorsFlag)
{
    constexpr auto kOptions_ = CodeCvt<wchar_t>::Options{.fInvalidCharacterReplacement = UTFConvert::Options::kDefaultMissingReplacementCharacter};
#if qPlatform_MacOS
    static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8, kOptions_};
    return kCvt_.Bytes2String<wstring> (as_bytes (s));
#else
    // If - as is not uncommon - kDefaultMissingReplacementCharacter is not representable in the current locale code page,
    // then try something that will be. This API says 'AllowMissingCharacter....' - so allow it!
    auto codeCvt = [&] () {
        try {
            return CodeCvt<wchar_t>{locale{}, kOptions_};
        }
        catch (...) {
            auto o                         = kOptions_;
            o.fInvalidCharacterReplacement = '?';
            return CodeCvt<wchar_t>{locale{}, o};
        }
    }();
    return codeCvt.Bytes2String<wstring> (as_bytes (s));
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
#if qPlatform_MacOS
    static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8};
    return kCvt_.String2Bytes<SDKString> (s);
#else
    return CodeCvt<wchar_t>{locale{}}.String2Bytes<SDKString> (s);
#endif
}
SDKString Characters::Wide2SDK (span<const wchar_t> s, AllowMissingCharacterErrorsFlag)
{
    constexpr auto kOptions_ = CodeCvt<wchar_t>::Options{.fInvalidCharacterReplacement = UTFConvert::Options::kDefaultMissingReplacementCharacter};
#if qPlatform_MacOS
    static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8, kOptions_};
    return kCvt_.String2Bytes<SDKString> (s);
#else
    return CodeCvt<wchar_t>{locale{}, kOptions_}.String2Bytes<SDKString> (s);
#endif
}
#endif
