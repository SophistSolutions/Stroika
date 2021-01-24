/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Property_h_
#define _Stroika_Foundation_Execution_Property_h_ 1

#include "../StroikaPreComp.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 */

namespace Stroika::Foundation::Execution {

    template <typename T>
    struct ReadOnlyProperty {
        /**
         *  oneTimeGetter is a function (can be a lambda()) which computes the given value. It is called 
         *  just once, and LAZILY, the first time the given VirtualConstant value is required.
         */
        ReadOnlyProperty () = delete;
        template <typename G>
        constexpr ReadOnlyProperty (G getter)
            : fGetter_ (getter)
        {
        }

        // get/set syntax
        T Get () const
        {
            return fGetter_ ();
        }

        /**
         *  A virtual constant can be automatically assigned to its underlying base type.
         *  Due to how conversion operators work, this won't always be helpful (like with overloading
         *  or multiple levels of conversions). But when it works (80% of the time) - its helpful.
         */
        nonvirtual operator const T () const
        {
            return fGetter_ ();
        }

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
        nonvirtual const T operator() () const
        {
            return fGetter_ ();
        }

        typedef T value_type; // might be useful for template deductions

    private:
        const function<T ()> fGetter_;
    };

    template <typename T>
    struct WriteOnlyProperty {
        /**
         *  oneTimeGetter is a function (can be a lambda()) which computes the given value. It is called 
         *  just once, and LAZILY, the first time the given VirtualConstant value is required.
         */
        WriteOnlyProperty () = delete;
        template <typename S>
        constexpr WriteOnlyProperty (S setter)
            : fSetter_ (setter)
        {
        }

        void Set (T const& value)
        {
            fSetter_ (value);
        }
        T operator= (T const& value)
        {
            Set (value);
            return value;
        }
        typedef T value_type; // might be useful for template deductions

    private:
        const function<void (T)> fSetter_;
    };

    template <typename T>
    struct Property : ReadOnlyProperty<T>, WriteOnlyProperty<T> {
        /**
         */
        Property () = delete;
        template <typename G, typename S>
        Property (G getter, S setter)
            : ReadOnlyProperty<T> (getter)
            , WriteOnlyProperty<T> (setter)
        {
        }
        T operator= (const T& value)
        {
            WriteOnlyProperty<T>::operator= (value);
            return value;
        }
        Property& operator= (const Property& value)
        {
            Set (value.Get ());
            return *this;
        }
        T Get () const
        {
            return ReadOnlyProperty<T>::Get ();
        }
        void Set (const T& value)
        {
            WriteOnlyProperty<T>::Set (value);
        }
        typedef T value_type; // might be useful for template deductions
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Property.inl"

#endif /*_Stroika_Foundation_Execution_Property_h_*/
