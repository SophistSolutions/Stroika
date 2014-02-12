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
            inline  Instrument::Instrument (InstrumentNameType instrumentName, const CapturerCallback& capturer, const Set<MeasurementType>& capturedMeasurements)
                : fInstrumentName (instrumentName)
                , fCaptureFunction (capturer)
                , fCapturedMeasurements (capturedMeasurements)
            {
            }
            inline  MeasurementSet  Instrument::Capture () const
            {
                return fCaptureFunction ();
            }


        }
    }
}
#endif  /*_Stroika_Frameworks_SystemPerformance_Instrument_inl_*/
