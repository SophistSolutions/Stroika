/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_VirtaulConstant_h_
#define _Stroika_Foundation_Execution_VirtaulConstant_h_ 1

#include "../StroikaPreComp.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /**
             * \brief Declare what appears to be a constant, but where the value is derived from a function call result each time its used
             *
             *  In C++, you sometimes want to define a global constant, but run into problems because of order of initialization
             *  of global constants (across files). This class solves that problem by allowing you to manage the construction
             *  timing of your constant in a provided function, 
             *
             *  Allow use of regular constant declaration use when we have
             *  an underlying system where the constant is actually FETECHED from the argument function.
             *
             *  \par Example Usage
             *      \code
             *          // Use with ModuleInitializer<> to manage one-time creation of underlying constant, so created just
             *          // once
             *          namespace PRIVATE_ {
             *              inline const InternetMediaType& OctetStream_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kOctetStream_CT; }
             *          }
             *          constexpr VirtaulConstant<InternetMediaType, PRIVATE_::OctetStream_CT> kOctetStream;
             *          ...
             *          InternetMediaType ia = kOctetStream;
             *      \endcode
             *
             *  \par Example Usage
             *      \code
             *          // Use a single static file scope constant (perhaps in one file)
             *          static const String                            x{L"X"};
             *          inline const String&                           kX_ () { return x; }
             *          const VirtaulConstant<String, &kX_> kX;
             *          ...
             *          const String a = kX;
             *      \endcode
             *
             *  \par Example Usage
             *      \code
             *          // Use a static variable initialized within the fetching function
             *          inline const String& kX_ () { static const String x{L"6"}; return x; }
             *          const Execution::VirtaulConstant<String, &kX_> kX;
             *          ...
             *          const String a = kX;
             *      \endcode
             *
             *  @see https://stroika.atlassian.net/browse/STK-381 for details.
             *
             *  \note   it would be HIGHLY DESIRABLE if C++ allowed operator. overloading, as accessing one of these
             *          values without assinging to a temporary first - means that you cannot directly call its methods.
             *          Thats a bit awkward.
             *
             *          So if you have a type T, with method m(), and variable of type T t.
             *          Your starter code might be:
             *              T   t;
             *              t.m ();
             *          When you replace 'T t' with
             *              VirtaulConstant<T,...> t;
             *              you must call t().m();
             *
             *  \note   C++ also only allows one level of automatic operator conversions, so things like comparing
             *          Optional<T> {} == VirtaulConstant<T,...> {} won't work. To workaround, simply
             *          apply () after the VirtaulConstant<> instance.
             *          
             */
            template <typename BASETYPE, const BASETYPE& (*VALUE_GETTER) ()>
            struct VirtaulConstant {
                operator const BASETYPE () const;
                const BASETYPE operator() () const;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "VirtualConstant.inl"

#endif /*_Stroika_Foundation_Execution_VirtaulConstant_h_*/
