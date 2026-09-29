/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_TextInteractorMixins_h_
#define _Stroika_Frameworks_Led_TextInteractorMixins_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

/*
@MODULE:    TextInteractorMixins
@DESCRIPTION:
        <p>TextInteractorMixins are some templates that help deal with some of the drugery of C++ mixins.</p>

 */

#include "Stroika/Frameworks/Led/TextInteractor.h"

namespace Stroika::Frameworks::Led {

#if qStroika_Frameworks_Led_SupportGDI
    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning

    /*
    @CLASS:         InteractorImagerMixinHelper<IMAGER>
    @BASES:         virtual @'TextInteractor', IMAGER
    @DESCRIPTION:   <p>A utility class to facilitate mixing together Interactors with particular
                chosen TextImager subclasses.</p>
                    <p>This class is mostly an implementation detail, and shouldn't be of interest to Led users, unless
                they are creating a new TextImager subclass (which isn't also already a subclass of TextInteractor;
                this should be rare).</p>
    */
    template <typename IMAGER>
    class InteractorImagerMixinHelper : public virtual TextInteractor, public IMAGER {
    private:
        using inherited = void*; // prevent accidental references to this name in subclasses to base class name

    protected:
        InteractorImagerMixinHelper () = default;

        /*
            *  Must combine behaviors of different mixins.
            */
    public:
        virtual void Draw (const Led_Rect& subsetToDraw, bool printing) override;
        virtual void AboutToUpdateText (const MarkerOwner::UpdateInfo& updateInfo) override;
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

    protected:
        virtual void HookLosingTextStore () override;
        virtual void HookGainedNewTextStore () override;

        /*
            *  Disambiguate overloads than can happen down both base class chains.
            */
    public:
        using TextInteractor::ScrollByIfRoom;
        using TextInteractor::ScrollSoShowing;
        using TextInteractor::SetWindowRect;
    };

    /*
    @CLASS:         InteractorInteractorMixinHelper<INTERACTOR1,INTERACTOR2>
    @BASES:         INTERACTOR1, INTERACTOR2
    @DESCRIPTION:   <p>A utility class to facilitate mixing together two Interactors.</p>
                    <p>This class is mostly an implementation detail, and shouldn't be of interest to Led users, unless
                they are creating a new direct TextImagerInteractor subclass (which should be rare).</p>
    */
    template <typename INTERACTOR1, typename INTERACTOR2>
    class InteractorInteractorMixinHelper : public INTERACTOR1, public INTERACTOR2 {
    private:
        using inherited = void*; // prevent accidental references to this name in subclasses to base class name

    protected:
        InteractorInteractorMixinHelper ();

    protected:
        virtual void HookLosingTextStore () override;
        virtual void HookGainedNewTextStore () override;

    public:
        using INTERACTOR1::SetWindowRect; // Should both be the same - but arbitrarily pick one

        nonvirtual void Invariant ()
        {
            INTERACTOR1::Invariant ();
            INTERACTOR2::Invariant ();
        }

    public:
        virtual void DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) noexcept override;
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "TextInteractorMixIns.inl"

#endif /*_Stroika_Frameworks_Led_TextInteractorMixins_h_*/
