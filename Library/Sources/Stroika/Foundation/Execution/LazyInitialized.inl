/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_LazyInitialized_inl_
#define _Stroika_Foundation_Execution_LazyInitialized_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************************** LazyInitialized<T> ******************************
     ********************************************************************************
     */
    template <typename T>
    LazyInitialized<T>::LazyInitialized (const function<T (void)>& ctor)
        : fCTOR_{ctor}
    {
    }
    template <typename T>
    LazyInitialized<T>::LazyInitialized (const T& v)
        : fValue_{v}
    {
    }

    template <typename T>
    LazyInitialized<T>::operator T ()
    {
        if (not fValue_) {
            call_once (fOneFlag_, &LazyInitialized::doInit_, this);
        }
        return *fValue_;
    }
    template <typename T>
    T* LazyInitialized<T>::operator->()
    {
        if (not fValue_) {
            call_once (fOneFlag_, &LazyInitialized::doInit_, this);
        }
        return &*fValue_;
    }
    template <typename T>
    const T* LazyInitialized<T>::operator->() const
    {
        if (not fValue_) {
            call_once (fOneFlag_, &LazyInitialized::doInit_, this);
        }
        return &*fValue_;
    }
    template <typename T>
    void LazyInitialized<T>::doInit_ ()
    {
        fValue_ = fCTOR_ ();
    }

}

#endif /*_Stroika_Foundation_Execution_LazyInitialized_inl_*/
