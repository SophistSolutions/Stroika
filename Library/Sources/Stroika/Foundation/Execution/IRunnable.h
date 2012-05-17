/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_IRunnable_h_
#define _Stroika_Foundation_Execution_IRunnable_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedPtr.h"

#include    "CriticalSection.h"
#include    "Exceptions.h"
#include    "Event.h"




namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {

            /*
             * This simple class is the basis of Stroika deferred run-task support (and threading etc). Just create Shared<IRunnable>
             * entites and you can pass the tasks around, and the shared semantics exactly keep all the results and code etc alive
             * until the last reference.
             */
            class   IRunnable {
            private:
                NO_COPY_CONSTRUCTOR (IRunnable);
                NO_ASSIGNMENT_OPERATOR (IRunnable);

            public:
                IRunnable ();
                virtual ~IRunnable ();
                /*
                 * Note - is is perfectly legal to throw an exception out of 'Run()' - and it depends on the containier or 'runner' you are using if/how that
                 * exception will be reported/propagated.
                 */
            public:
                virtual void    Run () = 0;
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Execution_IRunnable_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "IRunnable.inl"
