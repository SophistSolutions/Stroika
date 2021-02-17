/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Property_inl_
#define _Stroika_Foundation_Common_Property_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#if !qStroika_Foundation_Common_Property_EmbedThisInProperties
#include "../Memory/ObjectFieldUtilities.h"
#endif

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ****************************** ReadOnlyProperty<T> *****************************
     ********************************************************************************
     */
    template <typename T>
    template <typename G>
    constexpr ReadOnlyProperty<T>::ReadOnlyProperty (G getter)
        : fGetter_ (getter) // no uniform initialization because this may involve conersions
    {
    }
#if !qCompilerAndStdLib_template_enable_if_const_nonconst_overload_Buggy
    template <typename T>
    template <typename CHECK, enable_if_t<not ReadOnlyProperty<T>::template kIsMutatableType<CHECK>>*>
    inline T ReadOnlyProperty<T>::Get () const
    {
        return fGetter_ (this);
    }
    template <typename T>
    template <typename CHECK, enable_if_t<ReadOnlyProperty<T>::template kIsMutatableType<CHECK>>*>
    inline T ReadOnlyProperty<T>::Get ()
    {
        return Get ();
    }
#endif
#if !qCompilerAndStdLib_template_enable_if_const_nonconst_overload_Buggy
    template <typename T>
    template <typename CHECK, enable_if_t<not ReadOnlyProperty<T>::template kIsMutatableType<CHECK>>*>
    inline ReadOnlyProperty<T>::operator const T () const
    {
        return Get ();
    }
    template <typename T>
    template <typename CHECK, enable_if_t<ReadOnlyProperty<T>::template kIsMutatableType<CHECK>>*>
    inline ReadOnlyProperty<T>::operator T ()
    {
        return Get ();
    }
#endif
#if !qCompilerAndStdLib_template_enable_if_const_nonconst_overload_Buggy
    template <typename T>
    template <typename CHECK, enable_if_t<not ReadOnlyProperty<T>::template kIsMutatableType<CHECK>>*>
    inline const T ReadOnlyProperty<T>::operator() () const
    {
        return Get ();
    }
    template <typename T>
    template <typename CHECK, enable_if_t<ReadOnlyProperty<T>::template kIsMutatableType<CHECK>>*>
    inline T ReadOnlyProperty<T>::operator() ()
    {
        return Get ();
    }
#endif

    /*
     ********************************************************************************
     ****************************** WriteOnlyProperty<T> ****************************
     ********************************************************************************
     */
    template <typename T>
    template <typename S>
    constexpr WriteOnlyProperty<T>::WriteOnlyProperty (S setter)
        : fSetter_ (setter) // no uniform initialization because this may involve conersions
    {
    }
    template <typename T>
    inline void WriteOnlyProperty<T>::Set (Configuration::ArgByValueType<T> value)
    {
        fSetter_ (this, value);
    }
    template <typename T>
    inline void WriteOnlyProperty<T>::operator() (Configuration::ArgByValueType<T> value)
    {
        Set (value);
    }
    template <typename T>
    inline void WriteOnlyProperty<T>::operator= (Configuration::ArgByValueType<T> value)
    {
        Set (value);
    }

    /*
     ********************************************************************************
     ******************************** Property<T> ***********************************
     ********************************************************************************
     */
    template <typename T>
    template <typename G, typename S>
    inline Property<T>::Property (G getter, S setter)
        : ReadOnlyProperty<T>{getter}
        , WriteOnlyProperty<decayed_value_type>{setter}
    {
    }
    template <typename T>
    inline T Property<T>::operator= (Configuration::ArgByValueType<T> value)
    {
        WriteOnlyProperty<decayed_value_type>::operator= (value);
        return value;
    }
    template <typename T>
    inline auto Property<T>::operator= (const Property& value) -> Property&
    {
        Set (value.Get ());
        return *this;
    }

}

#endif /*_Stroika_Foundation_Common_Property_inl_*/
