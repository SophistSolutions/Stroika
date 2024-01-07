/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Common_h_
#define _Stroika_Foundation_Common_Common_h_ 1

#include <cstddef>
#include <cstdint>

namespace Stroika {
    using namespace std;

    // deal with windows ambiguity - C:\Program Files (x86)\Windows Kits\10\include\10.0.22621.0\shared\rpcndr.h(202): note: could be 'unsigned char byte'
    using std::byte;
}

namespace Stroika::Foundation::Common {

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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Common_Common_h_*/
