/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_Platform_MFC_WordProcessor_h_
#define _Stroika_Frameworks_Led_Platform_MFC_WordProcessor_h_ 1

#include    "../../../Foundation/StroikaPreComp.h"

/*
@MODULE:    Led_MFC_WordProcessor
@DESCRIPTION:
        <p>Some stuff which applies only to use of @'Led_MFC' along with @'WordProcessor'
    classes (e.g. OLE embeddings)</p>.
 */



#if     _MSC_VER == 1200
//A bit of a hack for MSVC60, cuz this needs to be done before including <set> - otherwise we get
// lots of needless warnigns - regardless of what is done later -- LGP 980925
#pragma warning (4 : 4786)
#endif

#include    <afxwin.h>
#include    <afxole.h>
#include    <oleidl.h>

#include    <set>

#include    "MFC.h"
#include    "../StyledTextIO_RTF.h"
#include    "../WordProcessor.h"



#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4250)
#endif



namespace   Stroika {
    namespace   Frameworks {
        namespace   Led {
            namespace   Platform {








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
#define qSupportOLEControlEmbedding                             1
#endif










#if     qSupportOLEControlEmbedding
                class   Led_MFC_ControlItem;
#endif






                /*
                @CLASS:         WordProcessorCommonCommandHelper_MFC<BASECLASS>
                @BASES:         BASECLASS
                @DESCRIPTION:   <p>BASECLASS is generally @'WordProcessor'.
                            </p>
                                <p>To use this class, you must also
                            define DoDeclare_WordProcessorCommonCommandHelper_MFC_MessageMap() to declare the
                            actual message map for the template.</p>
                */
                template    <typename   BASECLASS>  class   WordProcessorCommonCommandHelper_MFC :
                    public BASECLASS {
                private:
                    typedef BASECLASS   inherited;

                public:
                    WordProcessorCommonCommandHelper_MFC ();


#if     qSupportOLEControlEmbedding
                protected:
                    afx_msg void    OnPaint ();

                protected:
                    /*
                    @CLASS:         WordProcessorCommonCommandHelper_MFC<BASECLASS>::ControlItemContextInternalizer
                    @BASES:         @'WordProcessor::WordProcessorFlavorPackageInternalizer'
                    @DESCRIPTION:   <p>Add call to @'Led_MFC_ControlItem::DocContextDefiner' so that we can instantiate OLE embeddings using the
                        MFC mechanism.</p>
                            <p>Note - as of Led 3.0d6, and MSVC60SP3, we had to declare this here cuz I couldn't get it compiling when nested locally
                        within the MakeDefaultInternalizer () function - which would probably have been better.</p>
                    */
                    class   ControlItemContextInternalizer : public WordProcessor::WordProcessorFlavorPackageInternalizer {
                    private:
                        typedef WordProcessor::WordProcessorFlavorPackageInternalizer   inherited;
                    public:
                        ControlItemContextInternalizer (COleDocument* doc,
                                                        TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase,
                                                        const WordProcessor::ParagraphDatabasePtr& paragraphDatabase,
                                                        const WordProcessor::HidableTextDatabasePtr& hiddenTextDatabase
                                                       ):
                            FlavorPackageInternalizer (ts),
                            inherited (ts, styleDatabase, paragraphDatabase, hiddenTextDatabase),
                            fDocument (doc) {
                        }
                    public:
                        override    bool    InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage, size_t from, size_t to) {
                            Led_MFC_ControlItem::DocContextDefiner tmp (fDocument);
                            return inherited::InternalizeBestFlavor (flavorPackage, from, to);
                        }
                    protected:
                        COleDocument*   fDocument;
                    };
                protected:
                    override    shared_ptr<FlavorPackageInternalizer>   MakeDefaultInternalizer ();
#endif
                public:
                    override        Led_SDK_String      GetPrettyTypeName (SimpleEmbeddedObjectStyleMarker* m);

                protected:
                    DECLARE_MESSAGE_MAP ()

                protected:
#if     qSupportOLEControlEmbedding
                    friend  class   Led_MFC_ControlItem;
#endif
                };







#if     qSupportOLEControlEmbedding
                /*
                @CLASS:         Led_MFC_ControlItem
                @BASES:         COleClientItem, @'SimpleEmbeddedObjectStyleMarker'
                @DESCRIPTION:
                        <p>A Led embedding object for embedded OLE controls. Useful for applications which
                    want to allow for embedded OLE objects. (these docs could really use embellishing!).</p>
                        <p>NB: This also supports the embedding format @'RTFIO::RTFOLEEmbedding::kEmbeddingTag'.</p>
                */
                class   Led_MFC_ControlItem : public COleClientItem, public SimpleEmbeddedObjectStyleMarker, public RTFIO::RTFOLEEmbedding  {
                public:
                    static  const   Led_ClipFormat          kClipFormat;
                    static  const   Led_PrivateEmbeddingTag kEmbeddingTag;

                    // Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
                    //  IMPLEMENT_SERIALIZE requires the class have a constructor with
                    //  zero arguments.  Normally, OLE items are constructed with a
                    //  non-NULL document pointer.
                    Led_MFC_ControlItem (COleDocument* pContainer = NULL);
                    ~Led_MFC_ControlItem ();

                public:
                    struct  DocContextDefiner;
                    static  SimpleEmbeddedObjectStyleMarker*    mkLed_MFC_ControlItemStyleMarker (const char* embeddingTag, const void* data, size_t len);
                    static  SimpleEmbeddedObjectStyleMarker*    mkLed_MFC_ControlItemStyleMarker (ReaderFlavorPackage& flavorPackage);
                protected:
                    static  SimpleEmbeddedObjectStyleMarker*    mkLed_MFC_ControlItemStyleMarker_ (const char* embeddingTag, const void* data, size_t len, Led_MFC_ControlItem* builtItem);
                    static  SimpleEmbeddedObjectStyleMarker*    mkLed_MFC_ControlItemStyleMarker_ (ReaderFlavorPackage& flavorPackage, Led_MFC_ControlItem* builtItem);

                public:
                    nonvirtual  COleDocument&   GetDocument () const;
                    nonvirtual  Led_MFC&        GetActiveView () const;

                public:
                    override    void    OnChange (OLE_NOTIFICATION wNotification, DWORD dwParam);
                    override    void    OnActivate ();
                    override    BOOL    DoVerb (LONG nVerb, CView* pView, LPMSG lpMsg = NULL);

                protected:
                    override    void    OnGetItemPosition (CRect& rPosition);
                    override    void    OnDeactivateUI (BOOL bUndoable);
                    override    BOOL    OnChangeItemPosition (const CRect& rectPos);

                public:
                    override    void            DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
                            size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                            Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
                                                            );
                    override    void            MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                            const Led_tChar* text,
                            Led_Distance* distanceResults
                                                                    ) const;
                    override    Led_Distance    MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const;
                    override    void            DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo);
                    override    bool            HandleOpen ();

                public:
                    override    vector<PrivateCmdNumber>    GetCmdNumbers () const;
                    override    bool                        IsCmdEnabled (PrivateCmdNumber cmd) const;

                private:
                    Led_Size    fSize;

                    // Support RTFIO::RTFOLEEmbedding API
                public:
                    override    void            PostCreateSpecifyExtraInfo (Led_TWIPS_Point size);
                    override    Led_SDK_String  GetObjClassName ()  const;
                    override    void            DoWriteToOLE1Stream (size_t* nBytes, Byte** resultData);
                    override    Led_Size        GetSize ();

                public:
                    override    const char*     GetTag () const;
                    override    void            Write (SinkStream& sink);
                    override    void            ExternalizeFlavors (WriterFlavorPackage& flavorPackage);

                    override    void    Serialize (CArchive& ar);

                private:
                    DECLARE_SERIAL(Led_MFC_ControlItem)
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
                struct  Led_MFC_ControlItem::DocContextDefiner {
                public:
                    DocContextDefiner (COleDocument* doc);
                    ~DocContextDefiner ();
                public:
                    static  COleDocument*   GetDoc ();
                private:
                    static  COleDocument*   sDoc;
                private:
                    COleDocument*   fOldDoc;
                public:
                    static  set<HWND>   sWindowsWhichHadDisplaySupressed;
                };
#endif









                /*
                 ********************************************************************************
                 ***************************** Implementation Details ***************************
                 ********************************************************************************
                 */

//  class   WordProcessorCommonCommandHelper_MFC<BASECLASS>
#define DoDeclare_WordProcessorCommonCommandHelper_MFC_MessageMap(BASECLASS)\
    typedef WordProcessorCommonCommandHelper_MFC<BASECLASS> WordProcessorCommonCommandHelper_MFC ## BASECLASS ## _HackTypeDef;\
    BEGIN_MESSAGE_MAP(WordProcessorCommonCommandHelper_MFC ## BASECLASS ## _HackTypeDef, WordProcessorCommonCommandHelper_MFC ## BASECLASS ## _HackTypeDef::inherited)\
        ON_WM_PAINT()\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kSelectTableIntraCellAll_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kSelectTableCell_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kSelectTableRow_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kSelectTableColumn_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kSelectTable_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kFontStylePlain_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kFontStyleBold_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kFontStyleItalic_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kFontStyleUnderline_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kFontStyleStrikeout_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kSubScriptCommand_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kSuperScriptCommand_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kChooseFontCommand_CmdID)\
        LED_MFC_HANDLE_COMMAND_RANGE_M  (WordProcessor::kBaseFontSize_CmdID,                WordProcessor::kLastFontSize_CmdID)\
        LED_MFC_HANDLE_COMMAND_RANGE_M  (WordProcessor::kFontMenuFirst_CmdID,               WordProcessor::kFontMenuLast_CmdID)\
        LED_MFC_HANDLE_COMMAND_RANGE_M  (WordProcessor::kBaseFontColor_CmdID,               WordProcessor::kLastFontColor_CmdID)\
        LED_MFC_HANDLE_COMMAND_RANGE_M  (WordProcessor::kHideSelection_CmdID,               WordProcessor::kUnHideSelection_CmdID)\
        LED_MFC_HANDLE_COMMAND_RANGE_M  (WordProcessor::kFirstJustification_CmdID,          WordProcessor::kLastJustification_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kParagraphSpacingCommand_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kParagraphIndentsCommand_CmdID)\
        LED_MFC_HANDLE_COMMAND_RANGE_M  (WordProcessor::kFirstListStyle_CmdID,              WordProcessor::kLastListStyle_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kIncreaseIndent_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kDecreaseIndent_CmdID)\
        LED_MFC_HANDLE_COMMAND_RANGE_M  (WordProcessor::kFirstShowHideGlyph_CmdID,          WordProcessor::kLastShowHideGlyph_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kInsertTable_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kInsertTableRowAbove_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kInsertTableRowBelow_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kInsertTableColBefore_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kInsertTableColAfter_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kRemoveTableRows_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kRemoveTableColumns_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kInsertURL_CmdID)\
        LED_MFC_HANDLE_COMMAND_M        (WordProcessor::kInsertSymbol_CmdID)\
        LED_MFC_HANDLE_COMMAND_RANGE_M  (WordProcessor::kFirstSelectedEmbedding_CmdID,      WordProcessor::kLastSelectedEmbedding_CmdID)\
    END_MESSAGE_MAP()

#if     qSupportLed30CompatAPI
                // Obsolete because of SPR#1407 - just call DoDeclare_WordProcessorCommonCommandHelper_MFC_MessageMap
                // directly...
#define DoDeclare_WordProcessorCommonCommandHelper_MFC_MessageMapX(BASECLASS,CMD_INFO)\
    DoDeclare_WordProcessorCommonCommandHelper_MFC_MessageMap (BASECLASS)
#endif

                template    <typename   BASECLASS>
                inline  WordProcessorCommonCommandHelper_MFC<BASECLASS>::WordProcessorCommonCommandHelper_MFC ():
                    inherited ()
                {
                }
#if     qSupportOLEControlEmbedding
                template    <typename   BASECLASS>
                void    WordProcessorCommonCommandHelper_MFC<BASECLASS>::OnPaint ()
                {
                    if (Led_MFC_ControlItem::DocContextDefiner::GetDoc () != NULL) {
                        // supress display but create CPaintDC object to eat update event (and accumulate window to re-inval just below)
                        CPaintDC    dc (this);
                        Led_MFC_ControlItem::DocContextDefiner::sWindowsWhichHadDisplaySupressed.insert (GetHWND ());
                        return;
                    }
                    inherited::OnPaint ();
                }
                template    <typename   BASECLASS>
                shared_ptr<FlavorPackageInternalizer>   WordProcessorCommonCommandHelper_MFC<BASECLASS>::MakeDefaultInternalizer ()
                {
                    return new ControlItemContextInternalizer (dynamic_cast<COleDocument*> (GetDocument ()), GetTextStore (), GetStyleDatabase (), GetParagraphDatabase (), GetHidableTextDatabase ());
                }
#endif
                template    <typename   BASECLASS>
                Led_SDK_String      WordProcessorCommonCommandHelper_MFC<BASECLASS>::GetPrettyTypeName (SimpleEmbeddedObjectStyleMarker* m)
                {
#if     qSupportOLEControlEmbedding
                    if (dynamic_cast<Led_MFC_ControlItem*> (m) != NULL) {
                        return Led_SDK_TCHAROF ("OLE embedding");
                    }
#endif
                    return inherited::GetPrettyTypeName (m);
                }






#if     qSupportOLEControlEmbedding
//class Led_MFC_ControlItem
#if     !qDebug
                inline  COleDocument&   Led_MFC_ControlItem::GetDocument () const
                {
                    // See debug version for ensures...
                    return *(COleDocument*)COleClientItem::GetDocument ();
                }
                inline  Led_MFC&    Led_MFC_ControlItem::GetActiveView () const
                {
                    // See debug version for ensures...
                    return *(Led_MFC*)COleClientItem::GetActiveView ();
                }
#endif


//class Led_MFC_ControlItem::DocContextDefiner
                inline  Led_MFC_ControlItem::DocContextDefiner::DocContextDefiner (COleDocument* doc):
                    fOldDoc (sDoc)
                {
                    RequireNotNull (doc);
                    sDoc = doc;
                }
                inline  Led_MFC_ControlItem::DocContextDefiner::~DocContextDefiner ()
                {
                    AssertNotNull (sDoc);
                    sDoc = fOldDoc;
                }
                inline  COleDocument*   Led_MFC_ControlItem::DocContextDefiner::GetDoc ()
                {
                    return sDoc;
                }
#endif


            }
        }
    }
}

#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (pop)
#endif


#endif  /*_Stroika_Frameworks_Led_Platform_MFC_WordProcessor_h_*/
