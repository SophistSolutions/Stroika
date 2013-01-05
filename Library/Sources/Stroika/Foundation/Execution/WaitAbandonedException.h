/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitAbandonedException_h_
#define _Stroika_Foundation_Execution_WaitAbandonedException_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"

#include    "StringException.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {

            class   WaitAbandonedException : public StringException {
            public:
                WaitAbandonedException ();
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Execution_WaitAbandonedException_h_*/
