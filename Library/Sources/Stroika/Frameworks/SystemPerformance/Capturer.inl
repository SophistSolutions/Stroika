/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Frameworks_SystemPerformance_Capturer_inl_
#define _Stroika_Frameworks_SystemPerformance_Capturer_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::SystemPerformance {

    /*
     ********************************************************************************
     *********************************** Capturer ***********************************
     ********************************************************************************
     */
    inline MeasurementSet Capturer::GetMostRecentMeasurements () const
    {
        return fCurrentMeasurementSet_;
    }

}

#endif /*_Stroika_Frameworks_SystemPerformance_Capturer_inl_*/
