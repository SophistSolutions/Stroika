/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_TypeHints_h_
#define _Stroika_Foundation_Configuration_TypeHints_h_ 1

#include    <type_traits>



namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /**
             *  \brief  This is an alias for 'T' - but how we want to pass it on stack as formal paraemter.
             *
             *  This is NOT intended to be used very widely, but can be used to decide systematically (by type)
             *  if you should pass a given argument by value or by const reference.
             */
            template    <typename   T>
            using   ArgByValueType = typename std::conditional < (sizeof(T) <= sizeof(void*)) and std::is_trivially_copyable<T>::value, T, const T& >::type;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif  /*_Stroika_Foundation_Configuration_TypeHints_h_*/
