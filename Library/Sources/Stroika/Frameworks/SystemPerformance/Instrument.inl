/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
        for (const auto& ii : ms.fMeasurements) {
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
        return MeasurementAs<T> (CaptureOneMeasurement<VariantValue> (measurementTimeOut));
    }
    template <typename T>
    inline T Instrument::MeasurementAs (const Measurement& m) const
    {
        Require (fType2MeasurementTypes.Contains (type_id (decay_t<T>)));
        return fObjectVariantMapper.ToObject<T> (m.fValue);
    }
    template <typename T>
    T Instrument::MeasurementAs (const MeasurementSet& m) const
    {
        Require (fType2MeasurementTypes.Contains (type_id (decay_t<T>)));
        MeasurementType mt = fType2MeasurementTypes[type_id (decay_t<T>)];
        Require (m.fMeasurements.Any ([=] (const Measurement& m) { return m.fType == mt; }));
        VariantValue vv = m.fMeasurements.FindFirstThat ([=] (const Measurement& m) { return m.fType == mt; })->fValue;
        return fObjectVariantMapper.ToObject<T> (vv);
    }
    inline bool Instrument::operator== (const Instrument& rhs) const
    {
        return fInstrumentName == rhs.fInstrumentName;
    }

}

#endif /*_Stroika_Frameworks_SystemPerformance_Instrument_inl_*/
