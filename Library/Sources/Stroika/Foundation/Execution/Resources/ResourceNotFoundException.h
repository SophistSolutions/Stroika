/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Resources_ResourceNotFoundException_h_
#define _Stroika_Foundation_Execution_Resources_ResourceNotFoundException_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"

#include    "../StringException.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            using   Characters::String;


            class   ResourceNotFoundException : public StringException {
            public:
                ResourceNotFoundException ();
            };


        }
    }
}



#endif  /*_Stroika_Foundation_Execution_Resources_ResourceNotFoundException_h_*/
