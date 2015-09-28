/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_AIX_GetEXEPathWithHint_h_
#define _Stroika_Foundation_Execution_Platform_AIX_GetEXEPathWithHint_h_   1

#include    "../../../StroikaPreComp.h"

#if     !qPlatform_AIX
#error  "ONLY INCLUDE FOR AIX"
#endif


#include    "../../../Configuration/Common.h"
#include    "../../../Characters/String.h"

#include    "../../Module.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Platform {
                namespace   AIX {


                    using   Characters::SDKString;
                    using   Characters::String;


                    /**
                    *  Return the full path to the given process(TODO EXPLAIN)
                    */
                    SDKString   GetEXEPathWithHintT (pid_t processID);
                    SDKString   GetEXEPathWithHintT (pid_t processID, const SDKString& associationHint);
                    String      GetEXEPathWithHint (pid_t processID);
                    String      GetEXEPathWithHint (pid_t processID, const String& associationHint);


                }
            }
        }
    }
}



#endif  /*_Stroika_Foundation_Execution_Platform_AIX_GetEXEPathWithHint_h_*/
