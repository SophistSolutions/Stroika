/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_WordWrappedTextImager_h_
#define _Stroika_Frameworks_Led_WordWrappedTextImager_h_ 1

#include "../StroikaPreComp.h"

/*
@MODULE:    WordWrappedTextImager
@DESCRIPTION:
        <p>This module implements the @'WordWrappedTextImager' class. This class provides a @'TextImager' which wraps
    its text into rows, at word-boudaries, filling in as much of a row of text as possible, before wrapping (subject
    to standard text UI rules).</p>
 */

#include "MultiRowTextImager.h"

namespace Stroika::Frameworks::Led {

/*
    @CONFIGVAR:     qDefaultLedSoftLineBreakChar
    @DESCRIPTION:   <p>This is the magic (sentinel) character that will appear in a Led text buffer to indicate
                a soft line break. This is generated in MSWord (on Mac 5.1 and Windows WinWord 7.0) by the SHIFT-RETURN
                key combination. I have no idea what value is stored internally in MSWord. This is the value WE will
                store internally for Led. And so its somewhat subject to change - if I find THIS value conflicts with anything
                useful.</p>
        */
#ifndef qDefaultLedSoftLineBreakChar
#define qDefaultLedSoftLineBreakChar '\001'
#endif

    /**
 *       <p>Sentinel character used to mark a soft line-break.</p>
 */
    constexpr Led_tChar kSoftLineBreakChar = qDefaultLedSoftLineBreakChar;

#if qStroika_Frameworks_Led_SupportGDI
    /*
    @CLASS:         WordWrappedTextImager
    @BASES:         @'MultiRowTextImager'
    @DESCRIPTION:   <p>Implement the multirow calculations of a MultiRowTextImager using standard word-wrapping algorithms.
                Though this will be commonly used for word-wrapping text editors, it isn't the only plasible way to break rows.
                Some other sort of semantic content in the text might be taken as the thing which guides breaking rows.</p>
    */
    class WordWrappedTextImager : public MultiRowTextImager {
    protected:
        WordWrappedTextImager ()          = default;
        virtual ~WordWrappedTextImager () = default;

    private:
        using inherited = MultiRowTextImager;

    public:
        /*
        @METHOD:        WordWrappedTextImager::GetLayoutMargins
        @DESCRIPTION:
                <p>NB: this routine must @'Ensure' that the *rhs and *lhs results be valid after a call (garbage in), and must ensure *rhs > *lhs,
            assuming that both 'lhs' and 'rhs' are non-null pointers (either or both of these pointers can be null).</p>
                <p>You must specify the wrap-width for each paragraph in a subclass. This routine can be overriden
            to return a constant width - eg. width of the window, or a different per-paragraph width. But note,
            it is the subclassers responsability to invalidate whatever caches need to be invalidated (typically in MutliRowTextImager)
            when the layout width is changed. (nb: this changed in Led 2.2 - 970623-LGP).</p>
                <p>It is because of the need to notify / invalidate caches that we don't provide a default implementation here.
            The most likely default would be to wrap to the width of the window, and to successfully implement that strategy,
            we would need to OVERRIDE SetWindowRect () here. But that would leave those <em>subclassing us</em> with the job
            of circumventing that needless invalidation, and that seems ugly, and awkward. Better to keep the choice of specifying
            this width in the same place as where we handle the invalidation.</p>
                <P>NEW FOR LED 3.1d1. This routine used to be used AMBIGUOUSLY. Sometimes it was interpretted that the return values
            were RELATIVE to the window rect, and sometimes it was interpretted that they took into account the window rect.
            REWRITE THE ABOVE DOCS SO CLEARER ABOUT THIS CHOICE. FROM NOW ON, RETURNS VALUES RELATIVE TO LHS OF WINDOW RECT - LGP 2002-10-25
            </p>
        */
        virtual void GetLayoutMargins (RowReference row, CoordinateType* lhs, CoordinateType* rhs) const = 0;

    public:
        virtual void FillCache (PartitionMarker* pm, PartitionElementCacheInfo& cacheInfo) override;

    protected:
        virtual void AdjustBestRowLength (size_t textStart, const Led_tChar* text, const Led_tChar* end, size_t* rowLength);

    protected:
        virtual bool   ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const override;
        virtual size_t RemoveMappedDisplayCharacters (Led_tChar* copyText, size_t nTChars) const override;
        virtual void PatchWidthRemoveMappedDisplayCharacters (const Led_tChar* srcText, DistanceType* distanceResults, size_t nTChars) const override;

        /*
         * Word wrapping helpers.
         */
    private:
        nonvirtual size_t FindWrapPointForMeasuredText (const Led_tChar* text, size_t length, DistanceType wrapWidth,
                                                        size_t offsetToMarkerCoords, const DistanceType* widthsVector, size_t startSoFar);
        nonvirtual size_t TryToFindWrapPointForMeasuredText1 (const Led_tChar* text, size_t length, DistanceType wrapWidth,
                                                              size_t offsetToMarkerCoords, const DistanceType* widthsVector,
                                                              size_t startSoFar, size_t searchStart, size_t wrapLength);
        nonvirtual size_t FindWrapPointForOneLongWordForMeasuredText (const Led_tChar* text, size_t length, DistanceType wrapWidth,
                                                                      size_t offsetToMarkerCoords, const DistanceType* widthsVector, size_t startSoFar);
    };

    /*
    @CLASS:         TrivialWordWrappedImager<TEXTSTORE, IMAGER>
    @BASES:         @'TrivialImager<TEXTSTORE,IMAGER>'
    @DESCRIPTION:   <p>Handy little class to image text directly. Like Mac "TextBox" or Win32 "DrawText", in that
        you can directly use it (no setup), and it images and wraps the text into the given box.</p>
            <p>The usuage can be as simple as:
            <code>
                void    SomeAppDrawCall (Tablet* t, const Led_Rect& r, bool printing)
                {
                    TrivialWordWrappedImager<ChunkedArrayTextStore> (t, r, LED_TCHAR_OF ("Hi mom")).Draw ();
                }
            </code>
            </p>
        *  @see    @'TrivialImager<TEXTSTORE,IMAGER>', and @'TrivialImager_Interactor<TEXTSTORE,IMAGER>'.</p>
        */
    template <typename TEXTSTORE, typename IMAGER = WordWrappedTextImager>
    class TrivialWordWrappedImager : public TrivialImager<TEXTSTORE, IMAGER> {
    protected:
        TrivialWordWrappedImager (Tablet* t);

    public:
        TrivialWordWrappedImager (Tablet* t, Led_Rect bounds, const Led_tString& initialText = LED_TCHAR_OF (""));

    public:
        virtual void GetLayoutMargins (MultiRowTextImager::RowReference row, CoordinateType* lhs, CoordinateType* rhs) const override;
        nonvirtual DistanceType GetHeight () const;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WordWrappedTextImager.inl"

#endif /*_Stroika_Frameworks_Led_WordWrappedTextImager_h_*/
