/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../Foundation/StroikaPreComp.h"

#include "../../../Foundation/Characters/CodePage.h"
#include "../../../Foundation/Characters/LineEndings.h"

#include "StyledTextIO_STYLText.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::StyledTextIO;

#if qPlatform_MacOS
/*
 ********************************************************************************
 ************************** StyledTextIOReader_STYLText *************************
 ********************************************************************************
 */
StyledTextIOReader_STYLText::StyledTextIOReader_STYLText (SrcStream* srcStream, SinkStream* sinkStream)
    : StyledTextIOReader (srcStream, sinkStream)
{
}

void StyledTextIOReader_STYLText::Read ()
{
    size_t totalRead = 0;
    {
#if 1
        // Read into a contiguous block of memory since it makes the dealing with CRLF
        // strattling a buffer-bounary problem go away. Note that the SmallStackBuffer<>::GrowToSize()
        // code grows exponentially so that we minimize buffer copies on grows...
        SmallStackBuffer<char> buf (totalRead);
        while (true) {
            size_t kTryToReadThisTime = 16 * 1024;
            buf.GrowToSize (totalRead + kTryToReadThisTime);
            void*  readPtr   = static_cast<char*> (buf) + totalRead;
            size_t bytesRead = GetSrcStream ().read (readPtr, kTryToReadThisTime);
            totalRead += bytesRead;
            if (bytesRead == 0) {
                break;
            }
        }
#else
        // read all at once. Requires contiguous block of memory, but is significantly faster
        // (for reasons I don't completely understand, but don't care so much about either...)
        // LGP 960515
        size_t oldPos = GetSrcStream ().current_offset ();
        GetSrcStream ().seek_to (UINT_MAX);
        size_t endPos = GetSrcStream ().current_offset ();
        Assert (endPos >= oldPos);
        GetSrcStream ().seek_to (oldPos);
        size_t                 len = endPos - oldPos;
        SmallStackBuffer<char> buf (len);
        if ((totalRead = GetSrcStream ().read (buf, len)) != len) {
            Led_ThrowBadFormatDataException ();
        }
#endif
#if qWideCharacters
        SmallStackBuffer<Led_tChar> tCharBuf (totalRead);
        CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (buf, totalRead, tCharBuf, &totalRead);
        totalRead = Characters::NormalizeTextToNL<Led_tChar> (tCharBuf, totalRead, tCharBuf, totalRead);
        GetSinkStream ().AppendText (tCharBuf, totalRead, NULL);
#else
        totalRead = Characters::NormalizeTextToNL<Led_tChar> (buf, totalRead, buf, totalRead);
        GetSinkStream ().AppendText (buf, totalRead, NULL);
#endif
    }
    Assert (GetSrcStream ().GetAUXResourceHandle ()); // If QuickLookAppearsToBeRightFormat () we shouldn't get here?
    if (GetSrcStream ().GetAUXResourceHandle () != NULL) {
        StScrpHandle stylHandle = (StScrpHandle)GetSrcStream ().GetAUXResourceHandle ();
        HLock (Handle (stylHandle));
        vector<StandardStyledTextImager::InfoSummaryRecord> ledStyleInfo = StandardStyledTextImager::Convert ((*stylHandle)->scrpStyleTab, (*stylHandle)->scrpNStyles);
        HUnlock (Handle (stylHandle));
        GetSinkStream ().ApplyStyle (0, totalRead, ledStyleInfo);
    }
    GetSinkStream ().EndOfBuffer ();
}

bool StyledTextIOReader_STYLText::QuickLookAppearsToBeRightFormat ()
{
    // Probably should check even more about the handle... - but this will do for now...
    if (GetSrcStream ().GetAUXResourceHandle () == NULL) {
        return false;
    }

    SrcStreamSeekSaver savePos (GetSrcStream ());

    Led_tChar buf[1024];
    size_t    bytesRead = GetSrcStream ().read (buf, NEltsOf (buf));

    return (ValidateTextForCharsetConformance (buf, bytesRead));
}
#endif
