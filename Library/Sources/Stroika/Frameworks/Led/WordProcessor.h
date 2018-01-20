/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_WordProcessor_h_
#define _Stroika_Frameworks_Led_WordProcessor_h_ 1

#include "../../Foundation/StroikaPreComp.h"

/*
@MODULE:    WordProcessor
@DESCRIPTION:
                <p>This module contains all the classes needed to implement a typical
            word-processor UI. These things include per-paragraph tab stops, justification,
            margins, etc. The main class you should be interested in is the @'WordProcessor' class.
            </p>
 */

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#if qPlatform_MacOS
#include <ColorPicker.h>
#endif

#include "HiddenText.h"
#include "IdleManager.h"
#include "MarkerCover.h"
#include "StandardStyledTextInteractor.h"
#include "TextInteractorMixins.h"
#include "WordWrappedTextInteractor.h"

namespace Stroika {
    namespace Frameworks {
        namespace Led {

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
#endif

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
                StandardStyledWordWrappedTextInteractor ();
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

#if qWideCharacters
            public:
                nonvirtual bool GetSmartQuoteMode () const;
                nonvirtual void SetSmartQuoteMode (bool smartQuoteMode);

            private:
                bool fSmartQuoteMode;
#endif

            public:
                class WordProcessorTextIOSinkStream;
                class WordProcessorTextIOSrcStream;

            public:
                class WordProcessorFlavorPackageInternalizer;
                class WordProcessorFlavorPackageExternalizer;

            public:
#if qTroubleLookingUpBaseClassInNestedClassDeclaration_VC6
                class NoParagraphDatabaseAvailable : public TextImager::NotFullyInitialized {
                };
#else
                /*
                @CLASS:         WordProcessor::NoParagraphDatabaseAvailable
                @BASES:         @'TextImager::NotFullyInitialized'
                @DESCRIPTION:   <p>Thrown by @'WordProcessor::GetLayoutMargins' etc when no @'WordProcessor::ParagraphDatabasePtr' available.</p>
                */
                class NoParagraphDatabaseAvailable : public NotFullyInitialized {
                };
#endif

            public:
                class WPPartition;

                virtual PartitionPtr MakeDefaultPartition () const override;

            public:
                class ParagraphInfo;
                class IncrementalParagraphInfo;
                class ParagraphInfoMarker;
                class AbstractParagraphDatabaseRep;
                class ParagraphDatabaseRep;
                /*
                @CLASS:         WordProcessor::ParagraphDatabasePtr
                @BASES:         @'shared_ptr<T>', (T=@'WordProcessor::AbstractParagraphDatabaseRep')
                @DESCRIPTION:   <p>A shared_ptr (smart pointer) to a @'WordProcessor::AbstractParagraphDatabaseRep'.</p>
                */
                using ParagraphDatabasePtr = shared_ptr<AbstractParagraphDatabaseRep>;
                nonvirtual ParagraphDatabasePtr GetParagraphDatabase () const;
                nonvirtual void                 SetParagraphDatabase (const ParagraphDatabasePtr& paragraphDatabase);

            private:
                ParagraphDatabasePtr fParagraphDatabase;
                bool                 fICreatedParaDB;

            protected:
                virtual void    HookParagraphDatabaseChanged ();
                nonvirtual void HookParagraphDatabaseChanged_ ();

            public:
                /*
                @CLASS:         WordProcessor::HidableTextDatabasePtr
                @BASES:         @'shared_ptr<T>', (T=@'HidableTextMarkerOwner')
                @DESCRIPTION:   <p>A shared_ptr (smart pointer) to a @'HidableTextMarkerOwner'.</p>
                */
                using HidableTextDatabasePtr = shared_ptr<HidableTextMarkerOwner>;
                nonvirtual HidableTextDatabasePtr GetHidableTextDatabase () const;
                nonvirtual void                   SetHidableTextDatabase (const HidableTextDatabasePtr& hidableTextDatabase);

            private:
                HidableTextDatabasePtr fHidableTextDatabase;
                bool                   fICreatedHidableTextDB;

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
                nonvirtual Led_Justification GetJustification (size_t characterPos) const;
                nonvirtual bool              GetJustification (size_t from, size_t to, Led_Justification* justification) const;
                nonvirtual void              SetJustification (size_t from, size_t to, Led_Justification justification);

            public:
                static StandardTabStopList GetDefaultStandardTabStopList ();
                nonvirtual StandardTabStopList GetStandardTabStopList (size_t characterPos) const;
                nonvirtual bool                GetStandardTabStopList (size_t from, size_t to, StandardTabStopList* tabStops) const;
                nonvirtual void                SetStandardTabStopList (size_t from, size_t to, StandardTabStopList tabStops);

            public:
                nonvirtual void GetMargins (size_t characterPos, Led_TWIPS* leftMargin, Led_TWIPS* rightMargin) const;
                nonvirtual bool GetMargins (size_t from, size_t to, Led_TWIPS* leftMargin, Led_TWIPS* rightMargin) const;
                nonvirtual void SetMargins (size_t from, size_t to, Led_TWIPS leftMargin, Led_TWIPS rightMargin);

            public:
                nonvirtual Led_TWIPS GetFirstIndent (size_t characterPos) const;
                nonvirtual bool      GetFirstIndent (size_t from, size_t to, Led_TWIPS* firstIndent) const;
                nonvirtual void      SetFirstIndent (size_t from, size_t to, Led_TWIPS firstIndent);

            public:
                nonvirtual Led_TWIPS GetSpaceBefore (size_t characterPos) const;
                nonvirtual bool      GetSpaceBefore (size_t from, size_t to, Led_TWIPS* sb) const;
                nonvirtual void      SetSpaceBefore (size_t from, size_t to, Led_TWIPS sb);
                nonvirtual Led_TWIPS GetSpaceAfter (size_t characterPos) const;
                nonvirtual bool      GetSpaceAfter (size_t from, size_t to, Led_TWIPS* sa) const;
                nonvirtual void      SetSpaceAfter (size_t from, size_t to, Led_TWIPS sa);
                nonvirtual Led_LineSpacing GetLineSpacing (size_t characterPos) const;
                nonvirtual bool            GetLineSpacing (size_t from, size_t to, Led_LineSpacing* sl) const;
                nonvirtual void            SetLineSpacing (size_t from, size_t to, Led_LineSpacing sl);
                nonvirtual ListStyle     GetListStyle (size_t characterPos) const;
                nonvirtual bool          GetListStyle (size_t from, size_t to, ListStyle* listStyle) const;
                nonvirtual void          SetListStyle (size_t from, size_t to, ListStyle listStyle, bool autoFormat = false);
                nonvirtual unsigned char GetListIndentLevel (size_t characterPos) const;
                nonvirtual bool          GetListIndentLevel (size_t from, size_t to, unsigned char* indentLevel) const;
                nonvirtual void          SetListIndentLevel (size_t from, size_t to, unsigned char indentLevel, bool autoFormat = false);

            protected:
                virtual Led_tString  GetListLeader (size_t paragraphMarkerPos) const;
                virtual Led_Distance GetListLeaderLength (size_t paragraphMarkerPos) const;
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
#if qUsingMemberNameToOverloadInTemplateClassBug
                void SetSelection (size_t start, size_t end, TextInteractor::UpdateMode updateMode)
                {
                    TextInteractor::SetSelection (start, end, updateMode);
                }
#else
                using TextInteractor::SetSelection;
#endif

            public:
                virtual bool     GetCaretShownSituation () const override;
                virtual Led_Rect CalculateCaretRect () const override;

            public:
                virtual void OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed) override;

            public:
                virtual bool ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection, size_t* dragAnchor) override;
                virtual void WhileSimpleMouseTracking (Led_Point newMousePos, size_t dragAnchor) override;

                // Table support
            public:
                class Table;
                virtual Table* InsertTable (size_t at);

                nonvirtual vector<Table*> GetTablesInRange (size_t from, size_t to = static_cast<size_t> (-1)) const;
                nonvirtual Table* GetTableAt (size_t from) const;

            public:
                nonvirtual Table* GetActiveTable () const;

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
#if qPlatform_MacOS
                    kFontStyleOutline_CmdID,
                    kFontStyleShadow_CmdID,
                    kFontStyleCondensed_CmdID,
                    kFontStyleExtended_CmdID,
#elif qPlatform_Windows
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
                virtual Led_Distance ComputeMaxHScrollPos () const override;

            protected:
                nonvirtual Led_TWIPS CalculateFarthestRightMarginInDocument () const;
                nonvirtual Led_TWIPS GetFarthestRightMarginInDocument () const;
                nonvirtual Led_TWIPS CalculateFarthestRightMarginInWindow () const;
                virtual Led_TWIPS    CalculateFarthestRightMargin () const;

            private:
                static const Led_TWIPS kBadCachedFarthestRightMarginInDocument;

            protected:
                virtual void InvalidateAllCaches () override;
                virtual void TabletChangedMetrics () override;

            public:
                virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

            public:
                nonvirtual Led_IncrementalFontSpecification GetCurSelFontSpec () const;

            private:
                mutable Led_IncrementalFontSpecification fCachedCurSelFontSpec;
                mutable Led_Justification                fCachedCurSelJustification;
                mutable bool                             fCachedCurSelJustificationUnique;
                mutable bool                             fCachedCurSelFontSpecValid;

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
#if qPlatform_MacOS
                virtual void OnUpdateFontStyleOutlineCommand (CommandUpdater* enabler);
                virtual void OnFontStyleOutlineCommand ();
                virtual void OnUpdateFontStyleShadowCommand (CommandUpdater* enabler);
                virtual void OnFontStyleShadowCommand ();
                virtual void OnUpdateFontStyleCondensedCommand (CommandUpdater* enabler);
                virtual void OnFontStyleCondensedCommand ();
                virtual void OnUpdateFontStyleExtendedCommand (CommandUpdater* enabler);
                virtual void OnFontStyleExtendedCommand ();
#elif qPlatform_Windows
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
                virtual Led_SDK_String GetPrettyTypeName (SimpleEmbeddedObjectStyleMarker* m);
                nonvirtual SimpleEmbeddedObjectStyleMarker* GetSoleSelectedEmbedding () const;

            public:
                class EmptySelectionParagraphSavedTextRep;

            protected:
                virtual InteractiveReplaceCommand::SavedTextRep* InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd) override;

            public:
                virtual void InteractiveSetFont (const Led_IncrementalFontSpecification& defaultFont) override;

            public:
                nonvirtual void InteractiveSetJustification (Led_Justification justification);
                nonvirtual void InteractiveSetStandardTabStopList (StandardTabStopList tabStops);
                nonvirtual void InteractiveSetMargins (Led_TWIPS leftMargin, Led_TWIPS rightMargin);
                nonvirtual void InteractiveSetFirstIndent (Led_TWIPS firstIndent);
                nonvirtual void InteractiveSetMarginsAndFirstIndent (Led_TWIPS leftMargin, Led_TWIPS rightMargin, Led_TWIPS firstIndent);
                nonvirtual void InteractiveSetParagraphSpacing (Led_TWIPS spaceBefore, bool spaceBeforeValid, Led_TWIPS spaceAfter, bool spaceAfterValid, Led_LineSpacing lineSpacing, bool lineSpacingValid);
                nonvirtual void InteractiveSetListStyle (ListStyle listStyle);
                nonvirtual void InteractiveDoIndentChange (bool increase);

            protected:
                virtual const TextImager::TabStopList& GetTabStopList (size_t containingPos) const override;

            protected:
                virtual void DrawBefore (const Led_Rect& subsetToDraw, bool printing) override;

            protected:
                virtual void DrawRowSegments (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
                                              const TextLayoutBlock& text, size_t rowStart, size_t rowEnd) override;

            protected:
                virtual vector<Led_Rect> GetRowHilightRects (const TextLayoutBlock& text, size_t rowStart, size_t rowEnd, size_t hilightStart, size_t hilightEnd) const override;

            public:
                virtual void DrawSegment (Led_Tablet tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& invalidRect,
                                          Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn) override;

            protected:
                virtual Led_Distance MeasureSegmentHeight (size_t from, size_t to) const override;
                virtual Led_Distance MeasureSegmentBaseLine (size_t from, size_t to) const override;

            private:
                nonvirtual Led_Distance MeasureMinSegDescent (size_t from, size_t to) const;

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
                virtual size_t ResetTabStops (size_t from, const Led_tChar* text, size_t nTChars, Led_Distance* charLocations, size_t startSoFar) const override;

            private:
                nonvirtual size_t ResetTabStopsWithMargin (Led_Distance lhsMargin, size_t from, const Led_tChar* text, size_t nTChars, Led_Distance* charLocations, size_t startSoFar) const;

            public:
                virtual void GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const override;

            protected:
                virtual void ExpandedFromAndToInPostReplace (size_t from, size_t newTo,
                                                             size_t stableTypingRegionStart, size_t stableTypingRegionEnd,
                                                             size_t startPositionOfRowWhereReplaceBegins, size_t startPositionOfRowAfterReplaceEnds,
                                                             size_t* expandedFrom, size_t* expandedTo) override;
                virtual void PostReplace (PreReplaceInfo& preReplaceInfo) override;

            private:
                nonvirtual Led_Distance CalcSpaceToEat (size_t rowContainingCharPos) const;

            private:
                friend class Table;
                friend class AbstractParagraphDatabaseRep;
                friend class ParagraphDatabaseRep;
            };

            /*
            @CLASS:         WordProcessor::ParagraphInfo
            @DESCRIPTION:   <p>An object which captures the per-paragraph information we store especially in the
                        @'WordProcessor' class. The attributes stored include:
                            <ul>
                                <li>Justification</li>
                                <li>TabStopList</li>
                                <li>Margin (left and right - NB: we require left < right)</li>
                                <li>FirstIdent (nb this is ADDED to the LHS margin, and CAN BE NEGATIVE</li>
                            </ul>
                            </p>
            */
            class WordProcessor::ParagraphInfo {
            public:
                using StandardTabStopList = TextImager::StandardTabStopList;

            public:
                ParagraphInfo ();

            public:
                nonvirtual Led_Justification GetJustification () const;
                nonvirtual void              SetJustification (Led_Justification justification);

            private:
                Led_Justification fJustification;

            public:
                nonvirtual const StandardTabStopList& GetTabStopList () const;
                nonvirtual void                       SetTabStopList (const StandardTabStopList& tabStops);

            private:
                StandardTabStopList fTabStops;

            public:
                nonvirtual Led_TWIPS GetLeftMargin () const;
                nonvirtual Led_TWIPS GetRightMargin () const;
                nonvirtual void      SetMargins (Led_TWIPS lhs, Led_TWIPS rhs);

            private:
                Led_TWIPS fLeftMargin;
                Led_TWIPS fRightMargin;

            public:
                nonvirtual Led_TWIPS GetFirstIndent () const;
                nonvirtual void      SetFirstIndent (Led_TWIPS firstIndent);

            private:
                Led_TWIPS fFirstIndent;

            public:
                nonvirtual Led_TWIPS GetSpaceBefore () const;
                nonvirtual void      SetSpaceBefore (Led_TWIPS sb);
                nonvirtual Led_TWIPS GetSpaceAfter () const;
                nonvirtual void      SetSpaceAfter (Led_TWIPS sa);
                nonvirtual Led_LineSpacing GetLineSpacing () const;
                nonvirtual void            SetLineSpacing (Led_LineSpacing sl);

            private:
                Led_TWIPS       fSpaceBefore;
                Led_TWIPS       fSpaceAfter;
                Led_LineSpacing fLineSpacing;

            public:
                nonvirtual ListStyle GetListStyle () const;
                nonvirtual void      SetListStyle (ListStyle listStyle);

            private:
                ListStyle fListStyle;

            public:
                nonvirtual unsigned char GetListIndentLevel () const;
                nonvirtual void          SetListIndentLevel (unsigned char indentLevel);

            private:
                unsigned char fListIndentLevel;

            public:
                nonvirtual void MergeIn (const IncrementalParagraphInfo& incParaInfo);

            public:
                nonvirtual bool operator== (const ParagraphInfo& rhs) const;
                nonvirtual bool operator!= (const ParagraphInfo& rhs) const;
            };

            /*
            @CLASS:         WordProcessor::IncrementalParagraphInfo
            @BASES:         @'WordProcessor::ParagraphInfo'
            @DESCRIPTION:   <p>Overload (hide) the @'WordProcessor::ParagraphInfo' methods, to assert that the
                        attribute is valid (for getters) and to set a ValidFlag for the setters. And add methods for
                        each attribute to test for validity, and to invalidate.</p>
                            <p>These are used if you want to set just part of a @'WordProcessor::ParagraphInfo'.</p>
            */
            class WordProcessor::IncrementalParagraphInfo : public WordProcessor::ParagraphInfo {
            private:
                using inherited = ParagraphInfo;

            public:
                IncrementalParagraphInfo ();
                explicit IncrementalParagraphInfo (const ParagraphInfo& pi);

            public:
                nonvirtual Led_Justification GetJustification () const;
                nonvirtual void              SetJustification (Led_Justification justification);
                nonvirtual bool              GetJustification_Valid () const;
                nonvirtual void              InvalidateJustification ();

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
                nonvirtual Led_TWIPS GetLeftMargin () const;
                nonvirtual Led_TWIPS GetRightMargin () const;
                nonvirtual void      SetMargins (Led_TWIPS lhs, Led_TWIPS rhs);
                nonvirtual bool      GetMargins_Valid () const;
                nonvirtual void      InvalidateMargins ();

            private:
                bool fMarginsValid;

            public:
                nonvirtual Led_TWIPS GetFirstIndent () const;
                nonvirtual void      SetFirstIndent (Led_TWIPS firstIndent);
                nonvirtual bool      GetFirstIndent_Valid () const;
                nonvirtual void      InvalidateFirstIndent ();

            private:
                bool fFirstIndentValid;

            public:
                nonvirtual Led_TWIPS GetSpaceBefore () const;
                nonvirtual void      SetSpaceBefore (Led_TWIPS sb);
                nonvirtual bool      GetSpaceBefore_Valid () const;
                nonvirtual void      InvalidateSpaceBefore ();
                nonvirtual Led_TWIPS GetSpaceAfter () const;
                nonvirtual void      SetSpaceAfter (Led_TWIPS sa);
                nonvirtual bool      GetSpaceAfter_Valid () const;
                nonvirtual void      InvalidateSpaceAfter ();
                nonvirtual Led_LineSpacing GetLineSpacing () const;
                nonvirtual void            SetLineSpacing (Led_LineSpacing sl);
                nonvirtual bool            GetLineSpacing_Valid () const;
                nonvirtual void            InvalidateLineSpacing ();

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
                nonvirtual bool operator!= (const IncrementalParagraphInfo& rhs) const;
            };

            /*
            @CLASS:         WordProcessor::ParagraphInfoMarker
            @BASES:         @'Marker'
            @DESCRIPTION:
            */
            class WordProcessor::ParagraphInfoMarker : public Marker {
            public:
                ParagraphInfoMarker (ParagraphInfo paragraphInfo = ParagraphInfo ());

                nonvirtual const ParagraphInfo& GetInfo () const;
                nonvirtual void                 SetInfo (ParagraphInfo paragraphInfo);

            private:
                ParagraphInfo fParagraphInfo;
            };

            /*
            @CLASS:         WordProcessor::AbstractParagraphDatabaseRep
            @BASES:         virtual @'MarkerOwner'
            @DESCRIPTION:   <p>.</p>
            */
            class WordProcessor::AbstractParagraphDatabaseRep : public virtual MarkerOwner {
            private:
                using inherited = MarkerOwner;

            public:
                AbstractParagraphDatabaseRep ();

            public:
                using PartitionPtr                                                   = shared_ptr<Partition>;
                virtual PartitionPtr GetPartition () const                           = 0;
                virtual void         SetPartition (const PartitionPtr& partitionPtr) = 0;

            public:
                virtual const ParagraphInfo&                               GetParagraphInfo (size_t charAfterPos) const                          = 0;
                virtual vector<pair<WordProcessor::ParagraphInfo, size_t>> GetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing) const = 0;

                virtual void SetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalParagraphInfo& infoForMarkers)             = 0;
                virtual void SetParagraphInfo (size_t charAfterPos, const vector<pair<WordProcessor::IncrementalParagraphInfo, size_t>>& infoForMarkers) = 0;
                virtual void SetParagraphInfo (size_t charAfterPos, const vector<pair<ParagraphInfo, size_t>>& infoForMarkers)                           = 0;

            private:
                bool fSomeInvalidTables;

            protected:
                Led_TWIPS fCachedFarthestRightMarginInDocument;

            private:
                friend class WordProcessor;
                friend class WordProcessor::Table;
                friend class WordProcessor::WPIdler;
            };

            /*
            @CLASS:         WordProcessor::ParagraphDatabaseRep
            @BASES:         @'public WordProcessor::AbstractParagraphDatabaseRep' @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>', (MARKER=@'WordProcessor::ParagraphInfoMarker',MARKERINFO=@'WordProcessor::ParagraphInfo',INCREMENTALMARKERINFO=@'WordProcessor::IncrementalParagraphInfo')
            @DESCRIPTION:   <p>A MarkerCover which associates @'WordProcessor::ParagraphInfo' with each paragraph in the next (merging together adjacent
                        identical ones). Paragraphs are defined by the @'Partition' object associated
                        (@'WordProcessor::ParagraphDatabaseRep::SetPartition').</p>
            */
            class WordProcessor::ParagraphDatabaseRep : public WordProcessor::AbstractParagraphDatabaseRep, private MarkerCover<WordProcessor::ParagraphInfoMarker, WordProcessor::ParagraphInfo, WordProcessor::IncrementalParagraphInfo> {
            private:
                using inheritedMC = MarkerCover<ParagraphInfoMarker, ParagraphInfo, IncrementalParagraphInfo>;

            public:
                ParagraphDatabaseRep (TextStore& textStore);

            public:
                using PartitionPtr = shared_ptr<Partition>;
                virtual PartitionPtr GetPartition () const override;
                virtual void         SetPartition (const PartitionPtr& partitionPtr) override;

            private:
                PartitionPtr fPartition;

            public:
                static ParagraphInfo GetStaticDefaultParagraphInfo ();

                // override the AbstractParagraphDatabase API
            public:
                virtual const ParagraphInfo&                               GetParagraphInfo (size_t charAfterPos) const override;
                virtual vector<pair<WordProcessor::ParagraphInfo, size_t>> GetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing) const override;
                virtual void                                               SetParagraphInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalParagraphInfo& infoForMarkers) override;
                virtual void                                               SetParagraphInfo (size_t charAfterPos, const vector<pair<WordProcessor::IncrementalParagraphInfo, size_t>>& infoForMarkers) override;
                virtual void                                               SetParagraphInfo (size_t charAfterPos, const vector<pair<ParagraphInfo, size_t>>& infoForMarkers) override;

            public:
                virtual void SetInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalParagraphInfo& infoForMarkers) override;
                virtual void SetInfos (size_t charAfterPos, const vector<pair<WordProcessor::IncrementalParagraphInfo, size_t>>& infoForMarkers) override;

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

            /*
            @CLASS:         WordProcessor::WordProcessorTextIOSinkStream
            @BASES:         @'StandardStyledTextInteractor::StandardStyledTextIOSinkStream'
            @DESCRIPTION:   <p>A @'StandardStyledTextInteractor::StandardStyledTextIOSinkStream', for use with the StyledTextIO module,
                        which adds support for a @'WordProcessor::ParagraphDatabasePtr'.</p>
            */
            class WordProcessor::WordProcessorTextIOSinkStream : public StandardStyledTextInteractor::StandardStyledTextIOSinkStream {
            private:
                using inherited = StandardStyledTextInteractor::StandardStyledTextIOSinkStream;

            public:
                WordProcessorTextIOSinkStream (TextStore*                                        textStore,
                                               const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
                                               const WordProcessor::ParagraphDatabasePtr&        paragraphDatabase,
                                               const WordProcessor::HidableTextDatabasePtr&      hidableTextDatabase,
                                               size_t                                            insertionStart = 0);
                WordProcessorTextIOSinkStream (WordProcessor* wp, size_t insertionStart = 0);
                ~WordProcessorTextIOSinkStream ();

            private:
                nonvirtual void CTOR_COMMON ();

            public:
                nonvirtual bool GetOverwriteTableMode () const;
                nonvirtual void SetOverwriteTableMode (bool overwriteTableMode);

            private:
                bool fOverwriteTableMode;

#if !qNestedTablesSupported
            public:
                nonvirtual bool GetNoTablesAllowed () const;
                nonvirtual void SetNoTablesAllowed (bool noTablesAllowed);

            private:
                bool fNoTablesAllowed;
#endif

            public:
                virtual void AppendText (const Led_tChar* text, size_t nTChars, const Led_FontSpecification* fontSpec) override;
                virtual void AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding) override;
                virtual void AppendSoftLineBreak () override;
                virtual void SetJustification (Led_Justification justification) override;
                virtual void SetStandardTabStopList (const TextImager::StandardTabStopList& tabStops) override;
                virtual void SetFirstIndent (Led_TWIPS tx) override;
                virtual void SetLeftMargin (Led_TWIPS lhs) override;
                virtual void SetRightMargin (Led_TWIPS rhs) override;
                virtual void SetSpaceBefore (Led_TWIPS sb) override;
                virtual void SetSpaceAfter (Led_TWIPS sa) override;
                virtual void SetLineSpacing (Led_LineSpacing sl) override;
                virtual void SetTextHidden (bool hidden) override;
                virtual void StartTable () override;
                virtual void EndTable () override;
                virtual void StartTableRow () override;
                virtual void EndTableRow () override;
                virtual void StartTableCell (size_t colSpan) override;
                virtual void EndTableCell () override;
                virtual void SetListStyle (ListStyle listStyle) override;
                virtual void SetListIndentLevel (unsigned char indentLevel) override;
                virtual void SetTableBorderColor (Led_Color c) override;
                virtual void SetTableBorderWidth (Led_TWIPS bWidth) override;
                virtual void SetCellWidths (const vector<Led_TWIPS>& cellWidths) override;
                virtual void SetCellBackColor (const Led_Color c) override;
                virtual void SetDefaultCellMarginsForCurrentRow (Led_TWIPS top, Led_TWIPS left, Led_TWIPS bottom, Led_TWIPS right) override;
                virtual void SetDefaultCellSpacingForCurrentRow (Led_TWIPS top, Led_TWIPS left, Led_TWIPS bottom, Led_TWIPS right) override;
                virtual void EndOfBuffer () override;
                virtual void Flush () override;

            public:
                nonvirtual void SetIgnoreLastParaAttributes (bool ignoreLastParaAttributes);

            protected:
                nonvirtual void PushContext (TextStore*                                        ts,
                                             const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
                                             const WordProcessor::ParagraphDatabasePtr&        paragraphDatabase,
                                             const WordProcessor::HidableTextDatabasePtr&      hidableTextDatabase,
                                             size_t                                            insertionStart);
                nonvirtual void PopContext ();

            private:
                struct Context {
                    WordProcessor::ParagraphDatabasePtr   fParagraphDatabase;
                    WordProcessor::HidableTextDatabasePtr fHidableTextDatabase;
                };
                vector<Context> fSavedContexts;

            private:
                using ParaInfoNSize = pair<IncrementalParagraphInfo, size_t>;
                WordProcessor::ParagraphDatabasePtr   fParagraphDatabase;
                WordProcessor::HidableTextDatabasePtr fHidableTextDatabase;
                vector<ParaInfoNSize>                 fSavedParaInfo;
                IncrementalParagraphInfo              fNewParagraphInfo;
                bool                                  fTextHidden;
                DiscontiguousRun<bool>                fHidableTextRuns;
                bool                                  fEndOfBuffer;
                bool                                  fIgnoreLastParaAttributes;
                WordProcessor::Table*                 fCurrentTable;
                vector<Led_TWIPS>                     fCurrentTableCellWidths;
                Led_Color                             fCurrentTableCellColor;
                vector<size_t>                        fCurrentTableColSpanArray;
                vector<WordProcessor::Table*>         fTableStack; // for nesting
                size_t                                fNextTableRow;
                size_t                                fNextTableCell;
                size_t                                fCurrentTableCell;

#if qDebug
            private:
                unsigned int fTableOpenLevel;
                bool         fTableRowOpen;
                bool         fTableCellOpen;
#endif
            };

            /*
            @CLASS:         WordProcessor::WordProcessorTextIOSrcStream
            @BASES:         @'StandardStyledTextInteractor::StandardStyledTextIOSrcStream'
            @DESCRIPTION:   <p>A @'StandardStyledTextInteractor::StandardStyledTextIOSrcStream', for use with the StyledTextIO module,
                        which adds support for a @'WordProcessor::ParagraphDatabasePtr'.</p>
            */
            class WordProcessor::WordProcessorTextIOSrcStream : public StandardStyledTextInteractor::StandardStyledTextIOSrcStream {
            private:
                using inherited = StandardStyledTextInteractor::StandardStyledTextIOSrcStream;

            public:
                WordProcessorTextIOSrcStream (TextStore*                                        textStore,
                                              const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
                                              const WordProcessor::ParagraphDatabasePtr&        paragraphDatabase,
                                              const WordProcessor::HidableTextDatabasePtr&      hidableTextDatabase,
                                              size_t selectionStart = 0, size_t selectionEnd = kBadIndex);
                WordProcessorTextIOSrcStream (WordProcessor* textImager, size_t selectionStart = 0, size_t selectionEnd = kBadIndex);

            public:
                nonvirtual bool GetUseTableSelection () const;
                nonvirtual void SetUseTableSelection (bool useTableSelection);

            private:
                bool fUseTableSelection;

            public:
                virtual Led_Justification      GetJustification () const override;
                virtual StandardTabStopList    GetStandardTabStopList () const override;
                virtual Led_TWIPS              GetFirstIndent () const override;
                virtual void                   GetMargins (Led_TWIPS* lhs, Led_TWIPS* rhs) const override;
                virtual Led_TWIPS              GetSpaceBefore () const override;
                virtual Led_TWIPS              GetSpaceAfter () const override;
                virtual Led_LineSpacing        GetLineSpacing () const override;
                virtual void                   GetListStyleInfo (ListStyle* listStyle, unsigned char* indentLevel) const override;
                virtual Led_tChar              GetSoftLineBreakCharacter () const override;
                virtual DiscontiguousRun<bool> GetHidableTextRuns () const override;
                virtual Table*                 GetTableAt (size_t at) const override;
                virtual void                   SummarizeFontAndColorTable (set<Led_SDK_String>* fontNames, set<Led_Color>* colorsUsed) const override;

            protected:
                class TableIOMapper;

            private:
                WordProcessor::ParagraphDatabasePtr fParagraphDatabase;
                DiscontiguousRun<bool>              fHidableTextRuns;
            };

            /*
            @CLASS:         WordProcessor::WordProcessorTextIOSrcStream::TableIOMapper
            @ACCESS:        protected
            @BASES:         @'StyledTextIOWriter::SrcStream::Table'
            @DESCRIPTION:   <p></p>
            */
            class WordProcessor::WordProcessorTextIOSrcStream::TableIOMapper : public StyledTextIO::StyledTextIOWriter::SrcStream::Table {
            private:
                using inherited = StyledTextIO::StyledTextIOWriter::SrcStream::Table;

            public:
                TableIOMapper (WordProcessor::Table& realTable,
                               size_t startRow = 0, size_t endRow = static_cast<size_t> (-1),
                               size_t startCol = 0, size_t endCol = static_cast<size_t> (-1));

            public:
                virtual size_t                                       GetRows () const override;
                virtual size_t                                       GetColumns (size_t row) const override;
                virtual void                                         GetRowInfo (size_t row, vector<CellInfo>* cellInfos) override;
                virtual StyledTextIO::StyledTextIOWriter::SrcStream* MakeCellSubSrcStream (size_t row, size_t column) override;
                virtual size_t                                       GetOffsetEnd () const override;
                virtual Led_TWIPS_Rect                               GetDefaultCellMarginsForRow (size_t row) const override;
                virtual Led_TWIPS_Rect                               GetDefaultCellSpacingForRow (size_t row) const override;

            private:
                WordProcessor::Table& fRealTable;
                size_t                fStartRow;
                size_t                fEndRow;
                size_t                fStartCol;
                size_t                fEndCol;
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
                WordProcessorFlavorPackageInternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase,
                                                        const WordProcessor::ParagraphDatabasePtr&   paragraphDatabase,
                                                        const WordProcessor::HidableTextDatabasePtr& hidableTextDatabase);

            public:
                nonvirtual bool GetOverwriteTableMode () const;
                nonvirtual void SetOverwriteTableMode (bool overwriteTableMode);

            private:
                bool fOverwriteTableMode;

#if !qNestedTablesSupported
            public:
                nonvirtual bool GetNoTablesAllowed () const;
                nonvirtual void SetNoTablesAllowed (bool noTablesAllowed);

            private:
                bool fNoTablesAllowed;
#endif

            public:
                virtual StandardStyledTextIOSinkStream* mkStandardStyledTextIOSinkStream (size_t insertionStart) override;

            protected:
                WordProcessor::ParagraphDatabasePtr   fParagraphDatabase;
                WordProcessor::HidableTextDatabasePtr fHidableTextDatabase;
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
                using Table           = WordProcessor::Table;

            private:
                using inherited = LineBasedPartition;

            public:
                WPPartition (TextStore& textStore, MarkerOwner& tableMarkerOwner);

            protected:
                virtual vector<Table*> GetTablesInRange (size_t from, size_t to = static_cast<size_t> (-1)) const;

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
                friend class WordProcessor::Table;
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
                WordProcessorFlavorPackageExternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase,
                                                        const WordProcessor::ParagraphDatabasePtr&   paragraphDatabase,
                                                        const WordProcessor::HidableTextDatabasePtr& hidableTextDatabase);

            public:
                nonvirtual bool GetUseTableSelection () const;
                nonvirtual void SetUseTableSelection (bool useTableSelection);

            private:
                bool fUseTableSelection;

            protected:
                virtual StandardStyledTextIOSrcStream* mkStandardStyledTextIOSrcStream (size_t selectionStart, size_t selectionEnd) override;

            protected:
                WordProcessor::ParagraphDatabasePtr   fParagraphDatabase;
                WordProcessor::HidableTextDatabasePtr fHidableTextDatabase;
            };

            /*
            @CLASS:         WordProcessor::Table
            @BASES:         @'SimpleEmbeddedObjectStyleMarker'
            @DESCRIPTION:   <p>A table is an object that can be embedded in a document, and it contains multiple cells.
                        Each cell is basically another word-processor instance.</p>
                            <p>Since a table is assocated with a paragraph database, and can be viewed and edited
                        simultaneously be multiple views (WordProcessors) - we cannot make long-term assumptions
                        about association of a table with an owning WordProcessor object. Instead - during UI operations
                        like typing and so on - you temporarily associate an owning WP with the given table using
                        @'WordProcessor::Table::TemporarilySetOwningWP'</p>
                            <p>We support having any number of rows, and each row can have a different number of
                        columns (cells). Often however - the number of columns for each row of a table will be the
                        same (as will their widths be equal).</p>
                            <p>This class may not remain a subclass of simpleembeddedObject stuff - but maybe.</p>
                            <p>Note that most coordiantes in this routine are presumed to be table relative. Either that - or
                        they are window-rect relative. You can use the methods @'WordProcessor::Table::TableCoordinates2Window' and
                        @'WordProcessor::Table::WindowCoordinates2Table' to map between them.
                            </p>
                            <p>Each cell is surrounded by spacing (see @'WordProcessor::Table::GetCellSpacing'.
                        This defaults to zero. If you set it to non-zero - then the borders drawn for cells don't touch the
                        borders for the table itself.</p>
                            <p>The cell bounds for each cell (@'WordProcessor::Table::GetCellBounds') is the area that
                        gets a box drawn around it (on the outside bounds). These 'cell bounds' may not exactly touch each other on the
                        edges by so that they SHARE the border separating each from its sibling. Also - they may not touch because
                        of cell spacing (@'WordProcessor::Table::GetCellSpacing').</p>
                            <p>The actaul 'edit window' within a cell is inset by its cell margins. This value defaults
                        to that provided by @'WordProcessor::Table::GetDefaultCellMargins', but at some point (maybe for 3.1?)
                        will be overridable on a per-cell basis.</p>
            */
            class WordProcessor::Table : public SimpleEmbeddedObjectStyleMarker {
            private:
                using inherited = SimpleEmbeddedObjectStyleMarker;

            public:
                Table (AbstractParagraphDatabaseRep* tableOwner, size_t addAt);
                ~Table ();

            protected:
                virtual void FinalizeAddition (AbstractParagraphDatabaseRep* o, size_t addAt);

            public:
                class Cell;

            protected:
                class CellRep;

            public:
                virtual void         DrawSegment (const StyledTextImager* imager, const RunElement& runElement, Led_Tablet tablet,
                                                  size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& invalidRect,
                                                  Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn) override;
                virtual void         MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to,
                                                          const Led_tChar* text,
                                                          Led_Distance*    distanceResults) const override;
                virtual Led_Distance MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& runElement, size_t from, size_t to) const override;

            public:
                virtual vector<Led_Rect> GetRowHilightRects () const;

            protected:
                virtual void DrawTableBorders (WordProcessor& owningWP, Led_Tablet tablet, const Led_Rect& drawInto);
                virtual void DrawCellBorders (Led_Tablet tablet, size_t row, size_t column, const Led_Rect& cellBounds);

            public:
                nonvirtual Led_TWIPS GetCellSpacing () const;
                nonvirtual void      SetCellSpacing (Led_TWIPS cellSpacing);

            private:
                Led_TWIPS fCellSpacing;

            public:
                nonvirtual void GetDefaultCellMargins (Led_TWIPS* top, Led_TWIPS* left, Led_TWIPS* bottom, Led_TWIPS* right) const;
                nonvirtual void SetDefaultCellMargins (Led_TWIPS top, Led_TWIPS left, Led_TWIPS bottom, Led_TWIPS right);

            private:
                Led_TWIPS_Rect fDefaultCellMargins; // Not REALLY a rect - just a handy way to store 4 values... and OK since its private - not part of API

            public:
                virtual Led_Rect GetCellBounds (size_t row, size_t column) const;
                virtual Led_Rect GetCellEditorBounds (size_t row, size_t column) const;

                virtual void GetClosestCell (const Led_Point& p, size_t* row, size_t* col) const;

            public:
                nonvirtual Led_Point TableCoordinates2Window (const Led_Point& p) const;
                nonvirtual Led_Rect TableCoordinates2Window (const Led_Rect& r) const;
                nonvirtual Led_Point WindowCoordinates2Table (const Led_Point& p) const;
                nonvirtual Led_Rect WindowCoordinates2Table (const Led_Rect& r) const;

            public:
                virtual bool     GetCaretShownSituation () const;
                virtual Led_Rect CalculateCaretRect () const;

            public:
                virtual bool OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed);

            protected:
                virtual bool DoSingleCharCursorEdit (CursorMovementDirection direction, CursorMovementUnit movementUnit, CursorMovementAction action,
                                                     UpdateMode updateMode, bool scrollToSelection);

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
                nonvirtual void AssureCurSelFontCacheValid (Led_IncrementalFontSpecification* curSelFontSpec);

            public:
                nonvirtual void InteractiveSetFont (const Led_IncrementalFontSpecification& defaultFont);

                // SimpleEmbeddedObjectStyleMarker overrides
            public:
                virtual const char* GetTag () const override;
                virtual void        Write (SinkStream& sink) override;
                virtual void        ExternalizeFlavors (WriterFlavorPackage& flavorPackage) override;

            public:
                virtual bool ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection);
                virtual void WhileSimpleMouseTracking (Led_Point newMousePos);

            public:
                nonvirtual Led_Color GetTableBorderColor () const;
                nonvirtual void      SetTableBorderColor (Led_Color c);

            public:
                nonvirtual Led_TWIPS GetTableBorderWidth () const;
                nonvirtual void      SetTableBorderWidth (Led_TWIPS w);

            public:
                nonvirtual Led_TWIPS GetColumnWidth (size_t row, size_t column) const;
                nonvirtual void      SetColumnWidth (size_t row, size_t column, Led_TWIPS colWidth);

            public:
                nonvirtual Led_Color GetCellColor (size_t row, size_t column) const;
                nonvirtual void      SetCellColor (size_t row, size_t column, const Led_Color& c);

            public:
                nonvirtual Cell& GetCell (size_t row, size_t column);
                nonvirtual const Cell& GetCell (size_t row, size_t column) const;
                nonvirtual void        GetRealCell (size_t* row, size_t* column) const;
                nonvirtual const Cell& GetRealCell (size_t row, size_t column) const;

            public:
                enum CellMergeFlags { ePlainCell       = 0,
                                      eMergeCellLeft   = 1,
                                      eMergeCellUp     = 2,
                                      eMergeCellLeftUp = eMergeCellLeft + eMergeCellUp,
                                      eInvalidCell     = 99 };

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
                size_t fRowSelStart;
                size_t fRowSelEnd;
                size_t fColSelStart;
                size_t fColSelEnd;

            private:
                bool   fIntraCellMode;
                size_t fIntraSelStart;
                size_t fIntraSelEnd;
                size_t fIntraCellDragAnchor;

            protected:
                nonvirtual void SaveIntraCellContextInfo (
                    bool                         leftSideOfSelectionInteresting,
                    const Led_FontSpecification& intraCellSelectionEmptySelFontSpecification);
                nonvirtual bool RestoreIntraCellContextInfo (
                    bool*                  leftSideOfSelectionInteresting,
                    Led_FontSpecification* intraCellSelectionEmptySelFontSpecification);
                nonvirtual void InvalidateIntraCellContextInfo ();

            private:
                bool                  fSavedLeftSideOfSelectionInteresting;
                Led_FontSpecification fSavedIntraCellSelectionEmptySelFontSpecification;
                bool                  fSavedIntraCellInfoValid;

            private:
                size_t fTrackingAnchor_Row;
                size_t fTrackingAnchor_Col;

            private:
                class SuppressCellUpdatePropagationContext;
                bool fSuppressCellUpdatePropagationContext;

            protected:
                class AllowUpdateInfoPropagationContext;
                bool                      fAllowUpdateInfoPropagationContext;
                TextStore::SimpleUpdater* fCellUpdatePropationUpdater;

            protected:
#if qAccessChecksFailFromTemplatesBug
            public:
#endif
                class EmbeddedTableWordProcessor;

            protected:
                virtual EmbeddedTableWordProcessor* ConstructEmbeddedTableWordProcessor (WordProcessor& forWordProcessor, size_t forRow, size_t forColumn, const Led_Rect& cellWindowRect, bool captureChangesForUndo);
                virtual void                        ReleaseEmbeddedTableWordProcessor (EmbeddedTableWordProcessor* e);

            public:
                virtual void GetCellWordProcessorDatabases (size_t row, size_t column,
                                                            TextStore**                                 ts,
                                                            StandardStyledTextImager::StyleDatabasePtr* styleDatabase       = nullptr,
                                                            WordProcessor::ParagraphDatabasePtr*        paragraphDatabase   = nullptr,
                                                            WordProcessor::HidableTextDatabasePtr*      hidableTextDatabase = nullptr);

            private:
                WordProcessor* fCurrentOwningWP;
#if qAccessChecksSometimesBreakForNestedClasses
            public:
#endif
                class TemporarilySetOwningWP;
                class TemporarilyAllocateCellWP;
                class TemporarilyAllocateCellWithTablet;

            protected:
                nonvirtual void InvalidateLayout ();
                virtual void    PerformLayout ();

            private:
                enum LayoutFlag { eDone,
                                  eNeedFullLayout };
                mutable LayoutFlag fNeedLayout;

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

            protected:
                virtual void ReValidateSelection ();

            protected:
                /*
                @CLASS:         WordProcessor::Table::RowInfo
                @DESCRIPTION:
                    <p></p>
                */
                struct RowInfo {
                public:
                    RowInfo ();

                public:
                    vector<Cell> fCells;

                public:
                    Led_Distance fHeight; // height of the cell itself (not including the border)
                };
                vector<RowInfo> fRows;

            private:
                Led_TWIPS    fBorderWidth;
                Led_Color    fBorderColor;
                Led_Distance fTotalWidth;
                Led_Distance fTotalHeight;

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
#if 1
                friend class EmbeddedTableWordProcessor;
                friend class CellRep;
#else
                friend class WordProcessor::Table::EmbeddedTableWordProcessor;
                friend class WordProcessor::Table::CellRep;
#endif
                friend class TableCMD;
                friend class WordProcessor::WPIdler;
                friend class WordProcessor;
            };

            /*
            @CLASS:         WordProcessor::Table::Cell
            @DESCRIPTION:   <p>Used internally by the @'WordProcessor::Table' code.</p>
            */
            class WordProcessor::Table::Cell {
            public:
                Cell (Table& forTable, CellMergeFlags mergeFlags);

            public:
                nonvirtual CellMergeFlags GetCellMergeFlags () const;

            private:
                CellMergeFlags fCellMergeFlags;

            public:
                nonvirtual void GetCellWordProcessorDatabases (
                    TextStore**                                 ts,
                    StandardStyledTextImager::StyleDatabasePtr* styleDatabase       = nullptr,
                    WordProcessor::ParagraphDatabasePtr*        paragraphDatabase   = nullptr,
                    WordProcessor::HidableTextDatabasePtr*      hidableTextDatabase = nullptr);
                nonvirtual TextStore& GetTextStore () const;
                nonvirtual StandardStyledTextImager::StyleDatabasePtr GetStyleDatabase () const;
                nonvirtual WordProcessor::ParagraphDatabasePtr GetParagraphDatabase () const;
                nonvirtual WordProcessor::HidableTextDatabasePtr GetHidableTextDatabase () const;

                nonvirtual Led_Color GetBackColor () const;
                nonvirtual void      SetBackColor (Led_Color c);

                nonvirtual Led_TWIPS GetCellXWidth () const;
                nonvirtual void      SetCellXWidth (Led_TWIPS width);

                nonvirtual Led_Rect GetCachedBoundsRect () const;
                nonvirtual void     SetCachedBoundsRect (Led_Rect r);

            public:
                shared_ptr<CellRep> fCellRep;
            };

            /*
            @CLASS:         WordProcessor::Table::CellRep
            @DESCRIPTION:   <p>Used internally by the @'WordProcessor::Table' code.</p>
            */
            class WordProcessor::Table::CellRep : public MarkerOwner {
            private:
                using inherited = MarkerOwner;

            public:
                CellRep (Table& forTable);
                ~CellRep ();

            public:
                DECLARE_USE_BLOCK_ALLOCATION (CellRep);

            public:
                virtual TextStore* PeekAtTextStore () const override;
                virtual void       AboutToUpdateText (const UpdateInfo& updateInfo) override;
                virtual void       DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

            public:
                Table&                                     fForTable;
                TextStore*                                 fTextStore;
                StandardStyledTextImager::StyleDatabasePtr fStyleDatabase;
                WordProcessor::ParagraphDatabasePtr        fParagraphDatabase;
                WordProcessor::HidableTextDatabasePtr      fHidableTextDatabase;
                Led_Color                                  fBackColor;
                Led_Rect                                   fCachedBoundsRect;
                Led_TWIPS                                  fCellXWidth;
            };

            /*
            @CLASS:         WordProcessor::Table::SuppressCellUpdatePropagationContext
            @ACCESS:        private
            @DESCRIPTION:   <p>.</p>
            */
            class WordProcessor::Table::SuppressCellUpdatePropagationContext {
            public:
                SuppressCellUpdatePropagationContext (Table& t);
                ~SuppressCellUpdatePropagationContext ();

            private:
                Table& fTable;
                bool   fOldVal;
            };

            /*
            @CLASS:         WordProcessor::Table::AllowUpdateInfoPropagationContext
            @ACCESS:        protected
            @DESCRIPTION:   <p>.</p>
            */
            class WordProcessor::Table::AllowUpdateInfoPropagationContext {
            public:
                AllowUpdateInfoPropagationContext (Table& t);
                ~AllowUpdateInfoPropagationContext ();

            private:
                Table& fTable;
                bool   fOldVal;
            };

            /*
            @CLASS:         WordProcessor::Table::EmbeddedTableWordProcessor
            @BASES:         @'WordProcessor'
            @DESCRIPTION:   <p>Used internally by the @'WordProcessor::Table' code for mini embedded word processor objects
                        in each cell. A subclass of these are constructed by @'WordProcessor::ConstructEmbeddedTableWordProcessor'</p>
                            <p>Note that we choose to instantiate this WP object with a WindowRect in the same coordinates as
                        the the owning tables WindowRect: not relative to that table.
                        </p>
            */
            class WordProcessor::Table::EmbeddedTableWordProcessor : public WordProcessor {
            private:
                using inherited = WordProcessor;

            public:
                EmbeddedTableWordProcessor (WordProcessor& owningWordProcessor, Table& owningTable, size_t tRow, size_t tCol, bool activeEditCell);

            public:
                nonvirtual WordProcessor& GetOwningWordProcessor () const;
                nonvirtual Table& GetOwningTable () const;

            private:
                WordProcessor& fOwningWordProcessor;
                Table&         fOwningTable;
                size_t         fTableRow;
                size_t         fTableColumn;

            public:
                nonvirtual void SaveMiscActiveFocusInfo ();
                nonvirtual void RestoreMiscActiveFocusInfo ();

#if !qNestedTablesSupported
            protected:
                virtual void HookInternalizerChanged () override;
#endif

            protected:
                virtual bool OnCopyCommand_Before () override;
                virtual void OnCopyCommand_After () override;
                virtual bool OnPasteCommand_Before () override;
                virtual void OnPasteCommand_After () override;

            protected:
                virtual void DrawRowHilight (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
                                             const TextLayoutBlock& text, size_t rowStart, size_t rowEnd) override;

                //pure virtual overrides to make this class compile..
            public:
                virtual Led_Tablet AcquireTablet () const override;
                virtual void       ReleaseTablet (Led_Tablet tablet) const override;

            protected:
                virtual void RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const override;
                virtual void UpdateWindowRect_ (const Led_Rect& windowRectArea) const override;
                virtual bool QueryInputKeyStrokesPending () const override;

            protected:
                virtual void SetDefaultUpdateMode (UpdateMode defaultUpdateMode) override;

            public:
                virtual void GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const override;

            public:
                nonvirtual Led_Distance GetDesiredHeight () const;

            private:
                mutable Led_Distance fDesiredHeight;
                mutable bool         fDesiredHeightValid;
                bool                 fActiveEditCell;

            public:
                virtual void PostInteractiveUndoPostHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, InteractiveReplaceCommand::SavedTextRep** afterRep, size_t startOfInsert, const Led_SDK_String& cmdName) override;

            protected:
                virtual InteractiveReplaceCommand::SavedTextRep* InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd) override;

            private:
                class DisableRefreshContext;

            private:
                bool fSuppressRefreshCalls;

            private:
#if qAccessChecksSometimesBreakForNestedClasses
            public:
#endif
                class TemporarilyUseTablet;

            private:
                Led_Tablet fUpdateTablet; // assigned in stack-based fasion during update/draw calls.
            private:
                friend class WordProcessor::Table;
                friend class WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet;
                friend class WordProcessor::Table::EmbeddedTableWordProcessor::DisableRefreshContext;
                friend class WordProcessor::Table::TemporarilyAllocateCellWithTablet;
            };

            /*
            @CLASS:         InteractiveReplaceCommand::SavedTextRep
            @BASES:         @'WordProcessor::Table::SavedTextRepWSel'
            @DESCRIPTION:
            */
            class WordProcessor::Table::SavedTextRepWSel : public InteractiveReplaceCommand::SavedTextRep {
            private:
                using inherited = InteractiveReplaceCommand::SavedTextRep;

            public:
                DECLARE_USE_BLOCK_ALLOCATION (SavedTextRepWSel);

            public:
                enum WPRelativeFlag { eWPDirect,
                                      eWPAbove };

            public:
                SavedTextRepWSel (SavedTextRep* delegateTo, Table& table, WPRelativeFlag wPRelativeFlag);

            public:
                virtual size_t GetLength () const override;
                virtual void   InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite) override;
                virtual void   ApplySelection (TextInteractor* imager) override;

            private:
                WPRelativeFlag           fWPRelativeFlag;
                unique_ptr<SavedTextRep> fRealRep;
                size_t                   fRowSelStart;
                size_t                   fRowSelEnd;
                size_t                   fColSelStart;
                size_t                   fColSelEnd;
                bool                     fIntraCellMode;
                size_t                   fIntraCellSelStart;
                size_t                   fIntraCellSelEnd;
            };

            /*
            @CLASS:         WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet
            @DESCRIPTION:   <p>Utility class to use (with caution), to temporarily force a given tablet to be
                        used for a given @'WordProcessor::Table::EmbeddedTableWordProcessor'. NB: This causes
                        the @'TextImager::TabletChangedMetrics' method by default
                        (unless called with special arg).</p>
            */
            class WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet {
            public:
                using EmbeddedTableWordProcessor = WordProcessor::Table::EmbeddedTableWordProcessor;

            public:
                enum DoTextMetricsChangedCall { eDoTextMetricsChangedCall,
                                                eDontDoTextMetricsChangedCall };
                TemporarilyUseTablet (EmbeddedTableWordProcessor& editor, Led_Tablet t, DoTextMetricsChangedCall tmChanged = eDoTextMetricsChangedCall);
                ~TemporarilyUseTablet ();

            private:
                EmbeddedTableWordProcessor& fEditor;
                Led_Tablet                  fOldTablet;
                DoTextMetricsChangedCall    fDoTextMetricsChangedCall;
            };

            /*
            @CLASS:         WordProcessor::Table::EmbeddedTableWordProcessor::DisableRefreshContext
            @ACCESS:        private
            @DESCRIPTION:   <p>Utility class to use (with caution), to temporarily force a given tablet to be
                        used for a given @'WordProcessor::Table::EmbeddedTableWordProcessor'. NB: This causes
                        the @'TextImager::TabletChangedMetrics' method by default
                        (unless called with special arg).</p>
            */
            class WordProcessor::Table::EmbeddedTableWordProcessor::DisableRefreshContext {
            public:
                DisableRefreshContext (EmbeddedTableWordProcessor& wp);
                ~DisableRefreshContext ();

            private:
                EmbeddedTableWordProcessor& fWP;
                bool                        fOldVal;
            };

            /*
            @CLASS:         WordProcessor::Table::TemporarilySetOwningWP
            @DESCRIPTION:   <p>Since a table is assocated with a paragraph database, and can be viewed and edited
                        simultaneously be multiple views (WordProcessors) - we cannot make long-term assumptions
                        about association of a table with an owning WordProcessor object. Instead - during UI operations
                        like typing and so on - we temporarily associate an owning WP with the given table using
                        this class.</p>
            */
            class WordProcessor::Table::TemporarilySetOwningWP {
            public:
                TemporarilySetOwningWP (const Table& forTable, WordProcessor& forWordProcessor);
                ~TemporarilySetOwningWP ();

            private:
                Table&         fOwningTable;
                WordProcessor* fSavedTableOwningWP; // in case references are nested, though I'm not sure this can happen
            };

            /*
            @CLASS:         WordProcessor::Table::TemporarilyAllocateCellWP
            @DESCRIPTION:   <p></p>
            */
            class WordProcessor::Table::TemporarilyAllocateCellWP {
            public:
                using EmbeddedTableWordProcessor = WordProcessor::Table::EmbeddedTableWordProcessor;

            public:
                TemporarilyAllocateCellWP (Table& forTable, WordProcessor& forWordProcessor, size_t forRow, size_t forColumn, const Led_Rect& cellWindowRect, bool captureChangesForUndo = true);
                ~TemporarilyAllocateCellWP ();

                nonvirtual operator EmbeddedTableWordProcessor* ();
                nonvirtual EmbeddedTableWordProcessor& operator& ();
                nonvirtual EmbeddedTableWordProcessor* operator-> ();

            private:
                Table&                      fOwningTable;
                EmbeddedTableWordProcessor* fCellEditor;
            };

            /*
            @CLASS:         WordProcessor::Table::TemporarilyAllocateCellWithTablet
            @DESCRIPTION:   <p></p>
            */
            class WordProcessor::Table::TemporarilyAllocateCellWithTablet {
            public:
                using Tablet_Acquirer = TextInteractor::Tablet_Acquirer; // needed for GCC 2.96 - seems like the requirement maybe a compiler bug... LGP 2003-04-18

            public:
                TemporarilyAllocateCellWithTablet (Table& forTable, size_t row, size_t column, bool captureChangesForUndo = true);

            public:
                using TemporarilyUseTablet = EmbeddedTableWordProcessor::TemporarilyUseTablet;

            public:
                nonvirtual operator EmbeddedTableWordProcessor* ();
                nonvirtual EmbeddedTableWordProcessor& operator& ();
                nonvirtual EmbeddedTableWordProcessor* operator-> ();

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
                Led_SDK_String fJustificationCommandName;
                Led_SDK_String fStandardTabStopListCommandName;
                Led_SDK_String fMarginsCommandName;
                Led_SDK_String fFirstIndentCommandName;
                Led_SDK_String fMarginsAndFirstIndentCommandName;
                Led_SDK_String fParagraphSpacingCommandName;
                Led_SDK_String fHideCommandName;
                Led_SDK_String fUnHideCommandName;
                Led_SDK_String fSetListStyleCommandName;
                Led_SDK_String fIndentLevelChangeCommandName;
                Led_SDK_String fInsertTableCommandName;
                Led_SDK_String fInsertTableRowAboveCommandName;
                Led_SDK_String fInsertTableRowBelowCommandName;
                Led_SDK_String fInsertTableColBeforeCommandName;
                Led_SDK_String fInsertTableColAfterCommandName;
                Led_SDK_String fInsertURLCommandName;
                Led_SDK_String fRemoveTableRowsCommandName;
                Led_SDK_String fRemoveTableColumnsCommandName;
                Led_SDK_String fEmbeddingTypeName_ImageDIB;
                Led_SDK_String fEmbeddingTypeName_URL;
                Led_SDK_String fEmbeddingTypeName_ImageMacPict;
                Led_SDK_String fEmbeddingTypeName_Table;
                Led_SDK_String fEmbeddingTypeName_Unknown;
                Led_SDK_String fFontSizeChange_Other_NoArg;
                Led_SDK_String fFontSizeChange_Other_OneArg;
                Led_SDK_String fTablePropertiesCommandName;
                Led_SDK_String fGenericEmbeddingPropertiesCommandName;
                Led_SDK_String fChangeTablePropertiesCommandName;
            };

            /*
            @CLASS:         WordProcessor::DialogSupport
            @DESCRIPTION:   <p></p>
            */
            class WordProcessor::DialogSupport {
            public:
                using CommandNumber     = TextInteractor::CommandNumber;
                using FontNameSpecifier = Led_FontSpecification::FontNameSpecifier;

            public:
                virtual FontNameSpecifier CmdNumToFontName (CommandNumber cmdNum);
                virtual bool              IsPredefinedFontSize (Led_Distance fontSize); // return true iff the arg fontSize is one of the predefined ones from the menu
                virtual Led_Distance      FontCmdToSize (CommandNumber commandNum);
                virtual Led_Distance      PickOtherFontHeight (Led_Distance origHeight); // display UI (dialog box) and query user for new height)
                virtual bool              PickNewParagraphLineSpacing (Led_TWIPS* spaceBefore, bool* spaceBeforeValid, Led_TWIPS* spaceAfter, bool* spaceAfterValid, Led_LineSpacing* lineSpacing, bool* lineSpacingValid);
                virtual bool              PickNewParagraphMarginsAndFirstIndent (Led_TWIPS* leftMargin, bool* leftMarginValid, Led_TWIPS* rightMargin, bool* rightMarginValid, Led_TWIPS* firstIndent, bool* firstIndentValid);
                virtual Led_Color         FontCmdToColor (CommandNumber cmd);
                virtual CommandNumber     FontColorToCmd (Led_Color color);
                virtual bool              PickOtherFontColor (Led_Color* color);
                virtual bool              ChooseFont (Led_IncrementalFontSpecification* font); // display UI (e.g. CFontDialog) to pick a new font/color
                virtual void              ShowSimpleEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName);
                virtual bool              ShowURLEmbeddingInfoDialog (const Led_SDK_String& embeddingTypeName, Led_SDK_String* urlTitle, Led_SDK_String* urlValue);
                virtual bool              ShowAddURLEmbeddingInfoDialog (Led_SDK_String* urlTitle, Led_SDK_String* urlValue);
                virtual bool              AddNewTableDialog (size_t* nRows, size_t* nCols);
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

                Led_TWIPS fTableBorderWidth;
                Led_Color fTableBorderColor;

                Led_TWIPS_Rect fDefaultCellMargins;
                Led_TWIPS      fCellSpacing;

                bool      fCellWidth_Common;
                Led_TWIPS fCellWidth;

                bool      fCellBackgroundColor_Common;
                Led_Color fCellBackgroundColor;
            };

            /*
            @CLASS:         EmptySelectionParagraphSavedTextRep<BASECLASS>
            @BASES:         @'StandardStyledTextInteractor::EmptySelStyleTextRep'
            @DESCRIPTION:
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

            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */
        }
    }
}
#include <cstdio> // for sprintf() call
namespace Stroika {
    namespace Frameworks {
        namespace Led {

#if qTemplateGeneratedMixinsSometimesCorrupted
            inline void InteractorInteractorMixinHelper<StandardStyledTextInteractor, WordWrappedTextInteractor>::HookLosingTextStore ()
            {
                StandardStyledTextInteractor::HookLosingTextStore ();
                WordWrappedTextInteractor::HookLosingTextStore ();
            }
            inline void InteractorInteractorMixinHelper<StandardStyledTextInteractor, WordWrappedTextInteractor>::HookGainedNewTextStore ()
            {
                StandardStyledTextInteractor::HookGainedNewTextStore ();
                WordWrappedTextInteractor::HookGainedNewTextStore ();
            }
            inline void InteractorInteractorMixinHelper<StandardStyledTextInteractor, WordWrappedTextInteractor>::DidUpdateText (const UpdateInfo& updateInfo)
            {
                StandardStyledTextInteractor::DidUpdateText (updateInfo);
                WordWrappedTextInteractor::DidUpdateText (updateInfo);
            }
#endif

            //  class   StandardStyledWordWrappedTextInteractor
            inline StandardStyledWordWrappedTextInteractor::StandardStyledWordWrappedTextInteractor ()
                : inherited ()
            {
            }

            //  class   WordProcessor::ParagraphInfo
            /*
            @METHOD:        WordProcessor::ParagraphInfo::GetJustification
            @DESCRIPTION:
                <p>Return the @'Led_Justification' setting.</p>
            */
            inline Led_Justification WordProcessor::ParagraphInfo::GetJustification () const
            {
                return fJustification;
            }
            /*
            @METHOD:        WordProcessor::ParagraphInfo::SetJustification
            @DESCRIPTION:
                <p>Set the @'Led_Justification' setting.</p>
            */
            inline void WordProcessor::ParagraphInfo::SetJustification (Led_Justification justification)
            {
                fJustification = justification;
            }
            inline const WordProcessor::ParagraphInfo::StandardTabStopList& WordProcessor::ParagraphInfo::GetTabStopList () const
            {
                return fTabStops;
            }
            inline void WordProcessor::ParagraphInfo::SetTabStopList (const StandardTabStopList& tabStops)
            {
                fTabStops = tabStops;
            }
            inline Led_TWIPS WordProcessor::ParagraphInfo::GetLeftMargin () const
            {
                return fLeftMargin;
            }
            inline Led_TWIPS WordProcessor::ParagraphInfo::GetRightMargin () const
            {
                return fRightMargin;
            }
            inline void WordProcessor::ParagraphInfo::SetMargins (Led_TWIPS lhs, Led_TWIPS rhs)
            {
                Require (lhs < rhs);
                fLeftMargin  = lhs;
                fRightMargin = rhs;
            }
            inline Led_TWIPS WordProcessor::ParagraphInfo::GetFirstIndent () const
            {
                return fFirstIndent;
            }
            inline void WordProcessor::ParagraphInfo::SetFirstIndent (Led_TWIPS firstIndent)
            {
                fFirstIndent = firstIndent;
            }
            /*
            @METHOD:        WordProcessor::ParagraphInfo::GetSpaceBefore
            @DESCRIPTION:
                            <p>Get the 'space before' attribute of the given paragraph. This is the number of TWIPS of space inserted before
                        the paragraph - and defaults to zero. See the RTF \sb tag.</p>
                            <p>See also @'WordProcessor::ParagraphInfo::SetSpaceBefore'.</p>
            */
            inline Led_TWIPS WordProcessor::ParagraphInfo::GetSpaceBefore () const
            {
                return fSpaceBefore;
            }
            /*
            @METHOD:        WordProcessor::ParagraphInfo::SetSpaceBefore
            @DESCRIPTION:
                    <p>See also @'WordProcessor::ParagraphInfo::GetSpaceBefore'.</p>
            */
            inline void WordProcessor::ParagraphInfo::SetSpaceBefore (Led_TWIPS sb)
            {
                fSpaceBefore = sb;
            }
            /*
            @METHOD:        WordProcessor::ParagraphInfo::GetSpaceAfter
            @DESCRIPTION:
                    <p>Get the 'space after' attribute of the given paragraph. This is the number of TWIPS of space appended after
                the paragraph - and defaults to zero. See the RTF \sa tag.</p>
                    <p>See also @'WordProcessor::ParagraphInfo::SetSpaceAfter'.</p>
            */
            inline Led_TWIPS WordProcessor::ParagraphInfo::GetSpaceAfter () const
            {
                return fSpaceAfter;
            }
            /*
            @METHOD:        WordProcessor::ParagraphInfo::SetSpaceAfter
            @DESCRIPTION:
                    <p>See also @'WordProcessor::ParagraphInfo::GetSpaceAfter'.</p>
            */
            inline void WordProcessor::ParagraphInfo::SetSpaceAfter (Led_TWIPS sa)
            {
                fSpaceAfter = sa;
            }
            /*
            @METHOD:        WordProcessor::ParagraphInfo::GetLineSpacing
            @DESCRIPTION:
                    <p>Get the spacing used between rows of a given paragraph. Can either be magic value of 1000 - meaning automatic.</p>
                    <p>From the RTF docs:
                        <blockquote>
                        If this control word is missing or if \sl1000 is used, the line spacing is automatically
                        determined by the tallest character in the line; if N is a positive value, this size is
                        used only if it is taller than the tallest character (otherwise, the tallest character is used);
                        if N is a negative value, the absolute value of N is used, even if it is shorter than the tallest character.
                        </blockquote>
                    </p>
                    <p>See the RTF docs for \slN and \slmultN</p>
                    <p>See also @'WordProcessor::ParagraphInfo::SetLineSpacing'.</p>
            */
            inline Led_LineSpacing WordProcessor::ParagraphInfo::GetLineSpacing () const
            {
                return fLineSpacing;
            }
            /*
            @METHOD:        WordProcessor::ParagraphInfo::SetLineSpacing
            @DESCRIPTION:
                    <p>See also @'WordProcessor::ParagraphInfo::GetLineSpacing'.</p>
            */
            inline void WordProcessor::ParagraphInfo::SetLineSpacing (Led_LineSpacing sl)
            {
                fLineSpacing = sl;
            }
            /*
            @METHOD:        WordProcessor::ParagraphInfo::GetListStyle
            @DESCRIPTION:
                    <p>Retrieve the @'WordProcessor::ParagraphInfo::ListStyle' property associated with this paragraph
                (e.g. eListStyle_Bullet or eListStyle_None).</p>
            */
            inline ListStyle WordProcessor::ParagraphInfo::GetListStyle () const
            {
                return fListStyle;
            }
            /*
            @METHOD:        WordProcessor::ParagraphInfo::SetListStyle
            @DESCRIPTION:
                    <p>See also @'WordProcessor::ParagraphInfo::GetListStyle'.</p>
            */
            inline void WordProcessor::ParagraphInfo::SetListStyle (ListStyle listStyle)
            {
                fListStyle = listStyle;
            }
            /*
            @METHOD:        WordProcessor::ParagraphInfo::GetListIndentLevel
            @DESCRIPTION:
                    <p>Retrieve the indent level for the given list. Only valid value if @'WordProcessor::ParagraphInfo::GetListStyle'
                is not eListStyle_None.</p>
            */
            inline unsigned char WordProcessor::ParagraphInfo::GetListIndentLevel () const
            {
                return fListIndentLevel;
            }
            /*
            @METHOD:        WordProcessor::ParagraphInfo::SetListIndentLevel
            @DESCRIPTION:
                    <p>See also @'WordProcessor::ParagraphInfo::GetListIndentLevel'.</p>
            */
            inline void WordProcessor::ParagraphInfo::SetListIndentLevel (unsigned char indentLevel)
            {
                fListIndentLevel = indentLevel;
            }
            inline void WordProcessor::ParagraphInfo::MergeIn (const IncrementalParagraphInfo& incParaInfo)
            {
                if (incParaInfo.GetJustification_Valid ()) {
                    fJustification = incParaInfo.GetJustification ();
                }
                if (incParaInfo.GetTabStopList_Valid ()) {
                    fTabStops = incParaInfo.GetTabStopList ();
                }
                if (incParaInfo.GetMargins_Valid ()) {
                    fLeftMargin  = incParaInfo.GetLeftMargin ();
                    fRightMargin = incParaInfo.GetRightMargin ();
                }
                if (incParaInfo.GetFirstIndent_Valid ()) {
                    fFirstIndent = incParaInfo.GetFirstIndent ();
                }
                if (incParaInfo.GetSpaceBefore_Valid ()) {
                    fSpaceBefore = incParaInfo.GetSpaceBefore ();
                }
                if (incParaInfo.GetSpaceAfter_Valid ()) {
                    fSpaceAfter = incParaInfo.GetSpaceAfter ();
                }
                if (incParaInfo.GetLineSpacing_Valid ()) {
                    fLineSpacing = incParaInfo.GetLineSpacing ();
                }
                if (incParaInfo.GetListStyle_Valid ()) {
                    fListStyle = incParaInfo.GetListStyle ();
                }
                if (incParaInfo.GetListIndentLevel_Valid ()) {
                    fListIndentLevel = incParaInfo.GetListIndentLevel ();
                }
            }
            inline bool WordProcessor::ParagraphInfo::operator== (const ParagraphInfo& rhs) const
            {
                return fJustification == rhs.fJustification and
                       fTabStops == rhs.fTabStops and
                       fLeftMargin == rhs.fLeftMargin and
                       fRightMargin == rhs.fRightMargin and
                       fFirstIndent == rhs.fFirstIndent and
                       fSpaceBefore == rhs.fSpaceBefore and
                       fSpaceAfter == rhs.fSpaceAfter and
                       fLineSpacing == rhs.fLineSpacing and
                       fListStyle == rhs.fListStyle and
                       fListIndentLevel == rhs.fListIndentLevel;
            }
            inline bool WordProcessor::ParagraphInfo::operator!= (const ParagraphInfo& rhs) const
            {
                return not(*this == rhs);
            }

            //  class   IncrementalParagraphInfo;
            inline WordProcessor::IncrementalParagraphInfo::IncrementalParagraphInfo ()
                : inherited ()
                , fJustificationValid (false)
                , fTabStopListValid (false)
                , fMarginsValid (false)
                , fFirstIndentValid (false)
                , fSpaceBeforeValid (false)
                , fSpaceAfterValid (false)
                , fLineSpacingValid (false)
                , fListStyleValid (false)
                , fListIndentLevelValid (false)
            {
            }
            inline WordProcessor::IncrementalParagraphInfo::IncrementalParagraphInfo (const ParagraphInfo& pi)
                : inherited (pi)
                , fJustificationValid (true)
                , fTabStopListValid (true)
                , fMarginsValid (true)
                , fFirstIndentValid (true)
                , fSpaceBeforeValid (true)
                , fSpaceAfterValid (true)
                , fLineSpacingValid (true)
                , fListStyleValid (true)
                , fListIndentLevelValid (true)
            {
            }
            inline Led_Justification WordProcessor::IncrementalParagraphInfo::GetJustification () const
            {
                Require (fJustificationValid);
                return inherited::GetJustification ();
            }
            inline void WordProcessor::IncrementalParagraphInfo::SetJustification (Led_Justification justification)
            {
                fJustificationValid = true;
                inherited::SetJustification (justification);
            }
            inline bool WordProcessor::IncrementalParagraphInfo::GetJustification_Valid () const
            {
                return fJustificationValid;
            }
            inline void WordProcessor::IncrementalParagraphInfo::InvalidateJustification ()
            {
                fJustificationValid = false;
            }
            inline const TextImager::StandardTabStopList& WordProcessor::IncrementalParagraphInfo::GetTabStopList () const
            {
                Require (fTabStopListValid);
                return inherited::GetTabStopList ();
            }
            inline void WordProcessor::IncrementalParagraphInfo::SetTabStopList (const StandardTabStopList& tabStops)
            {
                fTabStopListValid = true;
                inherited::SetTabStopList (tabStops);
            }
            inline bool WordProcessor::IncrementalParagraphInfo::GetTabStopList_Valid () const
            {
                return fTabStopListValid;
            }
            inline void WordProcessor::IncrementalParagraphInfo::InvalidateTabStopList ()
            {
                fTabStopListValid = false;
            }
            inline Led_TWIPS WordProcessor::IncrementalParagraphInfo::GetLeftMargin () const
            {
                Require (fMarginsValid);
                return inherited::GetLeftMargin ();
            }
            inline Led_TWIPS WordProcessor::IncrementalParagraphInfo::GetRightMargin () const
            {
                Require (fMarginsValid);
                return inherited::GetRightMargin ();
            }
            inline void WordProcessor::IncrementalParagraphInfo::SetMargins (Led_TWIPS lhs, Led_TWIPS rhs)
            {
                fMarginsValid = true;
                inherited::SetMargins (lhs, rhs);
            }
            inline bool WordProcessor::IncrementalParagraphInfo::GetMargins_Valid () const
            {
                return fMarginsValid;
            }
            inline void WordProcessor::IncrementalParagraphInfo::InvalidateMargins ()
            {
                fMarginsValid = false;
            }
            inline Led_TWIPS WordProcessor::IncrementalParagraphInfo::GetFirstIndent () const
            {
                Require (fFirstIndentValid);
                return inherited::GetFirstIndent ();
            }
            inline void WordProcessor::IncrementalParagraphInfo::SetFirstIndent (Led_TWIPS firstIndent)
            {
                fFirstIndentValid = true;
                inherited::SetFirstIndent (firstIndent);
            }
            inline bool WordProcessor::IncrementalParagraphInfo::GetFirstIndent_Valid () const
            {
                return fFirstIndentValid;
            }
            inline void WordProcessor::IncrementalParagraphInfo::InvalidateFirstIndent ()
            {
                fFirstIndentValid = false;
            }
            inline Led_TWIPS WordProcessor::IncrementalParagraphInfo::GetSpaceBefore () const
            {
                Require (fSpaceBeforeValid);
                return inherited::GetSpaceBefore ();
            }
            inline void WordProcessor::IncrementalParagraphInfo::SetSpaceBefore (Led_TWIPS sb)
            {
                fSpaceBeforeValid = true;
                inherited::SetSpaceBefore (sb);
            }
            inline bool WordProcessor::IncrementalParagraphInfo::GetSpaceBefore_Valid () const
            {
                return fSpaceBeforeValid;
            }
            inline void WordProcessor::IncrementalParagraphInfo::InvalidateSpaceBefore ()
            {
                fSpaceBeforeValid = false;
            }
            inline Led_TWIPS WordProcessor::IncrementalParagraphInfo::GetSpaceAfter () const
            {
                Require (fSpaceAfterValid);
                return inherited::GetSpaceAfter ();
            }
            inline void WordProcessor::IncrementalParagraphInfo::SetSpaceAfter (Led_TWIPS sa)
            {
                fSpaceAfterValid = true;
                inherited::SetSpaceAfter (sa);
            }
            inline bool WordProcessor::IncrementalParagraphInfo::GetSpaceAfter_Valid () const
            {
                return fSpaceAfterValid;
            }
            inline void WordProcessor::IncrementalParagraphInfo::InvalidateSpaceAfter ()
            {
                fSpaceAfterValid = false;
            }
            inline Led_LineSpacing WordProcessor::IncrementalParagraphInfo::GetLineSpacing () const
            {
                Require (fLineSpacingValid);
                return inherited::GetLineSpacing ();
            }
            inline void WordProcessor::IncrementalParagraphInfo::SetLineSpacing (Led_LineSpacing sl)
            {
                fLineSpacingValid = true;
                inherited::SetLineSpacing (sl);
            }
            inline bool WordProcessor::IncrementalParagraphInfo::GetLineSpacing_Valid () const
            {
                return fLineSpacingValid;
            }
            inline void WordProcessor::IncrementalParagraphInfo::InvalidateLineSpacing ()
            {
                fLineSpacingValid = false;
            }
            inline ListStyle WordProcessor::IncrementalParagraphInfo::GetListStyle () const
            {
                Require (fListStyleValid);
                return inherited::GetListStyle ();
            }
            inline void WordProcessor::IncrementalParagraphInfo::SetListStyle (ListStyle listStyle)
            {
                fListStyleValid = true;
                inherited::SetListStyle (listStyle);
            }
            inline bool WordProcessor::IncrementalParagraphInfo::GetListStyle_Valid () const
            {
                return fListStyleValid;
            }
            inline void WordProcessor::IncrementalParagraphInfo::InvalidateListStyle ()
            {
                fListStyleValid = false;
            }
            inline unsigned char WordProcessor::IncrementalParagraphInfo::GetListIndentLevel () const
            {
                Require (fListIndentLevelValid);
                return inherited::GetListIndentLevel ();
            }
            inline void WordProcessor::IncrementalParagraphInfo::SetListIndentLevel (unsigned char indentLevel)
            {
                fListIndentLevelValid = true;
                inherited::SetListIndentLevel (indentLevel);
            }
            inline bool WordProcessor::IncrementalParagraphInfo::GetListIndentLevel_Valid () const
            {
                return fListIndentLevelValid;
            }
            inline void WordProcessor::IncrementalParagraphInfo::InvalidateListIndentLevel ()
            {
                fListIndentLevelValid = false;
            }
            inline bool WordProcessor::IncrementalParagraphInfo::operator== (const IncrementalParagraphInfo& rhs) const
            {
                //Justification
                if (GetJustification_Valid () != rhs.GetJustification_Valid ()) {
                    return false;
                }
                if (GetJustification_Valid () and (GetJustification () != rhs.GetJustification ())) {
                    return false;
                }

                //TabStopList
                if (GetTabStopList_Valid () != rhs.GetTabStopList_Valid ()) {
                    return false;
                }
                if (GetTabStopList_Valid () and (GetTabStopList () != rhs.GetTabStopList ())) {
                    return false;
                }

                //Margins
                if (GetMargins_Valid () != rhs.GetMargins_Valid ()) {
                    return false;
                }
                if (GetMargins_Valid () and (GetLeftMargin () != rhs.GetLeftMargin () or GetRightMargin () != rhs.GetRightMargin ())) {
                    return false;
                }

                //FirstIndent
                if (GetFirstIndent_Valid () != rhs.GetFirstIndent_Valid ()) {
                    return false;
                }
                if (GetFirstIndent_Valid () and (GetFirstIndent () != rhs.GetFirstIndent ())) {
                    return false;
                }

                //SpaceBefore
                if (GetSpaceBefore_Valid () != rhs.GetSpaceBefore_Valid ()) {
                    return false;
                }
                if (GetSpaceBefore_Valid () and (GetSpaceBefore () != rhs.GetSpaceBefore ())) {
                    return false;
                }

                //SpaceAfter
                if (GetSpaceAfter_Valid () != rhs.GetSpaceAfter_Valid ()) {
                    return false;
                }
                if (GetSpaceAfter_Valid () and (GetSpaceAfter () != rhs.GetSpaceAfter ())) {
                    return false;
                }

                //LineSpacing
                if (GetLineSpacing_Valid () != rhs.GetLineSpacing_Valid ()) {
                    return false;
                }
                if (GetLineSpacing_Valid () and (GetLineSpacing () != rhs.GetLineSpacing ())) {
                    return false;
                }

                //ListStyle/IndentLevel
                if (GetListStyle_Valid () != rhs.GetListStyle_Valid ()) {
                    return false;
                }
                if (GetListStyle_Valid () and (GetListStyle () != rhs.GetListStyle ())) {
                    return false;
                }
                if (GetListIndentLevel_Valid () != rhs.GetListIndentLevel_Valid ()) {
                    return false;
                }
                if (GetListIndentLevel_Valid () and (GetListIndentLevel () != rhs.GetListIndentLevel ())) {
                    return false;
                }

                return true;
            }
            inline bool WordProcessor::IncrementalParagraphInfo::operator!= (const IncrementalParagraphInfo& rhs) const
            {
                return not(*this == rhs);
            }

            //  class   WordProcessor::AbstractParagraphDatabaseRep
            inline WordProcessor::AbstractParagraphDatabaseRep::AbstractParagraphDatabaseRep ()
                : fSomeInvalidTables (false)
                , fCachedFarthestRightMarginInDocument (WordProcessor::kBadCachedFarthestRightMarginInDocument)
            {
            }

            //  class   WordProcessor::ParagraphInfoMarker
            inline WordProcessor::ParagraphInfoMarker::ParagraphInfoMarker (ParagraphInfo paragraphInfo)
                : fParagraphInfo (paragraphInfo)
            {
            }
            inline const WordProcessor::ParagraphInfo& WordProcessor::ParagraphInfoMarker::GetInfo () const
            {
                return fParagraphInfo;
            }
            inline void WordProcessor::ParagraphInfoMarker::SetInfo (ParagraphInfo paragraphInfo)
            {
                fParagraphInfo = paragraphInfo;
            }

            //  class   WordProcessor::ParagraphDatabaseRep
            inline WordProcessor::ParagraphDatabaseRep::PartitionPtr WordProcessor::ParagraphDatabaseRep::GetPartition () const
            {
                return fPartition;
            }

            //  class   WordProcessor::WordProcessorTextIOSinkStream
            inline bool WordProcessor::WordProcessorTextIOSinkStream::GetOverwriteTableMode () const
            {
                return fOverwriteTableMode;
            }
            inline void WordProcessor::WordProcessorTextIOSinkStream::SetOverwriteTableMode (bool overwriteTableMode)
            {
                fOverwriteTableMode = overwriteTableMode;
            }
#if !qNestedTablesSupported
            inline bool WordProcessor::WordProcessorTextIOSinkStream::GetNoTablesAllowed () const
            {
                return fNoTablesAllowed;
            }
            inline void WordProcessor::WordProcessorTextIOSinkStream::SetNoTablesAllowed (bool noTablesAllowed)
            {
                fNoTablesAllowed = noTablesAllowed;
            }
#endif

            //  class   WordProcessor::WordProcessorTextIOSrcStream
            inline bool WordProcessor::WordProcessorTextIOSrcStream::GetUseTableSelection () const
            {
                return fUseTableSelection;
            }
            inline void WordProcessor::WordProcessorTextIOSrcStream::SetUseTableSelection (bool useTableSelection)
            {
                fUseTableSelection = useTableSelection;
            }

//  class   WordProcessor
#if qWideCharacters
            /*
            @METHOD:        WordProcessor::GetSmartQuoteMode
            @DESCRIPTION:   <p>If true, then when a user types a quote character (&quot;) - it will be replaced
                        with either an OPEN quote character or a CLOSE quote character (depending on text context).
                            <p>Note this defaults ON if in UNICODE mode (@'qWideCharacters') and is unavailable otherwise.</p>
                            <p>See also @'WordProcessor::SetSmartQuoteMode'</p>
            */
            inline bool WordProcessor::GetSmartQuoteMode () const
            {
                return fSmartQuoteMode;
            }
            /*
            @METHOD:        WordProcessor::SetSmartQuoteMode
            @DESCRIPTION:   @see     @'WordProcessor::GetSmartQuoteMode'</p>
            */
            inline void WordProcessor::SetSmartQuoteMode (bool smartQuoteMode)
            {
                fSmartQuoteMode = smartQuoteMode;
            }
#endif
            inline WordProcessor::ParagraphDatabasePtr WordProcessor::GetParagraphDatabase () const
            {
                return fParagraphDatabase;
            }
            inline WordProcessor::HidableTextDatabasePtr WordProcessor::GetHidableTextDatabase () const
            {
                return fHidableTextDatabase;
            }
            /*
            @METHOD:        WordProcessor::GetShowParagraphGlyphs
            @DESCRIPTION:   <p>See @'WordProcessor::SetShowParagraphGlyphs'.</p>
            */
            inline bool WordProcessor::GetShowParagraphGlyphs () const
            {
                return fShowParagraphGlyphs;
            }
            /*
            @METHOD:        WordProcessor::GetShowTabGlyphs
            @DESCRIPTION:   <p>See @'WordProcessor::SetShowTabGlyphs'.</p>
            */
            inline bool WordProcessor::GetShowTabGlyphs () const
            {
                return fShowTabGlyphs;
            }
            /*
            @METHOD:        WordProcessor::GetShowSpaceGlyphs
            @DESCRIPTION:   <p>See @'WordProcessor::SetShowSpaceGlyphs'.</p>
            */
            inline bool WordProcessor::GetShowSpaceGlyphs () const
            {
                return fShowSpaceGlyphs;
            }
            /*
            @METHOD:        WordProcessor::GetCommandNames
            @DESCRIPTION:   <p>Returns command name for each of the user-visible commands produced by this module.
                        This name is used used in the constructed Undo command name, as
                        in, "Undo Justification". You can replace this name with whatever you like.You change this value with
                        WordProcessor::SetCommandNames.</p>
                            <p> The point of this is to allow for different UI-language localizations,
                        without having to change Led itself.</p>
            *   @see    @'WordProcessor::CommandNames'.
            */
            inline const WordProcessor::CommandNames& WordProcessor::GetCommandNames ()
            {
                return sCommandNames;
            }
            /*
            @METHOD:        WordProcessor::SetCommandNames
            @DESCRIPTION:   <p>See @'WordProcessor::GetCommandNames'.</p>
            */
            inline void WordProcessor::SetCommandNames (const WordProcessor::CommandNames& cmdNames)
            {
                sCommandNames = cmdNames;
            }
            /*
            @METHOD:        TextInteractor::GetDialogSupport
            @DESCRIPTION:   <p></p>
            */
            inline WordProcessor::DialogSupport& WordProcessor::GetDialogSupport ()
            {
                if (sDialogSupport == nullptr) {
                    static DialogSupport sDefSup;
                    sDialogSupport = &sDefSup;
                }
                return *sDialogSupport;
            }
            /*
            @METHOD:        WordProcessor::SetDialogSupport
            @DESCRIPTION:   <p></p>
            */
            inline void WordProcessor::SetDialogSupport (DialogSupport* ds)
            {
                sDialogSupport = ds;
            }
            inline Led_IncrementalFontSpecification WordProcessor::GetCurSelFontSpec () const
            {
                if (not fCachedCurSelFontSpecValid) {
                    AssureCurSelFontCacheValid ();
                }
                return fCachedCurSelFontSpec;
            }

            //class WordProcessor::Table::RowInfo
            inline WordProcessor::Table::RowInfo::RowInfo ()
                : fHeight (0)
            {
            }

            //class WordProcessor::Table::Cell
            inline WordProcessor::Table::CellMergeFlags WordProcessor::Table::Cell::GetCellMergeFlags () const
            {
                return fCellMergeFlags;
            }
            inline Led_TWIPS WordProcessor::Table::Cell::GetCellXWidth () const
            {
                return fCellRep->fCellXWidth;
            }
            inline void WordProcessor::Table::Cell::SetCellXWidth (Led_TWIPS width)
            {
                fCellRep->fCellXWidth = width;
            }
            inline Led_Rect WordProcessor::Table::Cell::GetCachedBoundsRect () const
            {
                return fCellRep->fCachedBoundsRect;
            }
            inline void WordProcessor::Table::Cell::SetCachedBoundsRect (Led_Rect r)
            {
                fCellRep->fCachedBoundsRect = r;
            }

            //  class   WordProcessor::WordProcessorFlavorPackageInternalizer
            inline bool WordProcessor::WordProcessorFlavorPackageInternalizer::GetOverwriteTableMode () const
            {
                return fOverwriteTableMode;
            }
            inline void WordProcessor::WordProcessorFlavorPackageInternalizer::SetOverwriteTableMode (bool overwriteTableMode)
            {
                fOverwriteTableMode = overwriteTableMode;
            }
#if !qNestedTablesSupported
            inline bool WordProcessor::WordProcessorFlavorPackageInternalizer::GetNoTablesAllowed () const
            {
                return fNoTablesAllowed;
            }
            inline void WordProcessor::WordProcessorFlavorPackageInternalizer::SetNoTablesAllowed (bool noTablesAllowed)
            {
                fNoTablesAllowed = noTablesAllowed;
            }
#endif

            //  class   WordProcessor::WordProcessorFlavorPackageExternalizer
            inline bool WordProcessor::WordProcessorFlavorPackageExternalizer::GetUseTableSelection () const
            {
                return fUseTableSelection;
            }
            inline void WordProcessor::WordProcessorFlavorPackageExternalizer::SetUseTableSelection (bool useTableSelection)
            {
                fUseTableSelection = useTableSelection;
            }

            //class WordProcessor::Table
            inline Led_TWIPS WordProcessor::Table::GetCellSpacing () const
            {
                return fCellSpacing;
            }
            inline void WordProcessor::Table::SetCellSpacing (Led_TWIPS cellSpacing)
            {
                if (fCellSpacing != cellSpacing) {
                    fCellSpacing = cellSpacing;
                    InvalidateLayout ();
                }
            }
            inline void WordProcessor::Table::GetDefaultCellMargins (Led_TWIPS* top, Led_TWIPS* left, Led_TWIPS* bottom, Led_TWIPS* right) const
            {
                if (top != nullptr) {
                    *top = fDefaultCellMargins.GetTop ();
                }
                if (left != nullptr) {
                    *left = fDefaultCellMargins.GetLeft ();
                }
                if (bottom != nullptr) {
                    *bottom = fDefaultCellMargins.GetBottom ();
                }
                if (right != nullptr) {
                    *right = fDefaultCellMargins.GetRight ();
                }
            }
            inline void WordProcessor::Table::SetDefaultCellMargins (Led_TWIPS top, Led_TWIPS left, Led_TWIPS bottom, Led_TWIPS right)
            {
                if (top != fDefaultCellMargins.GetTop () or
                    left != fDefaultCellMargins.GetLeft () or
                    bottom != fDefaultCellMargins.GetBottom () or
                    right != fDefaultCellMargins.GetRight ()) {
                    fDefaultCellMargins.top    = top;
                    fDefaultCellMargins.left   = left;
                    fDefaultCellMargins.bottom = bottom;
                    fDefaultCellMargins.right  = right;
                    InvalidateLayout ();
                }
            }
            inline void WordProcessor::Table::InvalidateLayout ()
            {
                if (fNeedLayout != eNeedFullLayout) {
                    AbstractParagraphDatabaseRep* o = dynamic_cast<AbstractParagraphDatabaseRep*> (GetOwner ());
                    AssertNotNull (o);
                    o->fSomeInvalidTables = true;
                    fNeedLayout           = eNeedFullLayout;
                }
            }
            inline size_t WordProcessor::Table::GetRowCount () const
            {
                size_t rows = 0;
                GetDimensions (&rows, nullptr);
                return rows;
            }
            inline size_t WordProcessor::Table::GetColumnCount () const
            {
                size_t columns = 0;
                GetDimensions (nullptr, &columns);
                return columns;
            }
            inline WordProcessor::Table::Cell& WordProcessor::Table::GetCell (size_t row, size_t column)
            {
                Require (row < GetRowCount ());
                Require (column < GetColumnCount (row));
                Assert (fRows.size () == GetRowCount ());
                Assert (fRows[row].fCells.size () == GetColumnCount (row));
                return fRows[row].fCells[column];
            }
            inline const WordProcessor::Table::Cell& WordProcessor::Table::GetCell (size_t row, size_t column) const
            {
                Require (row < GetRowCount ());
                Require (column < GetColumnCount (row));
                Assert (fRows.size () == GetRowCount ());
                Assert (fRows[row].fCells.size () == GetColumnCount (row));
                return fRows[row].fCells[column];
            }
            inline WordProcessor::Table::CellMergeFlags WordProcessor::Table::GetCellFlags (size_t row, size_t column) const
            {
                Require (row < GetRowCount ());
                const RowInfo& rowInfo = fRows[row];
                if (column < rowInfo.fCells.size ()) {
                    return rowInfo.fCells[column].GetCellMergeFlags ();
                }
                else {
                    return eInvalidCell;
                }
            }
            /*
            @METHOD:        WordProcessor::Table::GetCellSelection
            @DESCRIPTION:   <p>Retrieve the cell selection range for the given table. Note that we always have a rectangular
                        table selection (could be in whole rows or columns or not). The special case of a single cell selection
                        may indicate that the ENTIRE cell is selected, or just a subset (which is decided by ITS GetCellSelection property).
                        </p>
                            <p>Like marker positions and STL iterators, we use the selEnd to be just PAST the end of the selected cell,
                        and so if rowSelStart==rowSelEnd then this implies NO SELECTION, and if rowSelStart + 1 == rowSelEnd and
                        colSelStart + 1 = colSelEnd then we have selected a single cell.
                        </p>
                            <p>See @'WordProcessor::Table::SetCellSelection'.</p>
            */
            inline void WordProcessor::Table::GetCellSelection (size_t* rowSelStart, size_t* rowSelEnd, size_t* colSelStart, size_t* colSelEnd) const
            {
                Ensure (fRowSelStart <= fRowSelEnd);
                Ensure (fRowSelEnd <= GetRowCount ());
                Ensure (fColSelStart <= fColSelEnd);
                Ensure (fColSelEnd <= GetColumnCount ());
                if (rowSelStart != nullptr) {
                    *rowSelStart = fRowSelStart;
                }
                if (rowSelEnd != nullptr) {
                    *rowSelEnd = fRowSelEnd;
                }
                if (colSelStart != nullptr) {
                    *colSelStart = fColSelStart;
                }
                if (colSelEnd != nullptr) {
                    *colSelEnd = fColSelEnd;
                }
            }
            /*
            @METHOD:        WordProcessor::Table::GetIntraCellMode
            @ACCESS:        public
            @DESCRIPTION:   <p>Return true if the editor is in 'intraCell' editing mode. This edit mode means that the
                        current selection is a mini-editor inside of a cell. This is not the same as having selected
                        a single cell. It means characters typed go to the focused WP inside the currently selected
                        cell.</p>
            */
            inline bool WordProcessor::Table::GetIntraCellMode (size_t* row, size_t* col) const
            {
                if (fIntraCellMode) {
                    Assert (fRowSelEnd == fRowSelStart + 1);
                    if (row != nullptr) {
                        *row = fRowSelStart;
                    }
                    Assert (fColSelEnd == fColSelStart + 1);
                    if (col != nullptr) {
                        *col = fColSelStart;
                    }
                }
                return fIntraCellMode;
            }
            inline void WordProcessor::Table::GetIntraCellSelection (size_t* selStart, size_t* selEnd) const
            {
                RequireNotNull (selStart);
                RequireNotNull (selEnd);
                *selStart = fIntraSelStart;
                *selEnd   = fIntraSelEnd;
            }
            inline void WordProcessor::Table::SaveIntraCellContextInfo (
                bool                         leftSideOfSelectionInteresting,
                const Led_FontSpecification& intraCellSelectionEmptySelFontSpecification)
            {
                fSavedIntraCellInfoValid                          = true;
                fSavedLeftSideOfSelectionInteresting              = leftSideOfSelectionInteresting;
                fSavedIntraCellSelectionEmptySelFontSpecification = intraCellSelectionEmptySelFontSpecification;
            }
            inline bool WordProcessor::Table::RestoreIntraCellContextInfo (
                bool*                  leftSideOfSelectionInteresting,
                Led_FontSpecification* intraCellSelectionEmptySelFontSpecification)
            {
                RequireNotNull (leftSideOfSelectionInteresting);
                RequireNotNull (intraCellSelectionEmptySelFontSpecification);
                if (fSavedIntraCellInfoValid) {
                    *leftSideOfSelectionInteresting              = fSavedLeftSideOfSelectionInteresting;
                    *intraCellSelectionEmptySelFontSpecification = fSavedIntraCellSelectionEmptySelFontSpecification;
                    return true;
                }
                else {
                    return false;
                }
            }
            inline void WordProcessor::Table::InvalidateIntraCellContextInfo ()
            {
                fSavedIntraCellInfoValid = false;
            }

            //class WordProcessor::Table::SuppressCellUpdatePropagationContext
            inline WordProcessor::Table::SuppressCellUpdatePropagationContext::SuppressCellUpdatePropagationContext (WordProcessor::Table& t)
                : fTable (t)
                , fOldVal (t.fSuppressCellUpdatePropagationContext)
            {
                t.fSuppressCellUpdatePropagationContext = true;
            }
            inline WordProcessor::Table::SuppressCellUpdatePropagationContext::~SuppressCellUpdatePropagationContext ()
            {
                fTable.fSuppressCellUpdatePropagationContext = fOldVal;
            }

            //class WordProcessor::Table::AllowUpdateInfoPropagationContext
            inline WordProcessor::Table::AllowUpdateInfoPropagationContext::AllowUpdateInfoPropagationContext (WordProcessor::Table& t)
                : fTable (t)
                , fOldVal (t.fAllowUpdateInfoPropagationContext)
            {
                t.fAllowUpdateInfoPropagationContext = true;
            }
            inline WordProcessor::Table::AllowUpdateInfoPropagationContext::~AllowUpdateInfoPropagationContext ()
            {
                fTable.fAllowUpdateInfoPropagationContext = fOldVal;
            }

            //class WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet
            inline WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet::TemporarilyUseTablet (WordProcessor::Table::EmbeddedTableWordProcessor& editor, Led_Tablet t, DoTextMetricsChangedCall tmChanged)
                : fEditor (editor)
                , fOldTablet (editor.fUpdateTablet)
                , fDoTextMetricsChangedCall (tmChanged)
            {
                editor.fUpdateTablet = t;
                if (tmChanged == eDoTextMetricsChangedCall) {
                    editor.TabletChangedMetrics ();
                }
            }
            inline WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet::~TemporarilyUseTablet ()
            {
                fEditor.fUpdateTablet = fOldTablet;
                if (fDoTextMetricsChangedCall == eDoTextMetricsChangedCall) {
                    fEditor.TabletChangedMetrics ();
                }
            }

            //class WordProcessor::Table::EmbeddedTableWordProcessor::DisableRefreshContext
            inline WordProcessor::Table::EmbeddedTableWordProcessor::DisableRefreshContext::DisableRefreshContext (WordProcessor::Table::EmbeddedTableWordProcessor& wp)
                : fWP (wp)
                , fOldVal (wp.fSuppressRefreshCalls)
            {
                wp.fSuppressRefreshCalls = true;
            }
            inline WordProcessor::Table::EmbeddedTableWordProcessor::DisableRefreshContext::~DisableRefreshContext ()
            {
                if (fWP.fSuppressRefreshCalls != fOldVal) {
                    fWP.fSuppressRefreshCalls = fOldVal;
                    if (not fWP.fSuppressRefreshCalls) {
                        fWP.NoteWindowPartiallyUpdated ();
                    }
                }
            }

            //class WordProcessor::Table::TemporarilySetOwningWP
            inline WordProcessor::Table::TemporarilySetOwningWP::TemporarilySetOwningWP (const Table& forTable, WordProcessor& forWordProcessor)
                : fOwningTable (const_cast<Table&> (forTable))
                , fSavedTableOwningWP (forTable.fCurrentOwningWP)
            {
                fOwningTable.fCurrentOwningWP = &forWordProcessor;
            }
            inline WordProcessor::Table::TemporarilySetOwningWP::~TemporarilySetOwningWP ()
            {
                fOwningTable.fCurrentOwningWP = fSavedTableOwningWP;
            }

            //class WordProcessor::Table::TemporarilyAllocateCellWP
            inline WordProcessor::Table::TemporarilyAllocateCellWP::TemporarilyAllocateCellWP (Table& forTable, WordProcessor& forWordProcessor, size_t forRow, size_t forColumn, const Led_Rect& cellWindowRect, bool captureChangesForUndo)
                : fOwningTable (forTable)
                , fCellEditor (forTable.ConstructEmbeddedTableWordProcessor (forWordProcessor, forRow, forColumn, cellWindowRect, captureChangesForUndo))
            {
            }
            inline WordProcessor::Table::TemporarilyAllocateCellWP::~TemporarilyAllocateCellWP ()
            {
                AssertNotNull (fCellEditor);
                fOwningTable.ReleaseEmbeddedTableWordProcessor (fCellEditor);
            }
            inline WordProcessor::Table::TemporarilyAllocateCellWP::operator WordProcessor::Table::EmbeddedTableWordProcessor* ()
            {
                AssertNotNull (fCellEditor);
                return fCellEditor;
            }
            inline WordProcessor::Table::EmbeddedTableWordProcessor& WordProcessor::Table::TemporarilyAllocateCellWP::operator& ()
            {
                AssertNotNull (fCellEditor);
                return *fCellEditor;
            }
            inline WordProcessor::Table::EmbeddedTableWordProcessor* WordProcessor::Table::TemporarilyAllocateCellWP::operator-> ()
            {
                AssertNotNull (fCellEditor);
                return fCellEditor;
            }

            //  class   WordProcessor::Table::TemporarilyAllocateCellWithTablet
            inline WordProcessor::Table::TemporarilyAllocateCellWithTablet::TemporarilyAllocateCellWithTablet (WordProcessor::Table& forTable, size_t row, size_t column, bool captureChangesForUndo)
                : fWP (forTable, *forTable.fCurrentOwningWP, row, column, forTable.TableCoordinates2Window (forTable.GetCellEditorBounds (row, column)), captureChangesForUndo)
                , fTablet (forTable.fCurrentOwningWP)
                , fTmpUseTablet (*fWP, fTablet, TemporarilyUseTablet::eDontDoTextMetricsChangedCall)
            {
            }
            inline WordProcessor::Table::TemporarilyAllocateCellWithTablet::operator WordProcessor::Table::EmbeddedTableWordProcessor* ()
            {
                return fWP;
            }
            inline WordProcessor::Table::EmbeddedTableWordProcessor& WordProcessor::Table::TemporarilyAllocateCellWithTablet::operator& ()
            {
                return *fWP;
            }
            inline WordProcessor::Table::EmbeddedTableWordProcessor* WordProcessor::Table::TemporarilyAllocateCellWithTablet::operator-> ()
            {
                return fWP;
            }

            //  class   WordProcessor::DialogSupport::TableSelectionPropertiesInfo
            inline WordProcessor::DialogSupport::TableSelectionPropertiesInfo::TableSelectionPropertiesInfo ()
                : fTableBorderWidth (Led_TWIPS (0))
                , fTableBorderColor (Led_Color::kWhite)
                , fDefaultCellMargins ()
                , fCellSpacing (Led_TWIPS (0))
                , fCellWidth_Common (false)
                , fCellWidth (Led_TWIPS (0))
                , fCellBackgroundColor_Common (false)
                , fCellBackgroundColor (Led_Color::kWhite)
            {
            }
        }
    }
}

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

#endif /*_Stroika_Frameworks_Led_WordProcessor_h_*/
