/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
            inline  MeasurementSet  Instrument::Capture ()
            {
                return fCapFun_.get ()->Capture ();
                //return fCaptureFunction ();
            }
            template    <>
            inline  VariantValue    Instrument::CaptureOneMeasurement (DateTimeRange* measurementTimeOut)
            {
                MeasurementSet ms = Capture ();
                if (measurementTimeOut != nullptr) {
                    *measurementTimeOut = ms.fMeasuredAt;
                }
                for (auto ii : ms.fMeasurements) {
                    return ii.fValue;
                }
                AssertNotReached ();    // only use this on insturments with one result returned
                return VariantValue ();
            }
            template    <typename T>
            inline  T   Instrument::CaptureOneMeasurement (DateTimeRange* measurementTimeOut)
            {
                return fObjectVariantMapper.ToObject<T> (CaptureOneMeasurement<VariantValue> (measurementTimeOut));
            }


        }
    }
}
#endif  /*_Stroika_Frameworks_SystemPerformance_Instrument_inl_*/
