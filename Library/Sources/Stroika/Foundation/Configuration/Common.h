/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Common_h_
#define _Stroika_Foundation_Configuration_Common_h_ 1

#include    <cstddef>
#include    <cstdint>
#include    <ciso646>



namespace   Stroika {
    namespace   Foundation {
        using   namespace   std;
    }
}



namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /**
             *  \def nonvirtual
             *      Is for documentation purposes, to make clear a method is intended to be not 'virtual'
             */
#define nonvirtual


        }
    }
}





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Common.inl"

#endif  /*_Stroika_Foundation_Configuration_Common_h_*/
