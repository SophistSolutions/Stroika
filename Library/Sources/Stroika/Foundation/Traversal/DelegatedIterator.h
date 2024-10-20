/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DelegatedIterator_h_
#define _Stroika_Foundation_Traversal_DelegatedIterator_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Memory/SharedByValue.h"
#include "Stroika/Foundation/Traversal/Iterator.h"

/**
 *  \file
 *
 *  TODO:
 *          @todo   Review definition of DelegatedIterator<>::Rep::GetOwner(). Not sure we've selected the
 *                  the best definition.
 *
 */

namespace Stroika::Foundation::Traversal {

    /**
     *  Handy helper to combine (or track) iterators
     */
    template <typename T, typename EXTRA_DATA = void>
    class DelegatedIterator : public Iterator<T> {
    public:
        struct Rep : Iterator<T>::IRep {
            //  using RepSmartPtr = typename Iterator<T>::RepSmartPtr;
            using IRep = typename Iterator<T>::IRep;
            Iterator<T> fDelegateTo;
            EXTRA_DATA  fExtraData;
            Rep (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData = EXTRA_DATA ());
            virtual unique_ptr<IRep> Clone () const override;
            virtual void             More (optional<T>* result, bool advance) override;
            virtual bool             Equals (const IRep* rhs) const override;
        };
        DelegatedIterator (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData = EXTRA_DATA ());
    };
    template <typename T>
    class DelegatedIterator<T, void> : public Iterator<T> {
    public:
        struct Rep : Iterator<T>::IRep {
            // using RepSmartPtr = typename Iterator<T>::RepSmartPtr;
            using IRep = typename Iterator<T>::IRep;
            Iterator<T> fDelegateTo;
            Rep (const Iterator<T>& delegateTo);
            virtual unique_ptr<IRep> Clone () const override;
            virtual void             More (optional<T>* result, bool advance) override;
            virtual bool             Equals (const IRep* rhs) const override;
        };
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "DelegatedIterator.inl"

#endif /*_Stroika_Foundation_Traversal_DelegatedIterator_h_ */
