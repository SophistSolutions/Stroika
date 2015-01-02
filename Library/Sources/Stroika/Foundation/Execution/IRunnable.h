/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_IRunnable_h_
#define _Stroika_Foundation_Execution_IRunnable_h_  1

#include    "../StroikaPreComp.h"

#include    <functional>

#include    "../Configuration/Common.h"

#include    "Exceptions.h"



/**
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 *  \file
 *
 * TODO:
 *      @todo   Consider if IRunnable should inherit from shared_ptr<T>::enable_shared_from_this<>? Reason
 *              for YES is that it may sometimes be useful. We also ONLY use IRunnable with shared_ptr<T> -
 *              so probably causes no harm. I gues sit depends how hard it is to mixin later (mix Irunnable
 *              and enable_shared_from_this)?
 *
 *      @todo   Investigate if any codesize penalty for always using std::function instead of c++
 *              style function pointer in calls (LARGELY DONE BUT DOCUMENT CLEARLY AND TEST ON GCC AS WELL).
 *
 *              Document someplace this is why we use IRunnablePtr in many places, instead of std::function.
 *              If it turns out there is no performance overhead with std::function compared to IRunnablePtr
 *              consdier just switching to std::funtion.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  This simple class is the basis of Stroika deferred run-task support (and threading etc).
             *  Just create Shared<IRunnable> entites and you can pass the tasks around, and the shared
             *  semantics exactly keep all the results and code etc alive until the last reference.
             *
             *  @see mkIRunnablePtr
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            class   IRunnable {
            public:
                IRunnable ();
                IRunnable (const IRunnable&) = delete;
                virtual ~IRunnable ();

            public:
                nonvirtual  IRunnable& operator= (const IRunnable&) = delete;

            public:
                /**
                 *  Note - is is perfectly legal to throw an exception out of 'Run()' - and it depends on the
                 *  containier or 'runner' you are using if/how that exception will be reported/propagated.
                 */
                virtual void    Run () = 0;
            };


            /**
             */
            using   IRunnablePtr    =   shared_ptr<IRunnable>;


            /**
             *  mkIRunnablePtr is an easy way to map a simple object or function (optionally with args) to
             *  an IRunnable - the basic type used in threading and background processing classes.
             *
             *  Note - there is more runtime overhead in using the std::function<> variant than the
             *  bare function pointer variant, but not too much in either case. But the most efficent
             *  way is generally to do your own IRunnable subclass.
             */
            IRunnablePtr    mkIRunnablePtr (void (*fun2CallOnce) ());
            IRunnablePtr    mkIRunnablePtr (void (*fun2CallOnce) (void* arg), void* arg);
            IRunnablePtr    mkIRunnablePtr (const function<void()>& fun2CallOnce);
            IRunnablePtr    mkIRunnablePtr (const IRunnablePtr& fun2CallOnce);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "IRunnable.inl"

#endif  /*_Stroika_Foundation_Execution_IRunnable_h_*/
