/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::Led {

#if qStroika_Frameworks_Led_SupportGDI
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
    TrivialWordWrappedImager<TEXTSTORE, IMAGER>::TrivialWordWrappedImager (Tablet* t)
        : TrivialImager<TEXTSTORE, IMAGER> (t)
    {
    }
    template <typename TEXTSTORE, typename IMAGER>
    TrivialWordWrappedImager<TEXTSTORE, IMAGER>::TrivialWordWrappedImager (Tablet* t, Led_Rect bounds, const Led_tString& initialText)
        : TrivialImager<TEXTSTORE, IMAGER> (t)
    {
        this->SnagAttributesFromTablet ();
        this->SetWindowRect (bounds);
        this->GetTextStore ().Replace (0, 0, initialText.c_str (), initialText.length ());
    }
    template <typename TEXTSTORE, typename IMAGER>
    void TrivialWordWrappedImager<TEXTSTORE, IMAGER>::GetLayoutMargins (MultiRowTextImager::RowReference row, CoordinateType* lhs, CoordinateType* rhs) const
    {
        Ensure (this->GetWindowRect ().GetWidth () >= 1);
        if (lhs != nullptr) {
            *lhs = 0;
        }
        if (rhs != nullptr) {
            *rhs = this->GetWindowRect ().GetWidth ();
        }
#if qStroika_Foundation_Debug_AssertionsChecked
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
    inline DistanceType TrivialWordWrappedImager<TEXTSTORE, IMAGER>::GetHeight () const
    {
        return this->GetHeightOfRows (0, this->GetRowCount ());
    }
#endif

}
