/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef __TextInteractorMixins_h__
#define __TextInteractorMixins_h__  1

#include    "../../Foundation/StroikaPreComp.h"

/*
@MODULE:    TextInteractorMixins
@DESCRIPTION:
        <p>TextInteractorMixins are some templates that help deal with some of the drugery of C++ mixins.</p>

 */

#include    "TextInteractor.h"




namespace   Stroika {
    namespace   Frameworks {
        namespace   Led {




#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4250)        //qQuiteAnnoyingDominanceWarnings
#endif


            /*
            @CLASS:         InteractorImagerMixinHelper<IMAGER>
            @BASES:         virtual @'TextInteractor', IMAGER
            @DESCRIPTION:   <p>A utility class to facilitate mixing together Interactors with particular
                        chosen TextImager subclasses.</p>
                            <p>This class is mostly an implementation detail, and shouldn't be of interest to Led users, unless
                        they are creating a new TextImager subclass (which isn't also already a subclass of TextInteractor;
                        this should be rare).</p>
            */
            template    <typename   IMAGER> class   InteractorImagerMixinHelper : public virtual TextInteractor, public IMAGER {
            private:
                typedef void*   inherited;  // prevent accidental references to this name in subclasses to base class name

            protected:
                InteractorImagerMixinHelper ();

                /*
                 *  Must combine behaviors of different mixins.
                 */
            public:
                virtual     void    Draw (const Led_Rect& subsetToDraw, bool printing) override;
                virtual     void    AboutToUpdateText (const MarkerOwner::UpdateInfo& updateInfo) override;
                virtual     void    DidUpdateText (const UpdateInfo& updateInfo) throw () override;
            protected:
                virtual     void    HookLosingTextStore () override;
                virtual     void    HookGainedNewTextStore () override;

                /*
                 *  Disambiguate overloads than can happen down both base class chains.
                 */
            public:
#if     qUsingDirectiveSometimesCausesInternalCompilerErrorBug
                nonvirtual  void    SetWindowRect (const Led_Rect& windowRect, TextInteractor::UpdateMode updateMode = TextInteractor::eDefaultUpdate) {
                    TextInteractor::SetWindowRect (windowRect, updateMode);
                }
                nonvirtual  void    ScrollByIfRoom (long downByRows, UpdateMode updateMode = eDefaultUpdate) {
                    TextInteractor::ScrollByIfRoom (downByRows, updateMode);
                }
                nonvirtual  void    ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos = 0, UpdateMode updateMode = eDefaultUpdate) {
                    TextInteractor::ScrollSoShowing (markerPos, andTryToShowMarkerPos, updateMode);
                }
#else
                using   TextInteractor::SetWindowRect;
                using   TextInteractor::ScrollByIfRoom;
                using   TextInteractor::ScrollSoShowing;
#endif
            };







            /*
            @CLASS:         InteractorInteractorMixinHelper<INTERACTOR1,INTERACTOR2>
            @BASES:         INTERACTOR1, INTERACTOR2
            @DESCRIPTION:   <p>A utility class to facilitate mixing together two Interactors.</p>
                            <p>This class is mostly an implementation detail, and shouldn't be of interest to Led users, unless
                        they are creating a new direct TextImagerInteractor subclass (which should be rare).</p>
            */
            template    <typename   INTERACTOR1, typename INTERACTOR2>  class   InteractorInteractorMixinHelper : public INTERACTOR1, public INTERACTOR2 {
            private:
                typedef void*   inherited;  // prevent accidental references to this name in subclasses to base class name

            protected:
                InteractorInteractorMixinHelper ();

            protected:
                virtual     void    HookLosingTextStore () override;
                virtual     void    HookGainedNewTextStore () override;

            public:
#if     qUsingDirectiveSometimesCausesInternalCompilerErrorBug
                nonvirtual  void    SetWindowRect (const Led_Rect& windowRect, TextInteractor::UpdateMode updateMode = TextInteractor::eDefaultUpdate) {
                    TextInteractor::SetWindowRect (windowRect, updateMode);
                }
#else
                using   INTERACTOR1::SetWindowRect; // Should both be the same - but arbitrarily pick one
#endif

                nonvirtual  void    Invariant () {
                    INTERACTOR1::Invariant ();
                    INTERACTOR2::Invariant ();
                }

            public:
                virtual     void    DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) throw () override;

            };








            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */
//  class   InteractorImagerMixinHelper<IMAGER>
            template    <typename   IMAGER>
            InteractorImagerMixinHelper<IMAGER>::InteractorImagerMixinHelper ():
                TextInteractor (),
                IMAGER () {
            }
            template    <typename   IMAGER>
            void    InteractorImagerMixinHelper<IMAGER>::Draw (const Led_Rect& subsetToDraw, bool printing) {
                DrawBefore (subsetToDraw, printing);
                TextInteractor::Draw (subsetToDraw, printing);
                IMAGER::Draw (subsetToDraw, printing);
                DrawAfter (subsetToDraw, printing);
            }
            template    <typename   IMAGER>
            void    InteractorImagerMixinHelper<IMAGER>::AboutToUpdateText (const UpdateInfo& updateInfo) {
                IMAGER::AboutToUpdateText (updateInfo);
                TextInteractor::AboutToUpdateText (updateInfo);
            }
            template    <typename   IMAGER>
            void    InteractorImagerMixinHelper<IMAGER>::DidUpdateText (const UpdateInfo& updateInfo) throw () {
                IMAGER::DidUpdateText (updateInfo);
                TextInteractor::DidUpdateText (updateInfo);
            }
            template    <typename   IMAGER>
            void    InteractorImagerMixinHelper<IMAGER>::HookLosingTextStore () {
                /*
                 *  NB: See SPR#0836 - order of Imager/Interactor reversed compared to HookGainedNewTextStore () intentionally (mimic CTOR/DTOR ordering).
                 *  Now DO imager first, then interactor in GAINING text store, and - as always - reverse that order for the LosingTextStore code.
                 */
                TextInteractor::HookLosingTextStore ();
                IMAGER::HookLosingTextStore ();
            }
            template    <typename   IMAGER>
            void    InteractorImagerMixinHelper<IMAGER>::HookGainedNewTextStore () {
                IMAGER::HookGainedNewTextStore ();
                TextInteractor::HookGainedNewTextStore ();
            }





//  class   InteractorInteractorMixinHelper<INTERACTOR1,INTERACTOR2>
            template    <typename   INTERACTOR1, typename INTERACTOR2>
            InteractorInteractorMixinHelper<INTERACTOR1, INTERACTOR2>::InteractorInteractorMixinHelper ():
                INTERACTOR1 (),
                INTERACTOR2 () {
            }
            template    <typename   INTERACTOR1, typename INTERACTOR2>
            void    InteractorInteractorMixinHelper<INTERACTOR1, INTERACTOR2>::HookLosingTextStore () {
                INTERACTOR1::HookLosingTextStore ();
                INTERACTOR2::HookLosingTextStore ();
            }
            template    <typename   INTERACTOR1, typename INTERACTOR2>
            void    InteractorInteractorMixinHelper<INTERACTOR1, INTERACTOR2>::HookGainedNewTextStore () {
                INTERACTOR1::HookGainedNewTextStore ();
                INTERACTOR2::HookGainedNewTextStore ();
            }
            template    <typename   INTERACTOR1, typename INTERACTOR2>
            void    InteractorInteractorMixinHelper<INTERACTOR1, INTERACTOR2>::DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) throw () {
                INTERACTOR1::DidUpdateText (updateInfo);
                INTERACTOR2::DidUpdateText (updateInfo);
            }



#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (pop)
#endif


        }
    }
}



#endif  /*__TextInteractorMixins_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
