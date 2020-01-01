/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_HandySimple_inl_
#define _Stroika_Framework_Led_HandySimple_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

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

#endif /*_Stroika_Framework_Led_HandySimple_inl_*/
