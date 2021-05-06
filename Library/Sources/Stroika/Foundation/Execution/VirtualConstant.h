/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_VirtualConstant_h_
#define _Stroika_Foundation_Execution_VirtualConstant_h_ 1

#include "../StroikaPreComp.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 */

namespace Stroika::Foundation::Execution {

    /**
     * \brief Declare what appears to be a constant, but where the value is derived from a function call result (the first time) its used (so lazy initialize)
     *
     *  In C++, you sometimes want to define a global constant, but run into problems because of order of initialization
     *  of global constants (across files). This class solves that problem by allowing you to manage the construction
     *  timing of your constant in a provided function, 
     *
     *  Allow use of regular constant declaration use when we have
     *  an underlying system where the constant is actually FETECHED from the argument function.
     * 
     *  \note - this one-time-computed constant value is then CACHED (so re-used), and called LAZILY, so you can count on
     *          it not being called until the data is first required.
     *
     *  This doesn't work perfectly (e.g. see below about operator.) - but its pretty usable.
     * 
     *  \note The basic idea - any time you have a constant whose initializer depends on other constants and get into trouble
     *        with mutual constructor order issues before main, replace the dependent constant with a VirtualConstant
     *
     *  \par Example Usage
     *      \code
     *          inline String                 kXGetter_ () { return L"X"; }
     *          const VirtualConstant<String> kX {kXGetter_};
     *          ...
     *          const String a = kX;
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          const Execution::VirtualConstant<String> kX {[] () { return L"6"; }};
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
     *              VirtualConstant<T> t;
     *              you must call t().m();
     *          OR
     *              you must call t->m();
     *
     *  \note   C++ also only allows one level of automatic operator conversions, so things like comparing
     *          optional<T> {} == VirtualConstant<T,...> {} won't work. To workaround, simply
     *          apply () after the VirtualConstant<> instance.
     *
     *  \note   constexpr VirtualConstant<> not yet supported, but hopefully will be soon. In the meantime,
     *          it is suggested to use inline const variable declarations.
     *  
     *  TODO:
     *      @todo   See https://stackoverflow.com/questions/53977787/constexpr-version-of-stdfunction - and
     *              get constexpr version of VirtualConstant working
     *          
     */
    template <typename T>
    struct VirtualConstant {
        /**
         *  oneTimeGetter is a function (can be a lambda()) which computes the given value. It is called 
         *  just once, and LAZILY, the first time the given VirtualConstant value is required.
         */
        VirtualConstant () = delete;
        template <typename F>
        constexpr VirtualConstant (F oneTimeGetter);
        VirtualConstant& operator= (const VirtualConstant&) = delete;
        ~VirtualConstant ()                                 = default;

        /**
         *  A virtual constant can be automatically assigned to its underlying base type.
         *  Due to how conversion operators work, this won't always be helpful (like with overloading
         *  or multiple levels of conversions). But when it works (80% of the time) - its helpful.
         */
        nonvirtual operator const T () const;

        /**
         *  This works 100% of the time. Just use the function syntax, and you get back a constant of the desired
         *  underlying type.
         *
         *  \par Example Usage
         *      \code
         *          namespace PredefinedInternetMediaType {  const inline Execution::VirtualConstant<InternetMediaType> kPNG...
         *
         *          bool checkIsImage1 = PredefinedInternetMediaType::kPNG().IsImageFormat ();
         *      \endcode
         */
        nonvirtual const T operator() () const;

        /**
         *  This works 100% of the time. Just use the operator-> syntax, and you get back a constant of the desired
         *  underlying type.
         *
         *  \par Example Usage
         *      \code
         *          namespace PredefinedInternetMediaType {  const inline Execution::VirtualConstant<InternetMediaType> kPNG = ...
         *
         *          bool checkIsImage2 = PredefinedInternetMediaType::kPNG->IsImageFormat ();
         *      \endcode
         */
        nonvirtual const T* operator-> () const;

    private:
        const function<T ()> fOneTimeGetter_;
        const T&             Getter_ () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "VirtualConstant.inl"

#endif /*_Stroika_Foundation_Execution_VirtualConstant_h_*/
