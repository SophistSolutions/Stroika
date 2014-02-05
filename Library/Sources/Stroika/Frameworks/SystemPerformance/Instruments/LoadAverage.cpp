/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Debug/Assertions.h"

#include    "../CommonMeasurementTypes.h"

#include    "LoadAverage.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;




/*
 ********************************************************************************
 ************************* Instruments::GetLoadAverage **************************
 ********************************************************************************
 */
#if     qSupport_SystemPerformance_Instruments_LoadAverage
Instrument  SystemPerformance::Instruments::GetLoadAverage ()
{
    static  Instrument  kLoadAverageInstrument_    = Instrument (
                InstrumentNameType (L"Load-Average"),
    [] () -> Measurements {
        Measurements    results;
        double loadAve[3];
        DateTime    before = DateTime::Now ();
        int result = getloadavg(loadAve, 3);
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
        if (result == 3)
        {
            Mapping<String, VariantValue> v;
            v.Add (L"1-minute", loadAve[0]);
            v.Add (L"5-minute", loadAve[1]);
            v.Add (L"15-minute", loadAve[2]);
            Measurement m;
            m.fValue = VariantValue (v);
            m.fType = kLoadAverage;
            results.fMeasurements.Add (m);
        }
        return results;
    },
    {kLoadAverage}
            );
    return kLoadAverageInstrument_;
}
#endif
