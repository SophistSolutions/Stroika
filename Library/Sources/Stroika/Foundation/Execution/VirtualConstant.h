/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_VirtualConstant_h_
#define _Stroika_Foundation_Execution_VirtualConstant_h_ 1

#include "../StroikaPreComp.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
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
             *  This doesn't work perfectly (e.g. see below about operator.) - but its pretty usable.
             *
             *  \par Example Usage
             *      \code
             *          // Use with ModuleInitializer<> to manage one-time creation of underlying constant, so created just
             *          // once
             *          namespace PRIVATE_ {
             *              inline const InternetMediaType& OctetStream_CT () { return Execution::ModuleInitializer<Private_::InternetMediaType_ModuleData_>::Actual ().kOctetStream_CT; }
             *          }
             *
             *          // and use VirtualConstant to make that appear a regular constant of type InternetMediaType (mostly)
             *          constexpr VirtualConstant<InternetMediaType, PRIVATE_::OctetStream_CT> kOctetStream;
             *          ...
             *          InternetMediaType ia = kOctetStream;
             *      \endcode
             *
             *  \par Example Usage
             *      \code
             *          // Use a single static file scope constant (perhaps in one file)
             *          static const String                            x{L"X"};
             *          inline const String&                           kX_ () { return x; }
             *          const VirtualConstant<String, &kX_> kX;
             *          ...
             *          const String a = kX;
             *      \endcode
             *
             *  \par Example Usage
             *      \code
             *          // Use a static variable initialized within the fetching function
             *          inline const String& kX_ () { static const String x{L"6"}; return x; }
             *          const Execution::VirtualConstant<String, &kX_> kX;
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
             *              VirtualConstant<T,...> t;
             *              you must call t().m();
             *          OR
             *              you must call t->m();
             *
             *  \note   C++ also only allows one level of automatic operator conversions, so things like comparing
             *          optional<T> {} == VirtualConstant<T,...> {} won't work. To workaround, simply
             *          apply () after the VirtualConstant<> instance.
             *
             *  \note   It's suggested you always declare VirtualConstants as constexpr to help assure they take up no
             *          data space in your application - and are just syntactic sugar on accessing the underlying accessor
             *          function.
             *
             *  \req    VALUE_GETTER template parameter must return a pointer to a constant object of lifetime >=
             *          the first time VALUE_GETTER to well-after (OK  that's a little ambiguous - but should be OK
             *          in general).
             *
             *  TODO:
             *      @todo   https://stroika.atlassian.net/browse/STK-639 - It would be nice if the
             *              VirtualConstant<> template could auto-compute typename T, or support using 
             *              lambdas for the function
             *          
             */
            template <typename BASETYPE, const BASETYPE& (*VALUE_GETTER) ()>
            struct VirtualConstant {
                /**
                 *  A virtual constant can be automatically assigned to its underlying base type.
                 *  Due to how conversion operators work, this won't always be helpful (like with overloading
                 *  or multiple levels of conversions). But when it works (80% of the time) - its helpful.
                 */
                operator const BASETYPE () const;

                /**
                 *  This works 100% of the time. Just use the function syntax, and you get back a constant of the desired
                 *  underlying type.
                 *
                 *  \par Example Usage
                 *      \code
                 *          namespace PredefinedInternetMediaType {  constexpr Execution::VirtualConstant<InternetMediaType,...> kImage_PNG;
                 *
                 *          bool checkIsImage1 = kImage_PNG ().IsImageFormat ();
                 *          bool checkIsImage3 = kImage_PNG.IsImageFormat ();       // FAIL - WONT COMPILE - the main difference between VirtualConstant and actual constant of type T
                 *      \endcode
                 */
                const BASETYPE operator() () const;

                /**
                 *  This works 100% of the time. Just use the operator-> syntax, and you get back a constant of the desired
                 *  underlying type.
                 *
                 *  \par Example Usage
                 *      \code
                 *          namespace PredefinedInternetMediaType {  constexpr Execution::VirtualConstant<InternetMediaType,...> kImage_PNG;
                 *
                 *          bool checkIsImage2 = kImage_PNG->IsImageFormat ();
                 *          bool checkIsImage3 = kImage_PNG.IsImageFormat ();       // FAIL - WONT COMPILE - the main difference between VirtualConstant and actual constant of type T
                 *      \endcode
                 */
                const BASETYPE* operator-> () const;
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

#endif /*_Stroika_Foundation_Execution_VirtualConstant_h_*/
