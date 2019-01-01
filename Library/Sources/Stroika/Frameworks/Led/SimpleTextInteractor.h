/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_SimpleTextInteractor_h_
#define _Stroika_Frameworks_Led_SimpleTextInteractor_h_ 1

#include "../../Foundation/StroikaPreComp.h"

/*
@MODULE:    SimpleTextInteractor
@DESCRIPTION:
        <p>SimpleTextInteractor.</p>
 */

#include "SimpleTextImager.h"
#include "TextInteractorMixins.h"

namespace Stroika::Frameworks::Led {

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250) // inherits via dominance warning
#endif
    /*
    @CLASS:         SimpleTextInteractor
    @BASES:         InteractorImagerMixinHelper<SimpleTextImager>
    @DESCRIPTION:
            <p>Simple mixin of @'SimpleTextImager' and @'TextInteractor' (using the utility class @'InteractorImagerMixinHelper<IMAGER>').
        You might use this class as an argument to the mixin template for your class library wrapper class, as in
        Led_MFC_X&ltSimpleTextInteractor&gt, for MFC.</p>
    */
    class SimpleTextInteractor : public InteractorImagerMixinHelper<SimpleTextImager> {
    private:
        using inherited = InteractorImagerMixinHelper<SimpleTextImager>;

    protected:
        SimpleTextInteractor ();

    protected:
        virtual void TabletChangedMetrics () override;
        virtual void ChangedInterLineSpace (PartitionMarker* pm) override;

        /*
         *  Must combine behaviors of different mixins.
         */
    public:
        virtual void    SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont) override;
        nonvirtual void SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont, UpdateMode updateMode);
        virtual void    SetTopRowInWindow (size_t newTopRow) override;
        nonvirtual void SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode);
        virtual void    SetTopRowInWindow (RowReference row) override;
        nonvirtual void SetTopRowInWindow (RowReference row, UpdateMode updateMode);

        // Speed tweek - use rowreferences...
    public:
        virtual void SetTopRowInWindowByMarkerPosition (size_t markerPos, UpdateMode updateMode = eDefaultUpdate) override;
    };
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */
    //  class   SimpleTextInteractor
    inline void SimpleTextInteractor::SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont, TextInteractor::UpdateMode updateMode)
    {
        TextInteractor::SetDefaultFont (defaultFont, updateMode);
    }
    inline void SimpleTextInteractor::SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode)
    {
        TemporarilySetUpdateMode updateModeSetter (*this, updateMode);
        SetTopRowInWindow (newTopRow);
    }
    inline void SimpleTextInteractor::SetTopRowInWindow (RowReference row, UpdateMode updateMode)
    {
        TemporarilySetUpdateMode updateModeSetter (*this, updateMode);
        SetTopRowInWindow (row);
    }

}

#endif /*_Stroika_Frameworks_Led_SimpleTextInteractor_h_*/
