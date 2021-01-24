/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Property_inl_
#define _Stroika_Foundation_Execution_Property_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Execution {

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
        return fGetter_ ();
    }
    template <typename T>
    inline ReadOnlyProperty<T>::operator const T () const
    {
        return fGetter_ ();
    }
    template <typename T>
    inline const T ReadOnlyProperty<T>::operator() () const
    {
        return fGetter_ ();
    }

    /*
     ********************************************************************************
     ****************************** WriteOnlyProperty<T> *****************************
     ********************************************************************************
     */
    template <typename T>
    template <typename S>
    constexpr WriteOnlyProperty<T>::WriteOnlyProperty (S setter)
        : fSetter_ (setter)
    {
    }
    template <typename T>
    inline void WriteOnlyProperty<T>::Set (const Configuration::ArgByValueType<T>& value)
    {
        fSetter_ (value);
    }
    template <typename T>
    inline void WriteOnlyProperty<T>::operator= (const Configuration::ArgByValueType<T>& value)
    {
        Set (value);
    }

    /*
     ********************************************************************************
     ********************************** Property<T> *********************************
     ********************************************************************************
     */
    template <typename T>
    template <typename G, typename S>
    inline Property<T>::Property (G getter, S setter)
        : ReadOnlyProperty<T> (getter)
        , WriteOnlyProperty<T> (setter)
    {
    }
    template <typename T>
    inline T Property<T>::operator= (const Configuration::ArgByValueType<T>& value)
    {
        WriteOnlyProperty<T>::operator= (value);
        return value;
    }
    template <typename T>
    inline auto Property<T>::operator= (const Property& value) -> Property&
    {
        Set (value.Get ());
        return *this;
    }

}

#endif /*_Stroika_Foundation_Execution_Property_inl_*/
