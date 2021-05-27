/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_VirtualConstant_h_
#define _Stroika_Foundation_Execution_VirtualConstant_h_ 1

#include "../StroikaPreComp.h"

namespace Stroika::Foundation::Execution {
    template <typename T>
    struct [[deprecated ("Since Stroika 2.1b12 use ConstantProperty")]] VirtualConstant
    {
        VirtualConstant () = delete;
        template <typename F>
        constexpr VirtualConstant (F oneTimeGetter);

        VirtualConstant& operator= (const VirtualConstant&) = delete;

        ~VirtualConstant () = default;

        nonvirtual operator const T () const;

        nonvirtual const T operator() () const;

        nonvirtual const T* operator-> () const;

    private:
        const function<T ()> fOneTimeGetter_;
        mutable optional<T>  fCachedValue_;
        mutable once_flag    fValueInitialized_;
        const T&             Getter_ () const;
    };
    template <typename T>
    template <typename F>
    constexpr VirtualConstant<T>::VirtualConstant (F oneTimeGetter)
        : fOneTimeGetter_{oneTimeGetter}
    {
    }
    template <typename T>
    inline VirtualConstant<T>::operator const T () const
    {
        return Getter_ ();
    }
    template <typename T>
    inline const T VirtualConstant<T>::operator() () const
    {
        return Getter_ ();
    }
    template <typename T>
    inline const T* VirtualConstant<T>::operator-> () const
    {
        return &(Getter_ ());
    }
    template <typename T>
    const inline T& VirtualConstant<T>::Getter_ () const
    {
        call_once (fValueInitialized_, [&] () { fCachedValue_ = fOneTimeGetter_ (); });
        Ensure (fCachedValue_.has_value ());
        return *fCachedValue_;
    }
}

#endif /*_Stroika_Foundation_Execution_VirtualConstant_h_*/
