/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_MeasurementTypes_h_
#define _Stroika_Framework_SystemPerformance_MeasurementTypes_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Containers/Set.h"
#include    "../../Foundation/DataExchange/Atom.h"

#include    "Measurement.h"

/*
 * TODO:
 *      @todo       PERHAPS RENAME TO MeasuremetnTypeMetaInfoRegistry?
 *                  DOC OPTIONAL - bUT CNA BE used to store extra info about measurement types
 *
 *
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {


            using   namespace   Stroika::Foundation;
            using   Characters::String;
            using   Containers::Set;



            /**
             *          <<<not sure we need this registry>>>
             *
             *      THIS MAYBE HELPFUL - DO DOCUMENT METAINFO ABOUT REGISTERED METAINFO - PRETTY NAME ASSOCIATED WITH ATOM (without the dash),
             *      And maybe a doc-string?
             */
            struct  MeasurementTypeRegistry {
            public:
                static  MeasurementTypeRegistry&    Get ();

            public:
                nonvirtual  Set<MeasurementType>    GetRegisteredTypes () const;

            public:
                nonvirtual  void                    SetRegisteredTypes (const Set<MeasurementType>& mt);

            public:
                nonvirtual  void                    AddRegisteredTypes (MeasurementType mt);

            private:
                Set<MeasurementType>    fMeasuredTypes_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "MeasurementTypes.inl"

#endif  /*_Stroika_Framework_SystemPerformance_MeasurementTypes_h_*/
