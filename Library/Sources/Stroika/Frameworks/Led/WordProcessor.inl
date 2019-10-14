/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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

#endif /*_Stroika_Framework_Led_WordProcessor_inl_*/
