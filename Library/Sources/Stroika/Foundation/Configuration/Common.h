/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Common_h_
#define _Stroika_Foundation_Configuration_Common_h_ 1

#include <ciso646>
#include <cstddef>
#include <cstdint>

namespace Stroika {
    namespace Foundation {
        using namespace std;
    }
}

namespace Stroika {
    namespace Foundation {
        namespace Configuration {

            /**
             *  \def nonvirtual
             *      Is for documentation purposes, to make clear a method is intended to be not 'virtual'
             *
             *  \par Example Usage
             *      \code
             *          // Conventional std c++
             *          struct base {
             *              int f1 ();
             *              virtual int f2();
             *          };
             *          struct derived : base { 
             *              // f2 is a virtual override here, but it sure isn't clear from the declaration
             *              int f2 ();
             *          };
             *
             *          // In Stroika, this will always be written as:
             *          struct base {
             *              nonvirtual int f1 ();
             *              virtual int f2();
             *          };
             *          struct derived : base { 
             *              // f2 is a virtual override here, but it sure isn't clear from the declaration
             *              virtual int f2 () override;
             *          };
             *      \endcode
             *
             *  So its always clear at the point of declaration if a function is virtual or not, and if virtual, if its
             *  a new virtual or override. 
             *
             *  \note   This is \em not really enforced by the compiler (the 'conventional std c++ code' produces no diagnostic).
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
#include "Common.inl"

#endif /*_Stroika_Foundation_Configuration_Common_h_*/
