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



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *
 *      @todo   Cleanup implementation of GetEXEPathWithHintT, so the remaining popen/shell code is
 *              eliminated. This is not ONLY for performance reasons, but also correctness. The bit
 *              about greping through df output is very unsafe (if one block device name subset of another? or
 *              of mountpoint name?).
 *
 *      @todo   Sterl offered good performance trick - when we need to search - somehow pick better directories
 *              to look in first. For example, /bin, /usr/bin, etc. We can even do this adaptively, by keeping
 *              an LRUCache of names, and keep the top 10 or so around (maybe per FS)?
 *
 *              But as this code is all on AIX, its not been super easy to tell how much of a performance impact
 *              this code has, so its not been prioritized yet.
 */



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
