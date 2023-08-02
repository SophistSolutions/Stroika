/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "CodeCvt.h"

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
    return kCvt_.Bytes2String<wstring> (Memory::SpanReInterpretCast<const byte> (s));
#else
    return CodeCvt<wchar_t>{locale{}}.Bytes2String<wstring> (Memory::SpanReInterpretCast<const byte> (s));
#endif
}
wstring Characters::SDK2Wide (span<const SDKChar> s, AllowMissingCharacterErrorsFlag)
{
    // @todo RESPOECT AllowMissingCharacterErrorsFlag
#if qPlatform_MacOS
    static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8};
    return kCvt_.Bytes2String<wstring> (Memory::SpanReInterpretCast<const byte> (s));
#else
    return CodeCvt<wchar_t>{locale{}}.Bytes2String<wstring> (Memory::SpanReInterpretCast<const byte> (s));
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
    // @todo RESPOECT AllowMissingCharacterErrorsFlag
#if qPlatform_MacOS
    static const CodeCvt<wchar_t> kCvt_{UnicodeExternalEncodings::eUTF8};
    return kCvt_.String2Bytes<SDKString> (s);
#else
    return CodeCvt<wchar_t>{locale{}}.String2Bytes<SDKString> (s);
#endif
}
#endif
