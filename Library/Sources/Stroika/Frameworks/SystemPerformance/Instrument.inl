/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Frameworks_SystemPerformance_Instrument_inl_
#define _Stroika_Frameworks_SystemPerformance_Instrument_inl_  1


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
             ************************ SystemPerformance::Instrument *************************
             ********************************************************************************
             */
            inline  Instrument::Instrument (InstrumentNameType instrumentName, const CapturerCallback& capturer, const Set<MeasurementType>& capturedMeasurements, const DataExchange::ObjectVariantMapper& objectVariantMapper)
                : fInstrumentName (instrumentName)
                , fCaptureFunction (capturer)
                , fCapturedMeasurements (capturedMeasurements)
                , fObjectVariantMapper (objectVariantMapper)
            {
            }
            inline  MeasurementSet  Instrument::Capture () const
            {
                return fCaptureFunction ();
            }
            template    <>
            inline  VariantValue    Instrument::CaptureOneMeasurement () const
            {
                MeasurementSet ms = Capture ();
                for (auto ii : ms.fMeasurements) {
                    return ii.fValue;
                }
                AssertNotReached ();    // only use this on insturments with one result returned
                return VariantValue ();
            }
            template    <typename T>
            inline  T   Instrument::CaptureOneMeasurement () const
            {
                return fObjectVariantMapper.ToObject<T> (CaptureOneMeasurement<VariantValue> ());
            }


        }
    }
}
#endif  /*_Stroika_Frameworks_SystemPerformance_Instrument_inl_*/
