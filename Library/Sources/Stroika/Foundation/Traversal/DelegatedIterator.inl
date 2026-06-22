/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Traversal {

    /*
     ********************************************************************************
     *********************** DelegatedIterator<T, EXTRA_DATA>::Rep ******************
     ********************************************************************************
     */
    template <typename T, typename EXTRA_DATA>
    DelegatedIterator<T, EXTRA_DATA>::Rep::Rep (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData)
        : fDelegateTo{delegateTo}
        , fExtraData{extraData}
    {
    }
    template <typename T, typename EXTRA_DATA>
    auto DelegatedIterator<T, EXTRA_DATA>::Rep::Clone () const -> unique_ptr<IRep>
    {
        return make_unique<Rep> (*this);
    }
    template <typename T, typename EXTRA_DATA>
    bool DelegatedIterator<T, EXTRA_DATA>::Rep::AtEnd () const
    {
        return fDelegateTo.ConstGetRep ().AtEnd ();
    }
    template <typename T, typename EXTRA_DATA>
    optional<T> DelegatedIterator<T, EXTRA_DATA>::Rep::Current () const
    {
        return fDelegateTo.ConstGetRep ().Current ();
    }
    template <typename T, typename EXTRA_DATA>
    optional<T> DelegatedIterator<T, EXTRA_DATA>::Rep::More ()
    {
        return fDelegateTo.GetRep ().More ();
    }
    template <typename T, typename EXTRA_DATA>
    bool DelegatedIterator<T, EXTRA_DATA>::Rep::Equals (const IRep* rhs) const
    {
        return fDelegateTo.ConstGetRep ().Equals (rhs);
    }

    /*
     ********************************************************************************
     *********************** DelegatedIterator<T, EXTRA_DATA> ***********************
     ********************************************************************************
     */
    template <typename T, typename EXTRA_DATA>
    DelegatedIterator<T, EXTRA_DATA>::DelegatedIterator (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData)
        : Iterator<T>{make_unique<Rep> (delegateTo, extraData)}
    {
    }

}
