/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_MeasurementTypes_h_
#define _Stroika_Framework_SystemPerformance_MeasurementTypes_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Containers/Set.h"
#include    "../../Foundation/DataExchange/Atom.h"


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
