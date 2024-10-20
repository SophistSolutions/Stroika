/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_StyledTextIO_HTML_h_
#define _Stroika_Frameworks_Led_StyledTextIO_HTML_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Frameworks/Led/StyledTextIO/StyledTextIO.h"

/*
@MODULE:    StyledTextIO_HTML
@DESCRIPTION:
        <p>Subclasses of @'StyledTextIOReader' and @'StyledTextIOWriter' are where the knowledge of particular file formats resides.
    For example, the knowledge of how to read HTML is in @'StyledTextIOReader_HTML' and the knowledge of how to write HTML is in
    @'StyledTextIOWriter_HTML'.</p>
 */

namespace Stroika::Frameworks::Led::StyledTextIO {

/*
    @CONFIGVAR:     qThrowAwayMostUnknownHTMLTags
    @DESCRIPTION:   <p>By default, on reading, we throw away or interpret loosely most tags. This makes the reader
                very lossy, but produces the most human-readable result. For now control which behavior
                you want via this compile time flag. -- LGP 961015</p>
        */
#ifndef qThrowAwayMostUnknownHTMLTags
#define qThrowAwayMostUnknownHTMLTags 1
#endif

/*
    @CONFIGVAR:     qWriteOutMostHTMLEntitiesByName
    @DESCRIPTION:   <p>By default, off - cuz that works more compatably with many older web browser (such as Netscape 4.x).
                And writing them by number is slightly faster.</p>
        */
#ifndef qWriteOutMostHTMLEntitiesByName
#define qWriteOutMostHTMLEntitiesByName 0
#endif

    /*
    @CLASS:         HTMLInfo
    @DESCRIPTION:
    */
    class HTMLInfo {
    public:
        HTMLInfo () = default;

    public:
        struct EntityRefMapEntry {
            EntityRefMapEntry (const string& entityRefName, wchar_t charValue);

            string  fEntityRefName;
            wchar_t fCharValue;
        };

        static EntityRefMapEntry sDefaultEntityRefMapTable[];
        static const size_t      kDefaultEntityRefMapTable_Count;

        // HTMLFontSizes are numbers from 1..7, and the default/normal is 3.
        // Eventually, these could become virtual methods, and be hooked into stylesheets.
    public:
        static FontSpecification::FontSize HTMLFontSizeToRealFontSize (int size);
        static int                         RealFontSizeToHTMLFontSize (FontSpecification::FontSize size);

    public:
        string         fDocTypeTag;
        string         fHTMLTag;
        string         fHeadTag;
        string         fStartBodyTag;
        vector<string> fUnknownHeaderTags;
        Led_tString    fTitle;
    };

    /*
    @CLASS:         StyledTextIOReader_HTML
    @BASES:         @'StyledTextIOReader'
    @DESCRIPTION:
    */
    class StyledTextIOReader_HTML : public StyledTextIOReader {
    public:
        StyledTextIOReader_HTML (SrcStream* srcStream, SinkStream* sinkStream, HTMLInfo* saveHTMLInfoInto = nullptr);

    public:
        virtual void Read () override;
        virtual bool QuickLookAppearsToBeRightFormat () override;

    public:
        using EntityRefMapEntry = HTMLInfo::EntityRefMapEntry;

    protected:
        virtual const vector<EntityRefMapEntry>& GetEntityRefMapTable () const;

    protected:
        enum ThingyType {
            eEntityRef,
            eTag,
            eEOF,
            eBangComment
        };
        nonvirtual ThingyType ScanTilNextHTMLThingy ();
        nonvirtual void       ScanTilAfterHTMLThingy (ThingyType thingy);

    protected:
        nonvirtual bool LookingAt (const char* text) const;

    protected:
        nonvirtual Led_tString MapInputTextToTString (const string& text);

    protected:
        nonvirtual void EmitText (const Led_tString& text, bool skipNLCheck = false);
        virtual void    EmitText (const Led_tChar* text, size_t nBytes, bool skipNLCheck = false);
        nonvirtual void HandleHTMLThingy (ThingyType thingy, const string& text);
        virtual void    HandleHTMLThingy (ThingyType thingy, const char* text, size_t nBytes);

    protected:
        virtual void HandleHTMLThingy_EntityReference (const char* text, size_t nBytes);
        virtual void HandleHTMLThingy_Tag (const char* text, size_t nBytes);

    protected:
        nonvirtual void ExtractHTMLTagIntoTagNameBuf (const char* text, size_t nBytes, char* tagBuf, size_t tagBufSize, bool* isStartTag);

    protected:
        nonvirtual IncrementalFontSpecification ExtractFontSpecFromCSSStyleAttribute (const char* text, size_t nBytes);
        nonvirtual void                         ApplyCSSStyleAttributeToCurrentFontStack (const char* text, size_t nBytes);
        nonvirtual void                         GrabAndApplyCSSStyleFromTagText (const char* text, size_t nBytes);

    protected:
        virtual void HandleHTMLThingyTag_BANG_doctype (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_a (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_b (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_basefont (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_big (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_blockquote (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_br (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_body (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_code (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_comment (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_dir (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_div (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_em (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_font (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_head (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_html (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_hr (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_hN (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_i (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_img (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_li (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_listing (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_ol (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_p (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_plaintext (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_pre (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_s (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_samp (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_small (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_span (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_strike (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_strong (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_sub (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_sup (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_table (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_td (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_th (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_title (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_tr (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_tt (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_u (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_ul (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_var (bool start, const char* text, size_t nBytes);
        virtual void HandleHTMLThingyTag_xmp (bool start, const char* text, size_t nBytes);

        virtual void HandleHTMLThingyTagUnknown (bool start, const char* text, size_t nBytes);

    protected:
        nonvirtual void BasicFontStackOperation (bool start);
        nonvirtual void EmitForcedLineBreak ();

    protected:
        nonvirtual bool ParseHTMLTagArgOut (const string& tagText, const string& attrName, string* attrValue);
        nonvirtual bool ParseCSSTagArgOut (const string& text, const string& attrName, string* attrValue);

    protected:
        nonvirtual void StartPara ();
        nonvirtual void EndParaIfOpen ();

    private:
        bool fInAPara{false};

        // Implement the quirky font/size rules (1..7) HTML prescribes
    protected:
        virtual void SetHTMLFontSize (int to);

    protected:
        int fHTMLBaseFontSize{3};
        int fHTMLFontSize{3};

    protected:
        HTMLInfo*                 fSaveHTMLInfoInto{nullptr};
        bool                      fReadingBody{false};
        vector<FontSpecification> fFontStack;
        bool                      fComingTextIsTitle{false};
        bool                      fNormalizeInputWhitespace{true};
        bool                      fLastCharSpace{true};
        bool                      fHiddenTextMode{false};
        Led_tString               fHiddenTextAccumulation;
        size_t                    fCurAHRefStart{size_t (-1)};
        string                    fCurAHRefText;
        unsigned int              fULNestingCount{0};
        bool                      fLIOpen{false};
        unsigned int              fTableOpenCount{0};
        bool                      fTableRowOpen{false};
        bool                      fTableCellOpen{false};
    };

    /*
    @CLASS:         StyledTextIOWriter_HTML
    @BASES:         @'StyledTextIOWriter'
    @DESCRIPTION:
    */
    class StyledTextIOWriter_HTML : public StyledTextIOWriter {
    public:
        StyledTextIOWriter_HTML (SrcStream* srcStream, SinkStream* sinkStream, const HTMLInfo* getHTMLInfoFrom = nullptr);
        ~StyledTextIOWriter_HTML ();

    public:
        virtual void Write () override;

    public:
        using EntityRefMapEntry = HTMLInfo::EntityRefMapEntry;

    protected:
        virtual const vector<EntityRefMapEntry>& GetEntityRefMapTable () const;

    public:
        using Table = StyledTextIOWriter::SrcStream::Table;

    protected:
        class WriterContext;

    protected:
        nonvirtual void   WriteHeader (WriterContext& /*writerContext*/);
        nonvirtual void   WriteBody (WriterContext& writerContext);
        nonvirtual void   WriteInnerBody (WriterContext& writerContext);
        nonvirtual void   WriteBodyCharacter (WriterContext& writerContext, Led_tChar c);
        nonvirtual void   WriteTable (WriterContext& writerContext, Table* table);
        nonvirtual void   WriteOpenTag (WriterContext& writerContext, const string& tagName, const string& tagExtras = string{});
        nonvirtual void   WriteOpenTagSpecial (WriterContext& writerContext, const string& tagName, const string& tagFullText);
        nonvirtual void   WriteCloseTag (WriterContext& writerContext, const string& tagName);
        nonvirtual void   WriteOpenCloseTag (WriterContext& writerContext, const string& tagName, const string& tagExtras = string{});
        nonvirtual bool   IsTagOnStack (WriterContext& writerContext, const string& tagName);
        nonvirtual void   EmitBodyFontInfoChange (WriterContext& writerContext, const FontSpecification& newOne, bool skipDoingOpenTags);
        nonvirtual void   AssureStyleRunSummaryBuilt (WriterContext& writerContext);
        nonvirtual string MapOutputTextFromWString (const wstring& text);
        nonvirtual string MapOutputTextFromTString (const Led_tString& text);

    protected:
        const HTMLInfo*                 fGetHTMLInfoFrom;
        vector<StyledInfoSummaryRecord> fStyleRunSummary;
        Led_tChar                       fSoftLineBreakChar;
    };

    class StyledTextIOWriter_HTML::WriterContext {
    public:
        WriterContext (StyledTextIOWriter_HTML& writer);
        WriterContext (WriterContext& parentContext, SrcStream& srcStream);

    public:
        nonvirtual StyledTextIOWriter_HTML& GetWriter () const;

    private:
        StyledTextIOWriter_HTML& fWriter;

    public:
        nonvirtual SrcStream&  GetSrcStream () const;
        nonvirtual SinkStream& GetSinkStream () const;

    private:
        StyledTextIOWriter::SrcStream& fSrcStream;

    public:
        StyledInfoSummaryRecord fLastEmittedISR;
        size_t                  fLastStyleChangeAt;
        size_t                  fIthStyleRun;
        size_t                  fLastForcedNLAt;
        bool                    fEmittedStartOfPara;
        bool                    fEmittingList;
        bool                    fEmittingListItem;
        vector<string>          fTagStack;
        bool                    fInTableCell;

    public:
        nonvirtual size_t GetCurSrcOffset () const;
#if qStroika_Frameworks_Led_SupportGDI
        nonvirtual SimpleEmbeddedObjectStyleMarker* GetCurSimpleEmbeddedObjectStyleMarker () const;
#endif
    };

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */

    /*
     ********************************************************************************
     ***************************** HTMLInfo::EntityRefMapEntry ***************************
     ********************************************************************************
     */
    inline HTMLInfo::EntityRefMapEntry::EntityRefMapEntry (const string& entityRefName, wchar_t charValue)
        : fEntityRefName (entityRefName)
        , fCharValue (charValue)
    {
    }

    /*
     ********************************************************************************
     ***************************** StyledTextIOReader_HTML ***************************
     ********************************************************************************
     */
    inline void StyledTextIOReader_HTML::EmitText (const Led_tString& text, bool skipNLCheck)
    {
        EmitText (text.c_str (), text.length (), skipNLCheck);
    }
    inline void StyledTextIOReader_HTML::HandleHTMLThingy (StyledTextIOReader_HTML::ThingyType thingy, const string& text)
    {
        HandleHTMLThingy (thingy, text.c_str (), text.length ());
    }

    /*
     ********************************************************************************
     ********************** StyledTextIOWriter_HTML::WriterContext ***************************
     ********************************************************************************
     */
    inline StyledTextIOWriter_HTML::WriterContext::WriterContext (StyledTextIOWriter_HTML& writer)
        : fWriter (writer)
        , fSrcStream (fWriter.GetSrcStream ())
        , fLastEmittedISR (IncrementalFontSpecification (), 0)
        , fLastStyleChangeAt (0)
        , fIthStyleRun (0)
        , fLastForcedNLAt (0)
        , fEmittedStartOfPara (false)
        , fEmittingList (false)
        , fEmittingListItem (false)
        , fTagStack ()
        , fInTableCell (false)
    {
    }
    inline StyledTextIOWriter_HTML::WriterContext::WriterContext (WriterContext& parentContext, SrcStream& srcStream)
        : fWriter (parentContext.fWriter)
        , fSrcStream (srcStream)
        , fLastEmittedISR (IncrementalFontSpecification (), 0)
        , fLastStyleChangeAt (0)
        , fIthStyleRun (0)
        , fLastForcedNLAt (0)
        , fEmittedStartOfPara (false)
        , fEmittingList (false)
        , fEmittingListItem (false)
        , fTagStack ()
        , fInTableCell (true)
    {
    }
    inline StyledTextIOWriter_HTML& StyledTextIOWriter_HTML::WriterContext::GetWriter () const
    {
        return fWriter;
    }
    inline StyledTextIOWriter::SrcStream& StyledTextIOWriter_HTML::WriterContext::GetSrcStream () const
    {
        return fSrcStream;
    }
    inline StyledTextIOWriter::SinkStream& StyledTextIOWriter_HTML::WriterContext::GetSinkStream () const
    {
        return fWriter.GetSinkStream ();
    }

}

#endif /*_Stroika_Frameworks_Led_StyledTextIO_HTML_h_*/
