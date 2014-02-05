/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Frameworks_SystemPerformance_InstrumentSet_inl_
#define _Stroika_Frameworks_SystemPerformance_InstrumentSet_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Foundation/Containers/Common.h"

namespace   Stroika {
    namespace   Frameworks  {
        namespace   SystemPerformance {


            /*
             ********************************************************************************
             **************** CaptureSetTraits_::EqualsCompareFunctionType ******************
             ********************************************************************************
             */
            inline  bool    Private_::CaptureSetTraits_::EqualsCompareFunctionType::Equals (const Instrument& l, const Instrument& r)
            {
                return l.fInstrumentName == r.fInstrumentName;
            }


        }
    }
}
#endif  /*_Stroika_Frameworks_SystemPerformance_InstrumentSet_inl_*/
