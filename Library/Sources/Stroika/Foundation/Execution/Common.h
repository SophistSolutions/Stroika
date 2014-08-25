/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Common_h_
#define _Stroika_Foundation_Execution_Common_h_  1

#include    "../StroikaPreComp.h"

#include    <mutex>




namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  Because of defects with lock_guard, we use unique_lock. The defect is basically that
             *  you cannot type-infer the type of the mutex.
             *
             *  I briefly tried:
             *      Simple utility to allow replacing:
             *          lock_guard<mutex> critSec (fMutex);
             *      with:
             *          LOCK_GUARD_CONTEXT (fMutex);
             *
             *      The key advantage being not needed to KNOW the TYPEOF fMutex in two places (not brevity).
             *
             *      See:
             *          http://stackoverflow.com/questions/22502606/why-is-stdlock-guard-not-movable
             *          http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3602.html
             *
             *      #define LOCK_GUARD_CONTEXT(theMutex)\
             *          std::lock_guard<decltype(theMutex)> critSec { theMutex };
             *
             *  Hopefully this will be fixed and obsoleted by C++17.
             *
             *  BUT CONTINUE EXPLAIN - ONLY COST OF THIS IS CHECK IN DTOR WHICH COMPILER CAN PROBABLY OPTIMIZE AWAY
             *
             *  As of C++11, you cannot have type of object returned inferred from arguments. Buy you CAN do this for
             *  a regular function (e.g. make_unique_lock) and have that infer a type, which you then use in the return type.
             *
             *  Not sure this is safe, cuz of order of initializing copy to etc... Need to test on gcc/clang...
             */
            template    <typename   MUTEX>
            inline  std::unique_lock<MUTEX>   make_unique_lock (MUTEX& m)
            {
                return unique_lock<MUTEX> (m);
            }

            // test probs wont work
            template    <typename   MUTEX>
            inline  std::lock_guard<MUTEX>   make_lock_guard (MUTEX& m)
            {
                return lock_guard<MUTEX> (m);
            }


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "Common.inl"

#endif  /*_Stroika_Foundation_Execution_Common_h_*/
