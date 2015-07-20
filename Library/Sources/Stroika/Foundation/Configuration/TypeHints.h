/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_TypeHints_h_
#define _Stroika_Foundation_Configuration_TypeHints_h_ 1

#include    <type_traits>



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Late-Alpha">Late-Alpha</a>
 *
 * TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /**
             *  \brief  This is an alias for 'T' - but how we want to pass it on stack as formal paraemter.
             *
             *  This is NOT intended to be used very widely, but can be used to decide systematically (by type)
             *  if you should pass a given argument by value or by const reference.
             *
             *  @todo SHOULD use is_trivially_copyable<> but didnt work on gcc47 (and maybe more - look into which and iether
             *      add bug define or?? not worries - not much differnt)
             *
             *  \note   Use sizeof(T) <= 2*sizeof(void*) - because passing by reference involves copying one pointer and then if you
             *          access once, thats a second copy. So may as well copy 2 directly (very loosy goosy, as depends on
             *          releative cost of main memory access versus stack).
             */
            template    <typename   T>
            using   ArgByValueType = typename std::conditional < (sizeof(T) <= 2 * sizeof(void*)) and std::is_trivial<T>::value, T, const T& >::type;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Configuration_TypeHints_h_*/
