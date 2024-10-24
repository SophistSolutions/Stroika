/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StyledTextIO_h_
#define _Stroika_Frameworks_Led_StyledTextIO_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

/*
@MODULE:    StyledTextIO
@DESCRIPTION:
        <p>A portable attempt at abstracting away the details of styled text file IO and all
    the different formats for styled text.</p>
        <p>This code defines APIs which should allow for reading any different styled text format.
    And dumping it to any different output (eg. text buffer).</p>
        <p>The only real LED-SPECIFIC parts of this are that I only provide concrete output (aka sinks)
    implementations to Led StandardStyledTextImagers. And that some of the Src/Sink APIs are oriented towards what would be
    helpful for a Led-based editor (in other words, features not supported by Led aren't communicated to/from the src/sinks).</p>
        <p>The <em>big picture</em> for this module is that there are two main basic subdivisions. There are
    @'StyledTextIOReader' subclasses, and @'StyledTextIOWriter' subclasses. The readers are for READING some file format,
    and converting it to a stream of method calls (on a sink to be described later). And writers are for writing those
    formatted data files, based on results of method calls on an abstract source class.</p>
        <p>Though @'StyledTextIOReader' and @'StyledTextIOWriter' share no common base class, they <em>do</em> follow
    a very similar design pattern. They both define abstract 'sources' and 'sinks' for their operation.</p>
        <p>For a @'StyledTextIOReader', it reads its data from a @'StyledTextIOReader::SrcStream' (typically maybe a file),
    and writes it to a @'StyledTextIOReader::SinkStream' (typically a Led-text-buffer/view).</p>
        <p>A @'StyledTextIOWriter', writes data extracted from a @'StyledTextIOWriter::SrcStream'
    (typically view/textstore, much like a @'StyledTextIOReader::SinkStream'),
    and writes it to a @'StyledTextIOWriter::SinkStream' (typically an output file).</p>
        <p>These abstract sources and sinks are defined to just the minimal pure virtual APIs needed to extract/write bare bytes,
    or formatted text in a format Led can understand. Several concrete instantiations of each are provided by Led (some here, and
    some in other modules, as appropriate).</p>
        <p>Subclasses of @'StyledTextIOReader' and @'StyledTextIOWriter' are where the knowledge of particular file formats resides.
    For example, the knowledge of how to read RTF is in @'StyledTextIOReader_RTF' and the knowledge of how to write HTML is in
    @'StyledTextIOWriter_HTML'.</p>
 */

#include <set>

#include "Stroika/Foundation/Memory/Common.h"

#include "Stroika/Frameworks/Led/StandardStyledTextImager.h" //  For StyledInfoSummaryRecord declaration
#include "Stroika/Frameworks/Led/StyledTextEmbeddedObjects.h"
#include "Stroika/Frameworks/Led/StyledTextImager.h"
#include "Stroika/Frameworks/Led/Support.h"

namespace Stroika::Frameworks::Led::StyledTextIO {

    /*
    @CLASS:         StyledTextIOReader
    @DESCRIPTION:   <p>Abstract base class for styled text reading. Subclasses know about various styled text file formats, and
        take care of the details of mapping streams of bytes into Led internal data structures with that styled text.</p>
    */
    class StyledTextIOReader {
    public:
        /*
        @DESCRIPTION:   <p>A StyledTextIOReader needs a pointer to a function which is a source of raw bytes
            to be interpretted as text which will be inserted into a text buffer. SrcStream is an abstract class
            defining this API.</p>

             \@todo THIS IS OBSOLETE - and should be switched to using Streams::InputStream<byte> --LGP 2024-02-22

        */
        class SrcStream {
        public:
            virtual ~SrcStream () = default;

        public:
            /*
            @METHOD:        StyledTextIOReader::SrcStream::current_offset
            @DESCRIPTION:   <p>Return the current seekPos</p>
            */
            virtual size_t current_offset () const = 0;

        public:
            /*
            @METHOD:        StyledTextIOReader::SrcStream::seek_to
            @DESCRIPTION:   <p>Sets the current read-pointer to the given position (often used to scan backwards, but can be any direction).
                Note if 'to' is past end, this just pins one past end of buffer.</p>
            */
            virtual void seek_to (size_t to) = 0;

        public:
            /*
            @METHOD:        StyledTextIOReader::SrcStream::read
            @DESCRIPTION:   <p>Read the given number of bytes from the is source, and fill them into the buffer. Returns
                        the number of bytes read. Always return as many bytes as possible and block if they
                        are not yet available. Don't return partial reads (except on EOF). 'bytes' can be zero, and then
                        'read' will return immediately with a return value of zero.</p>
            */
            virtual size_t read (void* buffer, size_t bytes) = 0;

        public:
            /*
            @METHOD:        StyledTextIOReader::SrcStream::read1
            @DESCRIPTION:   <p>Read 1 character and return the number of characters read (0 or 1).
                        This is a trivial wrapper on @'StyledTextIOReader::SrcStream::read' which can be overriden
                        and is frequently used as a performance optimization.</p>
            */
            virtual size_t read1 (char* c)
            {
                return read (c, 1);
            }
        };

    public:
        class SinkStream;
        /*
    @CLASS:         StyledTextIOReader::BadInputHandler
    @DESCRIPTION:
            <p>Abstract base class for styled text writing. Subclasses know about various styled text file formats, and
        take care of the details of mapping Led internal data structures with styled text into streams of bytes in that format.</p>
    */
        class BadInputHandler {
        public:
            virtual ~BadInputHandler () = default;
            virtual void HandleBadlyFormattedInput (const StyledTextIOReader& reader, bool unrecoverable);
        };

    protected:
        StyledTextIOReader (SrcStream* srcStream, SinkStream* sinkStream,
                            const shared_ptr<BadInputHandler>& badInputHander = shared_ptr<BadInputHandler> ()); // callers responsability to destroy srcStream/sinkStream

        // The Read() method must be overriden by one subclass to provide the format interpretation
    public:
        virtual void Read ()                            = 0;
        virtual bool QuickLookAppearsToBeRightFormat () = 0;

    public:
        /*
        @CLASS:         StyledTextIOReader::BufferedIndirectSrcStream
        @BASES:         @'StyledTextIOReader::SrcStream'
        @DESCRIPTION:   <p></p>
        */
        class BufferedIndirectSrcStream : public SrcStream {
        public:
            BufferedIndirectSrcStream (SrcStream& realSrcStream);

        public:
            virtual size_t current_offset () const override;
            virtual void   seek_to (size_t to) override;
            virtual size_t read (void* buffer, size_t bytes) override;
            virtual size_t read1 (char* c) override;

        private:
            nonvirtual void FillCache ();

        private:
            SrcStream&  fRealSrcStream;
            char        fWindowTop_Data[4 * 1024]; // buffer for how much we buffer at a time...
            size_t      fWindowTop_Offset;
            const char* fWindowBottom_Data;
            size_t      fWindowBottom_Offset;
            const char* fCursor_Data;
            size_t      fCursor_Offset;
        };

    public:
        nonvirtual SrcStream&  GetSrcStream () const;
        nonvirtual SinkStream& GetSinkStream () const;

    private:
        mutable BufferedIndirectSrcStream fSrcStream;
        SinkStream*                       fSinkStream;

    public:
        nonvirtual shared_ptr<BadInputHandler> GetBadInputHandler () const;
        nonvirtual void                        SetBadInputHandler (const shared_ptr<BadInputHandler>& badInputHandler);

    private:
        shared_ptr<BadInputHandler> fBadInputHandler;

    public:
        nonvirtual void HandleBadlyFormattedInput (bool unrecoverable = false) const;

    protected:
        class SrcStreamSeekSaver;

    protected:
        struct ReadEOFException {}; // not an error state necesserily. Just allows our Read helper routines
        // to be simpler. Don't need to worry as much about this special case,
        // and they can just return chars (and throw on eof).

        nonvirtual void   PutBackLastChar () const;
        nonvirtual char   GetNextChar () const;
        nonvirtual char   PeekNextChar () const;
        nonvirtual void   ConsumeNextChar () const;
        nonvirtual string GrabString (size_t from, size_t to = size_t (-1)); // doesn't move seek_to () position (rather restores it)
        // if no end specified (-1), then grab from to current seekpos
    };

    // utility to auto-scroll back to place in stream where we were created on DTOR
    class StyledTextIOReader::SrcStreamSeekSaver {
    public:
        SrcStreamSeekSaver (SrcStream& srcStream);
        ~SrcStreamSeekSaver ();

    private:
        SrcStream& fSrcStream;
        size_t     fSavedPos;
    };

    /*
    @CLASS:         StyledTextIOReader::SinkStream
    @DESCRIPTION:   <p>A StyledTextIOReader needs a pointer to a function which is a sink for all the styled text
                and other information read. SinkStream is an abstract class defining this API.</p>
    */
    class StyledTextIOReader::SinkStream {
    public:
        virtual ~SinkStream ()
        {
        }

    public:
        /*
        @METHOD:        StyledTextIOReader::SinkStream::current_offset
        @DESCRIPTION:   <p>Return the current seekPos</p>
        */
        virtual size_t current_offset () const = 0; // current seekPos

    public:
        /*
        @METHOD:        StyledTextIOReader::SinkStream::AppendText
        @DESCRIPTION:   <p>Append the given text to the output text buffer. If fontSpec is nullptr, use default.
            Probably later we will return and update the fontspec with @'StyledTextIOReader::SinkStream::ApplyStyle'. Note, this style
            of API is defined cuz some format readers give us a bunch of text at once, and then later (elsewhere) store the style
            information. And still others provide them together, hand-in-hand.</p>
        */
        virtual void AppendText (const Led_tChar* text, size_t nTChars, const FontSpecification* fontSpec) = 0;

    public:
        /*
        @METHOD:        StyledTextIOReader::SinkStream::ApplyStyle
        @DESCRIPTION:   <p>Apply the given style information to the given range of text. See @'StyledTextIOReader::SinkStream::AppendText'.</p>
        */
        virtual void ApplyStyle (size_t from, size_t to, const vector<StyledInfoSummaryRecord>& styleRuns) = 0;

    public:
        /*
        @METHOD:        StyledTextIOReader::SinkStream::GetDefaultFontSpec
        @DESCRIPTION:
        */
        virtual FontSpecification GetDefaultFontSpec () const = 0;

#if qStroika_Frameworks_Led_SupportGDI
    public:
        /*
        @METHOD:        StyledTextIOReader::SinkStream::InsertEmbeddingForExistingSentinel
        @DESCRIPTION:
        */
        virtual void InsertEmbeddingForExistingSentinel (SimpleEmbeddedObjectStyleMarker* embedding, size_t at) = 0;

    public:
        /*
        @METHOD:        StyledTextIOReader::SinkStream::AppendEmbedding
        @DESCRIPTION:
        */
        virtual void AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding) = 0;
#endif

    public:
        /*
        @METHOD:        StyledTextIOReader::SinkStream::AppendSoftLineBreak
        @DESCRIPTION:
        */
        virtual void AppendSoftLineBreak () = 0;

    public:
        virtual void StartTable ();
        virtual void EndTable ();
        virtual void StartTableRow ();
        virtual void EndTableRow ();
        virtual void StartTableCell (size_t colSpan);
        virtual void EndTableCell ();

    public:
        /*
        @METHOD:        StyledTextIOReader::SinkStream::InsertMarker
        @DESCRIPTION:
        */
        virtual void InsertMarker (Marker* m, size_t at, size_t length, MarkerOwner* markerOwner) = 0;

    public:
        virtual void SetJustification (Justification justification);
        virtual void SetStandardTabStopList (const StandardTabStopList& tabStops);
        virtual void SetFirstIndent (TWIPS tx);
        virtual void SetLeftMargin (TWIPS lhs);
        virtual void SetRightMargin (TWIPS rhs);
        virtual void SetSpaceBefore (TWIPS sb);
        virtual void SetSpaceAfter (TWIPS sa);
        virtual void SetLineSpacing (LineSpacing sl);
        virtual void SetTextHidden (bool hidden);
        virtual void SetListStyle (ListStyle listStyle);
        virtual void SetListIndentLevel (unsigned char indentLevel);

    public:
        virtual void SetTableBorderColor (Color c);
        virtual void SetTableBorderWidth (TWIPS bWidth);
        virtual void SetCellWidths (const vector<TWIPS>& cellWidths);
        virtual void SetCellBackColor (const Color c);
        virtual void SetDefaultCellMarginsForCurrentRow (TWIPS top, TWIPS left, TWIPS bottom, TWIPS right);
        virtual void SetDefaultCellSpacingForCurrentRow (TWIPS top, TWIPS left, TWIPS bottom, TWIPS right);

    public:
        /*
        @METHOD:        StyledTextIOReader::SinkStream::EndOfBuffer
        @DESCRIPTION:   <p>Called by StyledText IO readers when end of source buffer is encountered. This means that
                the next Flush () call contains the last of the text.</p>
        */
        virtual void EndOfBuffer (){};

    public:
        /*
        @METHOD:        StyledTextIOReader::SinkStream::Flush
        @DESCRIPTION:
        */
        virtual void Flush () = 0;

    public:
        nonvirtual size_t GetCountOfTCharsInserted () const;
    };

    /*
    @CLASS:         StyledTextIOWriter
    @DESCRIPTION:
            <p>Abstract base class for styled text writing. Subclasses know about various styled text file formats, and
        take care of the details of mapping Led internal data structures with styled text into streams of bytes in that format.</p>
    */
    class StyledTextIOWriter {
    public:
        class SrcStream;
        class SinkStream;

    protected:
        StyledTextIOWriter (SrcStream* srcStream, SinkStream* sinkStream); // callers responsability to destroy srcStream/sinkStream

        // The Read() method must be overriden by one subclass to provide the format interpretation
    public:
        virtual void Write () = 0;

    public:
        nonvirtual SrcStream&  GetSrcStream () const;
        nonvirtual SinkStream& GetSinkStream () const;

    private:
        SrcStream*  fSrcStream;
        SinkStream* fSinkStream;

        // Utilities
    protected:
        nonvirtual void write (const void* data, size_t nBytes);
        nonvirtual void write (char c);
        nonvirtual void write (const char* str);
        nonvirtual void write (const string& str);
    };

    /*
    @CLASS:         StyledTextIOWriter::SrcStream
    @DESCRIPTION:   <p>Abstract base class for @'StyledTextIOWriter's to get their text content from.</p>
    */
    class StyledTextIOWriter::SrcStream {
    public:
        virtual ~SrcStream () = default;

    public:
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::readNTChars
        @DESCRIPTION:   <p>readNTChars can retun less than maxTChars before end of buffer, only to make us end on even
            mbyte char boundary.</p>
        */
        virtual size_t readNTChars (Led_tChar* intoBuf, size_t maxTChars) = 0;

    public:
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::current_offset
        @DESCRIPTION:   <p>current seekPos</p>
        */
        virtual size_t current_offset () const = 0;

    public:
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::seek_to
        @DESCRIPTION:   <p>'to' past end just pins one past end of buffer</p>
        */
        virtual void seek_to (size_t to) = 0;

    public:
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::GetTotalTextLength
        @DESCRIPTION:   <p>Total # of tChars</p>
        */
        virtual size_t GetTotalTextLength () const = 0;

    public:
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::GetStyleInfo
        @DESCRIPTION:
        */
        virtual vector<StyledInfoSummaryRecord> GetStyleInfo (size_t from, size_t len) const = 0;

#if qStroika_Frameworks_Led_SupportGDI
    public:
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::CollectAllEmbeddingMarkersInRange
        @DESCRIPTION:
        */
        virtual vector<SimpleEmbeddedObjectStyleMarker*> CollectAllEmbeddingMarkersInRange (size_t from, size_t to) const = 0;
#endif

    public:
        class Table;

        /*
        @METHOD:        StyledTextIOWriter::SrcStream::GetTableAt
        @DESCRIPTION:   Return a @'StyledTextIOWriter::SrcStream::Table' object. Note - this is not to be
                    confused with a @'WordProcessor::Table' object. Though they are closely related, this object
                    contains just the API required for writing tables to files.
        */
        virtual Table* GetTableAt (size_t at) const = 0;

    public:
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::SummarizeFontAndColorTable
        @DESCRIPTION:   <p>Produce a list of all fontnames and colors used in the document. This is needed for some formats
                    like RTF which require a list of all font names and colors before writing any of the rest of the document.</p>
        */
        virtual void SummarizeFontAndColorTable (set<SDKString>* fontNames, set<Color>* colorsUsed) const = 0;

    public:
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::GetEmbeddingMarkerPosOffset
        @DESCRIPTION:   <p>Since we maybe externalizing a subset of the buffer, and the marker positions in the embedding object
            are absolute, we need to know this to relativize them in the externalized stream</p>
        */
        virtual size_t GetEmbeddingMarkerPosOffset () const = 0;

    public:
        virtual Justification          GetJustification () const;
        virtual StandardTabStopList    GetStandardTabStopList () const;
        virtual TWIPS                  GetFirstIndent () const;
        virtual void                   GetMargins (TWIPS* lhs, TWIPS* rhs) const;
        virtual TWIPS                  GetSpaceBefore () const;
        virtual TWIPS                  GetSpaceAfter () const;
        virtual LineSpacing            GetLineSpacing () const;
        virtual void                   GetListStyleInfo (ListStyle* listStyle, unsigned char* indentLevel) const;
        virtual Led_tChar              GetSoftLineBreakCharacter () const;
        virtual DiscontiguousRun<bool> GetHidableTextRuns () const;
    };

    /*
    @CLASS:         StyledTextIOWriter::SrcStream::Table
    @DESCRIPTION:   <p>Simple abstract API so styled text IO code can ask key questions about a table object in order to persist it..</p>
    */
    class StyledTextIOWriter::SrcStream::Table {
    public:
        virtual ~Table () = default;

    public:
        struct CellInfo {
            CellInfo ();
            TWIPS f_cellx;
            Color f_clcbpat; // cell background color
        };

    public:
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::Table::GetRows
        @DESCRIPTION:   <p>Get the number of rows in the given table.</p>
        */
        virtual size_t GetRows () const = 0;
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::Table::GetColumns
        @DESCRIPTION:   <p>Get the number of columns in the given row. Note that this can be
                    different from row to row within a given table.</p>
        */
        virtual size_t GetColumns (size_t row) const = 0;
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::Table::GetRowInfo
        @DESCRIPTION:   <p>Get the @'StyledTextIOWriter::SrcStream::Table::CellInfo's for the given
                    row. The number of these records is the same as the return value from
                    @'StyledTextIOWriter::SrcStream::Table::GetColumns'.
                    </p>
        */
        virtual void GetRowInfo (size_t row, vector<CellInfo>* cellInfos) = 0;
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::Table::MakeCellSubSrcStream
        @DESCRIPTION:   <p>Returns a @'StyledTextIOWriter::SrcStream' containing the data for the given table cell.
                    Note that this can return nullptr for 'merged' cells.</p>
        */
        virtual StyledTextIOWriter::SrcStream* MakeCellSubSrcStream (size_t row, size_t column) = 0;
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::Table::GetOffsetEnd
        @DESCRIPTION:   <p>Returns the source-doc offset of the given table end relative to its start. Readers should
                    skip ahead that many positions after writing the tables contents.</p>
        */
        virtual size_t GetOffsetEnd () const = 0;
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::Table::GetDefaultCellMarginsForRow
        @DESCRIPTION:   <p>Return the default cell margins for the given row. Note that the @'TWIPS_Rect'
                    is not a true rectangle, but just a handy way to return 4 values - a top/left/bottom/right.</p>
        */
        virtual TWIPS_Rect GetDefaultCellMarginsForRow (size_t row) const = 0;
        /*
        @METHOD:        StyledTextIOWriter::SrcStream::Table::GetDefaultCellSpacingForRow
        @DESCRIPTION:   <p>Return the default cell spacing for the given row. Note that the @'TWIPS_Rect'
                    is not a true rectangle, but just a handy way to return 4 values - a top/left/bottom/right.</p>
        */
        virtual TWIPS_Rect GetDefaultCellSpacingForRow (size_t row) const = 0;
    };

    /*
       <p>Abstract base class for @'StyledTextIOWriter's to dump their text content to.</p>

       \@todo THIS IS OBSOLETE - and should be switched to using Streams::OutputStream<byte> --LGP 2024-02-22
    */
    class StyledTextIOWriter::SinkStream {
    public:
        virtual ~SinkStream ()
        {
        }

    public:
        /*
        @METHOD:        StyledTextIOWriter::SinkStream::current_offset
        @DESCRIPTION:   <p>Return the current seekPos.</p>
        */
        virtual size_t current_offset () const = 0;

    public:
        /*
        @METHOD:        StyledTextIOWriter::SinkStream::seek_to
        @DESCRIPTION:   <p>'to' past end just pins one past end of buffer</p>
        */
        virtual void seek_to (size_t to) = 0;

    public:
        /*
        @METHOD:        StyledTextIOWriter::SinkStream::write
        @DESCRIPTION:
        */
        virtual void write (const void* buffer, size_t bytes) = 0;
    };

    /*
        ********* Some StyledTextIOReader::SrcStream subclasses *********
        */
    /*
    @CLASS:         StyledTextIOSrcStream_Memory
    @BASES:         @'StyledTextIOReader::SrcStream'
    @DESCRIPTION:   <p>If you have a block of memory which contains the untyped contents which will be converted by some
        reader (@'StyledTextIOReader'), you use this as the @'StyledTextIOReader::SrcStream'. Just initialize one of these
        with the appropriate data, and pass this to the appropriate @'StyledTextIOReader'.</p>
            <p>NB: This class doesn't free up, or copy the given pointer. It is up the the caller todo that, and only after
        this SrcStream object has been destroyed. Typically, this follows trivially from a sequential, stack-based allocation
        strategy, where the data comes from some object declared earlier on the stack.</p>
    */
    class StyledTextIOSrcStream_Memory : public StyledTextIOReader::SrcStream {
    public:
        StyledTextIOSrcStream_Memory (const void* data, size_t nBytes);

    public:
        virtual size_t current_offset () const override;
        virtual void   seek_to (size_t to) override;
        virtual size_t read (void* buffer, size_t bytes) override;
        virtual size_t read1 (char* c) override;

    private:
        const void* fData;
        const void* fDataEnd;
        size_t      fBytesInBuffer;
        const void* fCurPtr;
    };

    inline StyledTextIOReader::BufferedIndirectSrcStream::BufferedIndirectSrcStream (SrcStream& realSrcStream)
        : fRealSrcStream (realSrcStream)
        ,
        //fWindowTop_Data (),
        fWindowTop_Offset (size_t (-1))
        , fWindowBottom_Data (nullptr)
        , fWindowBottom_Offset (size_t (-1))
        , fCursor_Data (nullptr)
        , fCursor_Offset (0)
    {
    }
    inline void StyledTextIOReader::BufferedIndirectSrcStream::FillCache ()
    {
        fWindowTop_Offset = fCursor_Offset;
        fRealSrcStream.seek_to (fWindowTop_Offset); // probably could frequently optimize this call way if we were careful to cache last seek-offset from buffer
        size_t bytesRead     = fRealSrcStream.read (fWindowTop_Data, Foundation::Memory::NEltsOf (fWindowTop_Data));
        fWindowBottom_Data   = fWindowTop_Data + bytesRead;
        fWindowBottom_Offset = fWindowTop_Offset + bytesRead;
        Assert (fCursor_Offset >= fWindowTop_Offset and fCursor_Offset <= fWindowBottom_Offset); // should only call FillCache in that case?
        Assert (fCursor_Offset == fWindowTop_Offset);                                            // should only call FillCache in that case?
        if (fCursor_Offset >= fWindowTop_Offset and fCursor_Offset <= fWindowBottom_Offset) {
            fCursor_Data = fWindowTop_Data + (fCursor_Offset - fWindowTop_Offset);
        }
        Ensure (fWindowTop_Data <= fCursor_Data and fCursor_Data <= fWindowBottom_Data);
        Ensure (fWindowTop_Offset <= fCursor_Offset and fCursor_Offset <= fWindowBottom_Offset);
    }
    inline size_t StyledTextIOReader::BufferedIndirectSrcStream::current_offset () const
    {
        return fCursor_Offset;
    }
    inline void StyledTextIOReader::BufferedIndirectSrcStream::seek_to (size_t to)
    {
        // If seekpos inside our window (at end of buffer counts as inside window even though next read may force a FillCache),
        // just update offset(s), and otherwise - mark fCursor_Data as nullptr so we know cache invalid
        if (fWindowTop_Offset <= to and to <= fWindowBottom_Offset) {
            fCursor_Data = fWindowTop_Data + (to - fWindowTop_Offset);
        }
        else {
            fCursor_Data = nullptr;
        }
        fCursor_Offset = to;
    }
    inline size_t StyledTextIOReader::BufferedIndirectSrcStream::read (void* buffer, size_t bytes)
    {
        RequireNotNull (buffer);

        byte*  destCursor     = reinterpret_cast<byte*> (buffer);
        size_t bytesReadSoFar = 0;

        /*
            *  See if the initial part of this request can be satisfied by our current buffered data
            *  and updated 'bytesReadSoFar' to reflect how much read from that buffer.
            */
        if (fCursor_Data != nullptr and fWindowTop_Offset >= fCursor_Offset and fCursor_Offset < fWindowBottom_Offset) {
            size_t bytesAvail    = fWindowBottom_Offset - fCursor_Offset; // must be > 0 UNLESS we are at EOF
            size_t thisReadCount = min (bytesAvail, bytes);
            AssertNotNull (fCursor_Data);
            (void)::memcpy (destCursor, fCursor_Data, thisReadCount);
            destCursor += thisReadCount;
            fCursor_Data += thisReadCount;
            fCursor_Offset += thisReadCount;
            bytesReadSoFar += thisReadCount;
        }

        /*
            *  If we've not completed the request, see if it can be accomodated by by filling the buffer,
            *  and trying to pull data out of that buffer. If not - then simply read the data directly.
            */
        if (bytesReadSoFar < bytes) {
            size_t bytesLeftToRead = bytes - bytesReadSoFar;
            if (bytesLeftToRead < Foundation::Memory::NEltsOf (fWindowTop_Data)) {
                FillCache ();
                size_t bytesAvail    = fWindowBottom_Offset - fCursor_Offset; // must be > 0 UNLESS we are at EOF
                size_t thisReadCount = min (bytesAvail, bytesLeftToRead);
                AssertNotNull (fCursor_Data);
                DISABLE_COMPILER_MSC_WARNING_START (6387) // fCursor_Data is not null because of assertion above
                (void)::memcpy (destCursor, fCursor_Data, thisReadCount);
                DISABLE_COMPILER_MSC_WARNING_END (6387)
                destCursor += thisReadCount;
                fCursor_Data += thisReadCount;
                fCursor_Offset += thisReadCount;
                bytesReadSoFar += thisReadCount;
            }
            else {
                fRealSrcStream.seek_to (fCursor_Offset);
                size_t bytesRead = fRealSrcStream.read (destCursor, bytesLeftToRead);
                bytesReadSoFar += bytesRead;
                fCursor_Offset += bytesRead;
                // Cache is invalid - so mark it so...
                fCursor_Data = nullptr;
            }
        }
        return bytesReadSoFar;
    }
    inline size_t StyledTextIOReader::BufferedIndirectSrcStream::read1 (char* c)
    {
        RequireNotNull (c);
        /*
            *  See if we can read ANY non-zero number of bytes out of our window. If yes - then just
            *  return those (even if thats less than the user requested - following standard UNIX read
            *  conventions). If we cannot read any bytes given our current window, refill the window, and
            *  try again.
            */
        if ((fCursor_Data == nullptr) or (fCursor_Offset < fWindowTop_Offset or fCursor_Offset >= fWindowBottom_Offset)) {
            FillCache ();
        }
        Assert (fWindowTop_Offset <= fCursor_Offset and fCursor_Offset <= fWindowBottom_Offset);
        if (fWindowBottom_Offset == fCursor_Offset) {
            return 0;
        }
        else {
            AssertNotNull (fCursor_Data);
            *c = *fCursor_Data;
            ++fCursor_Data;
            ++fCursor_Offset;
            return 1;
        }
    }

    /*
        ********* Some StyledTextIOReader::SrcStream subclasses *********
        */
    /*
    @CLASS:         StyledTextIOWriterSinkStream_Memory
    @BASES:         @'StyledTextIOWriter::SinkStream'
    @DESCRIPTION:
    */
    class StyledTextIOWriterSinkStream_Memory : public StyledTextIOWriter::SinkStream {
    public:
        StyledTextIOWriterSinkStream_Memory ();
        ~StyledTextIOWriterSinkStream_Memory ();

    private: // prevent accidental copying
        StyledTextIOWriterSinkStream_Memory (const StyledTextIOWriterSinkStream_Memory&) = delete;
        void operator= (const StyledTextIOWriterSinkStream_Memory&)                      = delete;

    public:
        virtual size_t current_offset () const override;
        virtual void   seek_to (size_t to) override;
        virtual void   write (const void* buffer, size_t bytes) override;

        nonvirtual const void* PeekAtData () const;
        nonvirtual size_t      GetLength () const;

    private:
        char*  fData;
        size_t fBytesUsed;
        size_t fBytesAllocated;
        char*  fCurPtr;
    };

#if qStroika_Frameworks_Led_SupportGDI
    /*
    @CLASS:         EmbeddingSinkStream
    @BASES:         @'SimpleEmbeddedObjectStyleMarker::SinkStream'
    @DESCRIPTION:
    */
    class EmbeddingSinkStream : public SimpleEmbeddedObjectStyleMarker::SinkStream {
    public:
        EmbeddingSinkStream (StyledTextIOWriter::SinkStream& realSinkStream);

        virtual void write (const void* buffer, size_t bytes) override;

    private:
        StyledTextIOWriter::SinkStream& fRealSinkStream;
    };
#endif

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */
    // class StyledTextIOReader::SrcStreamSeekSaver
    inline StyledTextIOReader::SrcStreamSeekSaver::SrcStreamSeekSaver (SrcStream& srcStream)
        : fSrcStream (srcStream)
        , fSavedPos (srcStream.current_offset ())
    {
    }
    inline StyledTextIOReader::SrcStreamSeekSaver::~SrcStreamSeekSaver ()
    {
        try {
            fSrcStream.seek_to (fSavedPos);
        }
        catch (...) {
            // ignore errors here cuz throwing out of DTORs appears to cause havoc with
            // MWERKS runtime?? Is it a MWERKS bug? Or mine - or has this been fixed?
            // LGP 960906
        }
    }

    // class StyledTextIOReader
    inline StyledTextIOReader::StyledTextIOReader (SrcStream* srcStream, SinkStream* sinkStream, const shared_ptr<BadInputHandler>& badInputHander)
        : fSrcStream (*srcStream)
        , fSinkStream (sinkStream)
        , fBadInputHandler (badInputHander)
    {
        RequireNotNull (srcStream);
        if (fBadInputHandler.get () == nullptr) {
            fBadInputHandler = make_shared<BadInputHandler> ();
        }
    }
    inline StyledTextIOReader::SrcStream& StyledTextIOReader::GetSrcStream () const
    {
        return fSrcStream;
    }
    inline StyledTextIOReader::SinkStream& StyledTextIOReader::GetSinkStream () const
    {
        EnsureNotNull (fSinkStream);
        return *fSinkStream;
    }
    /*
    @METHOD:        StyledTextIOReader::GetBadInputHandler
    @DESCRIPTION:   <p>Each reader class has associated with it an error handler - of type @'StyledTextIOReader::BadInputHandler'. This is used
                to handle syntactic or logical errors in the input. By default - this class is simple
                @'StyledTextIOReader::BadInputHandler'.</p>
                    <p>See also @'StyledTextIOReader::SetBadInputHandler' and @'StyledTextIOReader::HandleBadlyFormattedInput'.</p>
    */
    inline shared_ptr<StyledTextIOReader::BadInputHandler> StyledTextIOReader::GetBadInputHandler () const
    {
        Ensure (fBadInputHandler.get () != nullptr);
        return fBadInputHandler;
    }
    /*
    @METHOD:        StyledTextIOReader::SetBadInputHandler
    @DESCRIPTION:   <p>See @'StyledTextIOReader::GetBadInputHandler'</p>
    */
    inline void StyledTextIOReader::SetBadInputHandler (const shared_ptr<BadInputHandler>& badInputHandler)
    {
        fBadInputHandler = badInputHandler;
        if (fBadInputHandler == nullptr) {
            fBadInputHandler = make_shared<BadInputHandler> ();
        }
    }
    /*
    @METHOD:        StyledTextIOReader::HandleBadlyFormattedInput
    @DESCRIPTION:   <p>This routine is called whenever this is badly formatted input text to the reader.
        This is a simple wrapper on the owned @'StyledTextIOReader::BadInputHandler', which can be gotten/set with
        @'StyledTextIOReader::GetBadInputHandler' / @'StyledTextIOReader::SetBadInputHandler'</p>
    */
    inline void StyledTextIOReader::HandleBadlyFormattedInput (bool unrecoverable) const
    {
        GetBadInputHandler ()->HandleBadlyFormattedInput (*this, unrecoverable);
    }
    /*
    @METHOD:        StyledTextIOReader::PutBackLastChar
    @DESCRIPTION:   <p>Unread the last read character. Note - this can be done as many times as you want (allowing infinite unread)
                but it is a bug/error if you ever unread characters that handn't been read in the first place</p>
    */
    inline void StyledTextIOReader::PutBackLastChar () const
    {
        Require (fSrcStream.current_offset () > 0);
        fSrcStream.seek_to (fSrcStream.current_offset () - 1);
    }
    inline char StyledTextIOReader::GetNextChar () const
    {
        //char  c   =   '\0';
        char c; // Better to leave uninitialized for performance reasons - LGP 2003-03-17
        if (fSrcStream.read1 (&c) == 1) {
            return c;
        }
        else {
            throw ReadEOFException ();
            Assert (false);
            return 0; // NOT REACHED
        }
    }
    inline char StyledTextIOReader::PeekNextChar () const
    {
        //char  c   =   '\0';
        char c; // Better to leave uninitialized for performance reasons - LGP 2003-03-17
        if (fSrcStream.read1 (&c) == 1) {
            PutBackLastChar ();
            return c;
        }
        else {
            throw ReadEOFException ();
            AssertNotReached ();
            return 0; // NOT REACHED
        }
    }
    inline void StyledTextIOReader::ConsumeNextChar () const
    {
        (void)GetNextChar ();
    }

    // class StyledTextIOWriter
    inline StyledTextIOWriter::StyledTextIOWriter (SrcStream* srcStream, SinkStream* sinkStream)
        : fSrcStream (srcStream)
        , fSinkStream (sinkStream)
    {
        RequireNotNull (srcStream);
        RequireNotNull (sinkStream);
    }
    inline StyledTextIOWriter::SrcStream& StyledTextIOWriter::GetSrcStream () const
    {
        EnsureNotNull (fSrcStream);
        return *fSrcStream;
    }
    inline StyledTextIOWriter::SinkStream& StyledTextIOWriter::GetSinkStream () const
    {
        EnsureNotNull (fSinkStream);
        return *fSinkStream;
    }

    // class StyledTextIOReader::SinkStream
    /*
    @METHOD:        StyledTextIOReader::SinkStream::GetCountOfTCharsInserted
    @DESCRIPTION:
    */
    inline size_t StyledTextIOReader::SinkStream::GetCountOfTCharsInserted () const
    {
        return current_offset ();
    }

    // class StyledTextIOWriter::SrcStream::Table::CellInfo
    inline StyledTextIOWriter::SrcStream::Table::CellInfo::CellInfo ()
        : f_cellx (TWIPS (0))
        , f_clcbpat (Color::kWhite)
    {
    }

    // class StyledTextIOWriterSinkStream_Memory
    inline const void* StyledTextIOWriterSinkStream_Memory::PeekAtData () const
    {
        return fData;
    }
    inline size_t StyledTextIOWriterSinkStream_Memory::GetLength () const
    {
        return fBytesUsed;
    }

}

#endif /*_Stroika_Frameworks_Led_StyledTextIO_h_*/
