/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_WordWrappedTextInteractor_h_
#define _Stroika_Frameworks_Led_WordWrappedTextInteractor_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

/*
@MODULE:    WordWrappedTextInteractor
@DESCRIPTION:
        <p>This module provides the class @'WordWrappedTextInteractor' - a simple helper class to deal
    with mixing together @'WordWrappedTextImager' and @'TextInteractor'.</p>
 */

#include "Stroika/Frameworks/Led/TextInteractorMixins.h"
#include "Stroika/Frameworks/Led/WordWrappedTextImager.h"

namespace Stroika::Frameworks::Led {

#if qStroika_Frameworks_Led_SupportGDI
    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning

    /*
    @CLASS:         WordWrappedTextInteractor
    @BASES:         @'InteractorImagerMixinHelper<IMAGER>', <IMAGER=@'WordWrappedTextImager'>
    @DESCRIPTION:
                <p>Simple mixin of @'WordWrappedTextImager' and @'TextInteractor'
            (using the utility class @'InteractorImagerMixinHelper<IMAGER>').</p>
    */
    class WordWrappedTextInteractor : public InteractorImagerMixinHelper<WordWrappedTextImager> {
    private:
        using inherited = InteractorImagerMixinHelper<WordWrappedTextImager>;

    protected:
        WordWrappedTextInteractor () = default;

    public:
        virtual void OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed,
                                             bool controlPressed, bool altKeyPressed) override;

        /*
            *  Must combine behaviors of different mixins.
            */
    public:
        virtual void    SetTopRowInWindow (size_t newTopRow) override;
        nonvirtual void SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode);
        virtual void    SetTopRowInWindow (RowReference row) override;
        nonvirtual void SetTopRowInWindow (RowReference row, UpdateMode updateMode);

        // Speed tweek - use rowreferences...
    public:
        virtual void SetTopRowInWindowByMarkerPosition (size_t markerPos, UpdateMode updateMode = eDefaultUpdate) override;
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WordWrappedTextInteractor.inl"

#endif /*_Stroika_Frameworks_Led_WordWrappedTextInteractor_h_*/
