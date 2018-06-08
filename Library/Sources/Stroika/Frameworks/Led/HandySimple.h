/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef ____Led_HandySimple_h__
#define ____Led_HandySimple_h__ 1

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

namespace Stroika {
    namespace Frameworks {
        namespace Led {

            template <typename WORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
            Led_Size GetTextExtent (Led_Tablet tablet, const Led_tString& text, const Led_Rect& r, bool wordWrap);

            template <typename WORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
            void DrawTextBox (Led_Tablet tablet, const Led_tString& text, const Led_Rect& r, bool wordWrap);

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250)
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

            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */

            /*
            @METHOD:        GetTextExtent<WORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>
            @DESCRIPTION:   <p>Trivial wrapper on TrivialImager<TEXTSTORE,IMAGER> etc, except it handles
                case of word wrapping as a parameter. So this is roughly a replacement for the Win32 SDK
                routine GetTextExtent() - except its implemented by Led (and so UNICODE friendly, for example).</p>
                    <p>Note - this is done as a template - rather than directly as a function - so as to avoid forcing
                people who include LedHandySimple from also including all these other modules required for this.</p>
                    <p>See also DrawTextBox<WORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE></p>
            */
            template <typename WORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
            Led_Size GetTextExtent (Led_Tablet tablet, const Led_tString& text, const Led_Rect& r, bool wordWrap)
            {
                RequireNotNull (tablet);
                Led_Size textExtent (0, 0);
                if (wordWrap) {
                    if (r.right > r.left) {
                        TrivialWordWrappedImager<TEXTSTORE, WORDWRAPPEDTEXTIMAGER> imager (tablet, r, text);
                        textExtent.v = imager.GetHeight ();
                        textExtent.h = r.GetWidth ();
                    }
                }
                else {
                    TrivialImager<TEXTSTORE, SIMPLETEXTIMAGER> imager (tablet, r, text);
                    textExtent.v = imager.GetHeightOfRows (0, imager.GetRowCount ());
                    textExtent.h = imager.CalculateLongestRowInWindowPixelWidth ();
                }
                return (textExtent);
            }

            /*
            @METHOD:        DrawTextBox<WORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>
            @DESCRIPTION:   <p>Trivial wrapper on TrivialImager<TEXTSTORE,IMAGER> etc, except it handles
                case of word wrapping as a parameter. So this is roughly a replacement for the Win32 SDK
                routine DrawText () - except its implemented by Led (and so UNICODE friendly, for example).</p>
                    <p>Note - this is done as a template - rather than directly as a function - so as to avoid forcing
                people who include Led_MFC from also including all these other modules required for this. There is
                a global function version of this function (@'Led_DrawText') which will be enabled/included in
                your program if you define @'qSupportDrawTextGetTextExtent'.</p>
                    <p>See also GetTextExtent<WORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE></p>
            */
            template <typename WORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
            void DrawTextBox (Led_Tablet tablet, const Led_tString& text, const Led_Rect& r, bool wordWrap)
            {
                RequireNotNull (tablet);
                if (wordWrap) {
                    TrivialWordWrappedImager<TEXTSTORE, WORDWRAPPEDTEXTIMAGER> imager (tablet, r, text);
                    imager.Draw (r, false);
                }
                else {
                    TrivialImager<TEXTSTORE, SIMPLETEXTIMAGER> imager (tablet, r, text);
                    imager.Draw (r, false);
                }
            }

            //  class   WaterMarkHelper<TEXTSTORE,WORDPROCESSOR>
            template <typename TEXTSTORE, typename WORDPROCESSOR>
            WaterMarkHelper<TEXTSTORE, WORDPROCESSOR>::WaterMarkHelper (const Led_tString& watermMarkText)
                : fWatermarkColor (Led_Color::kYellow)
                , fWatermarkText (watermMarkText)
                , fCachedImager (NULL)
                , fCachedIntoRect (Led_Rect (0, 0, 0, 0))
                , fCachedIntoTablet (NULL)
            {
            }
            template <typename TEXTSTORE, typename WORDPROCESSOR>
            WaterMarkHelper<TEXTSTORE, WORDPROCESSOR>::~WaterMarkHelper ()
            {
                delete fCachedImager;
            }
            template <typename TEXTSTORE, typename WORDPROCESSOR>
            Led_Color WaterMarkHelper<TEXTSTORE, WORDPROCESSOR>::GetWatermarkColor () const
            {
                return fWatermarkColor;
            }
            template <typename TEXTSTORE, typename WORDPROCESSOR>
            void WaterMarkHelper<TEXTSTORE, WORDPROCESSOR>::SetWatermarkColor (const Led_Color& c)
            {
                fWatermarkColor = c;
            }
            template <typename TEXTSTORE, typename WORDPROCESSOR>
            void WaterMarkHelper<TEXTSTORE, WORDPROCESSOR>::DrawWatermark (Led_Tablet tablet, const Led_Rect& intoRect, const Led_Rect& subsetToDraw)
            {
                /*
                 *  Draw a watermark static text label to indicate in DEMO MODE.
                 *
                 *      Dynamicly choose the size of the text - biggest that will fit (within reason).
                 *      Be sure to properly adjust clip region cuz this gets called many times - once for each row potentially, os
                 *      respect the 'subsetToDraw'.
                 */
                if (fCachedImager != NULL) {
                    // del and set null if anything important changed...
                    if (fCachedIntoRect != intoRect or fCachedIntoTablet != tablet) {
                        delete fCachedImager;
                        fCachedImager = NULL;
                    }
                }
                Led_Rect centeredRect = intoRect; // find appropriate small/centered rect for this text

                // It looks a bit better if we inset the text
                centeredRect.left += 5;
                centeredRect.right -= 5;
                if (centeredRect.right <= centeredRect.left) {
                    centeredRect.right = centeredRect.left + 1;
                }
                if (fCachedImager == NULL) {
                    fCachedImager                                = new MyTrivImager (tablet, centeredRect, fWatermarkText);
                    fCachedIntoRect                              = centeredRect;
                    fCachedIntoTablet                            = tablet;
                    Led_IncrementalFontSpecification extraStyles = TextImager::GetStaticDefaultFont ();
                    extraStyles.SetTextColor (GetWatermarkColor ());
                    fCachedImager->SetStyleInfo (0, fCachedImager->GetLength (), extraStyles);

                    // Try and adjust the fontsize so it fits well, and then patch the windowrect so centered.
                    {
                        extraStyles.SetPointSize (72);
                        fCachedImager->SetStyleInfo (0, fCachedImager->GetLength (), extraStyles);

                        while (fCachedImager->GetRowCount () > 1) {
                            extraStyles.SetPointSize (extraStyles.GetPointSize () / 2);
                            fCachedImager->SetStyleInfo (0, fCachedImager->GetLength (), extraStyles);

                            if (extraStyles.GetPointSize () <= 12) {
                                break;
                            }
                        }
                        // Now center the rect of the text...
                        fCachedImager->SetJustification (0, fCachedImager->GetLength (), eCenterJustify);

                        // Now center vertically
                        Led_Distance rowHeight = fCachedImager->GetHeightOfRows (0, 1);
                        Led_Rect     wr        = fCachedImager->GetWindowRect ();
                        wr.top += (wr.GetHeight () - rowHeight) / 2;
                        wr.bottom = wr.top + rowHeight;
                        fCachedImager->SetWindowRect (wr);
                    }
                }

                Led_Tablet_::ClipNarrowAndRestore clipFurtherTo (tablet, intoRect * subsetToDraw);

                bool printing = true; // not really printing - but set this to disable erase call
                fCachedImager->Draw (subsetToDraw, printing);
            }
        }
    }
}

#endif /*____Led_HandySimple_h__*/
