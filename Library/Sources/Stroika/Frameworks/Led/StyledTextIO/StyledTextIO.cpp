/*/
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../../Foundation/StroikaPreComp.h"

#include <cctype>
#include <climits>
#include <cstdio> // for a couple sprintf() calls - could pretty easily be avoided

#if qPlatform_MacOS
#include <Errors.h>
#include <Files.h>
#include <TextEdit.h> // for Apple TE scrap format and TEContinuous etc compatability
#endif

#include "../../../Foundation/Memory/SmallStackBuffer.h"

#include "../StyledTextEmbeddedObjects.h"
#include "../StyledTextImager.h"

#include "StyledTextIO.h"

// Include moved down here cuz of GCC lib bug...
#if qBitSetTemplateAvailable
#include <bitset>
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::StyledTextIO;

#ifndef qUseCompiledSetHack
#if qBitSetTemplateAvailable
// Preliminary testing on Mac indicates this compiled set hack doesn't help. Test on PC, as well...
// LGP 970326
#define qUseCompiledSetHack 1
#else
#define qUseCompiledSetHack 0
#endif
#endif

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(4 : 4800) //qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
#pragma warning(4 : 4786) //qQuiteAnnoyingDebugSymbolTruncationWarnings
#endif

/*
 ********************************************************************************
 *************************** StyledTextIOReader::SinkStream *********************
 ********************************************************************************
 */

/*
@METHOD:        StyledTextIOReader::SinkStream::StartTable
@DESCRIPTION:   <p>Default is to do ignore. Override to implement tables.</p>
*/
void StyledTextIOReader::SinkStream::StartTable ()
{
}

/*
@METHOD:        StyledTextIOReader::SinkStream::EndTable
@DESCRIPTION:   <p>Default is to do ignore. Override to implement tables.</p>
*/
void StyledTextIOReader::SinkStream::EndTable ()
{
}

/*
@METHOD:        StyledTextIOReader::SinkStream::StartTableRow
@DESCRIPTION:   <p>Default is to do ignore. Override to implement tables.</p>
*/
void StyledTextIOReader::SinkStream::StartTableRow ()
{
}

/*
@METHOD:        StyledTextIOReader::SinkStream::EndTableRow
@DESCRIPTION:   <p>Default is to do ignore. Override to implement tables.</p>
*/
void StyledTextIOReader::SinkStream::EndTableRow ()
{
}

/*
@METHOD:        StyledTextIOReader::SinkStream::StartTableCell
@DESCRIPTION:   <p>Default is to do ignore. Override to implement tables.</p>
                <p>Note about how this API handles <em>merged cells</em> (merge cells).
            If two or more cells in a row are to be merged together - then the FileIO code that reads
            them will generate a SINGLE StartTableCell/EndTableCell pair, passing the number of horizontally merged cells
            as an argument to StartTableCell ('colSpan'). The total number of StartTableCell/EndTableCell
            calls should correspond to the length of the
            @'StyledTextIOReader::SinkStream::SetCellWidths' argument.
            </p>
*/
void StyledTextIOReader::SinkStream::StartTableCell (size_t /*colSpan*/)
{
}

/*
@METHOD:        StyledTextIOReader::SinkStream::EndTableCell
@DESCRIPTION:   <p>Default is to do ignore. Override to implement tables.</p>
*/
void StyledTextIOReader::SinkStream::EndTableCell ()
{
}

void StyledTextIOReader::SinkStream::SetJustification (Led_Justification /*justification*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

void StyledTextIOReader::SinkStream::SetStandardTabStopList (const TextImager::StandardTabStopList& /*tabStops*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

void StyledTextIOReader::SinkStream::SetFirstIndent (Led_TWIPS /*tx*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

void StyledTextIOReader::SinkStream::SetLeftMargin (Led_TWIPS /*lhs*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

void StyledTextIOReader::SinkStream::SetRightMargin (Led_TWIPS /*rhs*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetSpaceBefore
@DESCRIPTION:
*/
void StyledTextIOReader::SinkStream::SetSpaceBefore (Led_TWIPS /*sb*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetSpaceAfter
@DESCRIPTION:
*/
void StyledTextIOReader::SinkStream::SetSpaceAfter (Led_TWIPS /*sa*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetLineSpacing
@DESCRIPTION:
*/
void StyledTextIOReader::SinkStream::SetLineSpacing (Led_LineSpacing /*sl*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetTextHidden
@DESCRIPTION:
*/
void StyledTextIOReader::SinkStream::SetTextHidden (bool /*hidden*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetListStyle
@DESCRIPTION:
*/
void StyledTextIOReader::SinkStream::SetListStyle (ListStyle /*listStyle*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetListIndentLevel
@DESCRIPTION:
*/
void StyledTextIOReader::SinkStream::SetListIndentLevel (unsigned char /*indentLevel*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetTableBorderColor
@DESCRIPTION:
*/
void StyledTextIOReader::SinkStream::SetTableBorderColor (Led_Color /*c*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetTableBorderWidth
@DESCRIPTION:
*/
void StyledTextIOReader::SinkStream::SetTableBorderWidth (Led_TWIPS /*bWidth*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetCellWidths
@DESCRIPTION:   <p>Default is to do ignore. Override to implement tables.</p>
                <p>Note that this must be called after @'StyledTextIOReader::SinkStream::StartTableRow' and before
            and calls to @'StyledTextIOReader::SinkStream::StartTableCell'. The number and widths here must correspond to the
            number of @'StyledTextIOReader::SinkStream::StartTableCell'/@'StyledTextIOReader::SinkStream::EndTableCell' pairs
            of calls for the row. The number of actual
            cells in the table created could be larger (due to implied cell merges from the
            @'StyledTextIOReader::SinkStream::StartTableCell' call).
            </p>
*/
void StyledTextIOReader::SinkStream::SetCellWidths (const vector<Led_TWIPS>& /*cellWidths*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetCellBackColor
@DESCRIPTION:   <p>Default is to do ignore. Override to implement tables.</p>
*/
void StyledTextIOReader::SinkStream::SetCellBackColor (const Led_Color /*c*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetDefaultCellMarginsForCurrentRow
@DESCRIPTION:   <p>Default is to do ignore. Override to implement tables.</p>
*/
void StyledTextIOReader::SinkStream::SetDefaultCellMarginsForCurrentRow (Led_TWIPS /*top*/, Led_TWIPS /*left*/, Led_TWIPS /*bottom*/, Led_TWIPS /*right*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetDefaultCellSpacingForCurrentRow
@DESCRIPTION:   <p>Default is to do ignore. Override to implement tables.</p>
*/
void StyledTextIOReader::SinkStream::SetDefaultCellSpacingForCurrentRow (Led_TWIPS /*top*/, Led_TWIPS /*left*/, Led_TWIPS /*bottom*/, Led_TWIPS /*right*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
 ********************************************************************************
 ********************* StyledTextIOReader::BadInputHandler **********************
 ********************************************************************************
 */

/*
@METHOD:        StyledTextIOReader::BadInputHandler::HandleBadlyFormattedInput
@DESCRIPTION:   <p>This routine is called whenever this is badly formatted input text to the reader.
    By default - this routine does nothing (unless the unrecoverable argument is 'true').</p>
        <p>This is a change of behavior from Led 2.3. In Led 2.3 - format errors would always throw
    by calling @'Led_ThrowBadFormatDataException'. Instead - this method now calls this virtual method of @'StyledTextIOReader::BadInputHandler' - which your
    subclass can OVERRIDE to throw @'Led_ThrowBadFormatDataException' - if thats the behavior you want. You can replace the error handler associated with
    a particular @'StyledTextIOReader' by calling @'StyledTextIOReader::SetBadInputHandler'.</p>
*/
void StyledTextIOReader::BadInputHandler::HandleBadlyFormattedInput (const StyledTextIOReader& /*reader*/, bool unrecoverable)
{
    if (unrecoverable) {
        Led_ThrowBadFormatDataException ();
    }
}

/*
 ********************************************************************************
 *************************** StyledTextIOWriter::SrcStream **********************
 ********************************************************************************
 */

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetJustification
@DESCRIPTION:
*/
Led_Justification StyledTextIOWriter::SrcStream::GetJustification () const
{
    return eLeftJustify;
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetStandardTabStopList
@DESCRIPTION:
*/
TextImager::StandardTabStopList StyledTextIOWriter::SrcStream::GetStandardTabStopList () const
{
    return StandardTabStopList ();
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetFirstIndent
@DESCRIPTION:
*/
Led_TWIPS StyledTextIOWriter::SrcStream::GetFirstIndent () const
{
    return Led_TWIPS (0);
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetMargins
@DESCRIPTION:
*/
void StyledTextIOWriter::SrcStream::GetMargins (Led_TWIPS* lhs, Led_TWIPS* rhs) const
{
    RequireNotNull (lhs);
    RequireNotNull (rhs);
    *lhs = Led_TWIPS (0);
    *rhs = Led_TWIPS (6 * 1440); // Not sure what I should return here??? maybe special case and not write anything to output file if returns zero? LGP-990221
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetSpaceBefore
@DESCRIPTION:
*/
Led_TWIPS StyledTextIOWriter::SrcStream::GetSpaceBefore () const
{
    return Led_TWIPS (0);
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetSpaceAfter
@DESCRIPTION:
*/
Led_TWIPS StyledTextIOWriter::SrcStream::GetSpaceAfter () const
{
    return Led_TWIPS (0);
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetLineSpacing
@DESCRIPTION:
*/
Led_LineSpacing StyledTextIOWriter::SrcStream::GetLineSpacing () const
{
    return Led_LineSpacing ();
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetListStyleInfo
@DESCRIPTION:
*/
void StyledTextIOWriter::SrcStream::GetListStyleInfo (ListStyle* listStyle, unsigned char* indentLevel) const
{
    RequireNotNull (listStyle);
    RequireNotNull (indentLevel);
    *listStyle   = eListStyle_None;
    *indentLevel = 0;
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetSoftLineBreakCharacter
@DESCRIPTION:   <p>Returns the special character which should be treated (in RTF output) as the SOFT-RETURN line
    break character (in RTF \line).</p>
*/
Led_tChar StyledTextIOWriter::SrcStream::GetSoftLineBreakCharacter () const
{
    return 0; // bogus value - this class doesn't know about this stuff... LGP 991227
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetHidableTextRuns
@DESCRIPTION:
*/
DiscontiguousRun<bool> StyledTextIOWriter::SrcStream::GetHidableTextRuns () const
{
    return DiscontiguousRun<bool> ();
}

/*
 ********************************************************************************
 *************************** StyledTextIOSrcStream_Memory ***********************
 ********************************************************************************
 */
StyledTextIOSrcStream_Memory::StyledTextIOSrcStream_Memory (
    const void* data, size_t nBytes
#if qPlatform_MacOS
    ,
    Handle resourceHandle
#endif
    )
    : StyledTextIOReader::SrcStream ()
    , fData (data)
    , fDataEnd (((char*)data) + nBytes)
    , fBytesInBuffer (nBytes)
    , fCurPtr (data) //,
//  fBytesLeft (nBytes)
#if qPlatform_MacOS
    , fResourceHandle (resourceHandle)
#endif
{
}

size_t StyledTextIOSrcStream_Memory::current_offset () const
{
    return (((char*)fCurPtr - (char*)fData));
}

void StyledTextIOSrcStream_Memory::seek_to (size_t to)
{
    Require (to >= 0);
    to = min (to, fBytesInBuffer);
    //  fBytesLeft = fBytesInBuffer - to;
    fCurPtr = ((char*)fData) + to;
}

size_t StyledTextIOSrcStream_Memory::read (void* buffer, size_t bytes)
{
    RequireNotNull (buffer);

#if 1
    char*  curBytePtr = (char*)fCurPtr;
    size_t curIdx     = curBytePtr - ((char*)fData);
    size_t bytesLeft  = fBytesInBuffer - curIdx;
    bytes             = min (bytesLeft, bytes);
    (void)::memcpy (buffer, curBytePtr, bytes);
    fCurPtr = curBytePtr + bytes;
    return bytes;
#else
    bytes = min (fBytesLeft, bytes);
    (void)::memcpy (buffer, fCurPtr, bytes);
    fCurPtr = ((char*)fCurPtr) + bytes;
    fBytesLeft -= bytes;
    return bytes;
#endif
}

size_t StyledTextIOSrcStream_Memory::read1 (char* c)
{
    RequireNotNull (c);
    char* curBytePtr = (char*)fCurPtr;
    if (curBytePtr != fDataEnd) {
        *c      = *curBytePtr;
        fCurPtr = curBytePtr + 1;
        return 1;
    }
    return 0;
}

#if qPlatform_MacOS
Handle StyledTextIOSrcStream_Memory::GetAUXResourceHandle () const
{
    return fResourceHandle;
}
#endif

/*
 ********************************************************************************
 ******************* StyledTextIOSrcStream_FileDescriptor ***********************
 ********************************************************************************
 */
StyledTextIOSrcStream_FileDescriptor::StyledTextIOSrcStream_FileDescriptor (
    int fd
#if qPlatform_MacOS
    ,
    Handle resourceHandle
#endif
    )
    : StyledTextIOReader::SrcStream ()
    , fFileDescriptor (fd)
    , fCurSeekPos (0)
    , fInputBuffer (nullptr)
    , fInputBufferSize (0)
    , fBufferWindowStart (0)
    , fBufferWindowEnd (0)
#if qPlatform_MacOS
    , fResourceHandle (resourceHandle)
#endif
{
}

StyledTextIOSrcStream_FileDescriptor::~StyledTextIOSrcStream_FileDescriptor ()
{
    delete[] fInputBuffer;
}

size_t StyledTextIOSrcStream_FileDescriptor::current_offset () const
{
    return fCurSeekPos;
}

void StyledTextIOSrcStream_FileDescriptor::seek_to (size_t to)
{
    Require (to >= 0);

    size_t targetSeekPos = (to);
    if (targetSeekPos > fBufferWindowEnd) {
// If we seek PAST that point, we must actually query the file system, so we don't seek
// past EOF
#if qPlatform_MacOS
        /*
         *  We used to just call SetFPos(), and it would return posErr
         *  if I seeked beyond EOF. The trouble is that WindowsNT 3.51's
         *  AppleShare fileserver doesn't work that way. It lets you seek
         *  past EOF. And then returns that larger EOF in ::GetFPos().
         *  The breaks the idiom I use of seeking to BIGNUM and calling
         *  current_offset() to measure how big the file was.
         */
        long logEOF = 0;
        Led_ThrowOSErr (::GetEOF (fFileDescriptor, &logEOF));
        targetSeekPos = min (logEOF, targetSeekPos);
#else
        Assert (false); // NYI
#endif
    }
    fCurSeekPos = targetSeekPos;
}

size_t StyledTextIOSrcStream_FileDescriptor::read (void* buffer, size_t bytes)
{
    RequireNotNull (buffer);

    if (bytes == 0) {
        return 0; // get out early in this case so I can assert bytesToCopyNow!=0 below... assert making progress
    }

    /*
     *  Grab as much as we can from the existing buffered data.
     */
    size_t bytesCopiedSoFar = 0;
NotherRead:
    if (fCurSeekPos >= fBufferWindowStart and fCurSeekPos < fBufferWindowEnd) {
        Assert (bytes > bytesCopiedSoFar);
        size_t bytesToCopyNow = min ((bytes - bytesCopiedSoFar), (fBufferWindowEnd - fCurSeekPos));
        Assert (bytesToCopyNow > 0);
        Assert (bytesToCopyNow <= bytes);
        (void)::memcpy (&((char*)buffer)[bytesCopiedSoFar], &fInputBuffer[fCurSeekPos - fBufferWindowStart], bytesToCopyNow);
        bytesCopiedSoFar += bytesToCopyNow;
        fCurSeekPos += bytesToCopyNow;
    }

    size_t bytesLeftToCopy = bytes - bytesCopiedSoFar;
    if (bytesLeftToCopy > 0) {
        // If there is enuf room in buffer, read in an fill the window
        Assert (bytesLeftToCopy <= bytes);
        if (bytesLeftToCopy < fInputBufferSize) {
            ReadInWindow (fCurSeekPos);
            if (fCurSeekPos < fBufferWindowEnd) {
                goto NotherRead;
            }
            else {
                // must have been EOF if we couldn't read anymore...
                return bytesCopiedSoFar;
            }
        }

// Then we must do a DIRECT read, and don't bother with the buffer
// (we could loop filling/emtpying buffer, but that would be needless copying).
#if qPlatform_MacOS
        Led_ThrowOSErr (::SetFPos (fFileDescriptor, fsFromStart, fCurSeekPos));
        {
            long  count = bytesLeftToCopy;
            OSErr err   = ::FSRead (fFileDescriptor, &count, &((char*)buffer)[bytesCopiedSoFar]);
            Led_ThrowOSErr ((err == eofErr) ? noErr : err);
            bytesCopiedSoFar += count;
            fCurSeekPos += count;
        }
#else
        Assert (false); // NYI
#endif
    }

    return bytesCopiedSoFar;
}

void StyledTextIOSrcStream_FileDescriptor::SetBufferSize (size_t bufSize)
{
    if (fInputBufferSize != bufSize) {
        delete[] fInputBuffer;
        fInputBuffer = nullptr;

        fInputBuffer       = new char[bufSize];
        fInputBufferSize   = bufSize;
        fBufferWindowStart = 0;
        fBufferWindowEnd   = 0;
    }
}

void StyledTextIOSrcStream_FileDescriptor::ReadInWindow ([[maybe_unused]] size_t startAt)
{
    RequireNotNull (fInputBuffer);
    Require (fInputBufferSize > 0);
#if qPlatform_MacOS
    Led_ThrowOSErr (::SetFPos (fFileDescriptor, fsFromStart, startAt));
    long  count = fInputBufferSize;
    OSErr err   = ::FSRead (fFileDescriptor, &count, fInputBuffer);
    Led_ThrowOSErr ((err == eofErr) ? noErr : err);

    fBufferWindowStart = startAt;
    fBufferWindowEnd   = startAt + count;
#else
    Assert (false);     // NYI
#endif
}

#if qPlatform_MacOS
Handle StyledTextIOSrcStream_FileDescriptor::GetAUXResourceHandle () const
{
    return fResourceHandle;
}
#endif

/*
 ********************************************************************************
 ******************** StyledTextIOWriterSinkStream_Memory ***********************
 ********************************************************************************
 */
StyledTextIOWriterSinkStream_Memory::StyledTextIOWriterSinkStream_Memory ()
    : StyledTextIOWriter::SinkStream ()
    , fData (nullptr)
    , fBytesUsed (0)
    , fBytesAllocated (0)
    , fCurPtr (nullptr)
{
}

StyledTextIOWriterSinkStream_Memory::~StyledTextIOWriterSinkStream_Memory ()
{
    delete[] fData;
}

size_t StyledTextIOWriterSinkStream_Memory::current_offset () const
{
    return (fCurPtr - fData);
}

void StyledTextIOWriterSinkStream_Memory::seek_to (size_t to)
{
    Require (to >= 0);
    to      = min (to, fBytesUsed);
    fCurPtr = fData + to;
}

void StyledTextIOWriterSinkStream_Memory::write (const void* buffer, size_t bytes)
{
    const size_t kMemBlockOverhead = 40; // wild guess as to how much overhead this is in a memory request.
    // This is helpful so that we request the largest block size possible
    // from the underlying OS storage without any waste. If the number is wrong -
    // things will still work - but will be less memory efficient.
    const size_t kChunkSize = 16 * 1024; // alloc in this size chunks - at least...

    RequireNotNull (buffer);

    Assert (fBytesUsed <= fBytesAllocated);

    /*
     *  Now, re-alloc the pointer if we need even more space...
     */
    size_t curOffset = fCurPtr - fData;
    if (curOffset + bytes > fBytesAllocated) {
        /*
         *  Avoid quadratic copying - so if size bigger than fixed amount, then increase size allocated by some
         *  factor (so N*log N entries copied).
         */
        size_t newSize = ((static_cast<size_t> ((fBytesAllocated + bytes) * 1.5) + kChunkSize - 1 + kMemBlockOverhead) / kChunkSize) * kChunkSize - kMemBlockOverhead; // round to next larger chunksize
        Assert (newSize > fBytesAllocated);
        Assert (newSize >= fBytesAllocated + bytes);
        char* buf = new char[newSize];
        if (fData != nullptr) {
            ::memcpy (buf, fData, fBytesUsed);
        }
        delete[] fData;
        fData           = buf;
        fCurPtr         = buf + curOffset;
        fBytesAllocated = newSize;
    }

    /*
     *  Then use up what space we have left.
     */
    memcpy (fCurPtr, buffer, bytes);
    buffer     = ((char*)buffer) + bytes;
    fCurPtr    = ((char*)fCurPtr) + bytes;
    fBytesUsed = max (size_t (fCurPtr - fData), fBytesUsed);
}

/*
 ********************************************************************************
 ***************** StyledTextIOWriterSinkStream_FileDescriptor ******************
 ********************************************************************************
 */

/*
 *  Notes about buffering:
 *
 *      We implement a VERY SIMPLISTIC style of buffering. This is because when we seek,
 *  we would normally have to re-read from disk our window, and allow updates. Otherwise
 *  we don't know how much of our window is valid to write back to disk on a flush. To avoid
 *  this problem, we only buffer writes past our last write. That is - we don't buffer
 *  back-patch writes. This simplification still allows this buffering to be a useful speedup,
 *  and remain quite simple (and to NOT to needless reads).
 */
StyledTextIOWriterSinkStream_FileDescriptor::StyledTextIOWriterSinkStream_FileDescriptor (int fd)
    : StyledTextIOWriter::SinkStream ()
    , fFileDescriptor (fd)
    , fOutputBuffer (nullptr)
    , fOutputBufferSize (0)
    , fBufferWindowStart (0)
    , fBufferWindowEnd (0)
    , fFurthestDiskWriteAt (0)
    , fCurSeekPos (0)
{
}

StyledTextIOWriterSinkStream_FileDescriptor::~StyledTextIOWriterSinkStream_FileDescriptor ()
{
    try {
        Flush ();
        UpdateEOF ();
    }
    catch (...) {
        // errors ignored in DTOR cuz they cause 'unexpceted' to be called - at least for mwerks
        // CW8 - LGP 960423
    }
}

size_t StyledTextIOWriterSinkStream_FileDescriptor::current_offset () const
{
    return fCurSeekPos;
}

void StyledTextIOWriterSinkStream_FileDescriptor::seek_to (size_t to)
{
    Require (to >= 0);
    fCurSeekPos = to;
}

void StyledTextIOWriterSinkStream_FileDescriptor::write (const void* buffer, size_t bytes)
{
    if (bytes > fOutputBufferSize or fCurSeekPos < fFurthestDiskWriteAt) {
        /*
         *  No point in doing partial buffering. For large writes, just let 'em pass
         *  through. Also, as we describe above, we don't even try to allow buffering of
         *  "backpatch" writes.
         */
        if (fOutputBufferSize != 0) {
            Flush ();
        }
#if qPlatform_MacOS
        Led_ThrowOSErr (::SetFPos (fFileDescriptor, fsFromStart, fCurSeekPos));
        long count = bytes;
        Led_ThrowOSErr (::FSWrite (fFileDescriptor, &count, (char*)buffer));
        fCurSeekPos += count;
        fFurthestDiskWriteAt = fCurSeekPos;
#else
        Assert (false); // NYI
#endif
    }
    else {
        Assert (fCurSeekPos >= fFurthestDiskWriteAt); // Can only do buffering (easily) in this case
        // (otherwise we might need to do some reads off disk)

        /*
         * We can do SOME buffering then. First be sure buffer window setup at all.
         */
        if (fBufferWindowStart == fBufferWindowEnd) {
            fBufferWindowStart = fCurSeekPos;
            fBufferWindowEnd   = fCurSeekPos + fOutputBufferSize;
        }
        if (fCurSeekPos >= fBufferWindowStart and fCurSeekPos + bytes < fBufferWindowStart + fOutputBufferSize) {
            // then we fit in the window. Just slide the data in.
            memcpy (&fOutputBuffer[fCurSeekPos - fBufferWindowStart], buffer, bytes);
            fCurSeekPos += bytes;
        }
        else {
            // then we must have a nearly full buffer. We could either write what we can
            // into it, and then flush, and then write the rest. Or we could just flush,
            // and memcpy() into the buffer at the start. Unclear which is best, so do whats
            // simplest.
            Flush ();
            Assert (fBufferWindowStart == fBufferWindowEnd);
            fBufferWindowStart = fCurSeekPos;
            fBufferWindowEnd   = fCurSeekPos + fOutputBufferSize;
            Assert (fCurSeekPos >= fBufferWindowStart and fCurSeekPos + bytes < fBufferWindowEnd);
            memcpy (fOutputBuffer, buffer, bytes);
            fCurSeekPos += bytes;
        }
    }
}

void StyledTextIOWriterSinkStream_FileDescriptor::SetBufferSize (size_t bufSize)
{
    if (fOutputBufferSize != bufSize) {
        Flush ();

        delete[] fOutputBuffer;
        fOutputBuffer = nullptr;

        fOutputBuffer      = new char[bufSize];
        fOutputBufferSize  = bufSize;
        fBufferWindowStart = 0;
        fBufferWindowEnd   = 0;
    }
}

void StyledTextIOWriterSinkStream_FileDescriptor::Flush ()
{
    if (fBufferWindowStart != fBufferWindowEnd) {
        Assert (fOutputBufferSize != 0);
        [[maybe_unused]] size_t bytesInWindow = fBufferWindowEnd - fBufferWindowStart;
        Assert (fOutputBufferSize >= bytesInWindow);
#if qPlatform_MacOS
        Led_ThrowOSErr (::SetFPos (fFileDescriptor, fsFromStart, fBufferWindowStart));
        long count = bytesInWindow;
        Led_ThrowOSErr (::FSWrite (fFileDescriptor, &count, fOutputBuffer));
#elif qPlatform_Windows
        Assert (false);
#endif
        fFurthestDiskWriteAt = fBufferWindowEnd;
        fBufferWindowStart   = 0; // mark as no window now
        fBufferWindowEnd     = 0;
    }
}

void StyledTextIOWriterSinkStream_FileDescriptor::UpdateEOF ()
{
    Flush ();
#if qPlatform_MacOS
    Led_ThrowOSErr (::SetEOF (fFileDescriptor, fCurSeekPos));
#else
    Assert (false); // NYI
#endif
}

/*
 ********************************************************************************
 ***************************** StyledTextIOReader *******************************
 ********************************************************************************
 */

string StyledTextIOReader::GrabString (size_t from, size_t to)
{
    size_t onEntrySeekPos = GetSrcStream ().current_offset ();

    size_t effectiveTo = to;
    if (effectiveTo == size_t (-1)) {
        effectiveTo = onEntrySeekPos;
    }
    Require (from <= effectiveTo);

    size_t                         strLen = effectiveTo - from;
    Memory::SmallStackBuffer<char> buf (strLen + 1);
    GetSrcStream ().seek_to (from);
    if (GetSrcStream ().read (buf, strLen) != strLen) {
        GetSrcStream ().seek_to (onEntrySeekPos);
        HandleBadlyFormattedInput (true);
    }
    GetSrcStream ().seek_to (onEntrySeekPos);
    buf[strLen] = '\0';
    return string (buf);
}

/*
 ********************************************************************************
 ******************************* StyledTextIOWriter *****************************
 ********************************************************************************
 */
void StyledTextIOWriter::write (const void* data, size_t nBytes)
{
    GetSinkStream ().write (data, nBytes);
}

void StyledTextIOWriter::write (char c)
{
    GetSinkStream ().write (&c, 1);
}

void StyledTextIOWriter::write (const char* str)
{
    RequireNotNull (str);
    GetSinkStream ().write (str, strlen (str));
}

void StyledTextIOWriter::write (const string& str)
{
    GetSinkStream ().write (str.c_str (), str.length ());
}

/*
 ********************************************************************************
 ***************************** EmbeddingSinkStream ******************************
 ********************************************************************************
 */
EmbeddingSinkStream::EmbeddingSinkStream (StyledTextIOWriter::SinkStream& realSinkStream)
    : SimpleEmbeddedObjectStyleMarker::SinkStream ()
    , fRealSinkStream (realSinkStream)
{
}

void EmbeddingSinkStream::write (const void* buffer, size_t bytes)
{
    fRealSinkStream.write (buffer, bytes);
}
