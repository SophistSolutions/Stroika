/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Module_h_
#define _Stroika_Foundation_Execution_Module_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Configuration/Common.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            using   Characters::TString;
            using   Characters::String;


            /**
             */
            String GetEXEDir ();


            /**
             */
            String GetEXEPath ();


            /**
             */
            TString GetEXEDirT ();


            /**
             */
            TString GetEXEPathT ();


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Execution_Module_h_*/
