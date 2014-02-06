/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Frameworks_SystemPerformance_CaptureSet_inl_
#define _Stroika_Frameworks_SystemPerformance_CaptureSet_inl_  1


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
            inline  CaptureSet::CaptureSet (const Duration& period, const InstrumentSet& instruments)
                : fInstruments_ (instruments)
                , fPeriod_ (period)
            {
            }
            inline  Duration    CaptureSet::GetRunPeriod () const
            {
                return fPeriod_;
            }
            inline  void        CaptureSet::SetRunPeriod (const Duration& p)
            {
                fPeriod_ = p;
            }
            inline  InstrumentSet   CaptureSet::GetInstrumentSet () const
            {
                return fInstruments_;
            }
            inline  void            CaptureSet::SetInstrumentSet (const InstrumentSet& i)
            {
                fInstruments_ = i;
            }


        }
    }
}
#endif  /*_Stroika_Frameworks_SystemPerformance_CaptureSet_inl_*/
