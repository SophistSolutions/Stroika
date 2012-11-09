/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_SimpleRunnable_h_
#define _Stroika_Foundation_Execution_SimpleRunnable_h_ 1

#include    "../StroikaPreComp.h"

#include    <functional>

#include    "../Memory/BlockAllocated.h"

#include    "IRunnable.h"


/**
 *  \file
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {

            /**
             *	SimpleRunnable is an easy way to map a simple object or function (optionally with args) to
			 *	an IRunnable - the basic type used in threading and background processing classes.
			 *
			 *	Note - this class isn't free. Using std::function as a way to capture your funciton is
			 *	less efficient than just doing your own IRunnable subclass directly.
             */
            class   SimpleRunnable : public IRunnable {
            public:
                SimpleRunnable (const std::function<void()>& fun2CallOnce);

            public:
                virtual void    Run () override;

            public:
                static  IRunnablePtr    MAKE (const std::function<void()>& fun2CallOnce);

            private:
                std::function<void()>   fCall_;

            public:
                DECLARE_USE_BLOCK_ALLOCATION(SimpleRunnable);
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Execution_SimpleRunnable_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SimpleRunnable.inl"
