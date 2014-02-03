/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_CaptureRegistry_h_
#define _Stroika_Framework_SystemPerformance_CaptureRegistry_h_ 1

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
            struct  CaptureRegistry {
            public:
                static  CaptureRegistry&    Get ();

            public:
                struct Capturer {
                    function<Measurements()>    fCaptureFunction;
                    Set<MeasurementType>        fCaptureResults;
                    String                      fMeasurerName;

                };
                struct   CaptureSetTraits_ {
                    struct EqualsCompareFunctionType  {
                        static bool Equals (Capturer l, Capturer r)
                        {
                            return l.fMeasurerName == r.fMeasurerName;
                        }
                    };
                    using   SetTraitsType               =   CaptureSetTraits_;
                };
                using CapturerSetType = Set<MeasurementType, CaptureSetTraits_>;
            public:
                nonvirtual  CapturerSetType    GetCapturers () const;

            public:
                nonvirtual  void                    SetCapturers (const CapturerSetType& cs);

            public:
                nonvirtual  void                    AddCapturer (const Capturer& c);

            private:
                CapturerSetType fCaptureSet_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "CaptureRegistry.inl"

#endif  /*_Stroika_Framework_SystemPerformance_CaptureRegistry_h_*/
