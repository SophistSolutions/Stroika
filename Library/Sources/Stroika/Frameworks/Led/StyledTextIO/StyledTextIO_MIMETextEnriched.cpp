/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#include "../../../Foundation/StroikaPreComp.h"

#include <cctype>

#include "../../../Foundation/Characters/LineEndings.h"
#include "../../../Foundation/Memory/SmallStackBuffer.h"

#include "StyledTextIO_MIMETextEnriched.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::StyledTextIO;

using Memory::SmallStackBuffer;

/*
 ********************************************************************************
 ************************** StyledTextIOReader_MIMETextEnriched *****************
 ********************************************************************************
 */
// SEE RFC 1563
StyledTextIOReader_MIMETextEnriched::StyledTextIOReader_MIMETextEnriched (SrcStream* srcStream, SinkStream* sinkStream)
    : StyledTextIOReader (srcStream, sinkStream)
    , fBoldMode (0)
    , fItalicMode (0)
    , fUnderlineMode (0)
    , fFixedWidthMode (0)
    , fFontSizeAdjust (0)
    , fNoFillMode (0)
{
}

void StyledTextIOReader_MIMETextEnriched::Read ()
{
    SkipWhitespace ();
    ScanFor ("Content-type:"); // must find since checked in QuickLookAppearsToBeRightFormat
    SkipWhitespace ();
    ScanFor ("text/enriched"); // ""
    SkipOneLine ();
    SkipOneLine (); // SB one blank line after Content-type...

    /*
     *  Basiclly, we just scan for "<" since all command tokens inside these.
     *  Anything up to a "<" is plain text and we echo it - in current style -
     *  except for CR/LF mapping/handling
     */
    for (;;) {
        size_t lastOffset  = GetSrcStream ().current_offset ();
        bool   gotCmdStart = ScanFor ("<"); // side effect of scanning forward

        size_t currentOffset = GetSrcStream ().current_offset ();
        if (currentOffset == lastOffset and not gotCmdStart) {
            break; // we must be past the end of the document
        }

        if (gotCmdStart) {
            currentOffset--; // backup over <
        }

        Led_FontSpecification fontSpec = GetAdjustedCurrentFontSpec ();

        // Handle text before <
        {
            size_t                      len = currentOffset - lastOffset;
            SmallStackBuffer<Led_tChar> buf (len);
            GetSrcStream ().seek_to (lastOffset);
            GetSrcStream ().read (buf, len);
            len = Characters::NormalizeTextToNL<Led_tChar> (buf, len, buf, len);
            if (fNoFillMode == 0) {
                // Strip xtra CRLF, and merge spaces - not really sure what should be done here.
                // Just take a WILD STAB GUESS!!!
                // LGP 951103
                SmallStackBuffer<Led_tChar> buf2 (len);
                size_t                      i2 = 0;
                for (size_t i = 0; i < len; i++) {
                    Led_tChar prevChar = (i == 0) ? '\0' : buf[i - 1];
                    Led_tChar nextChar = (i == len - 1) ? '\0' : buf[i + 1];

                    if (buf[i] == '\n' and nextChar != '\n') {
                        buf[i] = ' '; // skip this NL
                    }

                    if (isspace (prevChar) and isspace (buf[i])) {
                        continue; // skip this char
                    }
                    buf2[i2++] = buf[i];
                }
                GetSinkStream ().AppendText (buf2, i2, &fontSpec);
            }
            else {
                GetSinkStream ().AppendText (buf, len, &fontSpec);
            }
        }

        if (gotCmdStart) {
            // Handle "<" command

            char c;
            GetSrcStream ().seek_to (currentOffset + 1);
            if (GetSrcStream ().read (&c, 1) != 0 and c == '<') {
// special case of << - not a real command - just echo <
#if qWideCharacters
                Led_tChar wc = c; // tmp-hack - assume src is ASCII - LGP990308
                GetSinkStream ().AppendText (&wc, 1, &fontSpec);
#else
                GetSinkStream ().AppendText (&c, 1, &fontSpec);
#endif
            }
            else {
                // HANDLE REAL COMMAND
                GetSrcStream ().seek_to (currentOffset);

                ScanFor (">");
                size_t                 len = GetSrcStream ().current_offset () - currentOffset;
                SmallStackBuffer<char> cmdBuf (len + 1);
                GetSrcStream ().seek_to (currentOffset);
                GetSrcStream ().read (cmdBuf, len);
                cmdBuf[len] = '\0';
                if (strcmp (cmdBuf, "<bold>") == 0) {
                    fBoldMode++;
                }
                else if (strcmp (cmdBuf, "</bold>") == 0 and fBoldMode > 0) {
                    fBoldMode--;
                }
                else if (strcmp (cmdBuf, "<italic>") == 0) {
                    fItalicMode++;
                }
                else if (strcmp (cmdBuf, "</italic>") == 0 and fItalicMode > 0) {
                    fItalicMode--;
                }
                else if (strcmp (cmdBuf, "<underline>") == 0) {
                    fItalicMode++;
                }
                else if (strcmp (cmdBuf, "</underline>") == 0 and fUnderlineMode > 0) {
                    fUnderlineMode--;
                }
                else if (strcmp (cmdBuf, "<fixed>") == 0) {
                    fFixedWidthMode++;
                }
                else if (strcmp (cmdBuf, "</fixed>") == 0 and fFixedWidthMode > 0) {
                    fFixedWidthMode--;
                }
                else if ((strcmp (cmdBuf, "<smaller>") == 0) or (strcmp (cmdBuf, "</bigger>") == 0)) {
                    fFontSizeAdjust -= 2;
                }
                else if ((strcmp (cmdBuf, "</smaller>") == 0) or (strcmp (cmdBuf, "<bigger>") == 0)) {
                    fFontSizeAdjust += 2;
                }
                else if (strcmp (cmdBuf, "<nofill>") == 0) {
                    fNoFillMode++;
                }
                else if (strcmp (cmdBuf, "</nofill>") == 0 and fNoFillMode > 0) {
                    fNoFillMode--;
                }
                else if (strcmp (cmdBuf, "<param>") == 0) {
                    ScanFor ("</param>"); // skip/ignore
                }
            }
        }
    }
    GetSinkStream ().EndOfBuffer ();
}

bool StyledTextIOReader_MIMETextEnriched::QuickLookAppearsToBeRightFormat ()
{
    SrcStreamSeekSaver savePos (GetSrcStream ());
    SkipWhitespace ();
    if (LookingAt ("Content-type:")) {
        SkipWhitespace ();
        return LookingAt ("text/enriched");
    }
    else {
        return false;
    }
    Assert (false);
    return false; // silence compiler warning
}

void StyledTextIOReader_MIMETextEnriched::SkipWhitespace ()
{
    char c;
    while (GetSrcStream ().read (&c, 1) != 0) {
        if (not isascii (c) or not isspace (c)) {
            // one char too far!!!
            GetSrcStream ().seek_to (GetSrcStream ().current_offset () - 1);
            return;
        }
    }
}

void StyledTextIOReader_MIMETextEnriched::SkipOneLine ()
{
    char c;
    while (GetSrcStream ().read (&c, 1) != 0) {
        if (c == '\r') {
            char nextChar;
            GetSrcStream ().read (&nextChar, 1);
            if (nextChar != '\n') {
                // mac format text - let legit, but we'll take it... put char back...
                GetSrcStream ().seek_to (GetSrcStream ().current_offset () - 1);
            }
            return;
        }
        if (c == '\n') {
            return;
        }
    }
}

bool StyledTextIOReader_MIMETextEnriched::ScanFor (const char* matchMe, bool ignoreCase)
{
    RequireNotNull (matchMe);

    char c;
    while (GetSrcStream ().read (&c, 1) != 0) {
        if (Led_CasedCharsEqual (c, matchMe[0], ignoreCase)) {
            size_t savedOffset = GetSrcStream ().current_offset ();
            if (LookingAt (&matchMe[1], ignoreCase)) {
                return true;
            }
            else {
                GetSrcStream ().seek_to (savedOffset);
            }
        }
    }
    return false;
}

bool StyledTextIOReader_MIMETextEnriched::LookingAt (const char* matchMe, bool ignoreCase)
{
    RequireNotNull (matchMe);
    size_t nBytesToMatch = ::strlen (matchMe);

    if (nBytesToMatch == 0) {
        return true;
    }
    else {
        char c;
        if (GetSrcStream ().read (&c, 1) != 0) {
            if (Led_CasedCharsEqual (c, matchMe[0], ignoreCase)) {
                size_t savedOffset = GetSrcStream ().current_offset ();
                if (LookingAt (&matchMe[1], ignoreCase)) {
                    return true;
                }
                else {
                    GetSrcStream ().seek_to (savedOffset);
                }
            }
        }
    }
    return false;
}

Led_FontSpecification StyledTextIOReader_MIMETextEnriched::GetAdjustedCurrentFontSpec () const
{
    Led_FontSpecification fsp = GetSinkStream ().GetDefaultFontSpec ();
    if (fBoldMode > 0) {
        fsp.SetStyle_Bold (true);
    }
    if (fItalicMode > 0) {
        fsp.SetStyle_Italic (true);
    }
    if (fUnderlineMode > 0) {
        fsp.SetStyle_Underline (true);
    }
    if (fFixedWidthMode > 0) {
#if qPlatform_MacOS
        fsp.SetFontNameSpecifier (kFontIDMonaco);
#else
        Assert (false); // just ignore - NYI
#endif
    }

    {
        int resultSize = fsp.GetPointSize () + fFontSizeAdjust;
        if (resultSize < 5) {
            resultSize = 5;
        }
        if (resultSize > 64) {
            resultSize = 64;
        }
        fsp.SetPointSize (static_cast<uint8_t> (resultSize)); // pinned 5..64 so fits in byte
    }
    return fsp;
}
