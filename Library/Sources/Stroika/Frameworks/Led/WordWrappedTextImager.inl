/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_WordWrappedTextImager_inl_
#define _Stroika_Framework_Led_WordWrappedTextImager_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     *************** TrivialWordWrappedImager<TEXTSTORE,IMAGER> *********************
     ********************************************************************************
     */
    template <typename TEXTSTORE, typename IMAGER>
    /*
    @METHOD:        TrivialWordWrappedImager<TEXTSTORE,IMAGER>::TrivialWordWrappedImager
    @DESCRIPTION:   <p>Two overloaded versions - one protected, and the other public. The protected one
                does NOT call @'TrivialImager<TEXTSTORE,IMAGER>::SnagAttributesFromTablet' - so you must in your subclass.</p>
                    <p>Most people will just call the public CTOR - as in the class documentation
                (@'TrivialWordWrappedImager<TEXTSTORE,IMAGER>')</p>
    */
    TrivialWordWrappedImager<TEXTSTORE, IMAGER>::TrivialWordWrappedImager (Led_Tablet t)
        : TrivialImager<TEXTSTORE, IMAGER> (t)
    {
    }
    template <typename TEXTSTORE, typename IMAGER>
    TrivialWordWrappedImager<TEXTSTORE, IMAGER>::TrivialWordWrappedImager (Led_Tablet t, Led_Rect bounds, const Led_tString& initialText)
        : TrivialImager<TEXTSTORE, IMAGER> (t)
    {
        SnagAttributesFromTablet ();
        SetWindowRect (bounds);
        GetTextStore ().Replace (0, 0, initialText.c_str (), initialText.length ());
    }
    template <typename TEXTSTORE, typename IMAGER>
    void TrivialWordWrappedImager<TEXTSTORE, IMAGER>::GetLayoutMargins (MultiRowTextImager::RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const
    {
        Ensure (GetWindowRect ().GetWidth () >= 1);
        if (lhs != nullptr) {
            *lhs = 0;
        }
        if (rhs != nullptr) {
            *rhs = GetWindowRect ().GetWidth ();
        }
#if qDebug
        if (lhs != nullptr and rhs != nullptr) {
            Ensure (*rhs > *lhs);
        }
#endif
    }
    template <typename TEXTSTORE, typename IMAGER>
    /*
    @METHOD:        TrivialWordWrappedImager<TEXTSTORE,IMAGER>::GetHeight
    @DESCRIPTION:   <p>Returns the height - in pixels - of the text in the imager.</p>
    */
    inline Led_Distance TrivialWordWrappedImager<TEXTSTORE, IMAGER>::GetHeight () const
    {
        return GetHeightOfRows (0, GetRowCount ());
    }

}

#endif /*_Stroika_Framework_Led_WordWrappedTextImager_inl_*/
