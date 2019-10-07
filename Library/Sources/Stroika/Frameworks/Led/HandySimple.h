/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_HandySimple_h_
#define _Stroika_Frameworks_Led_HandySimple_h_ 1

#include "../StroikaPreComp.h"

/*
@MODULE:    LedHandySimple
@DESCRIPTION:
        <p>A collection of fairly simple, hopefully (for some) handy utility claasses
    and routines using various parts of Led.This module will probably end up requiring
    linking in alot of Led - as its fairly diverse in nature. Perhaps that will
    get fixed in a future release?</p>
 */

#if _MSC_VER == 1200
//A bit of a hack for MSVC60, cuz this needs to be done before including <vector> - otherwise we get
// lots of needless warnigns - regardless of what is done later -- LGP 980925
#pragma warning(4 : 4786)
#endif

#include "GDI.h"

#include "ChunkedArrayTextStore.h"
#include "WordProcessor.h"

namespace Stroika::Frameworks::Led {

    template <typename WORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
    Led_Size GetTextExtent (Led_Tablet tablet, const Led_tString& text, const Led_Rect& r, bool wordWrap);

    template <typename WORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
    void DrawTextBox (Led_Tablet tablet, const Led_tString& text, const Led_Rect& r, bool wordWrap);

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250) // inherits via dominance warning
#endif
    /*
    @CLASS:         WaterMarkHelper<TEXTSTORE,WORDPROCESSOR>
    @DESCRIPTION:   <p>TEXTSTORE defaults to @'ChunkedArrayTextStore' and WORDPROCESSOR defaults to @'WordProcessor' </p>
                    <p>This can be used to draw a bit of styled (or other) as a watermark background. For example, you can
                override @'TextImager::EraseBackground' () like this:
        <code><pre>
        void    MyLedBasedView::EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing)
        {
            inherited::EraseBackground (tablet, subsetToDraw, printing);
            static  WaterMarkHelper<>   waterMarkerImager (LED_TCHAR_OF ("Demo Mode")); // make this static - just as a performance hack. Also could be an instance variable of 'this'.
            waterMarkerImager.DrawWatermark (tablet, GetWindowRect (), subsetToDraw);
        }
        </pre></code>
                    </p>
                    <p>Note - if you do this - you should also call
        <code><pre>
        SetUseBitmapScrollingOptimization (false);
        </pre></code>
        in your view's constructor, since scrolling bits won't work with a watermark background.
        </p>
    */
    template <typename TEXTSTORE = ChunkedArrayTextStore, typename WORDPROCESSOR = WordProcessor>
    class WaterMarkHelper {
    public:
        WaterMarkHelper (const Led_tString& watermMarkText);
        ~WaterMarkHelper ();

    public:
        nonvirtual Led_Color GetWatermarkColor () const;
        nonvirtual void      SetWatermarkColor (const Led_Color& c);

    private:
        Led_Color fWatermarkColor;

    public:
        nonvirtual void DrawWatermark (Led_Tablet tablet, const Led_Rect& intoRect, const Led_Rect& subsetToDraw);

    private:
        struct MyTrivImager : public TrivialImager_Interactor<TEXTSTORE, WORDPROCESSOR> {
            using inherited = TrivialImager_Interactor<TEXTSTORE, WORDPROCESSOR>;
            MyTrivImager (Led_Tablet t, Led_Rect bounds, const Led_tString& initialText)
                : inherited (t, bounds, initialText)
            {
                /*
                    * Good performance hack (shutting off ImageUsingOffscreenBitmaps), plus critical for
                    *  how we do the EraseBackground () below.
                    */
                SetImageUsingOffscreenBitmaps (false);
            }
            virtual void EraseBackground ([[maybe_unused]] Led_Tablet tablet, [[maybe_unused]] const Led_Rect& subsetToDraw, [[maybe_unused]] bool printing) override
            {
                // Do no erasing - just draw the text...
                // Note - its CRITICAL that we shutoff offscreen bitmaps for this imager so that we don't get garbage bits
                // from that offscreen bitmap being copied back to the original tablet.
            }
            virtual void GetLayoutMargins (typename WORDPROCESSOR::RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const override
            {
                // Make the layout right margin of each line (paragraph) equal to the windowrect. Ie, wrap to the
                // edge of the window.
                Led_Coordinate l = 0;
                Led_Coordinate r = 0;
                inherited::GetLayoutMargins (row, &l, &r);
                r = max (static_cast<Led_Coordinate> (GetWindowRect ().GetWidth ()), l + 1);
                Ensure (r > l);
                if (lhs != NULL) {
                    *lhs = l;
                }
                if (rhs != NULL) {
                    *rhs = r;
                }
            }
        };

    private:
        Led_tString   fWatermarkText;
        MyTrivImager* fCachedImager;
        Led_Rect      fCachedIntoRect;
        Led_Tablet    fCachedIntoTablet;
    };
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "HandySimple.inl"

#endif /*_Stroika_Frameworks_Led_HandySimple_h_*/
