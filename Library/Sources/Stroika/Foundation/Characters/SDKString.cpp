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
 ******************************* Characters::SDKString2Wide *********************
 ********************************************************************************
 */


#if !qPlatform_Windows
 wstring Characters::SDKString2Wide (const SDKString& s)
{
#if qPlatform_MacOS
    return CodeCvt<wchar_t>{UnicodeExternalEncodings::eUTF8}.Bytes2String<wstring> (Memory::SpanReInterpretCast<const byte> (span{s}));
#else
    return CodeCvt<wchar_t>{locale{}}.Bytes2String<wstring> (Memory::SpanReInterpretCast<const byte>  (span{s}));
#endif
}
#endif

/*
 ********************************************************************************
 ******************************* Characters::WideString2SDK *********************
 ********************************************************************************
 */
#if !qPlatform_Windows
SDKString Characters::WideString2SDK (const wstring& s)
{
#if qPlatform_MacOS
    return CodeCvt<wchar_t>{UnicodeExternalEncodings::eUTF8}.String2Bytes<SDKString> (span{s});
#else
    return CodeCvt<wchar_t>{locale{}}.String2Bytes<SDKString> (span{s});
#endif
}
#endif
