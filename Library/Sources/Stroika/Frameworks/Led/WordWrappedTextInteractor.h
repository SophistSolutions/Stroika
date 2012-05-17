/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef __WordWrappedTextInteractor_h__
#define __WordWrappedTextInteractor_h__ 1

#include    "../../Foundation/StroikaPreComp.h"

/*
@MODULE:    WordWrappedTextInteractor
@DESCRIPTION:
        <p>This module provides the class @'WordWrappedTextInteractor' - a simple helper class to deal
    with mixing together @'WordWrappedTextImager' and @'TextInteractor'.</p>
 */


#include    "TextInteractorMixins.h"
#include    "WordWrappedTextImager.h"




namespace   Stroika {
    namespace   Frameworks {
        namespace   Led {



#if     qSilenceAnnoyingCompilerWarnings && __MWERKS__
            // Shut-off SetDefaultFont() warning instantiating InteractorImagerMixinHelper<>
#pragma warn_hidevirtual    off
#endif
#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4250)        //qQuiteAnnoyingDominanceWarnings
#endif



            /*
            @CLASS:         WordWrappedTextInteractor
            @BASES:         @'InteractorImagerMixinHelper<IMAGER>', <IMAGER=@'WordWrappedTextImager'>
            @DESCRIPTION:
                        <p>Simple mixin of @'WordWrappedTextImager' and @'TextInteractor'
                    (using the utility class @'InteractorImagerMixinHelper<IMAGER>').</p>
            */
            class   WordWrappedTextInteractor : public InteractorImagerMixinHelper<WordWrappedTextImager> {
            private:
                typedef InteractorImagerMixinHelper<WordWrappedTextImager>  inherited;
            protected:
                WordWrappedTextInteractor ();

            public:
                virtual         void    OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed) override;

                /*
                 *  Must combine behaviors of different mixins.
                 */
            public:
                virtual     void    SetTopRowInWindow (size_t newTopRow) override;
                nonvirtual  void    SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode);
                virtual     void    SetTopRowInWindow (RowReference row) override;
                nonvirtual  void    SetTopRowInWindow (RowReference row, UpdateMode updateMode);

                // Speed tweek - use rowreferences...
            public:
                virtual     void    SetTopRowInWindowByMarkerPosition (size_t markerPos, UpdateMode updateMode = eDefaultUpdate) override;
            };









            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */
//  class   WordWrappedTextInteractor
            inline  void    WordWrappedTextInteractor::SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode) {
                TemporarilySetUpdateMode    updateModeSetter (*this, updateMode);
                SetTopRowInWindow (newTopRow);
            }
            inline  void    WordWrappedTextInteractor::SetTopRowInWindow (RowReference row, UpdateMode updateMode) {
                TemporarilySetUpdateMode    updateModeSetter (*this, updateMode);
                SetTopRowInWindow (row);
            }


        }
    }
}

#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (pop)
#endif
#if     qSilenceAnnoyingCompilerWarnings && __MWERKS__
#pragma warn_hidevirtual    reset
#endif


#endif  /*__WordWrappedTextInteractor_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
