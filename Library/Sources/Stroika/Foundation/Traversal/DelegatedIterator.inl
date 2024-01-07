/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DelegatedIterator_inl_
#define _Stroika_Foundation_Traversal_DelegatedIterator_inl_

#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Traversal {

    /*
     ********************************************************************************
     *********************** DelegatedIterator<T, EXTRA_DATA>::Rep ******************
     ********************************************************************************
     */
    template <typename T, typename EXTRA_DATA>
    DelegatedIterator<T, EXTRA_DATA>::Rep::Rep (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData)
        : fDelegateTo (delegateTo)
        , fExtraData (extraData)
    {
    }
    template <typename T, typename EXTRA_DATA>
    auto DelegatedIterator<T, EXTRA_DATA>::Rep::Clone () const -> unique_ptr<IRep>
    {
        return make_unique<Rep> (*this);
    }
    template <typename T, typename EXTRA_DATA>
    void DelegatedIterator<T, EXTRA_DATA>::Rep::More (optional<T>* result, bool advance)
    {
        fDelegateTo.GetRep ().More (result, advance);
    }
    template <typename T, typename EXTRA_DATA>
    bool DelegatedIterator<T, EXTRA_DATA>::Rep::Equals (const IRep* rhs) const
    {
        return fDelegateTo.ConstGetRep ().Equals (rhs);
    }
    template <typename T>
    inline DelegatedIterator<T, void>::Rep::Rep (const Iterator<T>& delegateTo)
        : fDelegateTo (delegateTo)
    {
    }

    /*
     ********************************************************************************
     *********************** DelegatedIterator<T, EXTRA_DATA> ***********************
     ********************************************************************************
     */
    template <typename T, typename EXTRA_DATA>
    DelegatedIterator<T, EXTRA_DATA>::DelegatedIterator (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData)
        : Iterator<T> (make_unique<Rep> (delegateTo, extraData))
    {
    }

}

#endif /* _Stroika_Foundation_Traversal_DelegatedIterator_inl_ */
