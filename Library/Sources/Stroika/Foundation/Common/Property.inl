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
        : fGetter_ (getter)
    {
    }
    template <typename T>
    inline T ReadOnlyProperty<T>::Get () const
    {
        return fGetter_ (this);
    }
    template <typename T>
    inline ReadOnlyProperty<T>::operator const T () const
    {
        return fGetter_ (this);
    }
    template <typename T>
    inline const T ReadOnlyProperty<T>::operator() () const
    {
        return fGetter_ (this);
    }

    /*
     ********************************************************************************
     ****************************** WriteOnlyProperty<T> ****************************
     ********************************************************************************
     */
    template <typename T>
    template <typename S>
    constexpr WriteOnlyProperty<T>::WriteOnlyProperty (S setter)
        : fSetter_ (setter)
    {
    }
    template <typename T>
    inline void WriteOnlyProperty<T>::Set (Configuration::ArgByValueType<T> value)
    {
        fSetter_ (this, value);
    }
    template <typename T>
    inline void WriteOnlyProperty<T>::operator= (Configuration::ArgByValueType<T> value)
    {
        Set (value);
    }

    /*
     ********************************************************************************
     ***************************** Property<T, READ_T> ******************************
     ********************************************************************************
     */
    template <typename T, typename READ_T>
    template <typename G, typename S>
    inline Property<T, READ_T>::Property (G getter, S setter)
        : ReadOnlyProperty<READ_T>{getter}
        , WriteOnlyProperty<T>{setter}
    {
    }
    template <typename T, typename READ_T>
    inline T Property<T, READ_T>::operator= (Configuration::ArgByValueType<T> value)
    {
        WriteOnlyProperty<T>::operator= (value);
        return value;
    }
    template <typename T, typename READ_T>
    inline auto Property<T, READ_T>::operator= (const Property& value) -> Property&
    {
        Set (value.Get ());
        return *this;
    }

}

#endif /*_Stroika_Foundation_Common_Property_inl_*/
