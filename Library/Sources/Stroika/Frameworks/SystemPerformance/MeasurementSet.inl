/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
            fMeasurements.RemoveIf ([=] (const Measurement& mm) { return mm.fType == mi.fType; });
            fMeasurements.Add (mi);
        }
    }

}
