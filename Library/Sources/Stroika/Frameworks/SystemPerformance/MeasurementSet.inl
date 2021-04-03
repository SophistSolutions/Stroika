/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Frameworks_SystemPerformance_MeasurementSet_inl_
#define _Stroika_Frameworks_SystemPerformance_MeasurementSet_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Frameworks::SystemPerformance {

    /*
     ********************************************************************************
     ********************************* MeasurementSet *******************************
     ********************************************************************************
     */
    inline void MeasurementSet::MergeAdditions (const MeasurementSet& m)
    {
        Require (this != &m);
        fMeasuredAt = m.fMeasuredAt;
        for (const auto& mi : m.fMeasurements) {
            fMeasurements.Remove ([=] (const Measurement& mm) { return mm.fType == mi.fType; });
            fMeasurements.Add (mi);
        }
    }

}

#endif /*_Stroika_Frameworks_SystemPerformance_MeasurementSet_inl_*/
