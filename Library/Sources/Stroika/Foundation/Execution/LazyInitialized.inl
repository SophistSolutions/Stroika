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
        #if qCompilerAndStdLib_lazyunion_Buggy
        call_once (fOnceFlag_, [&] () {  });
        #else
        call_once (fOnceFlag_, [&] () { destroy_at (&fOneTimeGetter_); }); // nothing todo but prevent call_once from invoking again
        #endif
    }
    template <typename T>
    constexpr LazyInitialized<T>::~LazyInitialized ()
    {
        #if !qCompilerAndStdLib_lazyunion_Buggy
        bool wasCallOnceInvoked = false;
        call_once (fOnceFlag_, [&] () { wasCallOnceInvoked = true; });
        if (wasCallOnceInvoked) {
            destroy_at (&fValue_);
        }
        else {
            destroy_at (&fOneTimeGetter_);
        }
        #endif
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
        call_once (fOnceFlag_, [&] () {
            // because of union, be careful about overwriting function pointer during function invocation
            auto tmp = fOneTimeGetter_ ();
            #if qCompilerAndStdLib_lazyunion_Buggy
            fValue_ = tmp;
            #else
            destroy_at (&fOneTimeGetter_);
            construct_at (&fValue_, move (tmp));
            #endif
        });
        #if qCompilerAndStdLib_lazyunion_Buggy
        return *fValue_;
        #else
        return fValue_;
        #endif
    }
    template <typename T>
    inline const T& LazyInitialized<T>::Getter_ () const
    {
        call_once (fOnceFlag_, [&] () {
            // because of union, be careful about overwriting function pointer during function invocation
            auto tmp = fOneTimeGetter_ ();
            #if qCompilerAndStdLib_lazyunion_Buggy
            fValue_ = tmp;
            #else
            destroy_at (&fOneTimeGetter_);
            construct_at (&fValue_, move (tmp));
            #endif
        });
        #if qCompilerAndStdLib_lazyunion_Buggy
        return *fValue_;
        #else
        return fValue_;
        #endif
    }

}
