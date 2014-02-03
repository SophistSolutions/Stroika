/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Capturers_LoadAverage_h_
#define _Stroika_Framework_SystemPerformance_Capturers_LoadAverage_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Containers/Set.h"
#include    "../../../Foundation/DataExchange/Atom.h"

#include    "../CaptureRegistry.h"


/*
 * TODO:
 *      @todo
 */


namespace   Stroika {
    namespace   Frameworks {
        namespace   SystemPerformance {
	        namespace   Capturers {


            using   namespace   Stroika::Foundation;
            using   Characters::String;
            using   Containers::Set;


			// @todo PROBABLY do substype style - like we do with containers (Seqwuqnce and Sqeunce_Array) so in CTOR you can specify params

			extern	CaptureRegistry::Capturer	kLoadAverage;

        }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "LoadAverage.inl"

#endif  /*_Stroika_Framework_SystemPerformance_Capturers_LoadAverage_h_*/
