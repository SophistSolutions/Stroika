/*/
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <bitset>
#include <cctype>
#include <climits>
#include <cstdio> // for a couple sprintf() calls - could pretty easily be avoided

#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Frameworks/Led/StyledTextEmbeddedObjects.h"
#include "Stroika/Frameworks/Led/StyledTextImager.h"

#include "StyledTextIO.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::StyledTextIO;

/*
 ********************************************************************************
 ************************ StyledTextIOReader::SinkStream ************************
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

void StyledTextIOReader::SinkStream::SetJustification (Justification /*justification*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

void StyledTextIOReader::SinkStream::SetStandardTabStopList (const StandardTabStopList& /*tabStops*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

void StyledTextIOReader::SinkStream::SetFirstIndent (TWIPS /*tx*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

void StyledTextIOReader::SinkStream::SetLeftMargin (TWIPS /*lhs*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

void StyledTextIOReader::SinkStream::SetRightMargin (TWIPS /*rhs*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetSpaceBefore
@DESCRIPTION:
*/
void StyledTextIOReader::SinkStream::SetSpaceBefore (TWIPS /*sb*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetSpaceAfter
@DESCRIPTION:
*/
void StyledTextIOReader::SinkStream::SetSpaceAfter (TWIPS /*sa*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetLineSpacing
@DESCRIPTION:
*/
void StyledTextIOReader::SinkStream::SetLineSpacing (LineSpacing /*sl*/)
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
void StyledTextIOReader::SinkStream::SetTableBorderColor (Color /*c*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetTableBorderWidth
@DESCRIPTION:
*/
void StyledTextIOReader::SinkStream::SetTableBorderWidth (TWIPS /*bWidth*/)
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
void StyledTextIOReader::SinkStream::SetCellWidths (const vector<TWIPS>& /*cellWidths*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetCellBackColor
@DESCRIPTION:   <p>Default is to do ignore. Override to implement tables.</p>
*/
void StyledTextIOReader::SinkStream::SetCellBackColor (const Color /*c*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetDefaultCellMarginsForCurrentRow
@DESCRIPTION:   <p>Default is to do ignore. Override to implement tables.</p>
*/
void StyledTextIOReader::SinkStream::SetDefaultCellMarginsForCurrentRow (TWIPS /*top*/, TWIPS /*left*/, TWIPS /*bottom*/, TWIPS /*right*/)
{
    // OVERRIDE, and ignore, since that feature isn't supported by this class
}

/*
@METHOD:        StyledTextIOReader::SinkStream::SetDefaultCellSpacingForCurrentRow
@DESCRIPTION:   <p>Default is to do ignore. Override to implement tables.</p>
*/
void StyledTextIOReader::SinkStream::SetDefaultCellSpacingForCurrentRow (TWIPS /*top*/, TWIPS /*left*/, TWIPS /*bottom*/, TWIPS /*right*/)
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
        Execution::Throw (DataExchange::BadFormatException::kThe);
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
Justification StyledTextIOWriter::SrcStream::GetJustification () const
{
    return eLeftJustify;
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetStandardTabStopList
@DESCRIPTION:
*/
StandardTabStopList StyledTextIOWriter::SrcStream::GetStandardTabStopList () const
{
    return StandardTabStopList{};
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetFirstIndent
@DESCRIPTION:
*/
TWIPS StyledTextIOWriter::SrcStream::GetFirstIndent () const
{
    return TWIPS{0};
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetMargins
@DESCRIPTION:
*/
void StyledTextIOWriter::SrcStream::GetMargins (TWIPS* lhs, TWIPS* rhs) const
{
    RequireNotNull (lhs);
    RequireNotNull (rhs);
    *lhs = TWIPS{0};
    *rhs = TWIPS (6 * 1440); // Not sure what I should return here??? maybe special case and not write anything to output file if returns zero? LGP-990221
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetSpaceBefore
@DESCRIPTION:
*/
TWIPS StyledTextIOWriter::SrcStream::GetSpaceBefore () const
{
    return TWIPS{0};
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetSpaceAfter
@DESCRIPTION:
*/
TWIPS StyledTextIOWriter::SrcStream::GetSpaceAfter () const
{
    return TWIPS{0};
}

/*
@METHOD:        StyledTextIOWriter::SrcStream::GetLineSpacing
@DESCRIPTION:
*/
LineSpacing StyledTextIOWriter::SrcStream::GetLineSpacing () const
{
    return LineSpacing ();
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
StyledTextIOSrcStream_Memory::StyledTextIOSrcStream_Memory (const void* data, size_t nBytes)
    : StyledTextIOReader::SrcStream ()
    , fData (data)
    , fDataEnd (((char*)data) + nBytes)
    , fBytesInBuffer (nBytes)
    , fCurPtr (data) //,
//  fBytesLeft (nBytes)
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
        size_t newSize = ((static_cast<size_t> ((fBytesAllocated + bytes) * 1.5) + kChunkSize - 1 + kMemBlockOverhead) / kChunkSize) * kChunkSize -
                         kMemBlockOverhead; // round to next larger chunksize
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

    size_t                    strLen = effectiveTo - from;
    Memory::StackBuffer<char> buf{Memory::eUninitialized, strLen + 1};
    GetSrcStream ().seek_to (from);
    if (GetSrcStream ().read (buf.data (), strLen) != strLen) {
        GetSrcStream ().seek_to (onEntrySeekPos);
        HandleBadlyFormattedInput (true);
    }
    GetSrcStream ().seek_to (onEntrySeekPos);
    buf[strLen] = '\0';
    return string{buf.data ()};
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

#if qStroika_Frameworks_Led_SupportGDI
/*
 ********************************************************************************
 ***************************** EmbeddingSinkStream ******************************
 ********************************************************************************
 */
EmbeddingSinkStream::EmbeddingSinkStream (StyledTextIOWriter::SinkStream& realSinkStream)
    : fRealSinkStream{realSinkStream}
{
}

void EmbeddingSinkStream::write (const void* buffer, size_t bytes)
{
    fRealSinkStream.write (buffer, bytes);
}
#endif
