/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_TextImager_h_
#define _Stroika_Frameworks_Led_TextImager_h_ 1

#include "../../Foundation/StroikaPreComp.h"

/*
@MODULE:    TextImager
@DESCRIPTION:
        <p>The @'TextImager' class is one of central importance in Led. It is the (abstract) class which
    does all the imaging - drawing of text to an output device.</p>
 */

#include "BiDiLayoutEngine.h"
#include "GDI.h"
#include "Marker.h"
#include "Support.h"
#include "TextStore.h"

namespace Stroika {
    namespace Frameworks {
        namespace Led {

            /*
            @CLASS:         TextImager
            @BASES:         virtual @'MarkerOwner'
            @DESCRIPTION:   <p>The @'TextImager' class is one of central importance in Led.
                        TextImagers are responsable for imaging to a @'Led_Tablet' the
                        contents of a particular @'TextStore'. A TextImager (except in the degenerate case)
                        is always associated with one TextStore (though a single TextStore can have
                        multiple TextImagers displaying its contents). This long-term relationship allows
                        the TextImagers to keep track of cached data like word-wrapping information etc.
                        TextStores have a flexable notification mechanism so that TextImagers get notifed
                        when 'interesting' modications are made to the text (or @'Marker's) in the TextStore
                        a TextImager is pointing to.</p>

                            <p>A particular TextImager subclass takes the given text,
                        and figures out how to compute metrics on it, lay it out, cache layout information, and then
                        finally display the text. Different subclasses will apply different sorts of rules (and perhaps
                        keep track of additional information) to render the text differently. For example - a
                        @'WordWrappedTextImager' will apply word-wrapping rules to display the text differently
                        than a @'SimpleTextImager' would.</p>

                            <p>A @'TextImager' has no notion of 'windows' - in the sense of a UI. It has no
                        notion of user interaction, screen
                        updates, etc. It has no notion of style runs, paragraph formatting etc.
                        It is a generic API for getting text (in a @'TextStore') imaged onto
                        a some output @'Led_Tablet'.</p>

                              <p>Interesting subclasses of @'TextImager' include @'PartitioningTextImager', @'SimpleTextImager',
                        @'StyledTextImager', @'StandardStyledTextImager', and @'TextInteractor'.</p>


                        <h5>Window/Scrolling support</h5>
                            <p>We support scrolling through the concept of a window. We chose
                        very purposefully to define the window in terms of the top row
                        of text visible in the window. This was because we wanted to allow
                        for the display of word-wrapped text without having to compute the text
                        height or word-wrapping of the text above and below the window.</p>

                            <p>But note - we associate the topleft of the window with a particular
                        ROW - NOT ROW NUMBER! This borrows from (in fact uses) the concept of
                        markers. If people insert rows ABOVE the current window, while this
                        affects the ROW NUMBER of the top row shown in the window - it doesn't
                        affect what is displayed.</p>

                            <p>Also, note that we deal with HORIZONTAL and VERTICAL scrolling through
                        very different means. This is because in the case of horizontal scrolling, it is
                        basicly just scrolling over a few - perhaps hundreds of pixels). And scrolling
                        in terms of pixels is clear, unambiguous, and there is really no other obvious choice
                        (characters don't work well cuz text may be proportional, or even multifont).</p>

                            <p>And vertical scrolling, pixels - though workable - is a undesirable choice
                        for performance reasons (as alluded to above).</p>


                        <h5>Future enhancments:</h5>
                        <ul>
                            <li>    Instead of a single ImageRect, we could have an array of image rects.
                            All would have real top/left/bottom/right, except the last which would
                            be interpretted as being bottomless (or at least WE define the button).
                            This would allow trivial implementation of multicolumn word processors,
                            and much more sophisticated page / text layout.
                            </li>

                            <li>Be sure we deal sensibly with exceptions (on insert/delete mainly - leave
                            markers in a good state - maybe preflight).
                            </li>
                        </ul>
            */
            class TextImager : public virtual MarkerOwner {
            protected:
                TextImager ();

            public:
                virtual ~TextImager ();

                /*
                 *  Note we can associate any textstore we want with this imager. One one
                 *  can be associated at a time. And it is an error to call most TextImager
                 *  methods (any that call GetTextStore()) while the textstore is set to
                 *  nullptr. Always reset the textstore to nullptr before destroying the textstore
                 *  pointed to. And note that switching textstores loses any information
                 *  that might have been saved there, like current selection etc.
                 */
            public:
                virtual TextStore* PeekAtTextStore () const override; // from MarkerOwner
            public:
                nonvirtual void SpecifyTextStore (TextStore* useTextStore);

            protected:
                virtual void    HookLosingTextStore ();
                nonvirtual void HookLosingTextStore_ ();
                virtual void    HookGainedNewTextStore ();
                nonvirtual void HookGainedNewTextStore_ ();

            private:
                TextStore* fTextStore;

            public:
                virtual void PurgeUnneededMemory ();

            protected:
                virtual void InvalidateAllCaches ();

                /*
                 *  Led's model of font application is slightly different than most
                 *  other editors. Led defines a DefaultFont to be applied to all
                 *  text. Then, this font can be overriden by the presence of
                 *  special markers in the text. This overriding is done
                 *  in subclasses of TextImager. So some textImagers will
                 *  only support a single monolithic font for the entire text buffer
                 *  and others will support refinements of the font specification
                 *  on a character by character basis (using markers).
                 *
                 *  DefaultFont defaults to GetStaticDefaultFont () (really - no puns intended :-))
                 */
            public:
                nonvirtual Led_FontSpecification GetDefaultFont () const;
                virtual void                     SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont);

            protected:
                nonvirtual void SetDefaultFont_ (const Led_IncrementalFontSpecification& defaultFont);

            private:
                Led_FontSpecification fDefaultFont;

                // Query the OS for the default font that should be used for new text windows
            public:
                static Led_FontSpecification GetStaticDefaultFont ();
#if qPlatform_Windows
                static Led_FontSpecification GetStaticDefaultFont (BYTE charSet);
#endif

            public:
                virtual Led_FontSpecification GetDefaultSelectionFont () const;

            public:
                /*
                @CLASS:         TextImager::NotFullyInitialized
                @DESCRIPTION:   <p>Led requires a complex of objects to all be setup, and hooked together in order to
                    perform several operations correctly. E.G. to invalidate part of the screen, it must indirectly
                    call on code to check the @'TextStore', or the @'Partition'.</p>
                        <p>As part of code re-use, during the setup process, and part of the mechanism of providing default
                    objects for many of these pieces, we call SetThisOrThat() methods which invoke various InvalidateThisOrThat()
                    methods, which in turn may occasionally try to compute something (like the max# of rows in a line). These
                    sorts of errors are innocuous, but must be handled somehow. We could either write the code to be friendly of
                    such uninitiualized variables/values, or to throw like this. My inclination is that throwing will be a more
                    robust solution, since it will prevent an further use of a guessed value (and maybe prevent its being cached).</p>
                */
                class NotFullyInitialized {
                };

            protected:
                /*
                @CLASS:         TextImager::NoTabletAvailable
                @BASES:         @'TextImager::NotFullyInitialized'
                @DESCRIPTION:   <p>Thrown by @'TextImager::AcquireTablet' when no tablet available. See that method docs
                    for more details.</p>
                */
                class NoTabletAvailable : public NotFullyInitialized {
                };

            protected:
#if qAccessCheckAcrossInstancesSometimesWrongWithVirtualBase
            public:
#endif
                /*
                @METHOD:        TextImager::AcquireTablet
                @DESCRIPTION:
                    <p>By "Tablet" I mean on the mac a grafport, or on windows a CDC (@'Led_Tablet'). Something
                we can draw into, and calculate text metrics with.</p>
                    <p>Sometimes Led is lucky enough to find itself in a position where it is
                handed a tabet (drawing). But sometimes it isn't so luck. Conisider if someone
                asks for a row number, and this causes a complex chain of events resulting
                in an attempt to word-wrap, which, in turn, requires a tablet. We need some API
                in the Imager to request a tablet. This API MUST return the appropriate tablet
                for the view, appropriately prepared for drawing/text metric calculations.</p>
                    <p>Calls to @'TextImager::AcquireTablet' must ALWAYS be balanced with calls to @'TextImager::ReleaseTablet ()'
                (including in the prescence of exceptions).</p>
                    <p>For windows, @'TextImager::AcquireTablet ()' must return the current PaintDC used in the PaintDC
                call (if we are in the context of a paint).</p>
                    <p>For Mac, we must not only return the grafPort, but be sure it is prepared.</p>
                    <p>@'TextImager::AcquireTablet ()' only fail by throwing - typically NoTabletAvailable (Led 2.1 and earlier
                didn't allow failure).</p>
                    <p>Calls to @'TextImager::AcquireTablet ()'/@'TextImager::ReleaseTablet ()' can be nested.</p>
                    <p>Also, sometimes something happens which causes all text metrics to become</p>
                invalid (say we are about to image to a print port). In this case, the imager
                needs to be notified that all its text metrics are invalid. Call @'TextImager::TabletChangedMetrics ()'
                in this case.</p>
                */
                virtual Led_Tablet AcquireTablet () const = 0;
                /*
                @METHOD:        TextImager::ReleaseTablet
                @DESCRIPTION:   <p>Tablet API</p>
                        <p>See @'TextImager::AcquireTablet'. Generally don't call this directly - but instead use the
                    @'TextImager::Tablet_Acquirer' class.</p>
                */
                virtual void ReleaseTablet (Led_Tablet tablet) const = 0;

            protected:
                virtual void TabletChangedMetrics ();

            public:
                class Tablet_Acquirer;
                friend class Tablet_Acquirer;

            public:
                //class TabStopList;
                /*
                @CLASS:         TextImager::TabStopList
                @DESCRIPTION:   <p>See @'Led_TabStopList'</p>
                */
                using TabStopList = Led_TabStopList;
                class SimpleTabStopList;
                class StandardTabStopList;

            public:
                virtual const TabStopList& GetTabStopList (size_t containingPos) const;

                /*
                 *  Window/Scrolling support.
                 */
            public:
                nonvirtual Led_Rect GetWindowRect () const;
                virtual void        SetWindowRect (const Led_Rect& windowRect);

            protected:
                nonvirtual void SetWindowRect_ (const Led_Rect& windowRect);

            private:
                Led_Rect fWindowRect;

                // Consider re-doing many of the below ROW APIs using this struct??? - LGP 2002-12-02
                // Then we'd ahve void  GetRowInfo (TransRowSpec tsr, size_t* rowStart, size_t* rowEnd) const = 0 declared here... and use that to get row info?
            public:
                struct TransientRowSpecification {
                    size_t fRowStartMarkerPos;
                };

                /*
                 *  Vertical Scrolling:
                 */
            public:
                /*
                @METHOD:        TextImager::GetTopRowInWindow
                @DESCRIPTION:   <p>Returns the row number of the top row visible in the scrolled display window.
                            This is a generally ineffient routine to call. It is generally much better
                            to call @'TextImager::GetMarkerPositionOfStartOfWindow'</p>
                */
                virtual size_t GetTopRowInWindow () const = 0;
                /*
                @METHOD:        TextImager::GetTotalRowsInWindow
                @DESCRIPTION:   <p></p>
                */
                virtual size_t GetTotalRowsInWindow () const = 0;
                /*
                @METHOD:        TextImager::GetLastRowInWindow
                @DESCRIPTION:   <p>Returns the row number of the bottom row visible in the scrolled display window.
                            This is a generally ineffient routine to call. It is generally much better
                            to call @'TextImager::GetMarkerPositionOfEndOfWindow'</p>
                */
                virtual size_t GetLastRowInWindow () const = 0;
                /*
                @METHOD:        TextImager::SetTopRowInWindow
                @DESCRIPTION:   <p></p>
                */
                virtual void SetTopRowInWindow (size_t newTopRow) = 0;
                /*
                @METHOD:        TextImager::GetMarkerPositionOfStartOfWindow
                @DESCRIPTION:   <p>This function returns the marker position of the start of the display window. The
                            display window here refers to the area which is currently visible though scrolling.</p>
                            <p>This doc should be clarified!</p>
                            <p>See also @'TextImager::GetMarkerPositionOfStartOfWindow'</p>
                */
                virtual size_t GetMarkerPositionOfStartOfWindow () const = 0;
                /*
                @METHOD:        TextImager::GetMarkerPositionOfEndOfWindow
                @DESCRIPTION:   <p>This function returns the marker position of the end of the display window. The
                            display window here refers to the area which is currently visible though scrolling.</p>
                            <p>This doc should be clarified!</p>
                            <p>See also @'TextImager::GetMarkerPositionOfStartOfWindow'</p>
                */
                virtual size_t GetMarkerPositionOfEndOfWindow () const = 0;
                /*
                @METHOD:        TextImager::GetMarkerPositionOfStartOfLastRowOfWindow
                @DESCRIPTION:   <p></p>
                */
                virtual size_t GetMarkerPositionOfStartOfLastRowOfWindow () const = 0;

                // Some helpful routines for the case where we don't have RowReferences available, but we don't want to be too slow...
            public:
                /*
                @METHOD:        TextImager::CalculateRowDeltaFromCharDeltaFromTopOfWindow
                @DESCRIPTION:   <p></p>
                */
                virtual ptrdiff_t CalculateRowDeltaFromCharDeltaFromTopOfWindow (long deltaChars) const = 0;
                /*
                @METHOD:        TextImager::CalculateCharDeltaFromRowDeltaFromTopOfWindow
                @DESCRIPTION:   <p></p>
                */
                virtual ptrdiff_t CalculateCharDeltaFromRowDeltaFromTopOfWindow (ptrdiff_t deltaRows) const = 0;

                // NB:  Though you CAN ask for, or set the top row by number - this is VASTLY less efficient than
                // doing it by relative position from the current window via ScrollByIfRoom ().
                // This is because the former method requires computing the text metrics of all the text (and wrapping)
                // above the row scrolled to (at), while the later - relative method - does not!!!
            public:
                /*
                @METHOD:        TextImager::ScrollByIfRoom
                @DESCRIPTION:   <p>If downBy negative then up
                    OK to ask to scroll further than allowed - return true
                    if any scrolling (not necesarily same amont requested) done</p>
                */
                virtual void ScrollByIfRoom (ptrdiff_t downByRows) = 0;

                /*
                @METHOD:        TextImager::ScrollSoShowing
                @DESCRIPTION:   <p>If andTryToShowMarkerPos == 0, then just try to show the characters
                    after markerPos. If it is NOT zero, then try to show the characters
                    BETWEEN the two marker positions (note they might not be in ascending
                    order). The (argument) order of the two marker positions determines PREFERENCE.
                    That is - if there is no room to show both, be sure the first is showing.</p>
                */
                virtual void ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos = 0) = 0;

            protected:
                nonvirtual void ScrollSoShowingHHelper (size_t markerPos, size_t andTryToShowMarkerPos);

                /*
                 *  This attribute controls if we force the scroll position to be adjusted
                 *  in InvalidateScrollBar() so that the entire window is being used. This is TRUE
                 *  by default. But might sometimes be handy to disable this behavior.
                 */
            public:
                nonvirtual bool GetForceAllRowsShowing () const;
                nonvirtual void SetForceAllRowsShowing (bool forceAllRowsShowing);

            private:
                bool fForceAllRowsShowing;

            protected:
                /*
                @METHOD:        TextImager::AssureWholeWindowUsedIfNeeded
                @ACCESS:        protected
                @DESCRIPTION:   <p></p>
                */
                virtual void AssureWholeWindowUsedIfNeeded () = 0;

            public:
                nonvirtual bool GetImageUsingOffscreenBitmaps () const;
                nonvirtual void SetImageUsingOffscreenBitmaps (bool imageUsingOffscreenBitmaps);

            private:
                bool fImageUsingOffscreenBitmaps;

                /*
                 *  Horizontal Scrolling:
                 *
                 *      The way horizontal scrolling works is that you specify a (horizontal) offset as to where
                 *  drawing into the WindowRect() will be relative to. Typically, this will be ZERO, indicating
                 *  the text is unscrolled. Typically (by default) the ComputeMaxHScrollPos () method will return
                 *  zero. At any time, a UI can call ComputeMaxHScrollPos () in order to inform adjustments to
                 *  a scrollbar. It is the TextImagers responsablity to assure that ComputeMaxHScrollPos () returns
                 *  a correct, but efficiently computed (ie cached) value, where possible.
                 *
                 *
                 *  Design Meanderings: - PRELIMINARY DESIGN THOUGHTS ON HOW TODO THIS - LGP 961231
                 *      (what is connection between ComputeMaxHScrollPos () and GetLayoutWidth()??? Logicly there must
                 *  be one. Really we must keep resetting Layout Width as people type text making a line longer. Then
                 *  ComputeMaxHScrollPos () really just returns GetLayoutWidth() - WindowRect.GetWidth () (or zero, whichever
                 *  is larger).
                 *
                 *  Then calls to SetHScrollPos & SetLayoutWidth() must clip (auto-reset if necessary) HScrollPos.
                 *
                 */
            public:
                nonvirtual Led_Coordinate GetHScrollPos () const;
                virtual void              SetHScrollPos (Led_Coordinate hScrollPos);

            protected:
                nonvirtual void SetHScrollPos_ (Led_Coordinate hScrollPos);

            public:
                virtual Led_Distance ComputeMaxHScrollPos () const;

            private:
                Led_Coordinate fHScrollPos;

                /*
                 *  Some utility methods, very handy for implementing horizontal scrolling. Can (and should be) overriden
                 *  in certain subclasses for efficiency. But the default implementation will work.
                 */
            public:
                virtual Led_Distance CalculateLongestRowInWindowPixelWidth () const;

            public:
                enum CursorMovementDirection { eCursorBack,
                                               eCursorForward,
                                               eCursorToStart,
                                               eCursorToEnd };
                enum CursorMovementUnit { eCursorByChar,
                                          eCursorByWord,
                                          eCursorByRow,
                                          eCursorByLine,
                                          eCursorByWindow,
                                          eCursorByBuffer };
                virtual size_t ComputeRelativePosition (size_t fromPos, CursorMovementDirection direction, CursorMovementUnit movementUnit);

            public:
                class GoalColumnRecomputerControlContext;
                nonvirtual void RecomputeSelectionGoalColumn ();

            private:
                bool fSuppressGoalColumnRecompute;

            public:
                nonvirtual Led_TWIPS GetSelectionGoalColumn () const;
                nonvirtual void      SetSelectionGoalColumn (Led_TWIPS selectionGoalColumn);

            private:
                Led_TWIPS fSelectionGoalColumn;

            public:
                /*
                @METHOD:        TextImager::GetStableTypingRegionContaingMarkerRange
                @DESCRIPTION:   <p>OK, this is a little obscure. But - believe me - it does make sense.
                            When doing updates to the text (say typing) the impact if the typing
                            (in terms of visual display) typically only extends over a narrow region
                            of the screen. If we are using a non-word-wrapped text editor, then this
                            is typically a line. If it is a word-wrapped text editor, then it is
                            typically a paragraph (or perhaps something less than that). This routine
                            allows communication from the imager to higher-level interactor
                            software about what areas of the text will need to be redrawn when
                            changes to localized regions of the text occur.</p>
                */
                virtual void GetStableTypingRegionContaingMarkerRange (size_t fromMarkerPos, size_t toMarkerPos,
                                                                       size_t* expandedFromMarkerPos, size_t* expandedToMarkerPos) const = 0;

            public:
                nonvirtual Led_Rect GetTextWindowBoundingRect (size_t fromMarkerPos, size_t toMarkerPos) const;

            public:
                nonvirtual Led_Rect GetIntraRowTextWindowBoundingRect (size_t fromMarkerPos, size_t toMarkerPos) const;

            public:
                virtual vector<Led_Rect> GetRowHilightRects (const TextLayoutBlock& text, size_t rowStart, size_t rowEnd, size_t hilightStart, size_t hilightEnd) const;

                /*
                 *      Utility routine to compute based on styleruns etc, all the fontinfo we can get
                 *  at a particular point in the text. This can be helpful drawing additional adornments
                 *  on the text in subclases.
                 */
            public:
                virtual Led_FontMetrics GetFontMetricsAt (size_t charAfterPos) const;

                // TextImagers don't really have a notion of a scrollbar. And yet for many different
                // subclasses of TextImager, certain things might be detected that would force
                // a subclass which DID have a scrollbar to update it.
                //  These things include:
                //      ¥   Any edit operation (including font change)
                //          which increases/decreases number of rows
                //      ¥   Any scroll operation
                //      ¥   Resizing the LayoutWidth.
                //
                // Call InvalidateScrollBarParameters () whenever any such change happens.
            protected:
                virtual void InvalidateScrollBarParameters ();

            public:
                nonvirtual bool GetUseSelectEOLBOLRowHilightStyle () const;
                nonvirtual void SetUseSelectEOLBOLRowHilightStyle (bool useEOLBOLRowHilightStyle);

            private:
                bool fUseEOLBOLRowHilightStyle;

                /*
                 *  Control if the caret is currently being displayed for this TE field. Typically it will
                 *  be turned on when this widget gets the focus, and off when it loses it. But that is
                 *  the responsability of higher level software (subclasses).
                 */
            public:
                nonvirtual bool GetSelectionShown () const;
                virtual void    SetSelectionShown (bool shown);

            private:
                bool fSelectionShown;

            public:
                nonvirtual size_t GetSelectionStart () const;
                nonvirtual size_t GetSelectionEnd () const;
                nonvirtual void   GetSelection (size_t* start, size_t* end) const;
                virtual void      SetSelection (size_t start, size_t end);

            protected:
                nonvirtual void SetSelection_ (size_t start, size_t end);

            protected:
                class HilightMarker : public Marker {
                public:
                    HilightMarker ();
                };
                HilightMarker* fHiliteMarker;
                bool           fWeAllocedHiliteMarker;

            protected:
                nonvirtual void SetHilightMarker (HilightMarker* newHilightMarker); // NB: Caller must free, and call SetHilightMarker (nullptr)

                /*
                 *  API to get postions of text.
                 */
            public:
                /*
                @METHOD:        TextImager::GetCharLocation
                @DESCRIPTION:   <p>Returns the (document relative) pixel location (rectangle) of the character after the given
                            marker position.</p>
                                <p>GetCharLocation () returns the rectangle bounding the character after marker-position
                            'afterPosition'. The position MUST be a valid one (0..GetEnd ()), but could
                            refer to the end of the buffer - and so there would be no character width in this case.</p>
                                <p>Note that this is also somewhat ineffecient for larger documents. You can often use
                            @'TextImager::GetCharLocationRowRelative' or @'TextImager::GetCharWindowLocation' instead.</p>
                */
                virtual Led_Rect GetCharLocation (size_t afterPosition) const = 0;
                /*
                @METHOD:        TextImager::GetCharAtLocation
                @DESCRIPTION:   <p>Always returns a valid location, though it might not make a lot of
                            since if it is outside the ImageRect.</p>
                                <p>Note that this is also somewhat ineffecient for larger documents. You can often use
                            @'TextImager::GetRowRelativeCharAtLoc' or @'TextImager::GetCharAtWindowLocation' instead.</p>
                */
                virtual size_t GetCharAtLocation (const Led_Point& where) const = 0;
                /*
                @METHOD:        TextImager::GetCharWindowLocation
                @DESCRIPTION:   <p>Like @'TextImager::GetCharLocation' but returns things relative to the start of the window.</p>
                                <p><em>NB:</em> if the location is outside of the current window, then the size/origin maybe pinned to some
                            arbitrary value, which is pixelwise above (or beleow according to before or after window start/end) the window.</p>
                */
                virtual Led_Rect GetCharWindowLocation (size_t afterPosition) const = 0;
                /*
                @METHOD:        TextImager::GetCharAtWindowLocation
                @DESCRIPTION:   <p>Like @'TextImager::GetCharAtLocation' but assumes 'where' is presented window-relative.</p>
                                <p>See also @'TextInteractor::GetCharAtClickLocation'</p>
                */
                virtual size_t GetCharAtWindowLocation (const Led_Point& where) const = 0;

                /*
                @METHOD:        TextImager::GetStartOfRow
                @DESCRIPTION:   <p>GetStartOfRow () returns the position BEFORE the first displayed character in the row.</p>
                */
                virtual size_t GetStartOfRow (size_t rowNumber) const = 0;
                /*
                @METHOD:        TextImager::GetStartOfRowContainingPosition
                @DESCRIPTION:   <p></p>
                */
                virtual size_t GetStartOfRowContainingPosition (size_t charPosition) const = 0;
                /*
                @METHOD:        TextImager::GetEndOfRow
                @DESCRIPTION:   <p>GetEndOfRow () returns the position AFTER the last displayed character in the row.
                    Note that this can differ from the end of row returned by @'TextImager::GetRealEndOfRow' because of characters removed by
                    @TextImager::RemoveMappedDisplayCharacters'.</p>
                        <p>See also @'TextImager::GetRealEndOfRow'.</p>
                */
                virtual size_t GetEndOfRow (size_t rowNumber) const = 0;
                /*
                @METHOD:        TextImager::GetEndOfRowContainingPosition
                @DESCRIPTION:   <p></p>
                */
                virtual size_t GetEndOfRowContainingPosition (size_t charPosition) const = 0;
                /*
                @METHOD:        TextImager::GetRealEndOfRow
                @DESCRIPTION:   <p>GetRealEndOfRow () returns the position AFTER the last character in the row. See
                    @'TextImager::GetEndOfRow'. Note - for the last row of the document, this could be include the
                    'bogus character'. So - this will always return a value &lt;= @'TextStore::GetLength' () + 1</p>
                */
                virtual size_t GetRealEndOfRow (size_t rowNumber) const = 0;
                /*
                @METHOD:        TextImager::GetRealEndOfRowContainingPosition
                @DESCRIPTION:   <p></p>
                */
                virtual size_t GetRealEndOfRowContainingPosition (size_t charPosition) const = 0;
                /*
                @METHOD:        TextImager::GetRowContainingPosition
                @DESCRIPTION:   <p></p>
                */
                virtual size_t GetRowContainingPosition (size_t charPosition) const = 0;
                /*
                @METHOD:        TextImager::GetRowCount
                @DESCRIPTION:   <p></p>
                */
                virtual size_t GetRowCount () const = 0;

                nonvirtual size_t GetRowLength (size_t rowNumber) const;

            public:
                /*
                @METHOD:        TextImager::GetCharLocationRowRelativeByPosition
                @DESCRIPTION:   <p></p>
                */
                virtual Led_Rect GetCharLocationRowRelativeByPosition (size_t afterPosition, size_t positionOfTopRow, size_t maxRowsToCheck) const = 0;

                // Simple wrappers on GetStartOfRowContainingPosition () etc to allow row navigation without the COSTLY
                // computation of row#s.
            public:
                nonvirtual size_t GetStartOfNextRowFromRowContainingPosition (size_t charPosition) const;
                nonvirtual size_t GetStartOfPrevRowFromRowContainingPosition (size_t charPosition) const;

            public:
                /*
                @METHOD:        TextImager::GetRowHeight
                @DESCRIPTION:   <p>This API is redundent, but can be much more efficient to get at this information
                            than GetCharLocation() - especially in subclasses like MultiRowImager using RowRefernces.</p>
                */
                virtual Led_Distance GetRowHeight (size_t rowNumber) const = 0;

            public:
                /*
                @METHOD:        TextImager::GetRowRelativeBaselineOfRowContainingPosition
                @DESCRIPTION:   <p>Returns the number of pixels from the top of the given row, to the baseline.
                    The 'charPosition' is a markerPosition just before any character in the row.</p>
                */
                virtual Led_Distance GetRowRelativeBaselineOfRowContainingPosition (size_t charPosition) const = 0;

            public:
                /*
                @METHOD:        TextImager::GetTextDirection
                @DESCRIPTION:   <p>Returns the text direction (left to right or right to left) of the given character
                            (the one just after the given marker position).</p>
                */
                virtual TextDirection GetTextDirection (size_t charPosition) const = 0;

            public:
                virtual TextLayoutBlock_Copy GetTextLayoutBlock (size_t rowStart, size_t rowEnd) const;

                /*
                 *  Figure the region bounding the given segment of text. Useful for displaying
                 *  some sort of text hilight, in addition (or apart from) the standard hilighting
                 *  of text. Note we use a VAR parameter for the region rather than returing it
                 *  cuz MFC's CRgn class doesn't support being copied.
                 */
            public:
                virtual vector<Led_Rect> GetSelectionWindowRects (size_t from, size_t to) const;
                virtual void             GetSelectionWindowRegion (Led_Region* r, size_t from, size_t to) const;

            public:
                /*
                @METHOD:        TextImager::Draw
                @DESCRIPTION:   <p>This is the API called by the underlying window system (or wrappers) to get the imager
                            to draw its currently displayed window. The argument 'subsetToDraw' defines the subset (in the same coordinate system
                            the windowRect (@'TextImager::GetWindowRect') was specified in (so it will generally be a subset of the windowrect).</p>
                */
                virtual void Draw (const Led_Rect& subsetToDraw, bool printing) = 0;

                // Misc default foreground/background color attributes
            public:
                /*
                @METHOD:        TextImager::DefaultColorIndex
                @DESCRIPTION:   <p>Enumerator of different color-table default names used by these routines:
                                    <ul>
                                        <li>@'TextImager::GetEffectiveDefaultTextColor'</li>
                                        <li>@'TextImager::GetEffectiveDefaultTextColor'</li>
                                        <li>@'TextImager::ClearDefaultTextColor'</li>
                                        <li>@'TextImager::SetDefaultTextColor'</li>
                                    </ul>
                                </p>
                                <p>The values are:
                                    <table>
                                        <tr>
                                            <td>Name</td>                               <td>Default</td>                                    <td>Description</td>
                                        </tr>
                                        <tr>
                                            <td>eDefaultTextColor</td>                  <td>Led_GetTextColor&nbsp;()</td>                   <td>Color used for plain text. This color choice is frequently overriden elsewhere.</td>
                                        </tr>
                                        <tr>
                                            <td>eDefaultBackgroundColor</td>            <td>Led_GetTextBackgroundColor&nbsp;()</td>         <td>The background color which the text is drawn against. This is mainly used in @'TextImager::EraseBackground'. NB: since TextImager::EraseBackground is frequently overriden - setting this value could have no effect.</td>
                                        </tr>
                                        <tr>
                                            <td>eDefaultSelectedTextColor</td>          <td>Led_GetSelectedTextColor&nbsp;()</td>           <td>The color used to display selected text. This would typically by used by @'TextImager::HilightARectangle' - but depending on the style of hilight it uses - this color maybe ignonred.</td>
                                        </tr>
                                        <tr>
                                            <td>eDefaultSelectedTextBackgroundColor</td><td>Led_GetSelectedTextBackgroundColor&nbsp;()</td> <td>The background color for hilighted (selected) text. Depending on the style of hilight it uses - this color maybe ignonred.</td>
                                        </tr>
                                    </table>
                                </p>
                */
                enum DefaultColorIndex { eDefaultTextColor,
                                         eDefaultBackgroundColor,
                                         eDefaultSelectedTextColor,
                                         eDefaultSelectedTextBackgroundColor,
                                         eMaxDefaultColorIndex };
                nonvirtual Led_Color* GetDefaultTextColor (DefaultColorIndex dci) const;
                nonvirtual Led_Color GetEffectiveDefaultTextColor (DefaultColorIndex dci) const;
                nonvirtual void      ClearDefaultTextColor (DefaultColorIndex dci);
                nonvirtual void      SetDefaultTextColor (DefaultColorIndex dci, const Led_Color& textColor);

            private:
                Led_Color* fDefaultColorIndex[eMaxDefaultColorIndex];

            public:
                virtual void EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing);

            public:
                virtual void HilightArea (Led_Tablet tablet, Led_Rect hiliteArea);
                virtual void HilightArea (Led_Tablet tablet, const Led_Region& hiliteArea);

            protected:
                virtual void DrawRow (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
                                      const TextLayoutBlock& text, size_t rowStart, size_t rowEnd, bool printing);
                virtual void DrawRowSegments (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
                                              const TextLayoutBlock& text, size_t rowStart, size_t rowEnd);
                virtual void DrawRowHilight (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
                                             const TextLayoutBlock& text, size_t rowStart, size_t rowEnd);
                virtual void DrawInterLineSpace (Led_Distance interlineSpace, Led_Tablet tablet, Led_Coordinate vPosOfTopOfInterlineSpace, bool segmentHilighted, bool printing);

            protected:
                virtual bool   ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const;
                virtual void   ReplaceMappedDisplayCharacters (const Led_tChar* srcText, Led_tChar* copyText, size_t nTChars) const;
                virtual size_t RemoveMappedDisplayCharacters (Led_tChar* copyText, size_t nTChars) const;
                virtual void   PatchWidthRemoveMappedDisplayCharacters (const Led_tChar* srcText, Led_Distance* distanceResults, size_t nTChars) const;

            protected:
                static bool   ContainsMappedDisplayCharacters_HelperForChar (const Led_tChar* text, size_t nTChars, Led_tChar charToMap);
                static void   ReplaceMappedDisplayCharacters_HelperForChar (Led_tChar* copyText, size_t nTChars, Led_tChar charToMap, Led_tChar charToMapTo);
                static size_t RemoveMappedDisplayCharacters_HelperForChar (Led_tChar* copyText, size_t nTChars, Led_tChar charToRemove);
                static void   PatchWidthRemoveMappedDisplayCharacters_HelperForChar (const Led_tChar* srcText, Led_Distance* distanceResults, size_t nTChars, Led_tChar charToRemove);

            protected:
                virtual void DrawSegment (Led_Tablet tablet,
                                          size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& invalidRect,
                                          Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn);

            public:
                // Note we REQUIRE that useBaseLine be contained within drawInto
                nonvirtual void DrawSegment_ (Led_Tablet tablet, const Led_FontSpecification& fontSpec,
                                              size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn) const;

            protected:
                // distanceResults must be an array of (to-from) elements - which is filled in with the widths
                // of subsegments from 'from' up to 'i', where 'i' is the ith-from element of 'distanceResults'.
                // For mbyte chars - splitting char results and looking at those values are UNDEFINED).
                //
                // In general - textwidth of text from from to b (where b MUST be contained in from/to)
                // is distanceResults[b-from-1] - and of course ZERO if b == from
                //
                virtual void MeasureSegmentWidth (size_t from, size_t to, const Led_tChar* text,
                                                  Led_Distance* distanceResults) const;

            public:
                nonvirtual void MeasureSegmentWidth_ (const Led_FontSpecification& fontSpec, size_t from, size_t to,
                                                      const Led_tChar* text,
                                                      Led_Distance*    distanceResults) const;

            protected:
                virtual Led_Distance MeasureSegmentHeight (size_t from, size_t to) const;

            public:
                nonvirtual Led_Distance MeasureSegmentHeight_ (const Led_FontSpecification& fontSpec, size_t from, size_t to) const;

            protected:
                virtual Led_Distance MeasureSegmentBaseLine (size_t from, size_t to) const;

            public:
                nonvirtual Led_Distance MeasureSegmentBaseLine_ (const Led_FontSpecification& fontSpec, size_t from, size_t to) const;

            public:
                /*
                @METHOD:        TextImager::CalcSegmentSize
                @DESCRIPTION:   <p>Measures the distance in pixels from 'from' to 'to'.
                            This function requires that 'from' and 'to' are on the same row.</p>
                                <p>Note that for bidirectional text, the notion of 'from' and 'to' CAN be ambiguous. A given
                            marker-position between two characters in the buffer could refer to characters that are VISUALLY
                            not next to one another. Luckily - there is a convenient disambiguator in this API. Since we are always
                            measuring 'from' one position 'to' another - we assume the region in question is the one <em>after</em>
                            the 'from' and <em>before</em> the 'to'.</p>
                                <p>Note - because of this definition - and because of bidirectional characters - trying to find
                            the left and right side of a character with:
                                    <CODE><PRE>
                                    LHS = CalcSegmentSize (ROWSTART, i)
                                    RHS = CalcSegmentSize (ROWSTART, i+1);
                                    </PRE></CODE>
                                will NOT work. Use @'TextImager::GetRowRelativeCharLoc' to properly take into account the script
                            directionality runs.
                                </P>
                */
                virtual Led_Distance CalcSegmentSize (size_t from, size_t to) const = 0;

            public:
                /*
                @METHOD:        TextImager::GetRowRelativeCharLoc
                @DESCRIPTION:   <p>Returns the row-relative bounds of the given character (the one from charLoc to charLoc + 1).
                            The left hand side is window-relative, so if you want window coordinates, add the window left.
                            This routine is more complex than just calling @'TextImager::CalcSegmentSize' () on the right and
                            left sides of the character, because of bidirectional editing.</p>
                                <p>Note that even though the character maybe RTL or LTR, we still return the *lhs <= *rhs for the
                            character cell. This routine calls @'TextImager::RemoveMappedDisplayCharacters' and if the given character is
                            removed, its proper LHS will still be returned, but the RHS will be EQUAL to its LHS.</p>
                                <p>Note that 'charLoc' must be a legal char location or just at the end of the buffer - 0 <= charLoc <= GetEnd ().
                            if charLoc==GetEnd() - you will get the same results as in the @'TextImager::RemoveMappedDisplayCharacters' case.</p>
                                <p>See also @'TextImager::GetRowRelativeCharLoc'</p>
                */
                virtual void GetRowRelativeCharLoc (size_t charLoc, Led_Distance* lhs, Led_Distance* rhs) const = 0;

            public:
                /*
                @METHOD:        TextImager::GetRowRelativeCharAtLoc
                @DESCRIPTION:   <p>Look in the row which begins at 'rowStart'. Look 'hOffset' pixels into the row, and return the character which
                            would be found at that point (assuming the vertical positioning is already correct). If its too far to the right, return ENDOFROW.</p>
                                <p>See also @'TextImager::GetRowRelativeCharLoc'</p>
                                <p>Returns the last character in the row (not a marker PAST that
                            character) if the point passed in is past the end of the last character in the row.
                                </p>
                 */
                virtual size_t GetRowRelativeCharAtLoc (Led_Coordinate hOffset, size_t rowStart) const = 0;

                // Font info caches...
            private:
                mutable Led_FontSpecification fCachedFontSpec;
                mutable Led_FontMetrics       fCachedFontInfo;
#if qPlatform_Windows
                mutable Led_FontObject* fCachedFont;
#else
                mutable bool fCachedFontValid;
#endif

            protected:
                class FontCacheInfoUpdater {
                public:
                    FontCacheInfoUpdater (const TextImager* imager, Led_Tablet tablet, const Led_FontSpecification& fontSpec);
                    ~FontCacheInfoUpdater ();

                public:
                    nonvirtual Led_FontMetrics GetMetrics () const;

                private:
                    const TextImager* fImager;

#if qPlatform_Windows
                private:
                    Led_Tablet fTablet;
                    HGDIOBJ    fRestoreObject;
                    HGDIOBJ    fRestoreAttribObject;
#endif
                };
                friend class FontCacheInfoUpdater;

            private:
                friend class GoalColumnRecomputerControlContext;
            };

            /*
            @CLASS:         TextImager::Tablet_Acquirer
            @DESCRIPTION:   <p>Stack-based resource allocation/deallocation. See @'TextImager::AcquireTablet'.</p>
            */
            class TextImager::Tablet_Acquirer {
            public:
                Tablet_Acquirer (const TextImager* textImager)
                    : fTextImager (textImager)
                {
                    AssertNotNull (fTextImager);
                    fTablet = fTextImager->AcquireTablet ();
                }
                operator Led_Tablet ()
                {
                    AssertNotNull (fTablet);
                    return (fTablet);
                }
                Led_Tablet operator-> ()
                {
                    return fTablet;
                }
                ~Tablet_Acquirer ()
                {
                    AssertNotNull (fTextImager);
                    AssertNotNull (fTablet);
                    fTextImager->ReleaseTablet (fTablet);
                }

            private:
                const TextImager* fTextImager;
                Led_Tablet        fTablet;
            };

            /*
            @CLASS:         TextImager::SimpleTabStopList
            @BASES:         @'TextImager::TabStopList'
            @DESCRIPTION:   <p>A simple tabstop implementation in which all tabstops are equidistant from each other, and start
                        at position zero. This is commonly used as the default tabstop object.</p>
            */
            class TextImager::SimpleTabStopList : public TextImager::TabStopList {
            public:
                SimpleTabStopList (Led_TWIPS twipsPerTabStop);

            public:
                virtual Led_TWIPS ComputeIthTab (size_t i) const override;
                virtual Led_TWIPS ComputeTabStopAfterPosition (Led_TWIPS afterPos) const override;

            public:
                Led_TWIPS fTWIPSPerTabStop;
            };

            /*
            @CLASS:         TextImager::StandardTabStopList
            @BASES:         @'TextImager::TabStopList'
            @DESCRIPTION:   <p>A simple tabstop implementation in the caller specifies the exact position of each tabstop. This
                        is most directly ananogous to the Win32SDK GetTabbedTextExtent () API apporach.</p>
            */
            class TextImager::StandardTabStopList : public TextImager::TabStopList {
            public:
                StandardTabStopList (); // default to 1/2 inch
                StandardTabStopList (Led_TWIPS eachWidth);
                StandardTabStopList (const vector<Led_TWIPS>& tabstops);
                StandardTabStopList (const vector<Led_TWIPS>& tabstops, Led_TWIPS afterTabsWidth);

            public:
                virtual Led_TWIPS ComputeIthTab (size_t i) const override;
                virtual Led_TWIPS ComputeTabStopAfterPosition (Led_TWIPS afterPos) const override;

            public:
                Led_TWIPS         fDefaultTabWidth; //  for tabs PAST the ones specified in the fTabStops list
                vector<Led_TWIPS> fTabStops;

            public:
                nonvirtual bool operator== (const StandardTabStopList& rhs) const;
                nonvirtual bool operator!= (const StandardTabStopList& rhs) const;
            };

            /*
            @CLASS:         TextImager::GoalColumnRecomputerControlContext
            @DESCRIPTION:   <p></p>
            */
            class TextImager::GoalColumnRecomputerControlContext {
            public:
                GoalColumnRecomputerControlContext (TextImager& imager, bool suppressRecompute);
                ~GoalColumnRecomputerControlContext ();

            private:
                TextImager& fTextImager;
                bool        fSavedSuppressRecompute;
            };

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(4 : 4250)
#endif

            /*
            @CLASS:         TrivialImager<TEXTSTORE,IMAGER>
            @DESCRIPTION:   <p>Handy little template, if you want to use the power of Led, but just to wrap a particular imager,
                        in a localized, one-time fasion, say todo printing, or some such. Not for interactors.</p>
                            <p>Depending on the IMAGER you wish to use, you may want to try @'TrivialImager_Interactor<TEXTSTORE,IMAGER>'
                        or @'TrivialWordWrappedImager<TEXTSTORE, IMAGER>' - both of which come with examples of their use.</p>
             */
            template <typename TEXTSTORE, typename IMAGER>
            class TrivialImager : public IMAGER {
            private:
                using inherited = IMAGER;

            protected:
                TrivialImager (Led_Tablet t);

            public:
                TrivialImager (Led_Tablet t, Led_Rect bounds, const Led_tString& initialText = LED_TCHAR_OF (""));
                ~TrivialImager ();

            public:
                nonvirtual void Draw (bool printing = false);
                virtual void    Draw (const Led_Rect& subsetToDraw, bool printing) override;

            protected:
                virtual Led_Tablet AcquireTablet () const override;
                virtual void       ReleaseTablet (Led_Tablet /*tablet*/) const override;

            protected:
                virtual void EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing) override;

            protected:
                nonvirtual void SnagAttributesFromTablet ();

            public:
                nonvirtual Led_Color GetBackgroundColor () const;
                nonvirtual void      SetBackgroundColor (Led_Color c);
                nonvirtual bool      GetBackgroundTransparent () const;
                nonvirtual void      SetBackgroundTransparent (bool transparent);

            private:
                TEXTSTORE  fTextStore;
                Led_Tablet fTablet;
                bool       fBackgroundTransparent;
            };
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

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
                HFONT        hFont = (HFONT)::GetCurrentObject (fTablet->m_hAttribDC, OBJ_FONT);
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
    }
}

#endif /*_Stroika_Frameworks_Led_TextImager_h_*/
