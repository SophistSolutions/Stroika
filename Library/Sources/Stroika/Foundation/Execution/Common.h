/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Common_h_
#define _Stroika_Foundation_Execution_Common_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>
#include <type_traits>

namespace Stroika::Foundation {
    namespace Execution {

        /**
         *      See:
         *          http://stackoverflow.com/questions/22502606/why-is-stdlock-guard-not-movable
         *          http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3602.html
         *
         *  This will be fixed and obsoleted by C++17 (http://en.cppreference.com/w/cpp/language/class_template_argument_deduction)
         */
        template <typename MUTEX, typename... ARGS>
        [[deprecated ("use              [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_}; - make_unique_lock () deprecated in Stroika v2.1d3")]] inline auto make_unique_lock (MUTEX&& m, ARGS&&... args) -> std::unique_lock<typename std::remove_reference<MUTEX>::type>
        {
            using namespace std;
            return unique_lock<typename remove_reference<MUTEX>::type> (forward<MUTEX> (m), forward<ARGS> (args)...);
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
