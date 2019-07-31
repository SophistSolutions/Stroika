/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_Platform_MFC_WordProcessor_h_
#define _Stroika_Frameworks_Led_Platform_MFC_WordProcessor_h_ 1

#include "../../../Foundation/StroikaPreComp.h"

/*
@MODULE:    Led_MFC_WordProcessor
@DESCRIPTION:
        <p>Some stuff which applies only to use of @'Led_MFC' along with @'WordProcessor'
    classes (e.g. OLE embeddings)</p>.
 */

static_assert (qHasFeature_ATLMFC, "Error: Stroika::Framework::Led::Platform MFC_WordProcessor code requires the ATLMFC feature to be set true");

#pragma warning(push)
#pragma warning(disable : 5054)
#include <afxole.h>
#pragma warning (pop)

#include <afxwin.h>
#include <oleidl.h>

#include <set>

#include "../StyledTextIO/StyledTextIO_RTF.h"
#include "../WordProcessor.h"
#include "MFC.h"

namespace Stroika::Frameworks::Led::Platform {

    /*
 **************** MFC/WordProcessor specific Configuration variables **************
 */

/*
    @CONFIGVAR:     qSupportOLEControlEmbedding
    @DESCRIPTION:   <p>You may want to shut this off for building an OLE control itself. Or for building applications that
        don't want to include the OLE libaries.</p>
            <p>Turn ON by default
        */
#ifndef qSupportOLEControlEmbedding
#define qSupportOLEControlEmbedding 1
#endif

#if qSupportOLEControlEmbedding
    class Led_MFC_ControlItem;
#endif

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250) // inherits via dominance warning (Stroika::Frameworks::Led::WordProcessor::Stroika::Frameworks::Led::WordProcessor::HookLosingTextStore)
#endif
    /*
    @CLASS:         WordProcessorCommonCommandHelper_MFC<BASECLASS>
    @BASES:         BASECLASS
    @DESCRIPTION:   <p>BASECLASS is generally @'WordProcessor'.
                </p>
    */
    template <typename BASECLASS>
    class WordProcessorCommonCommandHelper_MFC : public BASECLASS {
    private:
        using inherited = BASECLASS;

    public:
        WordProcessorCommonCommandHelper_MFC ();

#if qSupportOLEControlEmbedding
    protected:
        afx_msg void OnPaint ();

    protected:
        /*
        @CLASS:         WordProcessorCommonCommandHelper_MFC<BASECLASS>::ControlItemContextInternalizer
        @BASES:         @'WordProcessor::WordProcessorFlavorPackageInternalizer'
        @DESCRIPTION:   <p>Add call to @'Led_MFC_ControlItem::DocContextDefiner' so that we can instantiate OLE embeddings using the
            MFC mechanism.</p>
                <p>Note - as of Led 3.0d6, and MSVC60SP3, we had to declare this here cuz I couldn't get it compiling when nested locally
            within the MakeDefaultInternalizer () function - which would probably have been better.</p>
        */
        class ControlItemContextInternalizer : public WordProcessor::WordProcessorFlavorPackageInternalizer {
        private:
            using inherited = WordProcessor::WordProcessorFlavorPackageInternalizer;

        public:
            ControlItemContextInternalizer (COleDocument* doc,
                                            TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase,
                                            const WordProcessor::ParagraphDatabasePtr&   paragraphDatabase,
                                            const WordProcessor::HidableTextDatabasePtr& hiddenTextDatabase)
                : FlavorPackageInternalizer (ts)
                , inherited (ts, styleDatabase, paragraphDatabase, hiddenTextDatabase)
                , fDocument (doc)
            {
            }

        public:
            virtual bool InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage, size_t from, size_t to) override
            {
                Led_MFC_ControlItem::DocContextDefiner tmp (fDocument);
                return inherited::InternalizeBestFlavor (flavorPackage, from, to);
            }

        protected:
            COleDocument* fDocument;
        };

    protected:
        virtual shared_ptr<FlavorPackageInternalizer> MakeDefaultInternalizer () override;
#endif
    public:
        virtual Led_SDK_String GetPrettyTypeName (SimpleEmbeddedObjectStyleMarker* m) override;

    protected:
        DECLARE_MESSAGE_MAP ()

    protected:
#if qSupportOLEControlEmbedding
        friend class Led_MFC_ControlItem;
#endif
    };
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

#if qSupportOLEControlEmbedding
    /*
    @CLASS:         Led_MFC_ControlItem
    @BASES:         COleClientItem, @'SimpleEmbeddedObjectStyleMarker'
    @DESCRIPTION:
            <p>A Led embedding object for embedded OLE controls. Useful for applications which
        want to allow for embedded OLE objects. (these docs could really use embellishing!).</p>
            <p>NB: This also supports the embedding format @'RTFIO::RTFOLEEmbedding::kEmbeddingTag'.</p>
    */
    class Led_MFC_ControlItem : public COleClientItem, public SimpleEmbeddedObjectStyleMarker, public StyledTextIO::RTFIO::RTFOLEEmbedding {
    public:
        static const Led_ClipFormat          kClipFormat;
        static const Led_PrivateEmbeddingTag kEmbeddingTag;

        // Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
        //  IMPLEMENT_SERIALIZE requires the class have a constructor with
        //  zero arguments.  Normally, OLE items are constructed with a
        //  non-NULL document pointer.
        Led_MFC_ControlItem (COleDocument* pContainer = NULL);
        ~Led_MFC_ControlItem ();

    public:
        struct DocContextDefiner;
        static SimpleEmbeddedObjectStyleMarker* mkLed_MFC_ControlItemStyleMarker (const char* embeddingTag, const void* data, size_t len);
        static SimpleEmbeddedObjectStyleMarker* mkLed_MFC_ControlItemStyleMarker (ReaderFlavorPackage& flavorPackage);

    protected:
        static SimpleEmbeddedObjectStyleMarker* mkLed_MFC_ControlItemStyleMarker_ (const char* embeddingTag, const void* data, size_t len, Led_MFC_ControlItem* builtItem);
        static SimpleEmbeddedObjectStyleMarker* mkLed_MFC_ControlItemStyleMarker_ (ReaderFlavorPackage& flavorPackage, Led_MFC_ControlItem* builtItem);

    public:
        nonvirtual COleDocument& GetDocument () const;
        nonvirtual Led_MFC& GetActiveView () const;

    public:
        virtual void OnChange (OLE_NOTIFICATION wNotification, DWORD dwParam) override;
        virtual void OnActivate () override;
        virtual BOOL DoVerb (LONG nVerb, CView* pView, LPMSG lpMsg = NULL) override;

    protected:
        virtual void OnGetItemPosition (CRect& rPosition) override;
        virtual void OnDeactivateUI (BOOL bUndoable) override;
        virtual BOOL OnChangeItemPosition (const CRect& rectPos) override;

    public:
        virtual void         DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                                          Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn) override;
        virtual void         MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                  const Led_tChar* text,
                                                  Led_Distance*    distanceResults) const override;
        virtual Led_Distance MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;
        virtual void         DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) noexcept override;
        virtual bool         HandleOpen () override;

    public:
        virtual vector<PrivateCmdNumber> GetCmdNumbers () const override;
        virtual bool                     IsCmdEnabled (PrivateCmdNumber cmd) const override;

    private:
        Led_Size fSize;

        // Support RTFIO::RTFOLEEmbedding API
    public:
        virtual void           PostCreateSpecifyExtraInfo (Led_TWIPS_Point size) override;
        virtual Led_SDK_String GetObjClassName () const override;
        virtual void           DoWriteToOLE1Stream (size_t* nBytes, byte** resultData) override;
        virtual Led_Size       GetSize () override;

    public:
        virtual const char* GetTag () const override;
        virtual void        Write (SinkStream& sink) override;
        virtual void        ExternalizeFlavors (WriterFlavorPackage& flavorPackage) override;

        virtual void Serialize (CArchive& ar) override;

    private:
        DECLARE_SERIAL (Led_MFC_ControlItem)
    };

    /*
     *  Because this mkLed_MFC_ControlItemStyleMarker () gets called from so many levels
     *  removed from anything that knows about MFC/COleDocuments, and since building
     *  control items really requires we know the document (before we can call
     *  serialize), we need some sort of hack to communicate this info from that code
     *  that knows, to this produdure.
     *
     *  The hack chosen, is the venerable global variable. But we do this hack with
     *  a little but of control/circumspection. We use a tmp stack-based class which defines
     *  a context, and methods which might indirectly call this must create one of these
     *  little objects on the stack which will have the side-effect of setting/restoring 
     *  the global variable. We don't support nested calls (cuz no need, would be easy).
     */
    struct Led_MFC_ControlItem::DocContextDefiner {
    public:
        DocContextDefiner (COleDocument* doc);
        ~DocContextDefiner ();

    public:
        static COleDocument* GetDoc ();

    private:
        static COleDocument* sDoc;

    private:
        COleDocument* fOldDoc;

    public:
        static set<HWND> sWindowsWhichHadDisplaySuppressed;
    };
#endif

    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */
    //  class   WordProcessorCommonCommandHelper_MFC<BASECLASS>
    template <typename BASECLASS>
    inline WordProcessorCommonCommandHelper_MFC<BASECLASS>::WordProcessorCommonCommandHelper_MFC ()
        : inherited ()
    {
    }
#if qSupportOLEControlEmbedding
    template <typename BASECLASS>
    void WordProcessorCommonCommandHelper_MFC<BASECLASS>::OnPaint ()
    {
        if (Led_MFC_ControlItem::DocContextDefiner::GetDoc () != NULL) {
            // suppress display but create CPaintDC object to eat update event (and accumulate window to re-inval just below)
            CPaintDC dc (this);
            Led_MFC_ControlItem::DocContextDefiner::sWindowsWhichHadDisplaySuppressed.insert (GetHWND ());
            return;
        }
        inherited::OnPaint ();
    }
    template <typename BASECLASS>
    shared_ptr<FlavorPackageInternalizer> WordProcessorCommonCommandHelper_MFC<BASECLASS>::MakeDefaultInternalizer ()
    {
        return make_shared<ControlItemContextInternalizer> (dynamic_cast<COleDocument*> (this->GetDocument ()), this->GetTextStore (), this->GetStyleDatabase (), this->GetParagraphDatabase (), this->GetHidableTextDatabase ());
    }
#endif
    template <typename BASECLASS>
    Led_SDK_String WordProcessorCommonCommandHelper_MFC<BASECLASS>::GetPrettyTypeName (SimpleEmbeddedObjectStyleMarker* m)
    {
#if qSupportOLEControlEmbedding
        if (dynamic_cast<Led_MFC_ControlItem*> (m) != NULL) {
            return Led_SDK_TCHAROF ("OLE embedding");
        }
#endif
        return inherited::GetPrettyTypeName (m);
    }
    template <typename BASECLASS>
    const AFX_MSGMAP* WordProcessorCommonCommandHelper_MFC<BASECLASS>::GetMessageMap () const
    {
        return GetThisMessageMap ();
    }
    template <typename BASECLASS>
    const AFX_MSGMAP* PASCAL WordProcessorCommonCommandHelper_MFC<BASECLASS>::GetThisMessageMap ()
    {
        using ThisClass    = WordProcessorCommonCommandHelper_MFC<BASECLASS>;
        using TheBaseClass = BASECLASS;

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
// warning C4407: cast between different pointer to member representations, compiler may generate incorrect code
#pragma warning(push)
#pragma warning(disable : 4407) // Not sure this is safe to ignore but I think it is due to qMFCRequiresCWndLeftmostBaseClass
#endif
        static const AFX_MSGMAP_ENTRY _messageEntries[] = {
            ON_WM_PAINT ()

                LED_MFC_HANDLE_COMMAND_M (WordProcessor::kSelectTableIntraCellAll_CmdID)
                    LED_MFC_HANDLE_COMMAND_M (WordProcessor::kSelectTableCell_CmdID)
                        LED_MFC_HANDLE_COMMAND_M (WordProcessor::kSelectTableRow_CmdID)
                            LED_MFC_HANDLE_COMMAND_M (WordProcessor::kSelectTableColumn_CmdID)
                                LED_MFC_HANDLE_COMMAND_M (WordProcessor::kSelectTable_CmdID)
                                    LED_MFC_HANDLE_COMMAND_M (WordProcessor::kFontStylePlain_CmdID)
                                        LED_MFC_HANDLE_COMMAND_M (WordProcessor::kFontStyleBold_CmdID)
                                            LED_MFC_HANDLE_COMMAND_M (WordProcessor::kFontStyleItalic_CmdID)
                                                LED_MFC_HANDLE_COMMAND_M (WordProcessor::kFontStyleUnderline_CmdID)
                                                    LED_MFC_HANDLE_COMMAND_M (WordProcessor::kFontStyleStrikeout_CmdID)
                                                        LED_MFC_HANDLE_COMMAND_M (WordProcessor::kSubScriptCommand_CmdID)
                                                            LED_MFC_HANDLE_COMMAND_M (WordProcessor::kSuperScriptCommand_CmdID)
                                                                LED_MFC_HANDLE_COMMAND_M (WordProcessor::kChooseFontCommand_CmdID)
                                                                    LED_MFC_HANDLE_COMMAND_RANGE_M (WordProcessor::kBaseFontSize_CmdID, WordProcessor::kLastFontSize_CmdID)
                                                                        LED_MFC_HANDLE_COMMAND_RANGE_M (WordProcessor::kFontMenuFirst_CmdID, WordProcessor::kFontMenuLast_CmdID)
                                                                            LED_MFC_HANDLE_COMMAND_RANGE_M (WordProcessor::kBaseFontColor_CmdID, WordProcessor::kLastFontColor_CmdID)
                                                                                LED_MFC_HANDLE_COMMAND_RANGE_M (WordProcessor::kHideSelection_CmdID, WordProcessor::kUnHideSelection_CmdID)
                                                                                    LED_MFC_HANDLE_COMMAND_RANGE_M (WordProcessor::kFirstJustification_CmdID, WordProcessor::kLastJustification_CmdID)
                                                                                        LED_MFC_HANDLE_COMMAND_M (WordProcessor::kParagraphSpacingCommand_CmdID)
                                                                                            LED_MFC_HANDLE_COMMAND_M (WordProcessor::kParagraphIndentsCommand_CmdID)
                                                                                                LED_MFC_HANDLE_COMMAND_RANGE_M (WordProcessor::kFirstListStyle_CmdID, WordProcessor::kLastListStyle_CmdID)
                                                                                                    LED_MFC_HANDLE_COMMAND_M (WordProcessor::kIncreaseIndent_CmdID)
                                                                                                        LED_MFC_HANDLE_COMMAND_M (WordProcessor::kDecreaseIndent_CmdID)
                                                                                                            LED_MFC_HANDLE_COMMAND_RANGE_M (WordProcessor::kFirstShowHideGlyph_CmdID, WordProcessor::kLastShowHideGlyph_CmdID)
                                                                                                                LED_MFC_HANDLE_COMMAND_M (WordProcessor::kInsertTable_CmdID)
                                                                                                                    LED_MFC_HANDLE_COMMAND_M (WordProcessor::kInsertTableRowAbove_CmdID)
                                                                                                                        LED_MFC_HANDLE_COMMAND_M (WordProcessor::kInsertTableRowBelow_CmdID)
                                                                                                                            LED_MFC_HANDLE_COMMAND_M (WordProcessor::kInsertTableColBefore_CmdID)
                                                                                                                                LED_MFC_HANDLE_COMMAND_M (WordProcessor::kInsertTableColAfter_CmdID)
                                                                                                                                    LED_MFC_HANDLE_COMMAND_M (WordProcessor::kRemoveTableRows_CmdID)
                                                                                                                                        LED_MFC_HANDLE_COMMAND_M (WordProcessor::kRemoveTableColumns_CmdID)
                                                                                                                                            LED_MFC_HANDLE_COMMAND_M (WordProcessor::kInsertURL_CmdID)
                                                                                                                                                LED_MFC_HANDLE_COMMAND_M (WordProcessor::kInsertSymbol_CmdID)
                                                                                                                                                    LED_MFC_HANDLE_COMMAND_RANGE_M (WordProcessor::kFirstSelectedEmbedding_CmdID, WordProcessor::kLastSelectedEmbedding_CmdID)

                                                                                                                                                        {0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0}};
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif
        static const AFX_MSGMAP messageMap = {&TheBaseClass::GetThisMessageMap, &_messageEntries[0]};
        return &messageMap;
    }

#if qSupportOLEControlEmbedding
//class Led_MFC_ControlItem
#if !qDebug
    inline COleDocument& Led_MFC_ControlItem::GetDocument () const
    {
        // See debug version for ensures...
        return *(COleDocument*)COleClientItem::GetDocument ();
    }
    inline Led_MFC& Led_MFC_ControlItem::GetActiveView () const
    {
        // See debug version for ensures...
        return *(Led_MFC*)COleClientItem::GetActiveView ();
    }
#endif

    //class Led_MFC_ControlItem::DocContextDefiner
    inline Led_MFC_ControlItem::DocContextDefiner::DocContextDefiner (COleDocument* doc)
        : fOldDoc (sDoc)
    {
        RequireNotNull (doc);
        sDoc = doc;
    }
    inline Led_MFC_ControlItem::DocContextDefiner::~DocContextDefiner ()
    {
        AssertNotNull (sDoc);
        sDoc = fOldDoc;
    }
    inline COleDocument* Led_MFC_ControlItem::DocContextDefiner::GetDoc ()
    {
        return sDoc;
    }
#endif
}

#endif /*_Stroika_Frameworks_Led_Platform_MFC_WordProcessor_h_*/
