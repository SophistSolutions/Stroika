/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_InstrumentSet_h_
#define _Stroika_Framework_SystemPerformance_InstrumentSet_h_ 1

#include    "../StroikaPreComp.h"

#include    "../../Foundation/Containers/Set.h"
#include    "../../Foundation/DataExchange/Atom.h"

#include    "Instrument.h"


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


            namespace Private_ {
                struct   CaptureSetTraits_ {
                    struct EqualsCompareFunctionType  {
                        static bool Equals (const Instrument& l, const Instrument& r);
                    };
                    using   SetTraitsType = CaptureSetTraits_;
                };
            }


            /*
             *
             */
            using InstrumentSet = Set<Instrument, Private_::CaptureSetTraits_>;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "InstrumentSet.inl"

#endif  /*_Stroika_Framework_SystemPerformance_InstrumentSet_h_*/
