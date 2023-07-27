/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../../Foundation/StroikaPreComp.h"

#include "../../../Foundation/Characters/CodeCvt.h"
#include "../../../Foundation/Characters/CodePage.h"
#include "../../../Foundation/Characters/LineEndings.h"
#include "../../../Foundation/Memory/StackBuffer.h"
#include "../../../Foundation/Streams/MemoryStream.h"
#include "../../../Foundation/Streams/TextWriter.h"

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
    : StyledTextIOReader{srcStream, sinkStream}
{
}

void StyledTextIOReader_PlainText::Read ()
{
    // Read into a contiguous block of memory since it makes the dealing with CRLF
    // strattling a buffer-bounary problem go away. Note that the Memory::StackBuffer<>::GrowToSize()
    // code grows exponentially so that we minimize buffer copies on grows...
    size_t                    len = 0;
    Memory::StackBuffer<char> buf{Memory::eUninitialized, len};
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
#if qWideCharacters
    span<const byte>   rawByteSpan{reinterpret_cast<const byte*> (buf.data ()), len};
    CodeCvt<Led_tChar>             converter{&rawByteSpan, CodeCvt<Led_tChar>{GetDefaultSDKCodePage ()}};
    size_t             outCharCnt = converter.ComputeTargetCharacterBufferSize (rawByteSpan);
    Memory::StackBuffer<Led_tChar> wbuf{outCharCnt};
    outCharCnt = converter.Bytes2Characters (&rawByteSpan, span{wbuf}).size ();
    size_t charsRead = outCharCnt;
    Assert (charsRead <= len);
    Led_tChar* useBuf = wbuf.data ();
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
    SrcStreamSeekSaver savePos{GetSrcStream ()};

    char   buf[1024];
    size_t bytesRead = GetSrcStream ().read (buf, sizeof (buf));

    return ValidateTextForCharsetConformance (buf, bytesRead);
#endif
}

/*
 ********************************************************************************
 ************************** StyledTextIOWriter_PlainText ************************
 ********************************************************************************
 */
StyledTextIOWriter_PlainText::StyledTextIOWriter_PlainText (SrcStream* srcStream, SinkStream* sinkStream)
    : StyledTextIOWriter{srcStream, sinkStream}
{
}

void StyledTextIOWriter_PlainText::Write ()
{
    Led_tChar buf[8 * 1024];
    size_t    bytesRead = 0;
    while ((bytesRead = GetSrcStream ().readNTChars (buf, Memory::NEltsOf (buf))) != 0) {
#if qPlatform_MacOS || qStroika_FeatureSupported_XWindows
        Led_tChar buf2[Memory::NEltsOf (buf)];
#elif qPlatform_Windows
        Led_tChar buf2[2 * Memory::NEltsOf (buf)];
#endif
        bytesRead = Characters::NLToNative<Led_tChar> (buf, bytesRead, buf2, Memory::NEltsOf (buf2));
#if qWideCharacters
        Streams::MemoryStream<byte>::Ptr memStream = Streams::MemoryStream<byte>::New ();
        Streams::TextWriter::New (memStream, Characters::CodeCvt<>{static_cast<CodePage> (CP_ACP)}).Write (buf2, buf2 + bytesRead);
        auto b = memStream.As<Memory::BLOB> ();
        write (b.data (), b.size());
#else
        write (buf2.data (), bytesRead);
#endif
    }
}
