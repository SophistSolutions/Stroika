/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_SimpleTextInteractor_h_
#define _Stroika_Frameworks_Led_SimpleTextInteractor_h_ 1

#include "../StroikaPreComp.h"

/*
@MODULE:    SimpleTextInteractor
@DESCRIPTION:
        <p>SimpleTextInteractor.</p>
 */

#include "SimpleTextImager.h"
#include "TextInteractorMixins.h"

namespace Stroika::Frameworks::Led {

    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
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
    DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SimpleTextInteractor.inl"

#endif /*_Stroika_Frameworks_Led_SimpleTextInteractor_h_*/
