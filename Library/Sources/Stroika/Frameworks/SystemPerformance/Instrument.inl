/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::SystemPerformance {

    /*
     ********************************************************************************
     ************************ SystemPerformance::Instrument *************************
     ********************************************************************************
     */
    inline Instrument::Instrument (Instrument&& src) noexcept
        : Instrument{move (src.fInstrumentName_), move (src.fCaptureRep_), move (src.fCapturedMeasurementTypes_),
                     move (src.fType2MeasurementTypes_), move (src.fObjectVariantMapper_)}
    {
    }
    inline Instrument::Instrument (const Instrument& src)
        : Instrument{src.fInstrumentName_, src.fCaptureRep_->Clone (), src.fCapturedMeasurementTypes_, src.fType2MeasurementTypes_, src.fObjectVariantMapper_}
    {
    }
    inline MeasurementSet Instrument::Capture ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
        AssertNotNull (fCaptureRep_);
        return fCaptureRep_->Capture ();
    }
    template <>
    inline VariantValue Instrument::CaptureOneMeasurement (Range<TimePointSeconds>* measurementTimeOut)
    {
        MeasurementSet ms = Capture ();
        if (measurementTimeOut != nullptr) {
            *measurementTimeOut = ms.fMeasuredAt;
        }
        for (const auto& ii : ms.fMeasurements) {
            return ii.fValue;
        }
        AssertNotReached (); // only use this on insturments with one result returned
        return VariantValue{};
    }
    template <typename T>
    inline T Instrument::CaptureOneMeasurement (Range<TimePointSeconds>* measurementTimeOut)
    {
        // This function is typically template specialized by Instruments to avoid the round trip through VariantValues, but this is
        // logically correct (just slower).
        return MeasurementAs<T> (CaptureOneMeasurement<VariantValue> (measurementTimeOut));
    }
    template <typename T>
    inline T Instrument::MeasurementAs (const Measurement& m) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock{fThisAssertExternallySynchronized_};
        Require (fType2MeasurementTypes_.ContainsKey (typeid (decay_t<T>)));
        Require (m.fType == fType2MeasurementTypes_[typeid (decay_t<T>)]);
        return fObjectVariantMapper_.ToObject<T> (m.fValue);
    }
    template <typename T>
    optional<T> Instrument::MeasurementAs (const MeasurementSet& m) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock{fThisAssertExternallySynchronized_};
        Require (fType2MeasurementTypes_.ContainsKey (typeid (decay_t<T>)));
        MeasurementType mt = fType2MeasurementTypes_[typeid (decay_t<T>)];
        if (auto i = m.fMeasurements.Find ([=] (const Measurement& m) { return m.fType == mt; })) {
            return fObjectVariantMapper_.ToObject<T> (i->fValue);
        }
        return nullopt;
    }
    inline bool Instrument::operator== (const Instrument& rhs) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext readLock{fThisAssertExternallySynchronized_};
        return fInstrumentName_ == rhs.fInstrumentName_;
    }

}
