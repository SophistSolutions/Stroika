/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_TextInteractorMixIns_inl_
#define _Stroika_Framework_Led_TextInteractorMixIns_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ********************* InteractorImagerMixinHelper<IMAGER> **********************
     ********************************************************************************
     */
    template <typename IMAGER>
    void InteractorImagerMixinHelper<IMAGER>::Draw (const Led_Rect& subsetToDraw, bool printing)
    {
        DrawBefore (subsetToDraw, printing);
        TextInteractor::Draw (subsetToDraw, printing);
        IMAGER::Draw (subsetToDraw, printing);
        DrawAfter (subsetToDraw, printing);
    }
    template <typename IMAGER>
    void InteractorImagerMixinHelper<IMAGER>::AboutToUpdateText (const UpdateInfo& updateInfo)
    {
        IMAGER::AboutToUpdateText (updateInfo);
        TextInteractor::AboutToUpdateText (updateInfo);
    }
    template <typename IMAGER>
    void InteractorImagerMixinHelper<IMAGER>::DidUpdateText (const UpdateInfo& updateInfo) noexcept
    {
        IMAGER::DidUpdateText (updateInfo);
        TextInteractor::DidUpdateText (updateInfo);
    }
    template <typename IMAGER>
    void InteractorImagerMixinHelper<IMAGER>::HookLosingTextStore ()
    {
        /*
         *  NB: See SPR#0836 - order of Imager/Interactor reversed compared to HookGainedNewTextStore () intentionally (mimic CTOR/DTOR ordering).
         *  Now DO imager first, then interactor in GAINING text store, and - as always - reverse that order for the LosingTextStore code.
         */
        TextInteractor::HookLosingTextStore ();
        IMAGER::HookLosingTextStore ();
    }
    template <typename IMAGER>
    void InteractorImagerMixinHelper<IMAGER>::HookGainedNewTextStore ()
    {
        IMAGER::HookGainedNewTextStore ();
        TextInteractor::HookGainedNewTextStore ();
    }

    //  class   InteractorInteractorMixinHelper<INTERACTOR1,INTERACTOR2>
    template <typename INTERACTOR1, typename INTERACTOR2>
    InteractorInteractorMixinHelper<INTERACTOR1, INTERACTOR2>::InteractorInteractorMixinHelper ()
        : INTERACTOR1 ()
        , INTERACTOR2 ()
    {
    }
    template <typename INTERACTOR1, typename INTERACTOR2>
    void InteractorInteractorMixinHelper<INTERACTOR1, INTERACTOR2>::HookLosingTextStore ()
    {
        INTERACTOR1::HookLosingTextStore ();
        INTERACTOR2::HookLosingTextStore ();
    }
    template <typename INTERACTOR1, typename INTERACTOR2>
    void InteractorInteractorMixinHelper<INTERACTOR1, INTERACTOR2>::HookGainedNewTextStore ()
    {
        INTERACTOR1::HookGainedNewTextStore ();
        INTERACTOR2::HookGainedNewTextStore ();
    }
    template <typename INTERACTOR1, typename INTERACTOR2>
    void InteractorInteractorMixinHelper<INTERACTOR1, INTERACTOR2>::DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) noexcept
    {
        INTERACTOR1::DidUpdateText (updateInfo);
        INTERACTOR2::DidUpdateText (updateInfo);
    }

}

#endif /*_Stroika_Framework_Led_TextInteractorMixIns_inl_*/
