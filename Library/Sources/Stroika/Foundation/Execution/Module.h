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
             *  @see GetEXEDir
             *
             *  The variant returning TString is useful especially when you need to avoid other Stroika
             *  dependencies, such as low level coding and avoiding deadly embraces with tracelog code.
             */
            TString GetEXEDirT ();


            /**
             *  @see GetEXEPath
             *
             *  The variant returning TString is useful especially when you need to avoid other Stroika
             *  dependencies, such as low level coding and avoiding deadly embraces with tracelog code.
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
