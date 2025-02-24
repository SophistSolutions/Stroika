/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */

#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ****************************** LazyInitialized<T> ******************************
     ********************************************************************************
     */
    template <typename T>
    template <invocable F>
    constexpr LazyInitialized<T>::LazyInitialized (F&& oneTimeGetter)
        requires (convertible_to<invoke_result_t<F>, T>)
        : fOneTimeGetter_{forward<F> (oneTimeGetter)}
    {
    }
    template <typename T>
    constexpr LazyInitialized<T>::LazyInitialized (const T& v)
        : fValue_{v}
    {
    }
    template <typename T>
    constexpr LazyInitialized<T>::operator const T () const
    {
        return Getter_ ();
    }
    template <typename T>
    inline const T LazyInitialized<T>::operator() () const
    {
        return Getter_ ();
    }
    template <typename T>
    inline T* LazyInitialized<T>::operator->()
    {
        return &(Getter_ ());
    }
    template <typename T>
    inline const T* LazyInitialized<T>::operator->() const
    {
        return &(Getter_ ());
    }
    template <typename T>
    inline T& LazyInitialized<T>::Getter_ ()
    {
        if (!fValue_) {
            call_once (fOneFlag_, [&] () { fValue_ = fOneTimeGetter_ (); });
        }
        Ensure (fValue_.has_value ());
        return *fValue_;
    }
    template <typename T>
    inline const T& LazyInitialized<T>::Getter_ () const
    {
        if (!fValue_) {
            call_once (fOneFlag_, [&] () { fValue_ = fOneTimeGetter_ (); });
        }
        Ensure (fValue_.has_value ());
        return *fValue_;
    }

}
