/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_WordProcessor_h_
#define _Stroika_Frameworks_Led_WordProcessor_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

/*
@MODULE:    WordProcessor
@DESCRIPTION:
                <p>This module contains all the classes needed to implement a typical
            word-processor UI. These things include per-paragraph tab stops, justification,
            margins, etc. The main class you should be interested in is the @'WordProcessor' class.
            </p>
 */

#include "Stroika/Frameworks/Led/HiddenText.h"
#include "Stroika/Frameworks/Led/IdleManager.h"
#include "Stroika/Frameworks/Led/MarkerCover.h"
#include "Stroika/Frameworks/Led/StandardStyledTextInteractor.h"
#include "Stroika/Frameworks/Led/TextInteractorMixins.h"
#include "Stroika/Frameworks/Led/WordWrappedTextInteractor.h"

namespace Stroika::Frameworks::Led {

    class IncrementalParagraphInfo;
    class WordProcessorTable;

    constexpr inline TWIPS kBadCachedFarthestRightMarginInDocument = TWIPS (-1);

    /**
     *  Currently (as of 2024-02-14) tightly coupled with GDI code, so conditionally disable table support if we dont have GDI support.
     */
#ifndef qStroika_Frameworks_Led_SupportTables
#define qStroika_Frameworks_Led_SupportTables qStroika_Frameworks_Led_SupportGDI
#endif

    /**
     *      An object which captures the per-paragraph information we store especially in the
     *          @'WordProcessor' class. The attributes stored include:
     *              <ul>
     *                  <li>Justification</li>
     *                  <li>TabStopList</li>
     *                  <li>Margin (left and right - NB: we require left < right)</li>
     *                  <li>FirstIdent (nb this is ADDED to the LHS margin, and CAN BE NEGATIVE</li>
     *               </ul>
     */
    class ParagraphInfo {
    public:
        constexpr ParagraphInfo () = default;

    public:
        nonvirtual Justification GetJustification () const;
        nonvirtual void          SetJustification (Justification justification);

    private:
        Justification fJustification{eLeftJustify};

    public:
        nonvirtual const StandardTabStopList& GetTabStopList () const;
        nonvirtual void                       SetTabStopList (const StandardTabStopList& tabStops);

    private:
        StandardTabStopList fTabStops{};

    public:
        nonvirtual TWIPS GetLeftMargin () const;
        nonvirtual TWIPS GetRightMargin () const;
        nonvirtual void  SetMargins (TWIPS lhs, TWIPS rhs);

    private:
        TWIPS fLeftMargin{0};
        TWIPS fRightMargin{1};

    public:
        nonvirtual TWIPS GetFirstIndent () const;
        nonvirtual void  SetFirstIndent (TWIPS firstIndent);

    private:
        TWIPS fFirstIndent{0};

    public:
        nonvirtual TWIPS       GetSpaceBefore () const;
        nonvirtual void        SetSpaceBefore (TWIPS sb);
        nonvirtual TWIPS       GetSpaceAfter () const;
        nonvirtual void        SetSpaceAfter (TWIPS sa);
        nonvirtual LineSpacing GetLineSpacing () const;
        nonvirtual void        SetLineSpacing (LineSpacing sl);

    private:
        TWIPS       fSpaceBefore{0};
        TWIPS       fSpaceAfter{0};
        LineSpacing fLineSpacing{};

    public:
        nonvirtual ListStyle GetListStyle () const;
        nonvirtual void      SetListStyle (ListStyle listStyle);

    private:
        ListStyle fListStyle{eListStyle_None};

    public:
        nonvirtual unsigned char GetListIndentLevel () const;
        nonvirtual void          SetListIndentLevel (unsigned char indentLevel);

    private:
        unsigned char fListIndentLevel{0};

    public:
        nonvirtual void MergeIn (const IncrementalParagraphInfo& incParaInfo);

    public:
        constexpr bool operator== (const ParagraphInfo&) const = default;
    };

    /**
     *      Overload (hide) the @'WordProcessor::ParagraphInfo' methods, to assert that the
     *          attribute is valid (for getters) and to set a ValidFlag for the setters. And add methods for
     *          each attribute to test for validity, and to invalidate.</p>
     *              <p>These are used if you want to set just part of a @'WordProcessor::ParagraphInfo'.</p>
     * 
     *  @todo REDO using:
     *      optioanl of each ParagraphInfo fields
     *      no subclass - instead conversion operator to ParagraphInfo (with defaults for stuff not specified?)
     */
    class IncrementalParagraphInfo : public ParagraphInfo {
    private:
        using inherited = ParagraphInfo;

    public:
        IncrementalParagraphInfo ();
        explicit IncrementalParagraphInfo (const ParagraphInfo& pi);

    public:
        nonvirtual Justification GetJustification () const;
        nonvirtual void          SetJustification (Justification justification);
        nonvirtual bool          GetJustification_Valid () const;
        nonvirtual void          InvalidateJustification ();

    private:
        bool fJustificationValid;

    public:
        nonvirtual const StandardTabStopList& GetTabStopList () const;
        nonvirtual void                       SetTabStopList (const StandardTabStopList& tabStops);
        nonvirtual bool                       GetTabStopList_Valid () const;
        nonvirtual void                       InvalidateTabStopList ();

    private:
        bool fTabStopListValid;

    public:
        nonvirtual TWIPS GetLeftMargin () const;
        nonvirtual TWIPS GetRightMargin () const;
        nonvirtual void  SetMargins (TWIPS lhs, TWIPS rhs);
        nonvirtual bool  GetMargins_Valid () const;
        nonvirtual void  InvalidateMargins ();

    private:
        bool fMarginsValid;

    public:
        nonvirtual TWIPS GetFirstIndent () const;
        nonvirtual void  SetFirstIndent (TWIPS firstIndent);
        nonvirtual bool  GetFirstIndent_Valid () const;
        nonvirtual void  InvalidateFirstIndent ();

    private:
        bool fFirstIndentValid;

    public:
        nonvirtual TWIPS       GetSpaceBefore () const;
        nonvirtual void        SetSpaceBefore (TWIPS sb);
        nonvirtual bool        GetSpaceBefore_Valid () const;
        nonvirtual void        InvalidateSpaceBefore ();
        nonvirtual TWIPS       GetSpaceAfter () const;
        nonvirtual void        SetSpaceAfter (TWIPS sa);
        nonvirtual bool        GetSpaceAfter_Valid () const;
        nonvirtual void        InvalidateSpaceAfter ();
        nonvirtual LineSpacing GetLineSpacing () const;
        nonvirtual void        SetLineSpacing (LineSpacing sl);
        nonvirtual bool        GetLineSpacing_Valid () const;
        nonvirtual void        InvalidateLineSpacing ();

        nonvirtual ListStyle GetListStyle () const;
        nonvirtual void      SetListStyle (ListStyle listStyle);
        nonvirtual bool      GetListStyle_Valid () const;
        nonvirtual void      InvalidateListStyle ();

        nonvirtual unsigned char GetListIndentLevel () const;
        nonvirtual void          SetListIndentLevel (unsigned char indentLevel);
        nonvirtual bool          GetListIndentLevel_Valid () const;
        nonvirtual void          InvalidateListIndentLevel ();

    private:
        bool fSpaceBeforeValid;
        bool fSpaceAfterValid;
        bool fLineSpacingValid;
        bool fListStyleValid;
        bool fListIndentLevelValid;

    public:
        nonvirtual bool operator== (const IncrementalParagraphInfo& rhs) const;
    };

    /**
     */
    class AbstractParagraphDatabaseRep : public virtual MarkerOwner {
    private:
        using inherited = MarkerOwner;

    public:
        AbstractParagraphDatabaseRep ();

    public:
        virtual shared_ptr<Partition> GetPartition () const                                    = 0;
        virtual void                  SetPartition (const shared_ptr<Partition>& partitionPtr) = 0;

    public:
        virtual const ParagraphInfo&                GetParagraphInfo (size_t charAfterPos) const                          = 0;
        virtual vector<pair<ParagraphInfo, size_t>> GetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing) const = 0;

        virtual void SetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalParagraphInfo& infoForMarkers) = 0;
        virtual void SetParagraphInfo (size_t charAfterPos, const vector<pair<IncrementalParagraphInfo, size_t>>& infoForMarkers)    = 0;
        virtual void SetParagraphInfo (size_t charAfterPos, const vector<pair<ParagraphInfo, size_t>>& infoForMarkers)               = 0;

    private:
        bool fSomeInvalidTables;

    protected:
        TWIPS fCachedFarthestRightMarginInDocument;

    private:
        friend class WordProcessor;
        friend class WordProcessorTable;
    };

    /**
     */
    class ParagraphInfoMarker : public Marker {
    public:
        ParagraphInfoMarker (ParagraphInfo paragraphInfo = {});

        nonvirtual const ParagraphInfo& GetInfo () const;
        nonvirtual void                 SetInfo (ParagraphInfo paragraphInfo);

    private:
        ParagraphInfo fParagraphInfo;
    };

    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    /**
     *   <p>A MarkerCover which associates @'WordProcessor::ParagraphInfo' with each paragraph in the next (merging together adjacent
     *          identical ones). Paragraphs are defined by the @'Partition' object associated
     *          (@'WordProcessor::ParagraphDatabaseRep::SetPartition').</p>
     */
    class ParagraphDatabaseRep : public AbstractParagraphDatabaseRep, private MarkerCover<ParagraphInfoMarker, ParagraphInfo, IncrementalParagraphInfo> {
    private:
        using inheritedMC = MarkerCover<ParagraphInfoMarker, ParagraphInfo, IncrementalParagraphInfo>;

    public:
        using PartitionMarker = Partition::PartitionMarker;

    public:
        ParagraphDatabaseRep (TextStore& textStore);

    public:
        virtual shared_ptr<Partition> GetPartition () const override;
        virtual void                  SetPartition (const shared_ptr<Partition>& partitionPtr) override;

    private:
        shared_ptr<Partition> fPartition;

    public:
        static ParagraphInfo GetStaticDefaultParagraphInfo ();

        // override the AbstractParagraphDatabase API
    public:
        virtual const ParagraphInfo&                GetParagraphInfo (size_t charAfterPos) const override;
        virtual vector<pair<ParagraphInfo, size_t>> GetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing) const override;
        virtual void SetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalParagraphInfo& infoForMarkers) override;
        virtual void SetParagraphInfo (size_t charAfterPos, const vector<pair<IncrementalParagraphInfo, size_t>>& infoForMarkers) override;
        virtual void SetParagraphInfo (size_t charAfterPos, const vector<pair<ParagraphInfo, size_t>>& infoForMarkers) override;

    public:
        virtual void SetInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalParagraphInfo& infoForMarkers) override;
        virtual void SetInfos (size_t charAfterPos, const vector<pair<IncrementalParagraphInfo, size_t>>& infoForMarkers) override;

    protected:
        virtual void NoteCoverRangeDirtied (size_t from, size_t to, const MarkerVector& rangeAndSurroundingsMarkers) override;
        virtual void ConstrainSetInfoArgs (size_t* charAfterPos, size_t* nTCharsFollowing) override;

    private:
        nonvirtual void CheckMarkerBounaryConstraints (size_t from, size_t to) noexcept;
        nonvirtual void CheckMarkerBounaryConstraints (const MarkerVector& rangeAndSurroundingsMarkers) noexcept;

#if qDebug
    protected:
        virtual void Invariant_ () const override;
#endif
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning

    /*
    @CLASS:         WordProcessorTable
    @BASES:         @'SimpleEmbeddedObjectStyleMarker'
    @DESCRIPTION:   <p>A table is an object that can be embedded in a document, and it contains multiple cells.
                Each cell is basically another word-processor instance.</p>
                    <p>Since a table is assocated with a paragraph database, and can be viewed and edited
                simultaneously be multiple views (WordProcessors) - we cannot make long-term assumptions
                about association of a table with an owning WordProcessor object. Instead - during UI operations
                like typing and so on - you temporarily associate an owning WP with the given table using
                @'WordProcessorTable::TemporarilySetOwningWP'</p>
                    <p>We support having any number of rows, and each row can have a different number of
                columns (cells). Often however - the number of columns for each row of a table will be the
                same (as will their widths be equal).</p>
                    <p>This class may not remain a subclass of simpleembeddedObject stuff - but maybe.</p>
                    <p>Note that most coordiantes in this routine are presumed to be table relative. Either that - or
                they are window-rect relative. You can use the methods @'WordProcessorTable::TableCoordinates2Window' and
                @'WordProcessorTable::WindowCoordinates2Table' to map between them.
                    </p>
                    <p>Each cell is surrounded by spacing (see @'WordProcessorTable::GetCellSpacing'.
                This defaults to zero. If you set it to non-zero - then the borders drawn for cells don't touch the
                borders for the table itself.</p>
                    <p>The cell bounds for each cell (@'WordProcessorTable::GetCellBounds') is the area that
                gets a box drawn around it (on the outside bounds). These 'cell bounds' may not exactly touch each other on the
                edges by so that they SHARE the border separating each from its sibling. Also - they may not touch because
                of cell spacing (@'WordProcessorTable::GetCellSpacing').</p>
                    <p>The actual 'edit window' within a cell is inset by its cell margins. This value defaults
                to that provided by @'WordProcessorTable::GetDefaultCellMargins', but at some point (maybe for 3.1?)
                will be overridable on a per-cell basis.</p>
    */
    class WordProcessorTable
#if qStroika_Frameworks_Led_SupportGDI
        : public SimpleEmbeddedObjectStyleMarker
#else
        : public StyleMarker
#endif
    {
    private:
#if qStroika_Frameworks_Led_SupportGDI
        using inherited = SimpleEmbeddedObjectStyleMarker;
#else
        using inherited = StyleMarker;
#endif

    public:
        WordProcessorTable (AbstractParagraphDatabaseRep* tableOwner, size_t addAt);
        ~WordProcessorTable ();

#if qStroika_Frameworks_Led_SupportGDI
    public:
        using CursorMovementDirection = TextImager::CursorMovementDirection;
        using CursorMovementUnit      = TextImager::CursorMovementUnit;
        using UpdateMode              = TextInteractor::UpdateMode;
        using CursorMovementAction    = TextInteractor::CursorMovementAction;
        using CommandUpdater          = TextInteractor::CommandUpdater;
        using CommandNumber           = TextInteractor::CommandNumber;
#endif

    protected:
        virtual void FinalizeAddition (AbstractParagraphDatabaseRep* o, size_t addAt);

    public:
        class Cell;

    protected:
        class CellRep;

#if qStroika_Frameworks_Led_SupportGDI
    public:
        virtual void DrawSegment (const StyledTextImager* imager, const StyleRunElement& runElement, Tablet* tablet, size_t from, size_t to,
                                  const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& invalidRect,
                                  CoordinateType useBaseLine, DistanceType* pixelsDrawn) override;
        virtual void MeasureSegmentWidth (const StyledTextImager* imager, const StyleRunElement& runElement, size_t from, size_t to,
                                          const Led_tChar* text, DistanceType* distanceResults) const override;
        virtual DistanceType MeasureSegmentHeight (const StyledTextImager* imager, const StyleRunElement& runElement, size_t from, size_t to) const override;

    public:
        virtual vector<Led_Rect> GetRowHilightRects () const;

    protected:
        virtual void DrawTableBorders (WordProcessor& owningWP, Tablet* tablet, const Led_Rect& drawInto);
        virtual void DrawCellBorders (Tablet* tablet, size_t row, size_t column, const Led_Rect& cellBounds);
#endif

    public:
        nonvirtual TWIPS GetCellSpacing () const;
        nonvirtual void  SetCellSpacing (TWIPS cellSpacing);

    private:
        TWIPS fCellSpacing{TWIPS{0}};

    public:
        nonvirtual void GetDefaultCellMargins (TWIPS* top, TWIPS* left, TWIPS* bottom, TWIPS* right) const;
        nonvirtual void SetDefaultCellMargins (TWIPS top, TWIPS left, TWIPS bottom, TWIPS right);

    private:
        // Not REALLY a rect - just a handy way to store 4 values... and OK since its private - not part of API
        // LHS and RHS both 90 TWIPS (tricky CTOR - last arg is WIDTH - not RHS).
        TWIPS_Rect fDefaultCellMargins{TWIPS (15), TWIPS (90), TWIPS{0}, TWIPS{0}};

#if qStroika_Frameworks_Led_SupportGDI
    public:
        virtual Led_Rect GetCellBounds (size_t row, size_t column) const;
        virtual Led_Rect GetCellEditorBounds (size_t row, size_t column) const;

        virtual void GetClosestCell (const Led_Point& p, size_t* row, size_t* col) const;

    public:
        nonvirtual Led_Point TableCoordinates2Window (const Led_Point& p) const;
        nonvirtual Led_Rect  TableCoordinates2Window (const Led_Rect& r) const;
        nonvirtual Led_Point WindowCoordinates2Table (const Led_Point& p) const;
        nonvirtual Led_Rect  WindowCoordinates2Table (const Led_Rect& r) const;

    public:
        virtual bool     GetCaretShownSituation () const;
        virtual Led_Rect CalculateCaretRect () const;

    public:
        virtual bool OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed,
                                             bool controlPressed, bool altKeyPressed);

    protected:
        virtual bool DoSingleCharCursorEdit (CursorMovementDirection direction, CursorMovementUnit movementUnit,
                                             CursorMovementAction action, UpdateMode updateMode, bool scrollToSelection);

    public:
        virtual bool OnUpdateCommand (CommandUpdater* enabler);
        virtual bool OnPerformCommand (CommandNumber commandNumber);

    protected:
        nonvirtual void BreakInGroupedCommands ();

    protected:
        virtual bool OnUpdateCommand_ApplyToEachSelectedCell (CommandUpdater* enabler);
        virtual bool OnPerformCommand_ApplyToEachSelectedCell (CommandNumber commandNumber, bool captureChangesForUndo = true);

    public:
        virtual void OnUpdateCutCommand (CommandUpdater* pCmdUI);
        virtual void OnCutCommand ();
        virtual void OnUpdateInsertTableRowAboveCommand (CommandUpdater* pCmdUI);
        virtual void OnInsertTableRowAboveCommand ();
        virtual void OnUpdateInsertTableRowBelowCommand (CommandUpdater* pCmdUI);
        virtual void OnInsertTableRowBelowCommand ();
        virtual void OnUpdateInsertTableColBeforeCommand (CommandUpdater* pCmdUI);
        virtual void OnInsertTableColBeforeCommand ();
        virtual void OnUpdateInsertTableColAfterCommand (CommandUpdater* pCmdUI);
        virtual void OnInsertTableColAfterCommand ();
        virtual void OnUpdateRemoveTableRowsCommand (CommandUpdater* pCmdUI);
        virtual void OnRemoveTableRowsCommand ();
        virtual void OnUpdateRemoveTableColumnsCommand (CommandUpdater* pCmdUI);
        virtual void OnRemoveTableColumnsCommand ();
        virtual void OnUpdateSelectTablePartsCommand (CommandUpdater* enabler);
        virtual void OnPerformTablePartsCommand (CommandNumber commandNumber);

    protected:
        nonvirtual void AssureCurSelFontCacheValid (IncrementalFontSpecification* curSelFontSpec);

    public:
        nonvirtual void InteractiveSetFont (const IncrementalFontSpecification& defaultFont);

        // SimpleEmbeddedObjectStyleMarker overrides
    public:
        virtual const char* GetTag () const override;
        virtual void        Write (SinkStream& sink) override;
        virtual void        ExternalizeFlavors (WriterFlavorPackage& flavorPackage) override;

    public:
        virtual bool ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection);
        virtual void WhileSimpleMouseTracking (Led_Point newMousePos);
#endif

    public:
        nonvirtual Color GetTableBorderColor () const;
        nonvirtual void  SetTableBorderColor (Color c);

    public:
        nonvirtual TWIPS GetTableBorderWidth () const;
        nonvirtual void  SetTableBorderWidth (TWIPS w);

    public:
        nonvirtual TWIPS GetColumnWidth (size_t row, size_t column) const;
        nonvirtual void  SetColumnWidth (size_t row, size_t column, TWIPS colWidth);

    public:
        nonvirtual Color GetCellColor (size_t row, size_t column) const;
        nonvirtual void  SetCellColor (size_t row, size_t column, const Color& c);

    public:
        nonvirtual Cell&       GetCell (size_t row, size_t column);
        nonvirtual const Cell& GetCell (size_t row, size_t column) const;
        nonvirtual void        GetRealCell (size_t* row, size_t* column) const;
        nonvirtual const Cell& GetRealCell (size_t row, size_t column) const;

    public:
        enum CellMergeFlags {
            ePlainCell       = 0,
            eMergeCellLeft   = 1,
            eMergeCellUp     = 2,
            eMergeCellLeftUp = eMergeCellLeft + eMergeCellUp,
            eInvalidCell     = 99
        };

    public:
        nonvirtual CellMergeFlags GetCellFlags (size_t row, size_t column) const;
        nonvirtual bool           CanMergeCells (size_t fromRow, size_t fromCol, size_t toRow, size_t toCol);
        nonvirtual void           MergeCells (size_t fromRow, size_t fromCol, size_t toRow, size_t toCol);
        nonvirtual void           UnMergeCells (size_t fromRow, size_t fromCol, size_t toRow, size_t toCol);

    public:
        nonvirtual void GetCellSelection (size_t* rowSelStart, size_t* rowSelEnd, size_t* colSelStart, size_t* colSelEnd) const;
        nonvirtual void SetCellSelection (size_t rowSelStart, size_t rowSelEnd, size_t colSelStart, size_t colSelEnd);

    public:
        nonvirtual bool GetIntraCellMode (size_t* row = nullptr, size_t* col = nullptr) const;
        nonvirtual void SetIntraCellMode ();
        nonvirtual void SetIntraCellMode (size_t row, size_t col);
        nonvirtual void UnSetIntraCellMode ();
        nonvirtual void GetIntraCellSelection (size_t* selStart, size_t* selEnd) const;
        nonvirtual void SetIntraCellSelection (size_t selStart, size_t selEnd);

    private:
        size_t fRowSelStart{0};
        size_t fRowSelEnd{0};
        size_t fColSelStart{0};
        size_t fColSelEnd{0};

    private:
        bool   fIntraCellMode{false};
        size_t fIntraSelStart{0};
        size_t fIntraSelEnd{0};
        size_t fIntraCellDragAnchor{0};

    protected:
        nonvirtual void SaveIntraCellContextInfo (bool leftSideOfSelectionInteresting, const FontSpecification& intraCellSelectionEmptySelFontSpecification);
        nonvirtual bool RestoreIntraCellContextInfo (bool* leftSideOfSelectionInteresting, FontSpecification* intraCellSelectionEmptySelFontSpecification);
        nonvirtual void InvalidateIntraCellContextInfo ();

    private:
        bool              fSavedLeftSideOfSelectionInteresting{false};
        FontSpecification fSavedIntraCellSelectionEmptySelFontSpecification{};
        bool              fSavedIntraCellInfoValid{false};

    private:
        size_t fTrackingAnchor_Row{0};
        size_t fTrackingAnchor_Col{0};

    private:
        class SuppressCellUpdatePropagationContext;
        bool fSuppressCellUpdatePropagationContext{false};

    protected:
        class AllowUpdateInfoPropagationContext;
        bool                      fAllowUpdateInfoPropagationContext;
        TextStore::SimpleUpdater* fCellUpdatePropationUpdater{nullptr};

#if qStroika_Frameworks_Led_SupportGDI
    protected:
        class EmbeddedTableWordProcessor;

    protected:
        virtual EmbeddedTableWordProcessor* ConstructEmbeddedTableWordProcessor (WordProcessor& forWordProcessor, size_t forRow, size_t forColumn,
                                                                                 const Led_Rect& cellWindowRect, bool captureChangesForUndo);
        virtual void ReleaseEmbeddedTableWordProcessor (EmbeddedTableWordProcessor* e);
#endif

    public:
        virtual void GetCellWordProcessorDatabases (size_t row, size_t column, TextStore** ts,
                                                    shared_ptr<AbstractStyleDatabaseRep>*     styleDatabase       = nullptr,
                                                    shared_ptr<AbstractParagraphDatabaseRep>* paragraphDatabase   = nullptr,
                                                    shared_ptr<HidableTextMarkerOwner>*       hidableTextDatabase = nullptr);

#if qStroika_Frameworks_Led_SupportGDI
    private:
        WordProcessor* fCurrentOwningWP{nullptr};
        class TemporarilySetOwningWP;
        class TemporarilyAllocateCellWP;
        class TemporarilyAllocateCellWithTablet;
#endif

    protected:
        nonvirtual void InvalidateLayout ();
#if qStroika_Frameworks_Led_SupportGDI
        virtual void PerformLayout ();
#endif

    private:
        enum LayoutFlag {
            eDone,
            eNeedFullLayout
        };
        mutable LayoutFlag fNeedLayout{eNeedFullLayout};

    public:
        nonvirtual size_t GetRowCount () const;
        nonvirtual size_t GetColumnCount () const;

    public:
        nonvirtual size_t GetColumnCount (size_t row) const;
        nonvirtual size_t GetColumnCount (size_t rowStart, size_t rowEnd) const;
        nonvirtual void   SetColumnCount (size_t row, size_t columns);

    public:
        nonvirtual void GetDimensions (size_t* rows, size_t* columns) const;
        virtual void    SetDimensions (size_t rows, size_t columns);
        nonvirtual void SetDimensionsAtLeast (size_t rows, size_t columns);
        virtual void    InsertRow (size_t at, size_t maxRowCopyCount = 0xffffffff);
        virtual void    DeleteRow (size_t at);
        virtual void    InsertColumn (size_t at);
        virtual void    DeleteColumn (size_t at);

#if qStroika_Frameworks_Led_SupportGDI
    protected:
        virtual void ReValidateSelection ();
#endif

    protected:
        /*
        @CLASS:         WordProcessorTable::RowInfo
        @DESCRIPTION:
            <p></p>
        */
        struct RowInfo {
        public:
            RowInfo () = default;

        public:
            vector<Cell> fCells;

        public:
            DistanceType fHeight{0}; // height of the cell itself (not including the border)
        };
        vector<RowInfo> fRows;

    private:
#if qStroika_Frameworks_Led_SupportGDI
        TWIPS fBorderWidth{Led_CvtScreenPixelsToTWIPSH (1)};
#else
        TWIPS fBorderWidth{1};
#endif
        Color        fBorderColor{Color::kSilver};
        DistanceType fTotalWidth{0};
        DistanceType fTotalHeight{0};

#if qStroika_Frameworks_Led_SupportGDI

    protected:
        class SavedTextRepWSel;

    private:
        class TableCMD;

    private:
        friend class SuppressCellUpdatePropagationContext;
        friend class AllowUpdateInfoPropagationContext;
        friend class TemporarilySetOwningWP;
        friend class TemporarilyAllocateCellWP;
        friend class TemporarilyAllocateCellWithTablet;
        friend class EmbeddedTableWordProcessor;
        friend class CellRep;
        friend class TableCMD;
        //       friend class WordProcessor::WPIdler;
        friend class WordProcessor;
#endif
    };

    /*
    @CLASS:         WordProcessorTable::Cell
    @DESCRIPTION:   <p>Used internally by the @'WordProcessorTable' code.</p>
    */
    class WordProcessorTable::Cell {
    public:
        Cell (WordProcessorTable& forTable, CellMergeFlags mergeFlags);

    public:
        nonvirtual CellMergeFlags GetCellMergeFlags () const;

    private:
        CellMergeFlags fCellMergeFlags;

    public:
        nonvirtual void       GetCellWordProcessorDatabases (TextStore** ts, shared_ptr<AbstractStyleDatabaseRep>* styleDatabase = nullptr,
                                                             shared_ptr<AbstractParagraphDatabaseRep>* paragraphDatabase   = nullptr,
                                                             shared_ptr<HidableTextMarkerOwner>*       hidableTextDatabase = nullptr);
        nonvirtual TextStore& GetTextStore () const;
        nonvirtual shared_ptr<AbstractStyleDatabaseRep> GetStyleDatabase () const;
        nonvirtual shared_ptr<AbstractParagraphDatabaseRep> GetParagraphDatabase () const;
        nonvirtual shared_ptr<HidableTextMarkerOwner> GetHidableTextDatabase () const;

        nonvirtual Color GetBackColor () const;
        nonvirtual void  SetBackColor (Color c);

        nonvirtual TWIPS GetCellXWidth () const;
        nonvirtual void  SetCellXWidth (TWIPS width);

        nonvirtual Led_Rect GetCachedBoundsRect () const;
        nonvirtual void     SetCachedBoundsRect (Led_Rect r);

    public:
        shared_ptr<CellRep> fCellRep;
    };

    /*
    @CLASS:         WordProcessorTable::CellRep
    @DESCRIPTION:   <p>Used internally by the @'WordProcessorTable' code.</p>
    */
    class WordProcessorTable::CellRep : public MarkerOwner, public Foundation::Memory::UseBlockAllocationIfAppropriate<CellRep> {
    private:
        using inherited = MarkerOwner;

    public:
        CellRep (WordProcessorTable& forTable);
        ~CellRep ();

    public:
        virtual TextStore* PeekAtTextStore () const override;
        virtual void       AboutToUpdateText (const UpdateInfo& updateInfo) override;
        virtual void       DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

    public:
        WordProcessorTable&                      fForTable;
        TextStore*                               fTextStore;
        shared_ptr<AbstractStyleDatabaseRep>     fStyleDatabase;
        shared_ptr<AbstractParagraphDatabaseRep> fParagraphDatabase;
        shared_ptr<HidableTextMarkerOwner>       fHidableTextDatabase;
        Color                                    fBackColor;
        Led_Rect                                 fCachedBoundsRect;
        TWIPS                                    fCellXWidth;
    };

    /**
     *      <p>A @'StandardStyledTextInteractor::StandardStyledTextIOSrcStream', for use with the StyledTextIO module,
     *  which adds support for a @'shared_ptr<AbstractParagraphDatabaseRep>'.</p>
     */
    class WordProcessorTextIOSrcStream : public StandardStyledTextIOSrcStream {
    private:
        using inherited = StandardStyledTextIOSrcStream;

    public:
        WordProcessorTextIOSrcStream (TextStore* textStore, const shared_ptr<AbstractStyleDatabaseRep>& textStyleDatabase,
                                      const shared_ptr<AbstractParagraphDatabaseRep>& paragraphDatabase,
                                      const shared_ptr<HidableTextMarkerOwner>& hidableTextDatabase, size_t selectionStart = 0,
                                      size_t selectionEnd = kBadIndex);
#if qStroika_Frameworks_Led_SupportGDI
        WordProcessorTextIOSrcStream (WordProcessor* textImager, size_t selectionStart = 0, size_t selectionEnd = kBadIndex);
#endif

    public:
        nonvirtual bool GetUseTableSelection () const;
        nonvirtual void SetUseTableSelection (bool useTableSelection);

    private:
        bool fUseTableSelection{false};

    public:
        virtual Justification          GetJustification () const override;
        virtual StandardTabStopList    GetStandardTabStopList () const override;
        virtual TWIPS                  GetFirstIndent () const override;
        virtual void                   GetMargins (TWIPS* lhs, TWIPS* rhs) const override;
        virtual TWIPS                  GetSpaceBefore () const override;
        virtual TWIPS                  GetSpaceAfter () const override;
        virtual LineSpacing            GetLineSpacing () const override;
        virtual void                   GetListStyleInfo (ListStyle* listStyle, unsigned char* indentLevel) const override;
        virtual Led_tChar              GetSoftLineBreakCharacter () const override;
        virtual DiscontiguousRun<bool> GetHidableTextRuns () const override;
#if qStroika_Frameworks_Led_SupportTables
        virtual SrcStream::Table* GetTableAt (size_t at) const override;
#endif
        virtual void SummarizeFontAndColorTable (set<SDKString>* fontNames, set<Color>* colorsUsed) const override;

    protected:
        class TableIOMapper;

    private:
        shared_ptr<AbstractParagraphDatabaseRep> fParagraphDatabase;
        DiscontiguousRun<bool>                   fHidableTextRuns;
    };

    /**
     */
    class WordProcessorTextIOSrcStream::TableIOMapper : public StyledTextIO::StyledTextIOWriter::SrcStream::Table {
    private:
        using inherited = StyledTextIO::StyledTextIOWriter::SrcStream::Table;

    public:
        TableIOMapper (WordProcessorTable& realTable, size_t startRow = 0, size_t endRow = static_cast<size_t> (-1), size_t startCol = 0,
                       size_t endCol = static_cast<size_t> (-1));

    public:
        virtual size_t                                       GetRows () const override;
        virtual size_t                                       GetColumns (size_t row) const override;
        virtual void                                         GetRowInfo (size_t row, vector<CellInfo>* cellInfos) override;
        virtual StyledTextIO::StyledTextIOWriter::SrcStream* MakeCellSubSrcStream (size_t row, size_t column) override;
        virtual size_t                                       GetOffsetEnd () const override;
        virtual TWIPS_Rect                                   GetDefaultCellMarginsForRow (size_t row) const override;
        virtual TWIPS_Rect                                   GetDefaultCellSpacingForRow (size_t row) const override;

    private:
        WordProcessorTable& fRealTable;
        size_t              fStartRow;
        size_t              fEndRow;
        size_t              fStartCol;
        size_t              fEndCol;
    };

    /**
     *      <p>A @'StandardStyledTextInteractor::StandardStyledTextIOSinkStream', for use with the StyledTextIO module,
     *  which adds support for a @'WordProcessor::shared_ptr<AbstractParagraphDatabaseRep>'.</p>
     */
    class WordProcessorTextIOSinkStream : public StandardStyledTextIOSinkStream {
    private:
        using inherited = StandardStyledTextIOSinkStream;

    public:
        WordProcessorTextIOSinkStream (TextStore* textStore, const shared_ptr<AbstractStyleDatabaseRep>& textStyleDatabase,
                                       const shared_ptr<AbstractParagraphDatabaseRep>& paragraphDatabase,
                                       const shared_ptr<HidableTextMarkerOwner>& hidableTextDatabase, size_t insertionStart = 0);
#if qStroika_Frameworks_Led_SupportGDI
        WordProcessorTextIOSinkStream (WordProcessor* wp, size_t insertionStart = 0);
#endif
        ~WordProcessorTextIOSinkStream ();

    private:
        nonvirtual void CTOR_COMMON ();

    public:
        nonvirtual bool GetOverwriteTableMode () const;
        nonvirtual void SetOverwriteTableMode (bool overwriteTableMode);

    private:
        bool fOverwriteTableMode;

#if !qStroika_Frameworks_Led_NestedTablesSupported
    public:
        nonvirtual bool GetNoTablesAllowed () const;
        nonvirtual void SetNoTablesAllowed (bool noTablesAllowed);

    private:
        bool fNoTablesAllowed;
#endif

    public:
        virtual void AppendText (const Led_tChar* text, size_t nTChars, const FontSpecification* fontSpec) override;
#if qStroika_Frameworks_Led_SupportGDI
        virtual void AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding) override;
#endif
        virtual void AppendSoftLineBreak () override;
        virtual void SetJustification (Justification justification) override;
        virtual void SetStandardTabStopList (const StandardTabStopList& tabStops) override;
        virtual void SetFirstIndent (TWIPS tx) override;
        virtual void SetLeftMargin (TWIPS lhs) override;
        virtual void SetRightMargin (TWIPS rhs) override;
        virtual void SetSpaceBefore (TWIPS sb) override;
        virtual void SetSpaceAfter (TWIPS sa) override;
        virtual void SetLineSpacing (LineSpacing sl) override;
        virtual void SetTextHidden (bool hidden) override;
        virtual void StartTable () override;
        virtual void EndTable () override;
        virtual void StartTableRow () override;
        virtual void EndTableRow () override;
        virtual void StartTableCell (size_t colSpan) override;
        virtual void EndTableCell () override;
        virtual void SetListStyle (ListStyle listStyle) override;
        virtual void SetListIndentLevel (unsigned char indentLevel) override;
        virtual void SetTableBorderColor (Color c) override;
        virtual void SetTableBorderWidth (TWIPS bWidth) override;
        virtual void SetCellWidths (const vector<TWIPS>& cellWidths) override;
        virtual void SetCellBackColor (const Color c) override;
        virtual void SetDefaultCellMarginsForCurrentRow (TWIPS top, TWIPS left, TWIPS bottom, TWIPS right) override;
        virtual void SetDefaultCellSpacingForCurrentRow (TWIPS top, TWIPS left, TWIPS bottom, TWIPS right) override;
        virtual void EndOfBuffer () override;
        virtual void Flush () override;

    public:
        nonvirtual void SetIgnoreLastParaAttributes (bool ignoreLastParaAttributes);

    protected:
        nonvirtual void PushContext (TextStore* ts, const shared_ptr<AbstractStyleDatabaseRep>& textStyleDatabase,
                                     const shared_ptr<AbstractParagraphDatabaseRep>& paragraphDatabase,
                                     const shared_ptr<HidableTextMarkerOwner>& hidableTextDatabase, size_t insertionStart);
        nonvirtual void PopContext ();

    private:
        struct Context {
            shared_ptr<AbstractParagraphDatabaseRep> fParagraphDatabase;
            shared_ptr<HidableTextMarkerOwner>       fHidableTextDatabase;
        };
        vector<Context> fSavedContexts;

    private:
        using ParaInfoNSize = pair<IncrementalParagraphInfo, size_t>;
        shared_ptr<AbstractParagraphDatabaseRep> fParagraphDatabase;
        shared_ptr<HidableTextMarkerOwner>       fHidableTextDatabase;
        vector<ParaInfoNSize>                    fSavedParaInfo;
        IncrementalParagraphInfo                 fNewParagraphInfo;
        bool                                     fTextHidden;
        DiscontiguousRun<bool>                   fHidableTextRuns;
        bool                                     fEndOfBuffer;
        bool                                     fIgnoreLastParaAttributes;
        WordProcessorTable*                      fCurrentTable;
        vector<TWIPS>                            fCurrentTableCellWidths;
        Color                                    fCurrentTableCellColor;
        vector<size_t>                           fCurrentTableColSpanArray;
        vector<WordProcessorTable*>              fTableStack; // for nesting
        size_t                                   fNextTableRow;
        size_t                                   fNextTableCell;
        size_t                                   fCurrentTableCell;

#if qDebug
    private:
        unsigned int fTableOpenLevel;
        bool         fTableRowOpen;
        bool         fTableCellOpen;
#endif
    };

#if qStroika_Frameworks_Led_SupportGDI
    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    /*
    @CLASS:         StandardStyledWordWrappedTextInteractor
    @BASES:         @'InteractorInteractorMixinHelper<INTERACTOR1,INTERACTOR2>', (INTERACTOR1=@'StandardStyledTextInteractor',INTERACTOR2=@'WordWrappedTextInteractor')
    @DESCRIPTION:
            <p>A simple mixin of @'WordWrappedTextInteractor' and @'StandardStyledTextInteractor' using @'InteractorInteractorMixinHelper'.
        Takes care of the nitty-gritty of combining their behaviors.</p>
    */
    class StandardStyledWordWrappedTextInteractor : public InteractorInteractorMixinHelper<StandardStyledTextInteractor, WordWrappedTextInteractor> {
    private:
        using inherited = InteractorInteractorMixinHelper<StandardStyledTextInteractor, WordWrappedTextInteractor>;

    protected:
        StandardStyledWordWrappedTextInteractor () = default;
    };

    /*
    @CLASS:         WordProcessor
    @BASES:         @'StandardStyledWordWrappedTextInteractor'
    @DESCRIPTION:   <p>A fancy word processor. Add high-end editing, more word-processor like features here.
                For example, paragraph formating, justification, margins, remembered per-paragraph tabstops settings, etc.</p>
    */
    class WordProcessor : public StandardStyledWordWrappedTextInteractor {
    private:
        using inherited = StandardStyledWordWrappedTextInteractor;

    protected:
        WordProcessor ();

    public:
        ~WordProcessor ();

    protected:
        virtual void    HookLosingTextStore () override;
        nonvirtual void HookLosingTextStore_ ();
        virtual void    HookGainedNewTextStore () override;
        nonvirtual void HookGainedNewTextStore_ ();

    public:
        nonvirtual bool GetSmartQuoteMode () const;
        nonvirtual void SetSmartQuoteMode (bool smartQuoteMode);

    private:
        bool fSmartQuoteMode;

    public:
        class WordProcessorFlavorPackageInternalizer;
        class WordProcessorFlavorPackageExternalizer;

    public:
        /*
        @CLASS:         WordProcessor::NoParagraphDatabaseAvailable
        @BASES:         @'TextImager::NotFullyInitialized'
        @DESCRIPTION:   <p>Thrown by @'WordProcessor::GetLayoutMargins' etc when no @':shared_ptr<AbstractParagraphDatabaseRep>' available.</p>
        */
        class NoParagraphDatabaseAvailable : public NotFullyInitialized {};

    public:
        class WPPartition;

        virtual shared_ptr<Partition> MakeDefaultPartition () const override;

    public:
        nonvirtual shared_ptr<AbstractParagraphDatabaseRep> GetParagraphDatabase () const;
        nonvirtual void SetParagraphDatabase (const shared_ptr<AbstractParagraphDatabaseRep>& paragraphDatabase);

    private:
        shared_ptr<AbstractParagraphDatabaseRep> fParagraphDatabase;
        bool                                     fICreatedParaDB;

    protected:
        virtual void    HookParagraphDatabaseChanged ();
        nonvirtual void HookParagraphDatabaseChanged_ ();

    public:
        nonvirtual shared_ptr<HidableTextMarkerOwner> GetHidableTextDatabase () const;
        nonvirtual void SetHidableTextDatabase (const shared_ptr<HidableTextMarkerOwner>& hidableTextDatabase);

    private:
        shared_ptr<HidableTextMarkerOwner> fHidableTextDatabase;
        bool                               fICreatedHidableTextDB;

    protected:
        virtual void    HookHidableTextDatabaseChanged ();
        nonvirtual void HookHidableTextDatabaseChanged_ ();

    protected:
        virtual shared_ptr<FlavorPackageInternalizer> MakeDefaultInternalizer () override;
        virtual shared_ptr<FlavorPackageExternalizer> MakeDefaultExternalizer () override;

    protected:
        virtual void HookInternalizerChanged () override;
        virtual void HookExternalizerChanged () override;

    public:
        virtual void InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage, bool updateCursorPosition, bool autoScroll, UpdateMode updateMode) override;
        virtual void ExternalizeFlavors (WriterFlavorPackage& flavorPackage) override;

    public:
        virtual void InterectiveSetRegionHidable (bool hidable);

    public:
        nonvirtual Justification GetJustification (size_t characterPos) const;
        nonvirtual bool          GetJustification (size_t from, size_t to, Justification* justification) const;
        nonvirtual void          SetJustification (size_t from, size_t to, Justification justification);

    public:
        static StandardTabStopList     GetDefaultStandardTabStopList ();
        nonvirtual StandardTabStopList GetStandardTabStopList (size_t characterPos) const;
        nonvirtual bool                GetStandardTabStopList (size_t from, size_t to, StandardTabStopList* tabStops) const;
        nonvirtual void                SetStandardTabStopList (size_t from, size_t to, StandardTabStopList tabStops);

    public:
        nonvirtual void GetMargins (size_t characterPos, TWIPS* leftMargin, TWIPS* rightMargin) const;
        nonvirtual bool GetMargins (size_t from, size_t to, TWIPS* leftMargin, TWIPS* rightMargin) const;
        nonvirtual void SetMargins (size_t from, size_t to, TWIPS leftMargin, TWIPS rightMargin);

    public:
        nonvirtual TWIPS GetFirstIndent (size_t characterPos) const;
        nonvirtual bool  GetFirstIndent (size_t from, size_t to, TWIPS* firstIndent) const;
        nonvirtual void  SetFirstIndent (size_t from, size_t to, TWIPS firstIndent);

    public:
        nonvirtual TWIPS         GetSpaceBefore (size_t characterPos) const;
        nonvirtual bool          GetSpaceBefore (size_t from, size_t to, TWIPS* sb) const;
        nonvirtual void          SetSpaceBefore (size_t from, size_t to, TWIPS sb);
        nonvirtual TWIPS         GetSpaceAfter (size_t characterPos) const;
        nonvirtual bool          GetSpaceAfter (size_t from, size_t to, TWIPS* sa) const;
        nonvirtual void          SetSpaceAfter (size_t from, size_t to, TWIPS sa);
        nonvirtual LineSpacing   GetLineSpacing (size_t characterPos) const;
        nonvirtual bool          GetLineSpacing (size_t from, size_t to, LineSpacing* sl) const;
        nonvirtual void          SetLineSpacing (size_t from, size_t to, LineSpacing sl);
        nonvirtual ListStyle     GetListStyle (size_t characterPos) const;
        nonvirtual bool          GetListStyle (size_t from, size_t to, ListStyle* listStyle) const;
        nonvirtual void          SetListStyle (size_t from, size_t to, ListStyle listStyle, bool autoFormat = false);
        nonvirtual unsigned char GetListIndentLevel (size_t characterPos) const;
        nonvirtual bool          GetListIndentLevel (size_t from, size_t to, unsigned char* indentLevel) const;
        nonvirtual void          SetListIndentLevel (size_t from, size_t to, unsigned char indentLevel, bool autoFormat = false);

    protected:
        virtual Led_tString  GetListLeader (size_t paragraphMarkerPos) const;
        virtual DistanceType GetListLeaderLength (size_t paragraphMarkerPos) const;
        virtual void         AutoFormatIndentedText (size_t from, size_t to);

    private:
        class WPIdler : public Idler {
        public:
            WPIdler ();
            virtual void   SpendIdleTime () override;
            WordProcessor* fWP;
        };
        friend class WPIdler;
        WPIdler fWPIdler;

    public:
        virtual void SetSelection (size_t start, size_t end) override;
        using TextInteractor::SetSelection;

    public:
        virtual bool     GetCaretShownSituation () const override;
        virtual Led_Rect CalculateCaretRect () const override;

    public:
        virtual void OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed,
                                             bool controlPressed, bool altKeyPressed) override;

    public:
        virtual bool ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection, size_t* dragAnchor) override;
        virtual void WhileSimpleMouseTracking (Led_Point newMousePos, size_t dragAnchor) override;

        // WordProcessorTable support
    public:
        virtual WordProcessorTable* InsertTable (size_t at);

        nonvirtual vector<WordProcessorTable*> GetTablesInRange (size_t from, size_t to = static_cast<size_t> (-1)) const;
        nonvirtual WordProcessorTable*         GetTableAt (size_t from) const;

    public:
        nonvirtual WordProcessorTable* GetActiveTable () const;

    public:
        struct CommandNames;

        // This class builds commands with command names. The UI may wish to change these
        // names (eg. to customize for particular languages, etc)
        // Just patch these strings here, and commands will be created with these names.
        // (These names appear in text of undo menu item)
    public:
        static const CommandNames& GetCommandNames ();
        static void                SetCommandNames (const CommandNames& cmdNames);
        static CommandNames        MakeDefaultCommandNames ();

    private:
        static CommandNames sCommandNames;

    public:
        enum {
            kWordProcessorCommand_First = kTextInteractorCommand_Last + 1,

            kSelectTableIntraCellAll_CmdID = kWordProcessorCommand_First,
            kSelectTableCell_CmdID,
            kSelectTableRow_CmdID,
            kSelectTableColumn_CmdID,
            kSelectTable_CmdID,

            kBaseFontSize_CmdID = RoundUpToNearest_CMDNUMs_MACRO (kSelectTable_CmdID + 1, 0x100),
            kFontSize9_CmdID    = kBaseFontSize_CmdID,
            kFontSize10_CmdID,
            kFontSize12_CmdID,
            kFontSize14_CmdID,
            kFontSize18_CmdID,
            kFontSize24_CmdID,
            kFontSize36_CmdID,
            kFontSize48_CmdID,
            kFontSize72_CmdID,
            kFontSizeOther_CmdID,
            kFontSizeSmaller_CmdID,
            kFontSizeLarger_CmdID,
            kLastFontSize_CmdID = kFontSizeLarger_CmdID,

            kBaseFontColor_CmdID  = RoundUpToNearest_CMDNUMs_MACRO (kLastFontSize_CmdID + 1, 0x100),
            kFontColorBlack_CmdID = kBaseFontColor_CmdID,
            kFontColorMaroon_CmdID,
            kFontColorGreen_CmdID,
            kFontColorOlive_CmdID,
            kFontColorNavy_CmdID,
            kFontColorPurple_CmdID,
            kFontColorTeal_CmdID,
            kFontColorGray_CmdID,
            kFontColorSilver_CmdID,
            kFontColorRed_CmdID,
            kFontColorLime_CmdID,
            kFontColorYellow_CmdID,
            kFontColorBlue_CmdID,
            kFontColorFuchsia_CmdID,
            kFontColorAqua_CmdID,
            kFontColorWhite_CmdID,
            kLastNamedFontColor_CmdID = kFontColorWhite_CmdID,
            kFontColorOther_CmdID,
            kLastFontColor_CmdID = kFontColorOther_CmdID,

            kFirstJustification_CmdID = RoundUpToNearest_CMDNUMs_MACRO (kLastFontColor_CmdID + 1, 0x100),
            kJustifyLeft_CmdID        = kFirstJustification_CmdID,
            kJustifyCenter_CmdID,
            kJustifyRight_CmdID,
            kJustifyFull_CmdID,
            kLastJustification_CmdID = kJustifyFull_CmdID,

            kParagraphSpacingCommand_CmdID,
            kParagraphIndentsCommand_CmdID,
            kFirstListStyle_CmdID,
            kListStyle_None_CmdID = kFirstListStyle_CmdID,
            kListStyle_Bullet_CmdID,
            kLastListStyle_CmdID = kListStyle_Bullet_CmdID,
            kIncreaseIndent_CmdID,
            kDecreaseIndent_CmdID,

            kFontMenuFirst_CmdID = RoundUpToNearest_CMDNUMs_MACRO (kDecreaseIndent_CmdID + 1, 0x100),
            kFontMenuLast_CmdID  = kFontMenuFirst_CmdID + 0x400,

            kFontStylePlain_CmdID,
            kFontStyleBold_CmdID,
            kFontStyleItalic_CmdID,
            kFontStyleUnderline_CmdID,
#if qPlatform_Windows
            kFontStyleStrikeout_CmdID,
#endif
            kSubScriptCommand_CmdID,
            kSuperScriptCommand_CmdID,
            kFontStyleCommand_FirstCmdId = kFontStylePlain_CmdID,
            kFontStyleCommand_LastCmdId  = kSuperScriptCommand_CmdID,

            kChooseFontCommand_CmdID,

            kInsertTable_CmdID = RoundUpToNearest_CMDNUMs_MACRO (kChooseFontCommand_CmdID + 1, 0x100),
            kInsertTableRowAbove_CmdID,
            kInsertTableRowBelow_CmdID,
            kInsertTableColBefore_CmdID,
            kInsertTableColAfter_CmdID,
            kInsertURL_CmdID,
            kInsertSymbol_CmdID,

            kRemoveTableColumns_CmdID,
            kRemoveTableRows_CmdID,

            kFirstSelectedEmbedding_CmdID      = RoundUpToNearest_CMDNUMs_MACRO (kRemoveTableRows_CmdID + 1, 0x100),
            kSelectedEmbeddingProperties_CmdID = kFirstSelectedEmbedding_CmdID,
            kFirstPrivateEmbedding_CmdID,
            kLastPrivateEmbedding_CmdID  = kFirstPrivateEmbedding_CmdID + 5,
            kLastSelectedEmbedding_CmdID = kLastPrivateEmbedding_CmdID,

            kHideSelection_CmdID = RoundUpToNearest_CMDNUMs_MACRO (kLastSelectedEmbedding_CmdID + 1, 0x100),
            kUnHideSelection_CmdID,

            kFirstShowHideGlyph_CmdID      = RoundUpToNearest_CMDNUMs_MACRO (kUnHideSelection_CmdID + 1, 0x100),
            kShowHideParagraphGlyphs_CmdID = kFirstShowHideGlyph_CmdID,
            kShowHideTabGlyphs_CmdID,
            kShowHideSpaceGlyphs_CmdID,
            kLastShowHideGlyph_CmdID = kShowHideSpaceGlyphs_CmdID,

            kWordProcessorCommand_Last = RoundUpToNearest_CMDNUMs_MACRO (kShowHideSpaceGlyphs_CmdID + 1, 0x100)
        };

    public:
        class DialogSupport;
        static DialogSupport& GetDialogSupport ();
        static void           SetDialogSupport (DialogSupport* ds);

    private:
        static DialogSupport* sDialogSupport;

    public:
        virtual DistanceType ComputeMaxHScrollPos () const override;

    protected:
        nonvirtual TWIPS CalculateFarthestRightMarginInDocument () const;
        nonvirtual TWIPS GetFarthestRightMarginInDocument () const;
        nonvirtual TWIPS CalculateFarthestRightMarginInWindow () const;
        virtual TWIPS    CalculateFarthestRightMargin () const;

    protected:
        virtual void InvalidateAllCaches () override;
        virtual void TabletChangedMetrics () override;

    public:
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

    public:
        nonvirtual IncrementalFontSpecification GetCurSelFontSpec () const;

    private:
        mutable IncrementalFontSpecification fCachedCurSelFontSpec;
        mutable Justification                fCachedCurSelJustification;
        mutable bool                         fCachedCurSelJustificationUnique;
        mutable bool                         fCachedCurSelFontSpecValid;

        nonvirtual void AssureCurSelFontCacheValid () const;

    protected:
        virtual void DoSingleCharCursorEdit (CursorMovementDirection direction, CursorMovementUnit movementUnit, CursorMovementAction action,
                                             UpdateMode updateMode = eDefaultUpdate, bool scrollToSelection = true) override;

    public:
        virtual bool OnUpdateCommand (CommandUpdater* enabler) override;
        virtual bool OnPerformCommand (CommandNumber commandNumber) override;

    protected:
        virtual bool PassAlongCommandToIntraCellModeTableCell (CommandNumber commandNumber);
        virtual bool PassAlongCommandToEachSelectedTableCell (CommandNumber commandNumber);

    public:
        virtual void OnSelectAllCommand () override;

    public:
        virtual void OnUpdateFontNameChangeCommand (CommandUpdater* enabler);
        virtual void OnFontNameChangeCommand (CommandNumber cmdNum);
        virtual void OnUpdateFontStylePlainCommand (CommandUpdater* enabler);
        virtual void OnFontStylePlainCommand ();
        virtual void OnUpdateFontStyleBoldCommand (CommandUpdater* enabler);
        virtual void OnFontStyleBoldCommand ();
        virtual void OnUpdateFontStyleItalicCommand (CommandUpdater* enabler);
        virtual void OnFontStyleItalicCommand ();
        virtual void OnUpdateFontStyleUnderlineCommand (CommandUpdater* enabler);
        virtual void OnFontStyleUnderlineCommand ();
#if qPlatform_Windows
        virtual void OnUpdateFontStyleStrikeoutCommand (CommandUpdater* enabler);
        virtual void OnFontStyleStrikeoutCommand ();
#endif
        virtual void OnUpdateFontStyleSubscriptCommand (CommandUpdater* enabler);
        virtual void OnFontStyleSubscriptCommand ();
        virtual void OnUpdateFontStyleSuperscriptCommand (CommandUpdater* enabler);
        virtual void OnFontStyleSuperscriptCommand ();
        virtual void OnUpdateChooseFontCommand (CommandUpdater* enabler);
        virtual void OnChooseFontCommand ();
        virtual void OnUpdateFontSizeChangeCommand (CommandUpdater* enabler);
        virtual void OnFontSizeChangeCommand (CommandNumber cmdNum);
        virtual void OnUpdateFontColorChangeCommand (CommandUpdater* enabler);
        virtual void OnFontColorChangeCommand (CommandNumber cmdNum);
        virtual void OnUpdateInsertTableCommand (CommandUpdater* pCmdUI);
        virtual void OnInsertTableCommand ();
        virtual void OnUpdateInsertURLCommand (CommandUpdater* pCmdUI);
        virtual void OnInsertURLCommand ();
        virtual void OnUpdateInsertSymbolCommand (CommandUpdater* pCmdUI);
        virtual void OnInsertSymbolCommand ();
        virtual void OnUpdateSelectedEmbeddingExtendedCommand (CommandUpdater* pCmdUI);
        virtual bool OnSelectedEmbeddingExtendedCommand (CommandNumber cmdNum);

        virtual void OnEditTablePropertiesDialog ();

        virtual void OnUpdateHideSelectionCommands (CommandUpdater* enabler);
        virtual void OnHideSelection ();
        virtual void OnUnHideSelection ();
        virtual void OnUpdateParagraphJustificationCommand (CommandUpdater* enabler);
        virtual void OnParagraphJustificationCommand (CommandNumber cmdNum);
        virtual void OnUpdateParagraphSpacingChangeCommand (CommandUpdater* enabler);
        virtual void OnParagraphSpacingChangeCommand ();
        virtual void OnUpdateParagraphIndentsChangeCommand (CommandUpdater* enabler);
        virtual void OnParagraphIndentsChangeCommand ();
        virtual void OnUpdateListStyleChangeCommand (CommandUpdater* enabler);
        virtual void OnListStyleChangeCommand (CommandNumber cmdNum);
        virtual void OnUpdateIndentCommand (CommandUpdater* enabler);
        virtual void OnIndentCommand (CommandNumber cmdNum);
        virtual void OnUpdateShowHideGlyphCommand (CommandUpdater* enabler);
        virtual void OnShowHideGlyphCommand (CommandNumber cmdNum);

    protected:
        virtual SDKString                           GetPrettyTypeName (SimpleEmbeddedObjectStyleMarker* m);
        nonvirtual SimpleEmbeddedObjectStyleMarker* GetSoleSelectedEmbedding () const;

    public:
        class EmptySelectionParagraphSavedTextRep;

    protected:
        virtual InteractiveReplaceCommand::SavedTextRep* InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd,
                                                                                           size_t selStart, size_t selEnd) override;

    public:
        virtual void InteractiveSetFont (const IncrementalFontSpecification& defaultFont) override;

    public:
        nonvirtual void InteractiveSetJustification (Justification justification);
        nonvirtual void InteractiveSetStandardTabStopList (StandardTabStopList tabStops);
        nonvirtual void InteractiveSetMargins (TWIPS leftMargin, TWIPS rightMargin);
        nonvirtual void InteractiveSetFirstIndent (TWIPS firstIndent);
        nonvirtual void InteractiveSetMarginsAndFirstIndent (TWIPS leftMargin, TWIPS rightMargin, TWIPS firstIndent);
        nonvirtual void InteractiveSetParagraphSpacing (TWIPS spaceBefore, bool spaceBeforeValid, TWIPS spaceAfter, bool spaceAfterValid,
                                                        LineSpacing lineSpacing, bool lineSpacingValid);
        nonvirtual void InteractiveSetListStyle (ListStyle listStyle);
        nonvirtual void InteractiveDoIndentChange (bool increase);

    protected:
        virtual const TabStopList& GetTabStopList (size_t containingPos) const override;

    protected:
        virtual void DrawBefore (const Led_Rect& subsetToDraw, bool printing) override;

    protected:
        virtual void DrawRowSegments (Tablet* tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
                                      const TextLayoutBlock& text, size_t rowStart, size_t rowEnd) override;

    protected:
        virtual vector<Led_Rect> GetRowHilightRects (const TextLayoutBlock& text, size_t rowStart, size_t rowEnd, size_t hilightStart,
                                                     size_t hilightEnd) const override;

    public:
        virtual void DrawSegment (Tablet* tablet, size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto,
                                  const Led_Rect& invalidRect, CoordinateType useBaseLine, DistanceType* pixelsDrawn) override;

    protected:
        virtual DistanceType MeasureSegmentHeight (size_t from, size_t to) const override;
        virtual DistanceType MeasureSegmentBaseLine (size_t from, size_t to) const override;

    private:
        nonvirtual DistanceType MeasureMinSegDescent (size_t from, size_t to) const;

    protected:
        virtual void AdjustBestRowLength (size_t textStart, const Led_tChar* text, const Led_tChar* end, size_t* rowLength) override;

    public:
        nonvirtual bool GetShowParagraphGlyphs () const;
        nonvirtual void SetShowParagraphGlyphs (bool showParagraphGlyphs);

    private:
        bool fShowParagraphGlyphs;

    public:
        nonvirtual bool GetShowTabGlyphs () const;
        nonvirtual void SetShowTabGlyphs (bool showTabGlyphs);

    private:
        bool fShowTabGlyphs;

    public:
        nonvirtual bool GetShowSpaceGlyphs () const;
        nonvirtual void SetShowSpaceGlyphs (bool showSpaceGlyphs);

    private:
        bool fShowSpaceGlyphs;

    public:
        virtual size_t ComputeRelativePosition (size_t fromPos, CursorMovementDirection direction, CursorMovementUnit movementUnit) override;

    protected:
        virtual bool ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const override;
        virtual void ReplaceMappedDisplayCharacters (const Led_tChar* srcText, Led_tChar* copyText, size_t nTChars) const override;

    public:
        virtual Led_Rect GetCharLocationRowRelative (size_t afterPosition, RowReference topRow, size_t maxRowsToCheck) const override;
        virtual size_t   GetCharAtLocationRowRelative (const Led_Point& where, RowReference topRow, size_t maxRowsToCheck) const override;

    protected:
        virtual size_t ResetTabStops (size_t from, const Led_tChar* text, size_t nTChars, DistanceType* charLocations, size_t startSoFar) const override;

    private:
        nonvirtual size_t ResetTabStopsWithMargin (DistanceType lhsMargin, size_t from, const Led_tChar* text, size_t nTChars,
                                                   DistanceType* charLocations, size_t startSoFar) const;

    public:
        virtual void GetLayoutMargins (RowReference row, CoordinateType* lhs, CoordinateType* rhs) const override;

    protected:
        virtual void ExpandedFromAndToInPostReplace (size_t from, size_t newTo, size_t stableTypingRegionStart, size_t stableTypingRegionEnd,
                                                     size_t startPositionOfRowWhereReplaceBegins, size_t startPositionOfRowAfterReplaceEnds,
                                                     size_t* expandedFrom, size_t* expandedTo) override;
        virtual void PostReplace (PreReplaceInfo& preReplaceInfo) override;

    private:
        nonvirtual DistanceType CalcSpaceToEat (size_t rowContainingCharPos) const;

    private:
        friend class AbstractParagraphDatabaseRep;
        friend class ParagraphDatabaseRep;
        friend class WordProcessorTable;
    };

    /*
    @CLASS:         WordProcessor::WordProcessorFlavorPackageInternalizer
    @BASES:         virtual @'StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer'
    @DESCRIPTION:   <p>Add paragraph etc support support by creating a @'WordProcessor::WordProcessorTextIOSinkStream'.</p>
    */
    class WordProcessor::WordProcessorFlavorPackageInternalizer : public StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer {
    private:
        using inherited = StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer;

    public:
        WordProcessorFlavorPackageInternalizer (TextStore& ts, const shared_ptr<AbstractStyleDatabaseRep>& styleDatabase,
                                                const shared_ptr<AbstractParagraphDatabaseRep>& paragraphDatabase,
                                                const shared_ptr<HidableTextMarkerOwner>&       hidableTextDatabase);

    public:
        nonvirtual bool GetOverwriteTableMode () const;
        nonvirtual void SetOverwriteTableMode (bool overwriteTableMode);

    private:
        bool fOverwriteTableMode;

#if !qStroika_Frameworks_Led_NestedTablesSupported
    public:
        nonvirtual bool GetNoTablesAllowed () const;
        nonvirtual void SetNoTablesAllowed (bool noTablesAllowed);

    private:
        bool fNoTablesAllowed;
#endif

    public:
        virtual StandardStyledTextIOSinkStream* mkStandardStyledTextIOSinkStream (size_t insertionStart) override;

    protected:
        shared_ptr<AbstractParagraphDatabaseRep> fParagraphDatabase;
        shared_ptr<HidableTextMarkerOwner>       fHidableTextDatabase;
    };

    /*
    @CLASS:         WordProcessor::WPPartition
    @BASES:         @'LineBasedPartition'
    @DESCRIPTION:   <p>A partition object which respects the funny rules for tables - and makes sure tables appear within their
                own partition element.</p>
    */
    class WordProcessor::WPPartition : public LineBasedPartition {
    public:
        using PartitionMarker = PartitioningTextImager::PartitionMarker;
        using UpdateInfo      = MarkerOwner::UpdateInfo;

    private:
        using inherited = LineBasedPartition;

    public:
        WPPartition (TextStore& textStore, MarkerOwner& tableMarkerOwner);

    protected:
        virtual vector<WordProcessorTable*> GetTablesInRange (size_t from, size_t to = static_cast<size_t> (-1)) const;

    protected:
        virtual void FinalConstruct () override;

    public:
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

    protected:
        nonvirtual void DoHandleUpdateForTableRangeCheck (size_t from, size_t to) noexcept;

    protected:
        virtual bool NeedToCoalesce (PartitionMarker* pm) noexcept override;

#if qDebug
    protected:
        virtual void Invariant_ () const override;
#endif

    private:
        MarkerOwner& fTableMarkerOwner;

    private:
        friend class WordProcessorTable;
    };

    /*
    @CLASS:         WordProcessor::WordProcessorFlavorPackageExternalizer
    @BASES:         virtual @'StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer'
    @DESCRIPTION:   <p>Add paragraph etc support support by creating a @'WordProcessor::WordProcessorTextIOSrcStream'.</p>
    */
    class WordProcessor::WordProcessorFlavorPackageExternalizer : public StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer {
    private:
        using inherited = StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer;

    public:
        WordProcessorFlavorPackageExternalizer (TextStore& ts, const shared_ptr<AbstractStyleDatabaseRep>& styleDatabase,
                                                const shared_ptr<AbstractParagraphDatabaseRep>& paragraphDatabase,
                                                const shared_ptr<HidableTextMarkerOwner>&       hidableTextDatabase);

    public:
        nonvirtual bool GetUseTableSelection () const;
        nonvirtual void SetUseTableSelection (bool useTableSelection);

    private:
        bool fUseTableSelection{false};

    protected:
        virtual StandardStyledTextIOSrcStream* mkStandardStyledTextIOSrcStream (size_t selectionStart, size_t selectionEnd) override;

    protected:
        shared_ptr<AbstractParagraphDatabaseRep> fParagraphDatabase;
        shared_ptr<HidableTextMarkerOwner>       fHidableTextDatabase;
    };

    /*
    @CLASS:         WordProcessorTable::SuppressCellUpdatePropagationContext
    @ACCESS:        private
    @DESCRIPTION:   <p>.</p>
    */
    class WordProcessorTable::SuppressCellUpdatePropagationContext {
    public:
        SuppressCellUpdatePropagationContext (WordProcessorTable& t);
        ~SuppressCellUpdatePropagationContext ();

    private:
        WordProcessorTable& fTable;
        bool                fOldVal;
    };

    /*
    @CLASS:         WordProcessorTable::AllowUpdateInfoPropagationContext
    @ACCESS:        protected
    @DESCRIPTION:   <p>.</p>
    */
    class WordProcessorTable::AllowUpdateInfoPropagationContext {
    public:
        AllowUpdateInfoPropagationContext (WordProcessorTable& t);
        ~AllowUpdateInfoPropagationContext ();

    private:
        WordProcessorTable& fTable;
        bool                fOldVal;
    };

    /*
    @CLASS:         WordProcessorTable::EmbeddedTableWordProcessor
    @BASES:         @'WordProcessor'
    @DESCRIPTION:   <p>Used internally by the @'WordProcessorTable' code for mini embedded word processor objects
                in each cell. A subclass of these are constructed by @'WordProcessor::ConstructEmbeddedTableWordProcessor'</p>
                    <p>Note that we choose to instantiate this WP object with a WindowRect in the same coordinates as
                the the owning tables WindowRect: not relative to that table.
                </p>
    */
    class WordProcessorTable::EmbeddedTableWordProcessor : public WordProcessor {
    private:
        using inherited = WordProcessor;

    public:
        EmbeddedTableWordProcessor (WordProcessor& owningWordProcessor, WordProcessorTable& owningTable, size_t tRow, size_t tCol, bool activeEditCell);

    public:
        nonvirtual WordProcessor&      GetOwningWordProcessor () const;
        nonvirtual WordProcessorTable& GetOwningTable () const;

    private:
        WordProcessor&      fOwningWordProcessor;
        WordProcessorTable& fOwningTable;
        size_t              fTableRow;
        size_t              fTableColumn;

    public:
        nonvirtual void SaveMiscActiveFocusInfo ();
        nonvirtual void RestoreMiscActiveFocusInfo ();

#if !qStroika_Frameworks_Led_NestedTablesSupported
    protected:
        virtual void HookInternalizerChanged () override;
#endif

    protected:
        virtual bool OnCopyCommand_Before () override;
        virtual void OnCopyCommand_After () override;
        virtual bool OnPasteCommand_Before () override;
        virtual void OnPasteCommand_After () override;

    protected:
        virtual void DrawRowHilight (Tablet* tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
                                     const TextLayoutBlock& text, size_t rowStart, size_t rowEnd) override;

        //pure virtual overrides to make this class compile..
    public:
        virtual Tablet* AcquireTablet () const override;
        virtual void    ReleaseTablet (Tablet* tablet) const override;

    protected:
        virtual void RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const override;
        virtual void UpdateWindowRect_ (const Led_Rect& windowRectArea) const override;
        virtual bool QueryInputKeyStrokesPending () const override;

    protected:
        virtual void SetDefaultUpdateMode (UpdateMode defaultUpdateMode) override;

    public:
        virtual void GetLayoutMargins (RowReference row, CoordinateType* lhs, CoordinateType* rhs) const override;

    public:
        nonvirtual DistanceType GetDesiredHeight () const;

    private:
        mutable DistanceType fDesiredHeight;
        mutable bool         fDesiredHeightValid;
        bool                 fActiveEditCell;

    public:
        virtual void PostInteractiveUndoPostHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep,
                                                    InteractiveReplaceCommand::SavedTextRep** afterRep, size_t startOfInsert,
                                                    const SDKString& cmdName) override;

    protected:
        virtual InteractiveReplaceCommand::SavedTextRep* InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd,
                                                                                           size_t selStart, size_t selEnd) override;

    private:
        class DisableRefreshContext;

    private:
        bool fSuppressRefreshCalls;

    private:
        class TemporarilyUseTablet;

    private:
        Tablet* fUpdateTablet; // assigned in stack-based fasion during update/draw calls.
    private:
        friend class WordProcessorTable;
        friend class WordProcessorTable::EmbeddedTableWordProcessor::TemporarilyUseTablet;
        friend class WordProcessorTable::EmbeddedTableWordProcessor::DisableRefreshContext;
        friend class WordProcessorTable::TemporarilyAllocateCellWithTablet;
    };

    /**
     */
    class WordProcessorTable::SavedTextRepWSel : public InteractiveReplaceCommand::SavedTextRep,
                                                 public Foundation::Memory::UseBlockAllocationIfAppropriate<SavedTextRepWSel> {
    private:
        using inherited = InteractiveReplaceCommand::SavedTextRep;

    public:
        enum WPRelativeFlag {
            eWPDirect,
            eWPAbove
        };

    public:
        SavedTextRepWSel (SavedTextRep* delegateTo, WordProcessorTable& table, WPRelativeFlag wPRelativeFlag);

    public:
        virtual size_t GetLength () const override;
        virtual void   InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite) override;
        virtual void   ApplySelection (TextInteractor* imager) override;

    private:
        WPRelativeFlag           fWPRelativeFlag;
        unique_ptr<SavedTextRep> fRealRep;
        size_t                   fRowSelStart{0};
        size_t                   fRowSelEnd{0};
        size_t                   fColSelStart{0};
        size_t                   fColSelEnd{0};
        bool                     fIntraCellMode{false};
        size_t                   fIntraCellSelStart{0};
        size_t                   fIntraCellSelEnd{0};
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning

    /*
    @CLASS:         WordProcessorTable::EmbeddedTableWordProcessor::TemporarilyUseTablet
    @DESCRIPTION:   <p>Utility class to use (with caution), to temporarily force a given tablet to be
                used for a given @'WordProcessorTable::EmbeddedTableWordProcessor'. NB: This causes
                the @'TextImager::TabletChangedMetrics' method by default
                (unless called with special arg).</p>
    */
    class WordProcessorTable::EmbeddedTableWordProcessor::TemporarilyUseTablet {
    public:
        using EmbeddedTableWordProcessor = WordProcessorTable::EmbeddedTableWordProcessor;

    public:
        enum DoTextMetricsChangedCall {
            eDoTextMetricsChangedCall,
            eDontDoTextMetricsChangedCall
        };
        TemporarilyUseTablet (EmbeddedTableWordProcessor& editor, Tablet* t, DoTextMetricsChangedCall tmChanged = eDoTextMetricsChangedCall);
        ~TemporarilyUseTablet ();

    private:
        EmbeddedTableWordProcessor& fEditor;
        Tablet*                     fOldTablet;
        DoTextMetricsChangedCall    fDoTextMetricsChangedCall;
    };

    /*
    @CLASS:         WordProcessorTable::EmbeddedTableWordProcessor::DisableRefreshContext
    @ACCESS:        private
    @DESCRIPTION:   <p>Utility class to use (with caution), to temporarily force a given tablet to be
                used for a given @'WordProcessorTable::EmbeddedTableWordProcessor'. NB: This causes
                the @'TextImager::TabletChangedMetrics' method by default
                (unless called with special arg).</p>
    */
    class WordProcessorTable::EmbeddedTableWordProcessor::DisableRefreshContext {
    public:
        DisableRefreshContext (EmbeddedTableWordProcessor& wp);
        ~DisableRefreshContext ();

    private:
        EmbeddedTableWordProcessor& fWP;
        bool                        fOldVal;
    };

    /*
    @CLASS:         WordProcessorTable::TemporarilySetOwningWP
    @DESCRIPTION:   <p>Since a table is assocated with a paragraph database, and can be viewed and edited
                simultaneously be multiple views (WordProcessors) - we cannot make long-term assumptions
                about association of a table with an owning WordProcessor object. Instead - during UI operations
                like typing and so on - we temporarily associate an owning WP with the given table using
                this class.</p>
    */
    class WordProcessorTable::TemporarilySetOwningWP {
    public:
        TemporarilySetOwningWP (const WordProcessorTable& forTable, WordProcessor& forWordProcessor);
        ~TemporarilySetOwningWP ();

    private:
        WordProcessorTable& fOwningTable;
        WordProcessor*      fSavedTableOwningWP; // in case references are nested, though I'm not sure this can happen
    };

    /**
     */
    class WordProcessorTable::TemporarilyAllocateCellWP {
    public:
        using EmbeddedTableWordProcessor = WordProcessorTable::EmbeddedTableWordProcessor;

    public:
        TemporarilyAllocateCellWP (WordProcessorTable& forTable, WordProcessor& forWordProcessor, size_t forRow, size_t forColumn,
                                   const Led_Rect& cellWindowRect, bool captureChangesForUndo = true);
        ~TemporarilyAllocateCellWP ();

        nonvirtual                             operator EmbeddedTableWordProcessor* ();
        nonvirtual EmbeddedTableWordProcessor& operator& ();
        nonvirtual EmbeddedTableWordProcessor* operator->();

    private:
        WordProcessorTable&         fOwningTable;
        EmbeddedTableWordProcessor* fCellEditor;
    };

    /**
     */
    class WordProcessorTable::TemporarilyAllocateCellWithTablet {
    public:
        using Tablet_Acquirer = TextInteractor::Tablet_Acquirer; // needed for GCC 2.96 - seems like the requirement maybe a compiler bug... LGP 2003-04-18

    public:
        TemporarilyAllocateCellWithTablet (WordProcessorTable& forTable, size_t row, size_t column, bool captureChangesForUndo = true);

    public:
        using TemporarilyUseTablet = EmbeddedTableWordProcessor::TemporarilyUseTablet;

    public:
        nonvirtual                             operator EmbeddedTableWordProcessor* ();
        nonvirtual EmbeddedTableWordProcessor& operator& ();
        nonvirtual EmbeddedTableWordProcessor* operator->();

    private:
        TemporarilyAllocateCellWP fWP;
        Tablet_Acquirer           fTablet;
        TemporarilyUseTablet      fTmpUseTablet;
    };

    /*
    @CLASS:         WordProcessor::CommandNames
    @DESCRIPTION:   <p>Command names for each of the user-visible commands produced by the @'WordProcessor' module.
                This name is used used in the constructed Undo command name, as
                in, "Undo Justification". You can replace this name with whatever you like.
                You change this value with @'WordProcessor::SetCommandNames'.</p>
                    <p> The point of this is to allow for different UI-language localizations,
                without having to change Led itself. Note also that these strings aren't always exactly
                a command-name. Sometimes they are just other strings to be localized.</p>
                    <p>See also @'WordProcessor::GetCommandNames'.</p>
    */
    struct WordProcessor::CommandNames {
        SDKString fJustificationCommandName;
        SDKString fStandardTabStopListCommandName;
        SDKString fMarginsCommandName;
        SDKString fFirstIndentCommandName;
        SDKString fMarginsAndFirstIndentCommandName;
        SDKString fParagraphSpacingCommandName;
        SDKString fHideCommandName;
        SDKString fUnHideCommandName;
        SDKString fSetListStyleCommandName;
        SDKString fIndentLevelChangeCommandName;
        SDKString fInsertTableCommandName;
        SDKString fInsertTableRowAboveCommandName;
        SDKString fInsertTableRowBelowCommandName;
        SDKString fInsertTableColBeforeCommandName;
        SDKString fInsertTableColAfterCommandName;
        SDKString fInsertURLCommandName;
        SDKString fRemoveTableRowsCommandName;
        SDKString fRemoveTableColumnsCommandName;
        SDKString fEmbeddingTypeName_ImageDIB;
        SDKString fEmbeddingTypeName_URL;
        SDKString fEmbeddingTypeName_ImageMacPict;
        SDKString fEmbeddingTypeName_Table;
        SDKString fEmbeddingTypeName_Unknown;
        SDKString fFontSizeChange_Other_NoArg;
        SDKString fFontSizeChange_Other_OneArg;
        SDKString fTablePropertiesCommandName;
        SDKString fGenericEmbeddingPropertiesCommandName;
        SDKString fChangeTablePropertiesCommandName;
    };

    /*
    @CLASS:         WordProcessor::DialogSupport
    @DESCRIPTION:   <p></p>
    */
    class WordProcessor::DialogSupport {
    public:
        using CommandNumber     = TextInteractor::CommandNumber;
        using FontNameSpecifier = FontSpecification::FontNameSpecifier;

    public:
        virtual FontNameSpecifier CmdNumToFontName (CommandNumber cmdNum);
        virtual bool IsPredefinedFontSize (DistanceType fontSize); // return true iff the arg fontSize is one of the predefined ones from the menu
        virtual DistanceType FontCmdToSize (CommandNumber commandNum);
        virtual DistanceType PickOtherFontHeight (DistanceType origHeight); // display UI (dialog box) and query user for new height)
        virtual bool  PickNewParagraphLineSpacing (TWIPS* spaceBefore, bool* spaceBeforeValid, TWIPS* spaceAfter, bool* spaceAfterValid,
                                                   LineSpacing* lineSpacing, bool* lineSpacingValid);
        virtual bool  PickNewParagraphMarginsAndFirstIndent (TWIPS* leftMargin, bool* leftMarginValid, TWIPS* rightMargin,
                                                             bool* rightMarginValid, TWIPS* firstIndent, bool* firstIndentValid);
        virtual Color FontCmdToColor (CommandNumber cmd);
        virtual CommandNumber FontColorToCmd (Color color);
        virtual bool          PickOtherFontColor (Color* color);
        virtual bool          ChooseFont (IncrementalFontSpecification* font); // display UI (e.g. CFontDialog) to pick a new font/color
        virtual void          ShowSimpleEmbeddingInfoDialog (const SDKString& embeddingTypeName);
        virtual bool          ShowURLEmbeddingInfoDialog (const SDKString& embeddingTypeName, SDKString* urlTitle, SDKString* urlValue);
        virtual bool          ShowAddURLEmbeddingInfoDialog (SDKString* urlTitle, SDKString* urlValue);
        virtual bool          AddNewTableDialog (size_t* nRows, size_t* nCols);
        struct TableSelectionPropertiesInfo;
        virtual bool EditTablePropertiesDialog (TableSelectionPropertiesInfo* tableProperties);

#if qPlatform_Windows
    private:
        static UINT_PTR CALLBACK ColorPickerINITPROC (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif
    };

    /*
    @CLASS:         WordProcessor::DialogSupport::TableSelectionPropertiesInfo
    @DESCRIPTION:   <p></p>
    */
    struct WordProcessor::DialogSupport::TableSelectionPropertiesInfo {
        TableSelectionPropertiesInfo ();

        TWIPS fTableBorderWidth;
        Color fTableBorderColor;

        TWIPS_Rect fDefaultCellMargins;
        TWIPS      fCellSpacing;

        bool  fCellWidth_Common;
        TWIPS fCellWidth;

        bool  fCellBackgroundColor_Common;
        Color fCellBackgroundColor;
    };

    /**
     */
    class WordProcessor::EmptySelectionParagraphSavedTextRep : public StandardStyledTextInteractor::EmptySelStyleTextRep {
    private:
        using inherited = StandardStyledTextInteractor::EmptySelStyleTextRep;

    public:
        EmptySelectionParagraphSavedTextRep (WordProcessor* interactor, size_t selStart, size_t selEnd, size_t at);

    public:
        virtual void InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite) override;

    private:
        ParagraphInfo fSavedStyleInfo;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WordProcessor.inl"

#endif /*_Stroika_Frameworks_Led_WordProcessor_h_*/
