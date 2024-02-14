/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_WordProcessor_inl_
#define _Stroika_Framework_Led_WordProcessor_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include <cstdio> // for sprintf() call

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ************************************ ParagraphInfo *****************************
     ********************************************************************************
     */
    /*
    @METHOD:        ParagraphInfo::GetJustification
    @DESCRIPTION:
        <p>Return the @'Justification' setting.</p>
    */
    inline Justification ParagraphInfo::GetJustification () const
    {
        return fJustification;
    }
    /*
    @METHOD:        ParagraphInfo::SetJustification
    @DESCRIPTION:
        <p>Set the @'Justification' setting.</p>
    */
    inline void ParagraphInfo::SetJustification (Justification justification)
    {
        fJustification = justification;
    }
    inline const StandardTabStopList& ParagraphInfo::GetTabStopList () const
    {
        return fTabStops;
    }
    inline void ParagraphInfo::SetTabStopList (const StandardTabStopList& tabStops)
    {
        fTabStops = tabStops;
    }
    inline TWIPS ParagraphInfo::GetLeftMargin () const
    {
        return fLeftMargin;
    }
    inline TWIPS ParagraphInfo::GetRightMargin () const
    {
        return fRightMargin;
    }
    inline void ParagraphInfo::SetMargins (TWIPS lhs, TWIPS rhs)
    {
        Require (lhs < rhs);
        fLeftMargin  = lhs;
        fRightMargin = rhs;
    }
    inline TWIPS ParagraphInfo::GetFirstIndent () const
    {
        return fFirstIndent;
    }
    inline void ParagraphInfo::SetFirstIndent (TWIPS firstIndent)
    {
        fFirstIndent = firstIndent;
    }
    /*
    @METHOD:        ParagraphInfo::GetSpaceBefore
    @DESCRIPTION:
                    <p>Get the 'space before' attribute of the given paragraph. This is the number of TWIPS of space inserted before
                the paragraph - and defaults to zero. See the RTF \sb tag.</p>
                    <p>See also @'ParagraphInfo::SetSpaceBefore'.</p>
    */
    inline TWIPS ParagraphInfo::GetSpaceBefore () const
    {
        return fSpaceBefore;
    }
    /*
    @METHOD:        ParagraphInfo::SetSpaceBefore
    @DESCRIPTION:
            <p>See also @'ParagraphInfo::GetSpaceBefore'.</p>
    */
    inline void ParagraphInfo::SetSpaceBefore (TWIPS sb)
    {
        fSpaceBefore = sb;
    }
    /*
    @METHOD:        ParagraphInfo::GetSpaceAfter
    @DESCRIPTION:
            <p>Get the 'space after' attribute of the given paragraph. This is the number of TWIPS of space appended after
        the paragraph - and defaults to zero. See the RTF \sa tag.</p>
            <p>See also @'ParagraphInfo::SetSpaceAfter'.</p>
    */
    inline TWIPS ParagraphInfo::GetSpaceAfter () const
    {
        return fSpaceAfter;
    }
    /*
    @METHOD:        ParagraphInfo::SetSpaceAfter
    @DESCRIPTION:
            <p>See also @'ParagraphInfo::GetSpaceAfter'.</p>
    */
    inline void ParagraphInfo::SetSpaceAfter (TWIPS sa)
    {
        fSpaceAfter = sa;
    }
    /*
    @METHOD:        ParagraphInfo::GetLineSpacing
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
            <p>See also @'ParagraphInfo::SetLineSpacing'.</p>
    */
    inline LineSpacing ParagraphInfo::GetLineSpacing () const
    {
        return fLineSpacing;
    }
    /*
    @METHOD:        ParagraphInfo::SetLineSpacing
    @DESCRIPTION:
            <p>See also @'ParagraphInfo::GetLineSpacing'.</p>
    */
    inline void ParagraphInfo::SetLineSpacing (LineSpacing sl)
    {
        fLineSpacing = sl;
    }
    /*
    @METHOD:        ParagraphInfo::GetListStyle
    @DESCRIPTION:
            <p>Retrieve the @'ParagraphInfo::ListStyle' property associated with this paragraph
        (e.g. eListStyle_Bullet or eListStyle_None).</p>
    */
    inline ListStyle ParagraphInfo::GetListStyle () const
    {
        return fListStyle;
    }
    /*
    @METHOD:        ParagraphInfo::SetListStyle
    @DESCRIPTION:
            <p>See also @'ParagraphInfo::GetListStyle'.</p>
    */
    inline void ParagraphInfo::SetListStyle (ListStyle listStyle)
    {
        fListStyle = listStyle;
    }
    /*
    @METHOD:        ParagraphInfo::GetListIndentLevel
    @DESCRIPTION:
            <p>Retrieve the indent level for the given list. Only valid value if @'ParagraphInfo::GetListStyle'
        is not eListStyle_None.</p>
    */
    inline unsigned char ParagraphInfo::GetListIndentLevel () const
    {
        return fListIndentLevel;
    }
    /*
    @METHOD:        ParagraphInfo::SetListIndentLevel
    @DESCRIPTION:
            <p>See also @'ParagraphInfo::GetListIndentLevel'.</p>
    */
    inline void ParagraphInfo::SetListIndentLevel (unsigned char indentLevel)
    {
        fListIndentLevel = indentLevel;
    }
    inline void ParagraphInfo::MergeIn (const IncrementalParagraphInfo& incParaInfo)
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
    inline bool ParagraphInfo::operator== (const ParagraphInfo& rhs) const
    {
        return fJustification == rhs.fJustification and fTabStops == rhs.fTabStops and fLeftMargin == rhs.fLeftMargin and
               fRightMargin == rhs.fRightMargin and fFirstIndent == rhs.fFirstIndent and fSpaceBefore == rhs.fSpaceBefore and
               fSpaceAfter == rhs.fSpaceAfter and fLineSpacing == rhs.fLineSpacing and fListStyle == rhs.fListStyle and
               fListIndentLevel == rhs.fListIndentLevel;
    }

    /*
     ********************************************************************************
     ********************* IncrementalParagraphInfo ******************
     ********************************************************************************
     */
    inline IncrementalParagraphInfo::IncrementalParagraphInfo ()
        : fJustificationValid (false)
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
    inline IncrementalParagraphInfo::IncrementalParagraphInfo (const ParagraphInfo& pi)
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
    inline Justification IncrementalParagraphInfo::GetJustification () const
    {
        Require (fJustificationValid);
        return inherited::GetJustification ();
    }
    inline void IncrementalParagraphInfo::SetJustification (Justification justification)
    {
        fJustificationValid = true;
        inherited::SetJustification (justification);
    }
    inline bool IncrementalParagraphInfo::GetJustification_Valid () const
    {
        return fJustificationValid;
    }
    inline void IncrementalParagraphInfo::InvalidateJustification ()
    {
        fJustificationValid = false;
    }
    inline const StandardTabStopList& IncrementalParagraphInfo::GetTabStopList () const
    {
        Require (fTabStopListValid);
        return inherited::GetTabStopList ();
    }
    inline void IncrementalParagraphInfo::SetTabStopList (const StandardTabStopList& tabStops)
    {
        fTabStopListValid = true;
        inherited::SetTabStopList (tabStops);
    }
    inline bool IncrementalParagraphInfo::GetTabStopList_Valid () const
    {
        return fTabStopListValid;
    }
    inline void IncrementalParagraphInfo::InvalidateTabStopList ()
    {
        fTabStopListValid = false;
    }
    inline TWIPS IncrementalParagraphInfo::GetLeftMargin () const
    {
        Require (fMarginsValid);
        return inherited::GetLeftMargin ();
    }
    inline TWIPS IncrementalParagraphInfo::GetRightMargin () const
    {
        Require (fMarginsValid);
        return inherited::GetRightMargin ();
    }
    inline void IncrementalParagraphInfo::SetMargins (TWIPS lhs, TWIPS rhs)
    {
        fMarginsValid = true;
        inherited::SetMargins (lhs, rhs);
    }
    inline bool IncrementalParagraphInfo::GetMargins_Valid () const
    {
        return fMarginsValid;
    }
    inline void IncrementalParagraphInfo::InvalidateMargins ()
    {
        fMarginsValid = false;
    }
    inline TWIPS IncrementalParagraphInfo::GetFirstIndent () const
    {
        Require (fFirstIndentValid);
        return inherited::GetFirstIndent ();
    }
    inline void IncrementalParagraphInfo::SetFirstIndent (TWIPS firstIndent)
    {
        fFirstIndentValid = true;
        inherited::SetFirstIndent (firstIndent);
    }
    inline bool IncrementalParagraphInfo::GetFirstIndent_Valid () const
    {
        return fFirstIndentValid;
    }
    inline void IncrementalParagraphInfo::InvalidateFirstIndent ()
    {
        fFirstIndentValid = false;
    }
    inline TWIPS IncrementalParagraphInfo::GetSpaceBefore () const
    {
        Require (fSpaceBeforeValid);
        return inherited::GetSpaceBefore ();
    }
    inline void IncrementalParagraphInfo::SetSpaceBefore (TWIPS sb)
    {
        fSpaceBeforeValid = true;
        inherited::SetSpaceBefore (sb);
    }
    inline bool IncrementalParagraphInfo::GetSpaceBefore_Valid () const
    {
        return fSpaceBeforeValid;
    }
    inline void IncrementalParagraphInfo::InvalidateSpaceBefore ()
    {
        fSpaceBeforeValid = false;
    }
    inline TWIPS IncrementalParagraphInfo::GetSpaceAfter () const
    {
        Require (fSpaceAfterValid);
        return inherited::GetSpaceAfter ();
    }
    inline void IncrementalParagraphInfo::SetSpaceAfter (TWIPS sa)
    {
        fSpaceAfterValid = true;
        inherited::SetSpaceAfter (sa);
    }
    inline bool IncrementalParagraphInfo::GetSpaceAfter_Valid () const
    {
        return fSpaceAfterValid;
    }
    inline void IncrementalParagraphInfo::InvalidateSpaceAfter ()
    {
        fSpaceAfterValid = false;
    }
    inline LineSpacing IncrementalParagraphInfo::GetLineSpacing () const
    {
        Require (fLineSpacingValid);
        return inherited::GetLineSpacing ();
    }
    inline void IncrementalParagraphInfo::SetLineSpacing (LineSpacing sl)
    {
        fLineSpacingValid = true;
        inherited::SetLineSpacing (sl);
    }
    inline bool IncrementalParagraphInfo::GetLineSpacing_Valid () const
    {
        return fLineSpacingValid;
    }
    inline void IncrementalParagraphInfo::InvalidateLineSpacing ()
    {
        fLineSpacingValid = false;
    }
    inline ListStyle IncrementalParagraphInfo::GetListStyle () const
    {
        Require (fListStyleValid);
        return inherited::GetListStyle ();
    }
    inline void IncrementalParagraphInfo::SetListStyle (ListStyle listStyle)
    {
        fListStyleValid = true;
        inherited::SetListStyle (listStyle);
    }
    inline bool IncrementalParagraphInfo::GetListStyle_Valid () const
    {
        return fListStyleValid;
    }
    inline void IncrementalParagraphInfo::InvalidateListStyle ()
    {
        fListStyleValid = false;
    }
    inline unsigned char IncrementalParagraphInfo::GetListIndentLevel () const
    {
        Require (fListIndentLevelValid);
        return inherited::GetListIndentLevel ();
    }
    inline void IncrementalParagraphInfo::SetListIndentLevel (unsigned char indentLevel)
    {
        fListIndentLevelValid = true;
        inherited::SetListIndentLevel (indentLevel);
    }
    inline bool IncrementalParagraphInfo::GetListIndentLevel_Valid () const
    {
        return fListIndentLevelValid;
    }
    inline void IncrementalParagraphInfo::InvalidateListIndentLevel ()
    {
        fListIndentLevelValid = false;
    }
    inline bool IncrementalParagraphInfo::operator== (const IncrementalParagraphInfo& rhs) const
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

    /*
     ********************************************************************************
     ****************** AbstractParagraphDatabaseRep *****************
     ********************************************************************************
     */
    inline AbstractParagraphDatabaseRep::AbstractParagraphDatabaseRep ()
        : fSomeInvalidTables (false)
        , fCachedFarthestRightMarginInDocument (WordProcessor::kBadCachedFarthestRightMarginInDocument)
    {
    }

#if qStroika_Frameworks_Led_SupportGDI

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

    /*
     ********************************************************************************
     ************************ ParagraphInfoMarker ***********************************
     ********************************************************************************
     */
    inline ParagraphInfoMarker::ParagraphInfoMarker (ParagraphInfo paragraphInfo)
        : fParagraphInfo (paragraphInfo)
    {
    }
    inline const ParagraphInfo& ParagraphInfoMarker::GetInfo () const
    {
        return fParagraphInfo;
    }
    inline void ParagraphInfoMarker::SetInfo (ParagraphInfo paragraphInfo)
    {
        fParagraphInfo = paragraphInfo;
    }

    /*
     ********************************************************************************
     ******************************* ParagraphDatabaseRep ***************************
     ********************************************************************************
     */
    inline shared_ptr<Partition> ParagraphDatabaseRep::GetPartition () const
    {
        return fPartition;
    }

    /*
     ********************************************************************************
     ******************* WordProcessor::WordProcessorTextIOSinkStream ***************
     ********************************************************************************
     */
    inline bool WordProcessor::WordProcessorTextIOSinkStream::GetOverwriteTableMode () const
    {
        return fOverwriteTableMode;
    }
    inline void WordProcessor::WordProcessorTextIOSinkStream::SetOverwriteTableMode (bool overwriteTableMode)
    {
        fOverwriteTableMode = overwriteTableMode;
    }
#if !qStroika_Frameworks_Led_NestedTablesSupported
    inline bool WordProcessor::WordProcessorTextIOSinkStream::GetNoTablesAllowed () const
    {
        return fNoTablesAllowed;
    }
    inline void WordProcessor::WordProcessorTextIOSinkStream::SetNoTablesAllowed (bool noTablesAllowed)
    {
        fNoTablesAllowed = noTablesAllowed;
    }
#endif

    /*
     ********************************************************************************
     ******************* WordProcessor::WordProcessorTextIOSrcStream ****************
     ********************************************************************************
     */
    inline bool WordProcessor::WordProcessorTextIOSrcStream::GetUseTableSelection () const
    {
        return fUseTableSelection;
    }
    inline void WordProcessor::WordProcessorTextIOSrcStream::SetUseTableSelection (bool useTableSelection)
    {
        fUseTableSelection = useTableSelection;
    }

    /*
     ********************************************************************************
     ************************************ WordProcessor *****************************
     ********************************************************************************
     */
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
    inline shared_ptr<AbstractParagraphDatabaseRep> WordProcessor::GetParagraphDatabase () const
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
    inline IncrementalFontSpecification WordProcessor::GetCurSelFontSpec () const
    {
        if (not fCachedCurSelFontSpecValid) {
            AssureCurSelFontCacheValid ();
        }
        return fCachedCurSelFontSpec;
    }

    /*
     ********************************************************************************
     ********************* WordProcessor::Table::RowInfo ****************************
     ********************************************************************************
     */
    inline WordProcessor::Table::RowInfo::RowInfo ()
        : fHeight (0)
    {
    }

    /*
     ********************************************************************************
     ************************ WordProcessor::Table::Cell ****************************
     ********************************************************************************
     */
    inline WordProcessor::Table::CellMergeFlags WordProcessor::Table::Cell::GetCellMergeFlags () const
    {
        return fCellMergeFlags;
    }
    inline TWIPS WordProcessor::Table::Cell::GetCellXWidth () const
    {
        return fCellRep->fCellXWidth;
    }
    inline void WordProcessor::Table::Cell::SetCellXWidth (TWIPS width)
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

    /*
     ********************************************************************************
     ********** WordProcessor::WordProcessorFlavorPackageInternalizer ***************
     ********************************************************************************
     */
    inline bool WordProcessor::WordProcessorFlavorPackageInternalizer::GetOverwriteTableMode () const
    {
        return fOverwriteTableMode;
    }
    inline void WordProcessor::WordProcessorFlavorPackageInternalizer::SetOverwriteTableMode (bool overwriteTableMode)
    {
        fOverwriteTableMode = overwriteTableMode;
    }
#if !qStroika_Frameworks_Led_NestedTablesSupported
    inline bool WordProcessor::WordProcessorFlavorPackageInternalizer::GetNoTablesAllowed () const
    {
        return fNoTablesAllowed;
    }
    inline void WordProcessor::WordProcessorFlavorPackageInternalizer::SetNoTablesAllowed (bool noTablesAllowed)
    {
        fNoTablesAllowed = noTablesAllowed;
    }
#endif

    /*
     ********************************************************************************
     ********** WordProcessor::WordProcessorFlavorPackageExternalizer ***************
     ********************************************************************************
     */
    inline bool WordProcessor::WordProcessorFlavorPackageExternalizer::GetUseTableSelection () const
    {
        return fUseTableSelection;
    }
    inline void WordProcessor::WordProcessorFlavorPackageExternalizer::SetUseTableSelection (bool useTableSelection)
    {
        fUseTableSelection = useTableSelection;
    }

    //class WordProcessor::Table
    inline TWIPS WordProcessor::Table::GetCellSpacing () const
    {
        return fCellSpacing;
    }
    inline void WordProcessor::Table::SetCellSpacing (TWIPS cellSpacing)
    {
        if (fCellSpacing != cellSpacing) {
            fCellSpacing = cellSpacing;
            InvalidateLayout ();
        }
    }
    inline void WordProcessor::Table::GetDefaultCellMargins (TWIPS* top, TWIPS* left, TWIPS* bottom, TWIPS* right) const
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
    inline void WordProcessor::Table::SetDefaultCellMargins (TWIPS top, TWIPS left, TWIPS bottom, TWIPS right)
    {
        if (top != fDefaultCellMargins.GetTop () or left != fDefaultCellMargins.GetLeft () or bottom != fDefaultCellMargins.GetBottom () or
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
    inline void WordProcessor::Table::SaveIntraCellContextInfo (bool                     leftSideOfSelectionInteresting,
                                                                const FontSpecification& intraCellSelectionEmptySelFontSpecification)
    {
        fSavedIntraCellInfoValid                          = true;
        fSavedLeftSideOfSelectionInteresting              = leftSideOfSelectionInteresting;
        fSavedIntraCellSelectionEmptySelFontSpecification = intraCellSelectionEmptySelFontSpecification;
    }
    inline bool WordProcessor::Table::RestoreIntraCellContextInfo (bool* leftSideOfSelectionInteresting, FontSpecification* intraCellSelectionEmptySelFontSpecification)
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

    /*
     ********************************************************************************
     ****** WordProcessor::Table::SuppressCellUpdatePropagationContext **************
     ********************************************************************************
     */
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

    /*
     ********************************************************************************
     ********* WordProcessor::Table::AllowUpdateInfoPropagationContext **************
     ********************************************************************************
     */
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

    /*
     ********************************************************************************
     *** WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet *****
     ********************************************************************************
     */
    inline WordProcessor::Table::EmbeddedTableWordProcessor::TemporarilyUseTablet::TemporarilyUseTablet (WordProcessor::Table::EmbeddedTableWordProcessor& editor,
                                                                                                         Tablet* t, DoTextMetricsChangedCall tmChanged)
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

    /*
     ********************************************************************************
     *** WordProcessor::Table::EmbeddedTableWordProcessor::DisableRefreshContext ****
     ********************************************************************************
     */
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

    /*
     ********************************************************************************
     ******************* WordProcessor::Table::TemporarilySetOwningWP ***************
     ********************************************************************************
     */
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

    /*
     ********************************************************************************
     ******************* WordProcessor::Table::TemporarilyAllocateCellWP ************
     ********************************************************************************
     */
    inline WordProcessor::Table::TemporarilyAllocateCellWP::TemporarilyAllocateCellWP (Table& forTable, WordProcessor& forWordProcessor,
                                                                                       size_t forRow, size_t forColumn,
                                                                                       const Led_Rect& cellWindowRect, bool captureChangesForUndo)
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
    inline WordProcessor::Table::EmbeddedTableWordProcessor* WordProcessor::Table::TemporarilyAllocateCellWP::operator->()
    {
        AssertNotNull (fCellEditor);
        return fCellEditor;
    }

    /*
     ********************************************************************************
     *********** WordProcessor::Table::TemporarilyAllocateCellWithTablet ************
     ********************************************************************************
     */
    inline WordProcessor::Table::TemporarilyAllocateCellWithTablet::TemporarilyAllocateCellWithTablet (WordProcessor::Table& forTable, size_t row,
                                                                                                       size_t column, bool captureChangesForUndo)
        : fWP (forTable, *forTable.fCurrentOwningWP, row, column,
               forTable.TableCoordinates2Window (forTable.GetCellEditorBounds (row, column)), captureChangesForUndo)
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
    inline WordProcessor::Table::EmbeddedTableWordProcessor* WordProcessor::Table::TemporarilyAllocateCellWithTablet::operator->()
    {
        return fWP;
    }

    /*
     ********************************************************************************
     *********** WordProcessor::DialogSupport::TableSelectionPropertiesInfo *********
     ********************************************************************************
     */
    inline WordProcessor::DialogSupport::TableSelectionPropertiesInfo::TableSelectionPropertiesInfo ()
        : fTableBorderWidth (TWIPS{0})
        , fTableBorderColor (Color::kWhite)
        , fDefaultCellMargins ()
        , fCellSpacing (TWIPS{0})
        , fCellWidth_Common (false)
        , fCellWidth (TWIPS{0})
        , fCellBackgroundColor_Common (false)
        , fCellBackgroundColor (Color::kWhite)
    {
    }
#endif

}

#endif /*_Stroika_Framework_Led_WordProcessor_inl_*/
