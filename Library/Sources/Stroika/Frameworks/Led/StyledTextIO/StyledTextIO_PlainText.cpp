/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../../Foundation/StroikaPreComp.h"

#include "../../../Foundation/Characters/CodePage.h"
#include "../../../Foundation/Characters/LineEndings.h"
#include "../../../Foundation/Memory/SmallStackBuffer.h"

#include "StyledTextIO_PlainText.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::StyledTextIO;

/*
 ********************************************************************************
 ************************** StyledTextIOReader_PlainText ************************
 ********************************************************************************
 */
StyledTextIOReader_PlainText::StyledTextIOReader_PlainText (SrcStream* srcStream, SinkStream* sinkStream)
    : StyledTextIOReader (srcStream, sinkStream)
{
}

void StyledTextIOReader_PlainText::Read ()
{
#if 1
    // Read into a contiguous block of memory since it makes the dealing with CRLF
    // strattling a buffer-bounary problem go away. Note that the Memory::SmallStackBuffer<>::GrowToSize()
    // code grows exponentially so that we minimize buffer copies on grows...
    size_t                         len = 0;
    Memory::SmallStackBuffer<char> buf (len);
    while (true) {
        size_t kTryToReadThisTime = 16 * 1024;
        buf.GrowToSize (len + kTryToReadThisTime);
        void*  readPtr   = static_cast<char*> (buf) + len;
        size_t bytesRead = GetSrcStream ().read (readPtr, kTryToReadThisTime);
        len += bytesRead;
        if (bytesRead == 0) {
            break;
        }
    }
#else
    // read all at once. Requires contiguous block of memory, but is significantly faster
    // (for reasons I don't completely understand, but don't care so much about either...)
    // LGP 960515
    // Also note: this makes the dealing with CRLF strattling a buffer-bounary problem go away.
    // If we tried to read in chunks, we'd need to be more careful about the Characters::NormalizeTextToNL<Led_tChar> () code.
    size_t oldPos = GetSrcStream ().current_offset ();
    GetSrcStream ().seek_to (UINT_MAX);
    size_t endPos = GetSrcStream ().current_offset ();
    Assert (endPos >= oldPos);
    GetSrcStream ().seek_to (oldPos);
    size_t                         len = endPos - oldPos;
    Memory::SmallStackBuffer<char> buf (len);
    size_t                         bytesRead = 0;
    if ((bytesRead = GetSrcStream ().read (buf, len)) != len) {
        Led_ThrowBadFormatDataException ();
    }
#endif
#if qWideCharacters
    CodePage                            useCodePage = CodePagesGuesser ().Guess (buf, len);
    CodePageConverter                   cpc         = CodePageConverter (useCodePage);
    size_t                              outCharCnt  = cpc.MapToUNICODE_QuickComputeOutBufSize (static_cast<const char*> (buf), len + 1);
    Memory::SmallStackBuffer<Led_tChar> wbuf (outCharCnt);
    cpc.SetHandleBOM (true);
    cpc.MapToUNICODE (static_cast<const char*> (buf), len, static_cast<wchar_t*> (wbuf), &outCharCnt);
    size_t charsRead = outCharCnt;
    Assert (charsRead <= len);
    Led_tChar* useBuf = wbuf;
#else
    size_t             charsRead = len;
    Led_tChar*         useBuf    = buf;
#endif
    charsRead = Characters::NormalizeTextToNL<Led_tChar> (useBuf, charsRead, useBuf, charsRead);
    GetSinkStream ().AppendText (useBuf, charsRead, nullptr);
    GetSinkStream ().EndOfBuffer ();
}

bool StyledTextIOReader_PlainText::QuickLookAppearsToBeRightFormat ()
{
#if qWideCharacters
    return true;
#else
    SrcStreamSeekSaver savePos (GetSrcStream ());

    char   buf[1024];
    size_t bytesRead = GetSrcStream ().read (buf, sizeof (buf));

    return (ValidateTextForCharsetConformance (buf, bytesRead));
#endif
}

/*
 ********************************************************************************
 ************************** StyledTextIOWriter_PlainText ************************
 ********************************************************************************
 */
StyledTextIOWriter_PlainText::StyledTextIOWriter_PlainText (SrcStream* srcStream, SinkStream* sinkStream)
    : StyledTextIOWriter (srcStream, sinkStream)
{
}

void StyledTextIOWriter_PlainText::Write ()
{
    Led_tChar buf[8 * 1024];
    size_t    bytesRead = 0;
    while ((bytesRead = GetSrcStream ().readNTChars (buf, NEltsOf (buf))) != 0) {
#if qPlatform_MacOS || qStroika_FeatureSupported_XWindows
        Led_tChar buf2[NEltsOf (buf)];
#elif qPlatform_Windows
        Led_tChar buf2[2 * NEltsOf (buf)];
#endif
        bytesRead = Characters::NLToNative<Led_tChar> (buf, bytesRead, buf2, NEltsOf (buf2));
#if qWideCharacters
        Memory::SmallStackBuffer<char> ansiBuf (bytesRead * sizeof (Led_tChar));
        size_t                         nChars = bytesRead * sizeof (Led_tChar);
        CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (buf2, bytesRead, ansiBuf, &nChars);
        bytesRead = nChars;
        write (ansiBuf, bytesRead);
#else
        write (buf2, bytesRead);
#endif
    }
}
