/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instrument_h_
#define _Stroika_Framework_SystemPerformance_Instrument_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Containers/Set.h"
#include    "../../Foundation/DataExchange/Atom.h"

#include    "Measurement.h"


/*
 * TODO:
 *      @todo
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {


            using   namespace   Stroika::Foundation;
            using   Characters::String;
            using   Containers::Set;


            // @todo - consider using independent atom registry
            using   MeasurementType =  DataExchange::Atom<>;


            /**
             *          <<<not sure we need this registry>>>
             */
            struct  Instrument {
                String                      fInstrumentName;
                function<Measurements()>    fCaptureFunction;
                Set<MeasurementType>        fCapturedMeasurements;

                Instrument (const String& instrumentName, const function<Measurements()>& capturer, const Set<MeasurementType>& capturedMeasurements);
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Instrument.inl"

#endif  /*_Stroika_Framework_SystemPerformance_Instrument_h_*/
