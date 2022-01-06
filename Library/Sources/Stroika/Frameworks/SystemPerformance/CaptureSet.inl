/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Frameworks_SystemPerformance_CaptureSet_inl_
#define _Stroika_Frameworks_SystemPerformance_CaptureSet_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::SystemPerformance {

    /*
     ********************************************************************************
     *********************** SystemPerformance::CaptureSet **************************
     ********************************************************************************
     */
    inline void CaptureSet::AddInstrument (const Instrument& i)
    {
        fInstruments_.Add (i);
    }

}

#endif /*_Stroika_Frameworks_SystemPerformance_CaptureSet_inl_*/
