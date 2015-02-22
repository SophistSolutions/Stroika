/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ThreadAbortException_h_
#define _Stroika_Foundation_Execution_ThreadAbortException_h_   1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"

#include    "Exceptions.h"
#include    "Thread.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


#if 1
            using   ThreadAbortException = Thread::AbortException;
#else

            // Generally should not be reported. It's just to help force a thread to shut itself down
            class   ThreadAbortException {
            public:
                ThreadAbortException ();
            };
#endif


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ThreadAbortException.inl"

#endif  /*_Stroika_Foundation_Execution_ThreadAbortException_h_*/
