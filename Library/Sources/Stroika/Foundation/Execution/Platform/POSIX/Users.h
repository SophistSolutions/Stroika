/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_POSIX_Users_h_
#define _Stroika_Foundation_Execution_Platform_POSIX_Users_h_   1

#include    "../../../StroikaPreComp.h"

#if     !qPlatform_POSIX
#error  "ONLY INCLUDE FOR POSIX"
#endif

#include    <pwd.h>

#include    "../../../Configuration/Common.h"
#include    "../../../Characters/String.h"

#include    "../../Exceptions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Platform {
                namespace   POSIX {

                    using   Characters::String;

                    uid_t   UserName2UID (const String& name);
                    String  uid_t2UserName (uid_t uid);

                    // This returns the uid of the actual original user - not the effectvie permissions
                    uid_t   GetUID ();

                    uid_t   GetEffectiveUID ();

                }
            }
        }
    }
}



#endif  /*_Stroika_Foundation_Execution_Platform_POSIX_Users_h_*/
