/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Common_h_
#define _Stroika_Foundation_Execution_Common_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>
#include <type_traits>

namespace Stroika {
    namespace Foundation {
        namespace Execution {

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
             *  This will be fixed and obsoleted by C++17 (http://en.cppreference.com/w/cpp/language/class_template_argument_deduction)
             *
             *  BUT CONTINUE EXPLAIN - ONLY COST OF THIS IS CHECK IN DTOR WHICH COMPILER CAN PROBABLY OPTIMIZE AWAY
             *
             *  As of C++11, you cannot have type of object returned inferred from CTOR arguments. Buy you CAN do this for
             *  a regular function (e.g. make_unique_lock) and have that infer a type, which you then use in the return type.
             *
             *  Not sure this is safe, cuz of order of initializing copy to etc... Need to test on gcc/clang...
             *
             *  Note - though:
             *      template    <typename   MUTEX>
             *      inline  std::lock_guard<MUTEX>   make_lock_guard (MUTEX& m)
             *      {
             *          return std::lock_guard<MUTEX> (m);
             *      }
             *  works on visual studio.net 2k13, it should not (compiler bug - cannot copy std::unique_lock).
             *
             *  See:
             *      @see qCompilerAndStdLib_make_unique_lock_IsSlow
             *
             *  \par Example Usage
             *      \code
             *      static  SOME_MUTEXT_TYPE    sLock_;
             *      auto    critSec { Execution::make_unique_lock (sLock_) };
             *      \endcode
             */
            template <typename MUTEX, typename... ARGS>
            inline auto make_unique_lock (MUTEX&& m, ARGS&&... args) -> std::unique_lock<typename std::remove_reference<MUTEX>::type>
            {
                using namespace std;
                return unique_lock<typename remove_reference<MUTEX>::type> (forward<MUTEX> (m), forward<ARGS> (args)...);
            }

/**
             *  For use when qCompilerAndStdLib_make_unique_lock_IsSlow, and until we find a better way (probably C++17).
             *
             *  @see make_unique_lock and @see qCompilerAndStdLib_make_unique_lock_IsSlow.
             *
             *  Note that the utility of this macro is that you dont need to redundantly specify the type as is normal practice
             *  with lock_guard<>.
             */
#if __cplusplus >= kStrokia_Foundation_Configuration_cplusplus_17
#define MACRO_LOCK_GUARD_CONTEXT(theMutex) \
    auto critSec = std::lock_guard{theMutex};
#else
#define MACRO_LOCK_GUARD_CONTEXT(theMutex) \
    std::lock_guard<decltype (theMutex)> critSec{theMutex};
#endif
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "Common.inl"

#endif /*_Stroika_Foundation_Execution_Common_h_*/
