/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_TextImager_inl_
#define _Stroika_Framework_Led_TextImager_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {
    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */

    // class TextImager::SimpleTabStopList
    inline TextImager::SimpleTabStopList::SimpleTabStopList (Led_TWIPS twipsPerTabStop)
        : TabStopList ()
        , fTWIPSPerTabStop (twipsPerTabStop)
    {
        Require (twipsPerTabStop > 0);
    }
    inline Led_TWIPS TextImager::SimpleTabStopList::ComputeIthTab (size_t i) const
    {
        return Led_TWIPS (static_cast<long> ((i + 1) * fTWIPSPerTabStop));
    }
    inline Led_TWIPS TextImager::SimpleTabStopList::ComputeTabStopAfterPosition (Led_TWIPS afterPos) const
    {
        Assert (fTWIPSPerTabStop > 0);
        size_t    idx    = afterPos / fTWIPSPerTabStop;
        Led_TWIPS result = Led_TWIPS (static_cast<long> ((idx + 1) * fTWIPSPerTabStop));
        Ensure (result % fTWIPSPerTabStop == 0);
        Ensure (result > afterPos);
        return result;
    }

    //  class   TextImager::GoalColumnRecomputerControlContext
    inline TextImager::GoalColumnRecomputerControlContext::GoalColumnRecomputerControlContext (TextImager& imager, bool suppressRecompute)
        : fTextImager (imager)
        , fSavedSuppressRecompute (imager.fSuppressGoalColumnRecompute)
    {
        imager.fSuppressGoalColumnRecompute = suppressRecompute;
    }
    inline TextImager::GoalColumnRecomputerControlContext::~GoalColumnRecomputerControlContext ()
    {
        fTextImager.fSuppressGoalColumnRecompute = fSavedSuppressRecompute;
    }

    // class TextImager::StandardTabStopList
    inline TextImager::StandardTabStopList::StandardTabStopList ()
        : TabStopList ()
        , fDefaultTabWidth (720)
        , //  default to 1/2 inch - RTF spec default
        fTabStops ()
    {
        Assert (fDefaultTabWidth > 0);
    }
    inline TextImager::StandardTabStopList::StandardTabStopList (Led_TWIPS eachWidth)
        : TabStopList ()
        , fDefaultTabWidth (eachWidth)
        , fTabStops ()
    {
        Require (fDefaultTabWidth > 0);
    }
    inline TextImager::StandardTabStopList::StandardTabStopList (const vector<Led_TWIPS>& tabstops)
        : TabStopList ()
        , fDefaultTabWidth (720)
        , //  default to 1/2 inch - RTF spec default
        fTabStops (tabstops)
    {
        Assert (fDefaultTabWidth > 0);
    }
    inline TextImager::StandardTabStopList::StandardTabStopList (const vector<Led_TWIPS>& tabstops, Led_TWIPS afterTabsWidth)
        : TabStopList ()
        , fDefaultTabWidth (afterTabsWidth)
        , fTabStops (tabstops)
    {
        Require (fDefaultTabWidth > 0);
    }
    inline Led_TWIPS TextImager::StandardTabStopList::ComputeIthTab (size_t i) const
    {
        Led_TWIPS r      = Led_TWIPS (0);
        size_t    smallI = min (i + 1, fTabStops.size ());
        for (size_t j = 0; j < smallI; ++j) {
            r += fTabStops[j];
        }
        if (smallI <= i) {
            r = Led_TWIPS (static_cast<long> ((r / fDefaultTabWidth + (i - smallI + 1)) * fDefaultTabWidth));
        }
        return r;
    }
    inline Led_TWIPS TextImager::StandardTabStopList::ComputeTabStopAfterPosition (Led_TWIPS afterPos) const
    {
        // Instead if walking all tabstops til we find the right one - GUESS where the right one is (division) and then
        // walk back and forth if/as needed to narrow it down. This will guess perfectly if there are no user-defined tabstops.
        size_t    guessIdx = afterPos / fDefaultTabWidth;
        Led_TWIPS guessVal = ComputeIthTab (guessIdx);

        // Go back first to assure we've gotten the FIRST one after 'afterPos' - not just 'A' tabstop after 'afterPos'.
        while (guessIdx > 0 and guessVal > afterPos) {
            Assert (guessIdx == 0 or ComputeIthTab (guessIdx - 1) < ComputeIthTab (guessIdx)); // assure monotonicly increasing so this will complete!
            guessIdx--;
            guessVal = ComputeIthTab (guessIdx);
        }

        // Now we've scanned to a good spot to start looking...
        Assert (guessIdx == 0 or guessVal <= afterPos);
        for (;; ++guessIdx) {
            Assert (guessIdx == 0 or ComputeIthTab (guessIdx - 1) < ComputeIthTab (guessIdx)); // assure monotonicly increasing so this will complete!
            Led_TWIPS d = ComputeIthTab (guessIdx);
            if (d > afterPos) {
                return d;
            }
        }
        AssertNotReached ();
        return afterPos;
    }
    inline bool TextImager::StandardTabStopList::operator== (const StandardTabStopList& rhs) const
    {
        return fDefaultTabWidth == rhs.fDefaultTabWidth and fTabStops == rhs.fTabStops;
    }
    inline bool TextImager::StandardTabStopList::operator!= (const StandardTabStopList& rhs) const
    {
        return not(*this == rhs);
    }

    inline void TextImager::SetWindowRect_ (const Led_Rect& windowRect)
    {
        fWindowRect = windowRect;
    }
    /*
    @METHOD:        TextImager::GetForceAllRowsShowing
    @DESCRIPTION:   <p>Return the ForceAllRowsShowing flag. If this flag is true, then the TextImager
                will never allow scrolling so that the last row doesn't reach the end of the buffer.
                This is useful primarily for printing. Because otherwise, you would probably always want
                to disable scrolling past the point where the last row is showing.</p>
                    <p>But thats a UI choice we allow you to make if you like (or if you have some other application
                like printing where it makes sense).</p>
                    <p>Call @'TextImager::SetForceAllRowsShowing' () to set the value.</p>
    */
    inline bool TextImager::GetForceAllRowsShowing () const
    {
        return (fForceAllRowsShowing);
    }
    /*
    @METHOD:        TextImager::SetForceAllRowsShowing
    @DESCRIPTION:   <p>See @'TextImager::GetForceAllRowsShowing'.</p>
    */
    inline void TextImager::SetForceAllRowsShowing (bool forceAllRowsShowing)
    {
        if (forceAllRowsShowing != fForceAllRowsShowing) {
            fForceAllRowsShowing = forceAllRowsShowing;
            AssureWholeWindowUsedIfNeeded ();
        }
    }
    /*
    @METHOD:        TextImager::GetImageUsingOffscreenBitmaps
    @DESCRIPTION:   <p>Led already has very little flicker. This is because we are very careful to
                draw as little as possible, and to draw quickly. But some cases still exist.
                Like large pictures being drawn are flicker, cuz we must erase the bounds and then
                draw the picture.</p>
                    <p>Using this gets rid of these few cases of flicker, but at a small performance cost
            in overall draw speed.</p>
                    <p>This value defaults to true (temporarily defaulting to @'qUseOffscreenBitmapsToReduceFlicker).</p>
                    <p>See @'TextImager::SetImageUsingOffscreenBitmaps'.</p>
    */
    inline bool TextImager::GetImageUsingOffscreenBitmaps () const
    {
        return fImageUsingOffscreenBitmaps;
    }
    /*
    @METHOD:        TextImager::SetImageUsingOffscreenBitmaps
    @DESCRIPTION:   <p>See @'TextImager::GetForceAllRowsShowing'.</p>
    */
    inline void TextImager::SetImageUsingOffscreenBitmaps (bool imageUsingOffscreenBitmaps)
    {
        if (imageUsingOffscreenBitmaps != fImageUsingOffscreenBitmaps) {
            fImageUsingOffscreenBitmaps = imageUsingOffscreenBitmaps;
            // add some hook function in the future if there is a need for subclasses to know this has changed...
        }
    }
    /*
    @METHOD:        TextImager::GetHScrollPos
    @DESCRIPTION:   <p>Returns the horizontal offset the image is scrolled to. If this is zero, then
                no horizontal scrolling has taken place. Call @'TextImager::SetHScrollPos ()' to set where you have
                horizontally scrolled to.</p>
    */
    inline Led_Coordinate TextImager::GetHScrollPos () const
    {
        return (fHScrollPos);
    }
    inline void TextImager::SetHScrollPos_ (Led_Coordinate hScrollPos)
    {
        fHScrollPos = hScrollPos;
    }
    /*
    @METHOD:        TextImager::GetSelectionGoalColumn
    @DESCRIPTION:   <p></p>
    */
    inline Led_TWIPS TextImager::GetSelectionGoalColumn () const
    {
        return (fSelectionGoalColumn);
    }
    /*
    @METHOD:        TextImager::SetSelectionGoalColumn
    @DESCRIPTION:   <p></p>
    */
    inline void TextImager::SetSelectionGoalColumn (Led_TWIPS selectionGoalColumn)
    {
        fSelectionGoalColumn = selectionGoalColumn;
    }
    inline void TextImager::InvalidateScrollBarParameters ()
    {
        // typically you would OVERRIDE this to update your scrollbar position, and perhaps
        // hilight state (if you have a scrollbar). Typically only used in TextInteractor subclasses.
    }
    /*
    @METHOD:        TextImager::GetUseSelectEOLBOLRowHilightStyle
    @DESCRIPTION:   <p>On MacOS - when you select a region of text which includes a newline - the visual display of selecting
                the newline is that you 'invert' or 'hilight' all the way to the end of the following row and in the following
                row all the way to the start of the next character. On Windows - you simply don't show the hilighting for the newline
                (as in Notepad) or you display a very narrow empty character (as in WinWord XP).</p>
                    <p>I personally don't like this Windows style hilight display, and greatly prefer the MacOS style. Since
                nobody has ever complained - the MacOS style will remain the default. But - since I was rewriting the
                draw-hilgiht code anyhow (SPR#1207 etc), and I noted this would be easy - I now support both styles
                in case someone prefers the more standard windows style.</p>
                    <p>See also @'TextImager::SetUseSelectEOLBOLRowHilightStyle'.</p>
    */
    inline bool TextImager::GetUseSelectEOLBOLRowHilightStyle () const
    {
        return fUseEOLBOLRowHilightStyle;
    }
    /*
    @METHOD:        TextImager::SetUseSelectEOLBOLRowHilightStyle
    @DESCRIPTION:   <p>See also @'TextImager::GetUseSelectEOLBOLRowHilightStyle'.</p>
    */
    inline void TextImager::SetUseSelectEOLBOLRowHilightStyle (bool useEOLBOLRowHilightStyle)
    {
        fUseEOLBOLRowHilightStyle = useEOLBOLRowHilightStyle;
    }
    inline bool TextImager::GetSelectionShown () const
    {
        return (fSelectionShown);
    }
    /*
    @METHOD:        TextImager::GetWindowRect
    @DESCRIPTION:   <p>Returns the WindowRect associated with this TextImager. The WindowRect is essentially two things. Its
                origin specifies the offset mapping applied to all coordinate transformations before GDI output (and typically in TextInteractors
                UI input). And its Size dictate the size of the current window, used for deciding how much to draw, and scrolling operations.</p>
                    <p>You can logically think of this - in Win32 SDK terminology - as the 'client rect'. In MacOS SDK terms, its close
                to the 'portRect' - its the same if you presume you are already focused, or that your text
                editor view takes up the entire window.</p>
                    <p>See @'TextImager' for more information about Windows/Scrolling. See also @'TextImager::SetWindowRect'.</p>
    */
    inline Led_Rect TextImager::GetWindowRect () const
    {
        return (fWindowRect);
    }
    /*
    @METHOD:        TextImager::GetDefaultFont
    @DESCRIPTION:   <p>Returns the default font used to image new text. The exact meaning and use of this
        default font will depend some on the particular TextImager subclass you are using.</p>
    */
    inline Led_FontSpecification TextImager::GetDefaultFont () const
    {
        return (fDefaultFont);
    }
    /*
    @METHOD:        TextImager::GetRowLength
    @DESCRIPTION:   <p>Returns the number of tChars within the given row (by row index). This method's use is discouraged
                when word-wrapping is present, because it forces word-wrapping up to the given row # in the buffer.</p>
    */
    inline size_t TextImager::GetRowLength (size_t rowNumber) const
    {
        return (GetEndOfRow (rowNumber) - GetStartOfRow (rowNumber));
    }
    /*
    @METHOD:        TextImager::GetDefaultTextColor
    @DESCRIPTION:   <p>Returns the value of the given indexed color attribute. These colors values default to nullptr,
                which means @'TextImager::GetEffectiveDefaultTextColor' will use the system default for each color value.</p>
                    <p>See also
                        <ul>
                            <li>@'TextImager::GetEffectiveDefaultTextColor'</li>
                            <li>@'TextImager::ClearDefaultTextColor'</li>
                            <li>@'TextImager::SetDefaultTextColor'</li>
                        </ul>
                    </p>
    */
    inline Led_Color* TextImager::GetDefaultTextColor (DefaultColorIndex dci) const
    {
        Require (dci < eMaxDefaultColorIndex);
        return fDefaultColorIndex[dci];
    }
    /*
    @METHOD:        TextImager::GetEffectiveDefaultTextColor
    @DESCRIPTION:   <p>Returns either the effective color corresponding to the given @'TextImager::DefaultColorIndex',
                or its default value.</p>
                    <p>See also
                        <ul>
                            <li>@'TextImager::GetDefaultTextColor'</li>
                            <li>@'TextImager::ClearDefaultTextColor'</li>
                            <li>@'TextImager::SetDefaultTextColor'</li>
                        </ul>
                    </p>
    */
    inline Led_Color TextImager::GetEffectiveDefaultTextColor (DefaultColorIndex dci) const
    {
        Require (dci < eMaxDefaultColorIndex);
        if (fDefaultColorIndex[dci] == nullptr) {
            switch (dci) {
                case eDefaultTextColor:
                    return fDefaultFont.GetTextColor ();
                    ;
                case eDefaultBackgroundColor:
                    return Led_GetTextBackgroundColor ();
                case eDefaultSelectedTextColor:
                    return Led_GetSelectedTextColor ();
                case eDefaultSelectedTextBackgroundColor:
                    return Led_GetSelectedTextBackgroundColor ();
                default:
                    Assert (false); /*NOTREACHED*/
                    return Led_Color::kBlack;
            }
        }
        else {
            return *fDefaultColorIndex[dci];
        }
    }
    /*
    @METHOD:        TextImager::ClearDefaultTextColor
    @DESCRIPTION:   <p>Nulls out the given default color attribute, which means @'TextImager::GetEffectiveDefaultTextColor' will
                use the system default for each color value.</p>
                    <p>See also
                        <ul>
                            <li>@'TextImager::GetDefaultTextColor'</li>
                            <li>@'TextImager::GetEffectiveDefaultTextColor'</li>
                            <li>@'TextImager::SetDefaultTextColor'</li>
                        </ul>
                    </p>
    */
    inline void TextImager::ClearDefaultTextColor (DefaultColorIndex dci)
    {
        Require (dci < eMaxDefaultColorIndex);
        delete fDefaultColorIndex[dci];
        fDefaultColorIndex[dci] = nullptr;
        if (dci == eDefaultTextColor) {
            fDefaultFont.SetTextColor (Led_GetTextColor ());
        }
    }
    /*
    @METHOD:        TextImager::SetDefaultTextColor
    @DESCRIPTION:   <p>Sets the given indexed default text color attribute. The effect of this is to OVERRIDE the behavior
                of @'TextImager::GetEffectiveDefaultTextColor'.</p>
                    <p>See also
                        <ul>
                            <li>@'TextImager::GetDefaultTextColor'</li>
                            <li>@'TextImager::GetEffectiveDefaultTextColor'</li>
                            <li>@'TextImager::ClearDefaultTextColor'</li>
                        </ul>
                    </p>
    */
    inline void TextImager::SetDefaultTextColor (DefaultColorIndex dci, const Led_Color& textColor)
    {
        Require (dci < eMaxDefaultColorIndex);
        ClearDefaultTextColor (dci);
        if (dci == eDefaultTextColor) {
            fDefaultFont.SetTextColor (textColor);
        }
        fDefaultColorIndex[dci] = new Led_Color (textColor);
    }
    /*
    @METHOD:        TextImager::GetStartOfNextRowFromRowContainingPosition
    @DESCRIPTION:   <p>Returns the marker position of the start of the row following the row which contains the given marker position.
                If the given marker position is already in the last row, then it returns the start of that last row. So the result could
                posibly be less than the initial value.</p>
                    <p>The reason for this apparantly obscure API is that marker positions allow you to name rows in a way which doesn't involve
                word-wrapping (row numbers would). And yet this code doesn't assume use of MultiRowTextImager, or any of its data structures (so
                these APIs can be used in things like TextInteractor).</p>
    */
    inline size_t TextImager::GetStartOfNextRowFromRowContainingPosition (size_t charPosition) const
    {
        // Use startOfRow positions as 'tokens' for RowReferences...
        size_t rowEnd = FindNextCharacter (GetEndOfRowContainingPosition (charPosition));
        return GetStartOfRowContainingPosition (rowEnd);
    }
    /*
    @METHOD:        TextImager::GetStartOfPrevRowFromRowContainingPosition
    @DESCRIPTION:   <p>Returns the marker position of the start of the row preceeding the row
                which contains the given marker position, or the beginning of the buffer, if none preceed it.</p>
                    <p>See the docs for @'TextImager::GetStartOfNextRowFromRowContainingPosition' for
                insight into how to use this API.</p>
    */
    inline size_t TextImager::GetStartOfPrevRowFromRowContainingPosition (size_t charPosition) const
    {
        // Use startOfRow positions as 'tokens' for RowReferences...
        size_t rowStart = GetStartOfRowContainingPosition (charPosition);
        if (rowStart > 0) {
            rowStart = GetStartOfRowContainingPosition (FindPreviousCharacter (rowStart));
        }
        return rowStart;
    }

    //  class   TextImager::FontCacheInfoUpdater
    inline TextImager::FontCacheInfoUpdater::~FontCacheInfoUpdater ()
    {
#if qPlatform_Windows
        if (fRestoreObject != nullptr) {
            Verify (::SelectObject (fTablet->m_hDC, fRestoreObject));
        }
        if (fRestoreAttribObject != nullptr) {
            Verify (::SelectObject (fTablet->m_hAttribDC, fRestoreAttribObject));
        }
#endif
    }
    inline Led_FontMetrics TextImager::FontCacheInfoUpdater::GetMetrics () const
    {
        return fImager->fCachedFontInfo;
    }

    //  class   TrivialImager<TEXTSTORE,IMAGER>
    template <typename TEXTSTORE, typename IMAGER>
    /*
    @METHOD:        TrivialImager<TEXTSTORE,IMAGER>::TrivialImager
    @DESCRIPTION:   <p>Two overloaded versions - one protected, and the other public. The protected one
                does NOT call @'TrivialImager<TEXTSTORE,IMAGER>::SnagAttributesFromTablet' - so you must in your subclass.</p>
    */
    TrivialImager<TEXTSTORE, IMAGER>::TrivialImager (Led_Tablet t)
        : IMAGER ()
        , fTablet (t)
        , fTextStore ()
        , fBackgroundTransparent (false)
    {
        SpecifyTextStore (&fTextStore);
    }
    template <typename TEXTSTORE, typename IMAGER>
    TrivialImager<TEXTSTORE, IMAGER>::TrivialImager (Led_Tablet t, Led_Rect bounds, const Led_tString& initialText)
        : IMAGER ()
        , fTablet (t)
        , fTextStore ()
        , fBackgroundTransparent (false)
    {
        SpecifyTextStore (&fTextStore);
        SnagAttributesFromTablet ();
        SetWindowRect (bounds);
        fTextStore.Replace (0, 0, initialText.c_str (), initialText.length ());
    }
    template <typename TEXTSTORE, typename IMAGER>
    TrivialImager<TEXTSTORE, IMAGER>::~TrivialImager ()
    {
        SpecifyTextStore (nullptr);
    }
    template <typename TEXTSTORE, typename IMAGER>
    void TrivialImager<TEXTSTORE, IMAGER>::Draw (bool printing)
    {
        Draw (GetWindowRect (), printing);
    }
    template <typename TEXTSTORE, typename IMAGER>
    void TrivialImager<TEXTSTORE, IMAGER>::Draw (const Led_Rect& subsetToDraw, bool printing)
    {
        IMAGER::Draw (subsetToDraw, printing);
    }
    template <typename TEXTSTORE, typename IMAGER>
    Led_Tablet TrivialImager<TEXTSTORE, IMAGER>::AcquireTablet () const
    {
        return fTablet;
    }
    template <typename TEXTSTORE, typename IMAGER>
    void TrivialImager<TEXTSTORE, IMAGER>::ReleaseTablet (Led_Tablet /*tablet*/) const
    {
    }
    template <typename TEXTSTORE, typename IMAGER>
    void TrivialImager<TEXTSTORE, IMAGER>::EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing)
    {
        if (not fBackgroundTransparent) {
            inherited::EraseBackground (tablet, subsetToDraw, printing);
        }
    }
    template <typename TEXTSTORE, typename IMAGER>
    /*
    @METHOD:        TrivialImager<TEXTSTORE,IMAGER>::SnagAttributesFromTablet
    @DESCRIPTION:   <p>Snag font, background color etc, from the currently associated tablet.</p>
                    <p>Since this calls virtual methods of the imager (this) - it must be called in the final
                CTOR (most specific type). Really it shouldnt need to be THE most specific - just enough specific to
                get the right virtual methods called. But because of MSVC compiler bugs
                (@'qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR')- its generally best to be THE final
                CTOR.</p>
    */
    void
    TrivialImager<TEXTSTORE, IMAGER>::SnagAttributesFromTablet ()
    {
#if qPlatform_MacOS
// Should probably do something similar?
#elif qPlatform_Windows
        HFONT hFont = (HFONT)::GetCurrentObject (fTablet->m_hAttribDC, OBJ_FONT);
        Verify (hFont != nullptr);
        LOGFONT lf;
        Verify (::GetObject (hFont, sizeof (LOGFONT), &lf));
        SetDefaultFont (Led_FontSpecification (lf));
        SetDefaultTextColor (eDefaultBackgroundColor, Led_Color (::GetBkColor (fTablet->m_hAttribDC)));
        if (::GetBkMode (fTablet->m_hAttribDC) == TRANSPARENT) {
            SetBackgroundTransparent (true);
        }
#endif
    }
    template <typename TEXTSTORE, typename IMAGER>
    /*
    @METHOD:        TrivialImager<TEXTSTORE,IMAGER>::GetBackgroundColor
    @DESCRIPTION:   <p>See also @'TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundColor'.</p>
    */
    inline Led_Color
    TrivialImager<TEXTSTORE, IMAGER>::GetBackgroundColor () const
    {
        return GetEffectiveDefaultTextColor (eDefaultBackgroundColor);
    }
    template <typename TEXTSTORE, typename IMAGER>
    /*
    @METHOD:        TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundColor
    @DESCRIPTION:   <p>Specifies the color the background of the imager will be drawn with.</p>
                    <p>See also @'TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundTransparent',
                @'TrivialImager<TEXTSTORE,IMAGER>::GetBackgroundColor'.</p>
    */
    inline void
    TrivialImager<TEXTSTORE, IMAGER>::SetBackgroundColor (Led_Color c)
    {
        SetDefaultTextColor (eDefaultBackgroundColor, c);
    }
    template <typename TEXTSTORE, typename IMAGER>
    /*
    @METHOD:        TrivialImager<TEXTSTORE,IMAGER>::GetBackgroundTransparent
    @DESCRIPTION:   <p>See also @'TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundTransparent'.</p>
    */
    inline bool
    TrivialImager<TEXTSTORE, IMAGER>::GetBackgroundTransparent () const
    {
        return fBackgroundTransparent;
    }
    template <typename TEXTSTORE, typename IMAGER>
    /*
    @METHOD:        TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundTransparent
    @DESCRIPTION:   <p>Specifies that the given TrivialImager will (or will not) draw any background color. Use this if
                you are drawing over an existing background which you don't want touched - you just want the TEXT imaged
                and no 'erasing' to be done. When TRUE, it negates the effect of @'TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundColor'</p>
                    <p>This defaults to off (except if overriden by @'TrivialImager<TEXTSTORE,IMAGER>::SnagAttributesFromTablet' if your
                Led_Tablet's ::GetBkMode ()==TRANSPARENT)</p>
                    <p>See also @'TrivialImager<TEXTSTORE,IMAGER>::GetBackgroundTransparent',
                @'TrivialImager<TEXTSTORE,IMAGER>::SetBackgroundColor'.</p>
    */
    inline void
    TrivialImager<TEXTSTORE, IMAGER>::SetBackgroundTransparent (bool transparent)
    {
        fBackgroundTransparent = transparent;
    }
}

#endif /*_Stroika_Framework_Led_TextImager_inl_*/
