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
    void WriteOnlyProperty<T>::Set (T const& value)
    {
        fSetter_ (value);
    }

    template <typename T>
    T WriteOnlyProperty<T>::operator= (T const& value)
    {
        Set (value);
        return value;
    }

    /*
     ********************************************************************************
     ****************************** Property<T> *****************************
     ********************************************************************************
     */
    template <typename T>
    template <typename G, typename S>
    Property<T>::Property (G getter, S setter)
        : ReadOnlyProperty<T> (getter)
        , WriteOnlyProperty<T> (setter)
    {
    }
    template <typename T>
    T Property<T>::operator= (const T& value)
    {
        WriteOnlyProperty<T>::operator= (value);
        return value;
    }
    template <typename T>
    auto Property<T>::operator= (const Property& value) -> Property&
    {
        Set (value.Get ());
        return *this;
    }
    template <typename T>
    T Property<T>::Get () const
    {
        return ReadOnlyProperty<T>::Get ();
    }
    template <typename T>
    void Property<T>::Set (const T& value)
    {
        WriteOnlyProperty<T>::Set (value);
    }
}

#endif /*_Stroika_Foundation_Execution_Property_inl_*/
