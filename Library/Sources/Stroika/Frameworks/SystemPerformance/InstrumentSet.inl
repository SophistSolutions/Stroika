/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_SystemPerformance_InstrumentSet_inl_
#define _Stroika_Frameworks_SystemPerformance_InstrumentSet_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Foundation/Containers/Common.h"

namespace Stroika {
    namespace Frameworks {
        namespace SystemPerformance {

            /*
             ********************************************************************************
             **************** InstrumentSetTraits_::EqualsCompareFunctionType ***************
             ********************************************************************************
             */
            inline bool Private_::InstrumentSetTraits_::EqualsCompareFunctionType::Equals (const Instrument& l, const Instrument& r)
            {
                return l.fInstrumentName == r.fInstrumentName;
            }

            /*
             ********************************************************************************
             **************** InstrumentSetTraits_::WellOrderCompareFunctionType ************
             ********************************************************************************
             */
            inline int Private_::InstrumentSetTraits_::WellOrderCompareFunctionType::Compare (const Instrument& l, const Instrument& r)
            {
                return l.fInstrumentName.GetPrintName ().Compare (r.fInstrumentName.GetPrintName ());
            }
            inline bool Private_::InstrumentSetTraits_::WellOrderCompareFunctionType::Equals (const Instrument& l, const Instrument& r)
            {
                return l.fInstrumentName == r.fInstrumentName;
            }
        }
    }
}
#endif /*_Stroika_Frameworks_SystemPerformance_InstrumentSet_inl_*/
