/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Instrument_h_
#define _Stroika_Framework_SystemPerformance_Instrument_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Containers/Set.h"
#include    "../../Foundation/DataExchange/Atom.h"
#include    "../../Foundation/DataExchange/ObjectVariantMapper.h"
#include    "../../Foundation/Execution/Function.h"

#include    "Measurement.h"
#include    "MeasurementSet.h"


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


            /**
             *  @todo - consider using independent atom registry
             */
            using   InstrumentNameType =  DataExchange::Atom<>;


            /**
             */
            using   CapturerCallback = Execution::Function<MeasurementSet()>;


            /**
             *          <<<not sure we need this registry>>>
             */
            struct  Instrument {
                InstrumentNameType                  fInstrumentName;
                CapturerCallback                    fCaptureFunction;
                Set<MeasurementType>                fCapturedMeasurements;
                DataExchange::ObjectVariantMapper   fObjectVariantMapper;

                Instrument (InstrumentNameType instrumentName, const CapturerCallback& capturer, const Set<MeasurementType>& capturedMeasurements, const DataExchange::ObjectVariantMapper& objectVariantMapper);


                /**
                 */
                nonvirtual  MeasurementSet  Capture () const;


                /**
                 *  Require just one measurmenet
                 */
                template    <typename T>
                nonvirtual    T CaptureOneMeasurement () const;
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
