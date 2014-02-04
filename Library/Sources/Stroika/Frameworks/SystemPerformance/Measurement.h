/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Measurement_h_
#define _Stroika_Framework_SystemPerformance_Measurement_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Containers/Collection.h"
#include    "../../Foundation/DataExchange/VariantValue.h"
#include    "../../Foundation/Time/DateTimeRange.h"

#include    "MeasurementTypes.h"


/*
 * TODO:
 *      @todo
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {


            using   namespace   Stroika::Foundation;
            using   Characters::String;
            using   Containers::Collection;
            using   DataExchange::VariantValue;
            using   Time::DateTime;
            using   Time::DateTimeRange;


            /**
             *
             */
            struct  Measurement {
                MeasurementType fType;
                VariantValue    fValue;
            };


            /**
             *
             */
            struct  Measurements {
                DateTimeRange    fMeasuredAt;

                Collection<Measurement> fMeasurements;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Measurement.inl"

#endif  /*_Stroika_Framework_SystemPerformance_Measurement_h_*/
