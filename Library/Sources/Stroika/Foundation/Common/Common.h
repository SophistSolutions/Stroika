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

#if qCompilerAndStdLib_AssumeWarningSpamming_Buggy
    // INTENTIONALLY UNBALANCED WITH _END - cuz this is used all over the place!!!
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wassume\"");
#endif

    /**
     *  The assume attribute was introduced in c++23, and Stroika OPTIONALLY supports this, but doesn't require it as of Stroika v3.
     *  So use _ASSUME_ATTRIBUTE_ () to conditionally use [[assume(X)]]
     */
#if __has_cpp_attribute(assume)
#define _ASSUME_ATTRIBUTE_(X) [[assume (X)]];
#elif _MSC_VER
    // Docs not clear.
    // https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1774r4.pdf suggests this hack. BUT...
    // https://github.com/MicrosoftDocs/cpp-docs/blob/main/docs/build/optimization-best-practices.md seems to hint __assume doesn't evaluate X, except to pay attention to simple a>constant compares so this should be OK
#define _ASSUME_ATTRIBUTE_(X) __assume (X);
#else
#define _ASSUME_ATTRIBUTE_(X)
#endif

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
