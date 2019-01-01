/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include <cctype>

#include "Config.h"

#include "TextStore.h"

using namespace Stroika::Foundation;

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

inline bool IsASCIIAlpha (int c)
{
    return isascii (c) and isalpha (c);
}

/*
 ********************************************************************************
 ********************** TextStore::VectorMarkerSink *****************************
 ********************************************************************************
 */
/*
@METHOD:        TextStore::VectorMarkerSink::Append
@DESCRIPTION:   <p>Don't call directly. Called as a by-product of TextStore::CollectAllMarkersInRange ().
    This TextStore::VectorMarkerSink produces a vector for such collect calls.</p>
*/
void TextStore::VectorMarkerSink::Append (Marker* m)
{
    RequireNotNull (m);
    AssertNotNull (fMarkers);
    //fMarkers->push_back (m);
    PUSH_BACK (*fMarkers, m);
}

/*
 ********************************************************************************
 ******************* TextStore::SmallStackBufferMarkerSink **********************
 ********************************************************************************
 */
/*
@METHOD:        TextStore::SmallStackBufferMarkerSink::Append
@DESCRIPTION:   <p>Don't call directly. Called as a by-product of TextStore::CollectAllMarkersInRange ().
    This TextStore::VectorMarkerSink produces a @'Memory::SmallStackBuffer<T>' for such collect calls.</p>
*/
void TextStore::SmallStackBufferMarkerSink::Append (Marker* m)
{
    RequireNotNull (m);
    AssertNotNull (fMarkers);
    fMarkers.push_back (m);
}

/*
 ********************************************************************************
 ********************************** TextStore ***********************************
 ********************************************************************************
 */
/*
@METHOD:        TextStore::kAnyMarkerOwner
@DESCRIPTION:
    <p>Sentinal value meaning to match on ANY marker owner for @'TextStore::CollectAllMarkersInRangeInto' etc calls.</p>
*/
const MarkerOwner* TextStore::kAnyMarkerOwner = reinterpret_cast<MarkerOwner*> (1);

TextStore::~TextStore ()
{
    Require (fMarkerOwners.size () == 1 and fMarkerOwners[0] == this); // better have deleted 'em all by now (except for US)!
}

#if qMultiByteCharacters
// qSingleByteCharacters || qWideCharacters cases in headers
size_t TextStore::CharacterToTCharIndex (size_t i)
{
    size_t tCharIndex = 1;
    int    ii         = i;
    for (; ii > 0; ii--) {
        tCharIndex = FindNextCharacter (tCharIndex);
    }
    return (tCharIndex);
}
#endif

#if qMultiByteCharacters
// qSingleByteCharacters || qWideCharacters cases in headers
size_t TextStore::TCharToCharacterIndex (size_t i)
{
    size_t charCount = 1;
    for (size_t tCharIndex = 0; tCharIndex < i; tCharIndex = FindNextCharacter (tCharIndex)) {
        charCount++;
    }
    return (charCount);
}
#endif

/*
@METHOD:        TextStore::Replace
@DESCRIPTION:   <p>Replace the text in the range {from,to} with the withWhatCount Led_tChars of text pointed
            to by withWhat. This update will have the side effect of notifying MarkerOwners, and affected Markers.
            To avoid the marker/markerowner notification, you can call @'TextStore::ReplaceWithoutUpdate'.</p>
                <p>Note - this used to be pure virtual, and impelemented by subclasses. Now its a trivial wrapper
            on @'TextStore::ReplaceWithoutUpdate'. This change was made in Led 3.1.</p>
*/
void TextStore::Replace (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCount)
{
    Require (from <= to);
    Require (to <= GetEnd ());
#if qMultiByteCharacters
    Assert (Led_IsValidMultiByteString (withWhat, withWhatCount));
    Assert_CharPosDoesNotSplitCharacter (from);
    Assert_CharPosDoesNotSplitCharacter (to);
#endif

    if (from != to or withWhatCount != 0) {
        UpdateInfo    updateInfo (from, to, withWhat, withWhatCount, true, true);
        SimpleUpdater updater (*this, updateInfo);
        try {
            ReplaceWithoutUpdate (from, to, withWhat, withWhatCount);
        }
        catch (...) {
            updater.Cancel ();
            throw;
        }
    }
}

/*
@METHOD:        TextStore::GetStartOfLine
@DESCRIPTION:
    <p>Returns the marker position of the start of the given line #.</p>
*/
size_t TextStore::GetStartOfLine (size_t lineNumber) const
{
    Require (lineNumber >= 0);
    Require (lineNumber <= GetLineCount ());

    /*
     *  Just walk through each char looking for '\n's and count lines. Could be
     *  far more efficiently done elsewhere/in subclasses, but this tends to not be called directly anyhow.
     */
    size_t curLineNum = 0;
    size_t len        = GetLength ();
    for (size_t i = 0; i < len; i = FindNextCharacter (i)) {
        if (curLineNum == lineNumber) {
            return i;
        }
        Led_tChar c;
        CopyOut (i, 1, &c);
        if (c == '\n') {
            curLineNum++;
        }
    }
    Assert (curLineNum == lineNumber);
    return len; // this can happen when the last line is at end of buffer
}

/*
@METHOD:        TextStore::GetEndOfLine
@DESCRIPTION:
    <p>Returns the marker position of the end of the given line #.</p>
*/
size_t TextStore::GetEndOfLine (size_t lineNumber) const
{
    // LGP 961129 - not 100% sure this is right - quickie implementation...
    Require (lineNumber >= 0);
    Require (lineNumber <= GetLineCount ());

    /*
     *  Just walk through each char looking for '\n's and count lines. Could be
     *  far more efficiently done elsewhere/in subclasses, but this tends to not be called directly anyhow.
     */
    size_t curLineNum = 0;
    size_t len        = GetLength ();
    for (size_t i = 0; i < len; i = FindNextCharacter (i)) {
        Led_tChar c;
        CopyOut (i, 1, &c);
        if (c == '\n') {
            if (curLineNum == lineNumber) {
                return i;
            }
            curLineNum++;
        }
    }
    return len;
}

/*
@METHOD:        TextStore::GetLineContainingPosition
@DESCRIPTION:
    <p>Returns the # of the line which contains the given charPosition. NB: we use charPosition here
    to deal with the ambiguity of what line a markerPosition is at when it straddles two lines.</p>
*/
size_t TextStore::GetLineContainingPosition (size_t charPosition) const
{
    // LGP 961129 - not 100% sure this is right - quickie implementation...
    Assert (charPosition >= 0);
    Assert (charPosition <= GetEnd ());
    size_t curLineNum = 0;
    size_t lineStart  = 0;
    size_t lineEnd    = 0;
    size_t len        = GetLength ();
    for (size_t i = 0; i < len; i = FindNextCharacter (i)) {
        lineEnd = i + 1;
        if (Contains (lineStart, lineEnd, charPosition)) {
            return (curLineNum);
        }
        Led_tChar c;
        CopyOut (i, 1, &c);
        if (c == '\n') {
            curLineNum++;
            lineStart = i;
            lineEnd   = i;
        }
    }
    return curLineNum;
}

/*
@METHOD:        TextStore::GetLineCount
@DESCRIPTION:
    <p>Returns the # of the lines in the TextStore.</p>
*/
size_t TextStore::GetLineCount () const
{
    // LGP 961129 - not 100% sure this is right - quickie implementation...
    /*
     *  Just walk through each char looking for '\n's and count lines. Could be
     *  far more efficiently done, but this tends to not be called directly anyhow.
     */
    size_t curLineNum = 0;
    size_t len        = GetLength ();
    for (size_t i = 0; i < len; i = FindNextCharacter (i)) {
        Led_tChar c;
        CopyOut (i, 1, &c);
        if (c == '\n') {
            curLineNum++;
        }
    }
    return curLineNum;
}

/*
@METHOD:        TextStore::GetStartOfLineContainingPosition
@DESCRIPTION:
    <p>Returns the marker position of the start of the line contains the character after 'afterPos'.</p>
*/
size_t TextStore::GetStartOfLineContainingPosition (size_t afterPos) const
{
    Assert (afterPos >= 0);
    Assert (afterPos <= GetEnd ());

// Scan back looking for '\n', and the character AFTER that is the start of this line...

// Note, that this is OK todo a character at a time going back because '\n' is NOT
// a valid second byte (at least in SJIS - probaly should check out other MBYTE
// charsets - assert good enuf for now)!!!
#if qMultiByteCharacters
    Assert (not Led_IsValidSecondByte ('\n'));
#endif

    /*
     *  The main thing making this slightly non-trival is that we try to perform the charsearch
     *  using chunked reads to make things a bit faster.
     */
    size_t       lastReadAtPos = afterPos; // set as past where we need the text to be to force a read
    Led_tChar    charBuf[64];
    const size_t kBufSize = sizeof (charBuf) / sizeof (charBuf[0]);
    Assert (afterPos < INT_MAX); // cuz of casts - cannot go up to UINT_MAX
    for (ptrdiff_t curPos = afterPos - 1; curPos > -1; curPos--) {
        Assert (curPos >= 0);
        if (lastReadAtPos > static_cast<size_t> (curPos)) {
            if (lastReadAtPos > kBufSize) {
                lastReadAtPos -= kBufSize;
            }
            else {
                lastReadAtPos = 0;
            }
            Assert (lastReadAtPos >= 0);
            CopyOut (lastReadAtPos, min (kBufSize, GetLength ()), charBuf);
        }
        Assert (curPos - lastReadAtPos < kBufSize);
        Led_tChar& thisChar = charBuf[curPos - lastReadAtPos];

// sensible todo but too slow - this code is only called in DEBUG mode and its performance critical there...
#if qDebug && 0
        {
            Led_tChar xxx;
            CopyOut (curPos, 1, &xxx);
            Assert (xxx == thisChar);
        }
#endif

        if (thisChar == '\n') {
            return (curPos + 1);
        }
    }
    return (0);
}

/*
@METHOD:        TextStore::GetEndOfLineContainingPosition
@DESCRIPTION:
    <p>Returns the marker position of the end of the line contains the character after 'afterPos'.</p>
*/
size_t TextStore::GetEndOfLineContainingPosition (size_t afterPos) const
{
    Assert (afterPos >= 0);
    Assert (afterPos <= GetEnd ());

// Scan forward looking for '\n' (and don't count NL - just like GetEndOfLine())

// Note, that this is OK todo a character at a time going back because '\n' is NOT
// a valid second byte (at least in SJIS - probaly should check out other MBYTE
// charsets - assert good enuf for now)!!!
#if qMultiByteCharacters
    Assert (not Led_IsValidSecondByte ('\n'));
#endif

    /*
     *  The main thing making this slightly non-trival is that we try to perform the charsearch
     *  using chunked reads to make things a bit faster.
     */
    size_t       lastReadAtPos = 0; // flag no read so far
    Led_tChar    charBuf[64];
    const size_t kBufSize = sizeof (charBuf) / sizeof (charBuf[0]);
    for (size_t curPos = afterPos; curPos + 1 <= GetLength (); curPos++) {
        if (lastReadAtPos == 0 or lastReadAtPos + kBufSize <= curPos) {
            lastReadAtPos = curPos;
            Assert (lastReadAtPos >= 0);
            CopyOut (lastReadAtPos, min (kBufSize, GetLength () - (lastReadAtPos)), charBuf);
        }
        Assert (curPos - lastReadAtPos < kBufSize);
        Led_tChar& thisChar = charBuf[curPos - lastReadAtPos];
#if qDebug
        {
            Led_tChar xxx;
            CopyOut (curPos, 1, &xxx);
            Assert (xxx == thisChar);
        }
#endif
        if (thisChar == '\n') {
            return (curPos);
        }
    }
    return (GetEnd ());
}

/*
@METHOD:        TextStore::FindPreviousCharacter
@DESCRIPTION:   <p>Returns the marker position of the previous character. If at the start of the buffer, it returns
    0. Use this instead of pos--, in order to deal properly with multibyte character sets.</p>
*/
size_t TextStore::FindPreviousCharacter (size_t beforePos) const
{
    Assert (beforePos >= 0);
    if (beforePos == 0) {
        return (0);
    }
    else {
#if qMultiByteCharacters
        /*
         *  This code is based on the optimized Led_PreviousChar() in LedSupport, only it has
         *  been adapted to only call CopyOut() on as much text as it needs.
         */
        const size_t chunkSize = 16; // how many characters we read at a time from the TextStore
        Led_tChar    _stackBuf_[1024];
        Led_tChar*   buf = _stackBuf_;

        size_t           bytesReadOutSoFar = 0;
        const Led_tChar* cur               = nullptr;
        bool             notFoundSyncPoint = true;
        for (; notFoundSyncPoint;) { // we return out when we find what we are looking for...

            Assert (bytesReadOutSoFar < beforePos);
            bytesReadOutSoFar += chunkSize;
            bytesReadOutSoFar = min (bytesReadOutSoFar, beforePos - 1); // don't read more than there is!!!
            Assert (bytesReadOutSoFar > 0);
            if (bytesReadOutSoFar > sizeof (_stackBuf_)) {
                if (buf != _stackBuf_) {
                    delete[] buf;
                }
                buf = nullptr;
                buf = new Led_tChar[bytesReadOutSoFar];
            }
            Assert (beforePos > bytesReadOutSoFar);
            CopyOut (beforePos - bytesReadOutSoFar, bytesReadOutSoFar, buf);

            const Led_tChar* fromHere = &buf[bytesReadOutSoFar];

            /*
             *  If the previous byte is a lead-byte, then the real character boundsary
             *  is really TWO back.
             *
             *  Proof by contradiction:
             *      Assume prev character is back one byte. Then the chracter it is part of
             *  contains the first byte of the character we started with. This obviously
             *  cannot happen. QED.
             *
             *      This is actually a worth-while test since lots of second bytes look quite
             *  a lot like lead-bytes - so this happens a lot.
             */
            if (Led_IsLeadByte (*(fromHere - 1))) {
                Assert (fromHere - buf >= 2); // else split character...
                return ((fromHere - 2 - buf) + beforePos - bytesReadOutSoFar);
            }

            if (bytesReadOutSoFar == 1) {
                return (beforePos - 1); // if there is only one byte to go back, it must be an ASCII byte
            }

            // we go back by BYTES til we find a syncronization point
            for (cur = fromHere - 2; cur > buf; cur--) {
                Assert (cur >= buf);
                if (not Led_IsLeadByte (*cur)) {
                    // Then we are in case 1, 2, 3, 4 or 6 (not 5 or 7). So ew know we are looking
                    // at an ASCII byte, or a secondbyte. Therefore - the NEXT byte from here must be
                    // a valid mbyte char boundary.
                    cur++;
                    notFoundSyncPoint = false;
                    break;
                }
            }
            if (bytesReadOutSoFar == beforePos - 1) {
                // we've read all there is - so we must be at a sync-point - regardless.
                notFoundSyncPoint = false;
            }
        }
        Assert (not notFoundSyncPoint);
        Assert (cur >= buf);

        // Now we are pointing AT LEAST one mbyte char back from 'fromHere' so scan forward as we used
        // to to find the previous character...
        const Led_tChar* fromHere = &buf[bytesReadOutSoFar];
        Assert (cur < fromHere);
        Assert (Led_IsValidMultiByteString (cur, bytesReadOutSoFar - (cur - buf)));
        for (; cur < fromHere;) {
            const Led_tChar* next = Led_NextChar (cur);
            if (next == fromHere) {
                if (buf != _stackBuf_) {
                    delete[] buf;
                }
                return ((cur - buf) + beforePos - bytesReadOutSoFar);
            }
            Assert (next < fromHere); // if we've gone past - then fromHere must have split a mbyte char!
            cur = next;
        }
        Assert (false);
        return (0);
#else
        size_t newPos = beforePos - 1;
        Ensure (newPos >= 0); // cuz we cannot split Wide-Characters, and if we were already at beginning
        // of buffer, we'd have never gotten to this code
        return (newPos);
#endif
    }
}

/*
@METHOD:        TextStore::FindWordBreaks
@DESCRIPTION:   <p>NB: *wordEndResult refers to the MARKER (as opposed to character) position after the
        end of the word - so for example - the length of the word is *wordEndResult-*wordStartResult.
        *wordStartResult and *wordEndResult refer to actual marker positions in this TextStore.</p>
</p>
*/
void TextStore::FindWordBreaks (size_t afterPosition, size_t* wordStartResult, size_t* wordEndResult, bool* wordReal, TextBreaks* useTextBreaker)
{
    AssertNotNull (wordStartResult);
    AssertNotNull (wordEndResult);
    AssertNotNull (wordReal);
    size_t startOfThisLine = GetStartOfLineContainingPosition (afterPosition);
    size_t endOfThisLine   = GetEndOfLineContainingPosition (afterPosition);
    size_t len             = endOfThisLine - startOfThisLine;

    Memory::SmallStackBuffer<Led_tChar> buf (len);
    CopyOut (startOfThisLine, len, buf);

    Assert (afterPosition >= startOfThisLine);
    Assert (afterPosition <= endOfThisLine);
    size_t zeroBasedStart = 0;
    size_t zeroBasedEnd   = 0;
    if (useTextBreaker == nullptr) {
        useTextBreaker = GetTextBreaker ().get ();
    }
    AssertNotNull (useTextBreaker);
    useTextBreaker->FindWordBreaks (buf, len, afterPosition - startOfThisLine, &zeroBasedStart, &zeroBasedEnd, wordReal);
    Assert (zeroBasedStart <= zeroBasedEnd);
    Assert (zeroBasedEnd <= len);
    *wordStartResult = zeroBasedStart + startOfThisLine;
    *wordEndResult   = zeroBasedEnd + startOfThisLine;
}

/*
@METHOD:        TextStore::FindLineBreaks
@DESCRIPTION:   <p>NB: *wordEndResult refers to the MARKER (as opposed to character) position after the
        end of the word. *wordEndResult refers to actual marker position in this TextStore.</p>
</p>
*/
void TextStore::FindLineBreaks (size_t afterPosition, size_t* wordEndResult, bool* wordReal, TextBreaks* useTextBreaker)
{
    AssertNotNull (wordEndResult);
    AssertNotNull (wordReal);
    size_t startOfThisLine = GetStartOfLineContainingPosition (afterPosition);
    size_t endOfThisLine   = GetEndOfLineContainingPosition (afterPosition);
    size_t len             = endOfThisLine - startOfThisLine;

    Memory::SmallStackBuffer<Led_tChar> buf (len);
    CopyOut (startOfThisLine, len, buf);

    Assert (afterPosition >= startOfThisLine);
    Assert (afterPosition <= endOfThisLine);
    size_t zeroBasedEnd = 0;
    if (useTextBreaker == nullptr) {
        useTextBreaker = GetTextBreaker ().get ();
    }
    AssertNotNull (useTextBreaker);
    useTextBreaker->FindLineBreaks (buf, len, afterPosition - startOfThisLine, &zeroBasedEnd, wordReal);
    Assert (zeroBasedEnd <= len);
    *wordEndResult = zeroBasedEnd + startOfThisLine;
}

size_t TextStore::FindFirstWordStartBeforePosition (size_t position, bool wordMustBeReal)
{
    /*
     *  Quick and dirty algorithm. This is quite in-efficient - but should do
     *  for the time being.
     *
     *  Start with a goal of position and start looking for words.
     *  The first time we find one whose start is before position - we are done.
     *  Keep sliding the goal back til this is true.
     */
    for (size_t goalPos = position; goalPos > 0; goalPos = FindPreviousCharacter (goalPos)) {
        size_t wordStart = 0;
        size_t wordEnd   = 0;
        bool   wordReal  = false;
        FindWordBreaks (goalPos, &wordStart, &wordEnd, &wordReal);
        if (wordStart <= position and (not wordMustBeReal or wordReal)) {
            return (wordStart);
        }
    }
    return (0);
}

size_t TextStore::FindFirstWordStartStrictlyBeforePosition (size_t position, bool wordMustBeReal)
{
    // maybe just FindFirstWordStartBeforePosition (POS-1)????

    /*
     *  Quick and dirty algorithm. This is quite in-efficient - but should do
     *  for the time being.
     *
     *  Start with a goal of PREVIOUS CHAR (position) and start looking for words.
     *  The first time we find one whose start is before position - we are done.
     *  Keep sliding the goal back til this is true.
     */
    for (size_t goalPos = FindPreviousCharacter (position); goalPos > 0; goalPos = FindPreviousCharacter (goalPos)) {
        size_t wordStart = 0;
        size_t wordEnd   = 0;
        bool   wordReal  = false;
        FindWordBreaks (goalPos, &wordStart, &wordEnd, &wordReal);
        if (wordStart != wordEnd and wordStart < position and (not wordMustBeReal or wordReal)) {
            return (wordStart);
        }
    }
    return (0);
}

size_t TextStore::FindFirstWordEndAfterPosition (size_t position, bool wordMustBeReal)
{
    /*
     *  Quick and dirty algorithm. This is quite in-efficient - but should do
     *  for the time being.
     *
     *  Start with a goal of PREVIOUS CHAR OF POSITION (since position might
     *  be a word ending) and start looking for words.
     *  The first time we find one whose end is AFTER position - we are done.
     */
    for (size_t goalPos = FindPreviousCharacter (position); goalPos < GetEnd (); goalPos = FindNextCharacter (goalPos)) {
        size_t wordStart = 0;
        size_t wordEnd   = 0;
        bool   wordReal  = false;
        FindWordBreaks (goalPos, &wordStart, &wordEnd, &wordReal);
        if (wordStart != wordEnd and wordEnd >= position and (not wordMustBeReal or wordReal)) {
            return (wordEnd);
        }
    }
    return (GetEnd ());
}

size_t TextStore::FindFirstWordStartAfterPosition (size_t position)
{
    /*
     *  Quick and dirty algorithm. This is quite in-efficient - but should do
     *  for the time being.
     *
     *  Start with a goal of position and start looking for words.
     *  The first time we find one whose end is AFTER position - we are done.
     */
    for (size_t goalPos = position; goalPos < GetEnd (); goalPos = FindNextCharacter (goalPos)) {
        size_t wordStart = 0;
        size_t wordEnd   = 0;
        bool   wordReal  = false;
        FindWordBreaks (goalPos, &wordStart, &wordEnd, &wordReal);
        if (wordStart != wordEnd and wordStart >= position and wordReal) {
            return (wordStart);
        }
    }
    return (GetEnd ());
}

/*
@METHOD:        TextStore::Find
@DESCRIPTION:   <p>Search within the given range for the text specified in <code>SearchParameters</code>. The <code>SearchParameters</code>
    specify a bunch of different matching criteria, as well. No 'regexp' searching supported as of yet.</p>
*/
size_t TextStore::Find (const SearchParameters& params, size_t searchFrom, size_t searchTo)
{
    Require (searchTo == eUseSearchParameters or searchTo <= GetEnd ());

    const Led_tChar* pattern    = params.fMatchString.c_str ();
    bool             matchCase  = params.fCaseSensativeSearch;
    bool             wholeWords = params.fWholeWordSearch;
    bool             wrapAtEnd  = (searchTo == eUseSearchParameters) ? params.fWrapSearch : false;

    bool   wrappedYet = false;
    size_t patternLen = Led_tStrlen (pattern);
    size_t bufferEnd  = (searchTo == eUseSearchParameters) ? GetEnd () : searchTo;
    size_t searchIdx  = searchFrom; // index of where we are currently searching from

    Memory::SmallStackBuffer<Led_tChar> lookingAtData (patternLen);

searchSMORE:
    if (searchIdx + patternLen > bufferEnd) {
        goto notFoundByBuffersEnd;
    }
    if (searchIdx >= bufferEnd) {
        goto notFoundByBuffersEnd;
    }

    // See if pattern matches current text
    CopyOut (searchIdx, patternLen, lookingAtData);
    for (size_t i = 0; i < patternLen; i++) {
        bool charsEqual = (lookingAtData[i] == pattern[i]);
        if (not matchCase and not charsEqual) {
// if we are doing case-IN-sensative compare, and characters not the same, maybe they are
// simply of different case?
#if qUseWin32CompareStringCallForCaseInsensitiveSearch
#if qWideCharacters
#define X_COMPARESTRING ::CompareStringW
#else
#define X_COMPARESTRING ::CompareStringA
#endif
            if (X_COMPARESTRING (LOCALE_USER_DEFAULT, NORM_IGNORECASE, &lookingAtData[i], 1, &pattern[i], 1) == CSTR_EQUAL) {
                charsEqual = true;
            }
#undef X_COMPARESTRING
#else
            if (IsASCIIAlpha (lookingAtData[i]) and IsASCIIAlpha (pattern[i]) and
                (tolower (lookingAtData[i]) == tolower (pattern[i]))) {
                charsEqual = true;
            }
#endif
        }

        if (not charsEqual) {
            searchIdx++;
            goto searchSMORE;
        }
    }

    // I've never really understood what the heck this means. So maybe I got it wrong.
    // But more likely, there is just no consistently applied definition across systems
    // (MWERKS IDE editor and MSVC editor seem to treat this differently for example).
    // So what I will do is (basicly what MSVC editor docs say) to
    // consider an otherwise good match to be failed if the text matched isn't preceeded
    // by space/puctuation and succeeded by space/puctuation (including text in the string
    // itself).
    // Some Q/A person will tell me if this is wrong :-) - LGP 960502
    //
    // NB: this is REALLY ASCII/English specific. Must find better way of dealing with
    // this for multi-charset stuff...
    if (wholeWords) {
        // find word-start at head of string and word-end at end of string (can be included
        // in matched text, or just outside)
        Led_tChar c;
        CopyOut (searchIdx, 1, &c);
        bool boundaryChar = (isascii (c) and (isspace (c) or ispunct (c)));
        if (not boundaryChar) {
            // if the selection starts with space, it must contain a whole (possibly zero) number of words
            // only if it DOESNT start with a space character must we look back one, and see that THAT is
            // a space/boundary char
            if (searchIdx > 0) {
                CopyOut (searchIdx - 1, 1, &c);
                boundaryChar = (isascii (c) and (isspace (c) or ispunct (c)));
                if (not boundaryChar) {
                    // If the first char if the match wasn't a space/ending char, and the one
                    // preceeding it wasn't, then we've split a word. Try again
                    searchIdx++;
                    goto searchSMORE;
                }
            }
        }

        // Now similarly check the ending
        if (patternLen > 0) {
            size_t lastCharMatchedIdx = searchIdx + (patternLen - 1);
            CopyOut (lastCharMatchedIdx, 1, &c);
            boundaryChar = (isascii (c) and (isspace (c) or ispunct (c)));
            if (not boundaryChar) {
                // if the selection ends with space, it must contain a whole (possibly zero) number of words
                // only if it DOESNT end with a space character must we look forward one, and see that THAT is
                // a space/boundary char
                if (lastCharMatchedIdx < GetEnd ()) {
                    CopyOut (lastCharMatchedIdx + 1, 1, &c);
                    boundaryChar = (isascii (c) and (isspace (c) or ispunct (c)));
                    if (not boundaryChar) {
                        // If the first char if the match wasn't a space/ending char, and the one
                        // preceeding it wasn't, then we've split a word. Try again
                        searchIdx++;
                        goto searchSMORE;
                    }
                }
            }
        }
    }

    // if I got here, I fell through the loop, and the pattern matches!!!
    return searchIdx;

notFoundByBuffersEnd:
    if (wrapAtEnd) {
        if (wrappedYet) {
            return kBadIndex;
        }
        else {
            wrappedYet = true;
            searchIdx  = 0;
            goto searchSMORE;
        }
    }
    // search to end of buffer (or searchTo limit) - well - not found...
    return kBadIndex;
}

/*
@METHOD:        TextStore::DoAboutToUpdateCalls
@DESCRIPTION:   <p>Utility to call AboutToUpdateText on registered MarkerOwners, and Markers. Don't call directly.
            Called by concrete TextStore subclasses. Note: we call the AbouToUpdate() calls first on the markers, and
            then on the @'MarkerOwner's. And in @'TextStore::DoDidUpdateCalls' we will call them in the <em>reverse</em>
            order.
            </p>
                <p>Note that if you want to update a region of text - use @'TextStore::SimpleUpdater'.
            </p>
            <p>Note also that this API changed arguments in Led 3.1a4</p>
*/
void TextStore::DoAboutToUpdateCalls (const UpdateInfo& updateInfo, Marker* const* markersBegin, Marker* const* markersEnd)
{
    /*
     *  Note that AboutToUpdateText calls ARE allowed to raise exceptions. In which
     *  case, we abandon the update.
     */
    {
        vector<MarkerOwner*>::const_iterator start = GetMarkerOwners ().begin ();
        vector<MarkerOwner*>::const_iterator end   = GetMarkerOwners ().end ();
        for (auto i = start; i != end; ++i) {
            (*i)->AboutToUpdateText (updateInfo);
        }
    }
    {
        for (Marker* const* i = markersBegin; i != markersEnd; ++i) {
            (*i)->AboutToUpdateText (updateInfo);
        }
    }
}

/*
@METHOD:        TextStore::DoDidUpdateCalls
@DESCRIPTION:   <p>Utility to call DidUpdateText on registered MarkerOwners, and Markers. Don't call directly.
            Called by concrete TextStore subclasses.</p>
                <p>NB: See @'TextStore::DoAboutToUpdateCalls'. And note that we call the @'MarkerOwner::DidUpdateText' calls
            in the <em>reverse</em> order in which the @'MarkerOwner::AboutToUpdateText' calls were made.</p>
            <p>Note also that this API changed arguments in Led 3.1a4</p>
*/
void TextStore::DoDidUpdateCalls (const UpdateInfo& updateInfo, Marker* const* markersBegin, Marker* const* markersEnd) noexcept
{
    vector<MarkerOwner*> markerOwners = GetMarkerOwners ();
    /*
     *  Not STRICTLY required that the list of markerowners doesn't change during this call - but would be a possible symptom of
     *  problems, so check with asserts.
     */
    {
        vector<MarkerOwner*>::reverse_iterator start = markerOwners.rbegin ();
        vector<MarkerOwner*>::reverse_iterator end   = markerOwners.rend ();
        for (auto i = start; i != end; ++i) {
            Assert (GetMarkerOwners () == markerOwners);
            (*i)->EarlyDidUpdateText (updateInfo);
            Assert (GetMarkerOwners () == markerOwners);
        }
    }
#if 1
    {
        for (Marker* const* i = markersEnd; i != markersBegin;) {
            --i;
            //          (*(i-1))->DidUpdateText (updateInfo);
            (*i)->DidUpdateText (updateInfo);
            Assert (GetMarkerOwners () == markerOwners);
        }
    }
#else
    {
        vector<Marker*>::const_reverse_iterator start = markers.rbegin ();
        vector<Marker*>::const_reverse_iterator end = markers.rend ();
        for (auto i = start; i != end; ++i) {
            (*i)->DidUpdateText (updateInfo);
            Assert (GetMarkerOwners () == markerOwners);
        }
    }
#endif
    {
        vector<MarkerOwner*>::reverse_iterator start = markerOwners.rbegin ();
        vector<MarkerOwner*>::reverse_iterator end   = markerOwners.rend ();
        for (auto i = start; i != end; ++i) {
            Assert (GetMarkerOwners () == markerOwners);
            (*i)->DidUpdateText (updateInfo);
            Assert (GetMarkerOwners () == markerOwners);
        }
    }
}

/*
@METHOD:        TextStore::PeekAtTextStore
@DESCRIPTION:   <p>Since a TextStore is a MarkerOwner, is must OVERRIDE the PeekAtTextStore method, and return itself.</p>
*/
TextStore* TextStore::PeekAtTextStore () const
{
    return const_cast<TextStore*> (this);
}

#if qDebug
/*
@METHOD:        TextStore::Invariant_
@DESCRIPTION:   <p>Check internal consitency of data structures. Don't call this directly. Call TextStore::Invariant instead.
    And only call at quiesent times; not in the midst of some update where data structures might not be fully consistent.</p>
*/
void TextStore::Invariant_ () const
{
    size_t                              len = GetLength ();
    Memory::SmallStackBuffer<Led_tChar> buf (len);
    CopyOut (0, len, buf);
    const Led_tChar* start   = buf;
    const Led_tChar* end     = &start[len];
    const Led_tChar* curChar = start;
    for (; curChar < end; curChar = Led_NextChar (curChar)) {
        // no need todo anything in here since Led_tChar() checks for
    }
    Assert (curChar == end);
}
#endif
