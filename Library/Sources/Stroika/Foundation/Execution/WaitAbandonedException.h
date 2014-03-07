/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitAbandonedException_h_
#define _Stroika_Foundation_Execution_WaitAbandonedException_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"

#include    "StringException.h"


/**
 *  \file
 *
 * TODO:
 *      DEPRECATD
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            class   WaitAbandonedException : public StringException {
            public:
                _DeprecatedFunction_ (WaitAbandonedException (), "Not sure what this was for - but its not used");
            };


        }
    }
}



#endif  /*_Stroika_Foundation_Execution_WaitAbandonedException_h_*/
