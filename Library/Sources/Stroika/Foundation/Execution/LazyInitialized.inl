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
    {
        // prevent call_once from invoking again
        call_once (fOnceFlag_, [&] () {
            // fOneTimeGetter_ never constructed
            construct_at (&fValue_, v);
        });
    }
    template <typename T>
    constexpr LazyInitialized<T>::~LazyInitialized ()
    {
        bool wasOneTimeGetterCalled = true;
        call_once (fOnceFlag_, [&] () { wasOneTimeGetterCalled = false; });
        if (wasOneTimeGetterCalled) {
            destroy_at (&fValue_);
        }
        else {
            destroy_at (&fOneTimeGetter_);
        }
    }
    template <typename T>
    inline LazyInitialized<T>& LazyInitialized<T>::operator= (const T& rhs)
    {
        bool wasOneTimeGetterCalled = true;
        call_once (fOnceFlag_, [&] () { wasOneTimeGetterCalled = false; });
        if (wasOneTimeGetterCalled) {
            fValue_ = rhs;
        }
        else {
            destroy_at (&fOneTimeGetter_);  // then never called, if set before read
            construct_at (&fValue_, rhs);
        }
        return *this;
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
        // same implementation...
        return const_cast<T&> (const_cast<const LazyInitialized<T>*> (this)->Getter_ ());
    }
    template <typename T>
    inline const T& LazyInitialized<T>::Getter_ () const
    {
        call_once (fOnceFlag_, [&] () {
            // because of union, be careful about overwriting function pointer during function invocation
            auto tmp = fOneTimeGetter_ ();
            destroy_at (&fOneTimeGetter_);
            construct_at (&fValue_, move (tmp));
        });
        return fValue_;
    }

}
