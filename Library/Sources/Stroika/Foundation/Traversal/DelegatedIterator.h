/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DelegatedIterator_h_
#define _Stroika_Foundation_Traversal_DelegatedIterator_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Empty.h"
#include "Stroika/Foundation/Memory/SharedByValue.h"
#include "Stroika/Foundation/Traversal/Iterator.h"

/**
 *  \file
 *
 */

namespace Stroika::Foundation::Traversal {

    /**
     *  Handy helper to combine (or track) iterators
     */
    template <typename T, typename EXTRA_DATA = Common::Empty>
    class DelegatedIterator : public Iterator<T> {
    public:
        struct Rep : Iterator<T>::IRep {
            using IRep = typename Iterator<T>::IRep;
            Iterator<T>                                             fDelegateTo;
            qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE EXTRA_DATA fExtraData;
            Rep (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData = EXTRA_DATA{});
            virtual unique_ptr<IRep> Clone () const override;
            virtual bool             AtEnd () const override;
            virtual optional<T>      Current () const override;
            virtual optional<T>      More () override;
            virtual bool             Equals (const IRep* rhs) const override;
        };
        DelegatedIterator (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData = EXTRA_DATA{});
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "DelegatedIterator.inl"

#endif /*_Stroika_Foundation_Traversal_DelegatedIterator_h_ */
