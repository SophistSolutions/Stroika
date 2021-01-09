/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_VirtualConstant_inl_
#define _Stroika_Foundation_Execution_VirtualConstant_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************* VirtualConstant<T> ********************
     ********************************************************************************
     */
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
        static const T kCachedValue_ = fOneTimeGetter_ ();
        return kCachedValue_;
    }


}

#endif /*_Stroika_Foundation_Execution_VirtualConstant_inl_*/
