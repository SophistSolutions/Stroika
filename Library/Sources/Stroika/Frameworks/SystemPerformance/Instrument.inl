/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_SystemPerformance_Instrument_inl_
#define _Stroika_Frameworks_SystemPerformance_Instrument_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::SystemPerformance {

    /*
     ********************************************************************************
     ************************ SystemPerformance::Instrument *************************
     ********************************************************************************
     */
    inline MeasurementSet Instrument::Capture ()
    {
        AssertNotNull (fCapFun_.get ());
        return fCapFun_.get ()->Capture ();
    }
    template <>
    inline VariantValue Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut)
    {
        MeasurementSet ms = Capture ();
        if (measurementTimeOut != nullptr) {
            *measurementTimeOut = ms.fMeasuredAt;
        }
        for (auto ii : ms.fMeasurements) {
            return ii.fValue;
        }
        AssertNotReached (); // only use this on insturments with one result returned
        return VariantValue ();
    }
    template <typename T>
    inline T Instrument::CaptureOneMeasurement (Range<DurationSecondsType>* measurementTimeOut)
    {
        // This function is typically template specialized by Instruments to avoid the round trip through VariantValues, but this is
        // logically correct (just slower).
        return fObjectVariantMapper.ToObject<T> (CaptureOneMeasurement<VariantValue> (measurementTimeOut));
    }

}

#endif /*_Stroika_Frameworks_SystemPerformance_Instrument_inl_*/
