/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DelegatedIterator_h_
#define _Stroika_Foundation_Traversal_DelegatedIterator_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "../Memory/SharedByValue.h"

#include "Iterator.h"

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
            using IteratorRepSharedPtr = typename Iterator<T>::IteratorRepSharedPtr;
            using IRep                 = typename Iterator<T>::IRep;
            Iterator<T> fDelegateTo;
            EXTRA_DATA  fExtraData;
            Rep (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData = EXTRA_DATA ());
#if qCompilerAndStdLib_TemplateIteratorOutOfLineTemplate_Buggy
            virtual IteratorRepSharedPtr Clone () const override
            {
                return IteratorRepSharedPtr (Iterator<T>::template MakeSharedPtr<Rep> (*this));
            }
#else
            virtual IteratorRepSharedPtr Clone () const override;
#endif
            virtual IteratorOwnerID GetOwner () const override;
            virtual void            More (optional<T>* result, bool advance) override;
            virtual bool            Equals (const IRep* rhs) const override;
        };
        DelegatedIterator (const Iterator<T>& delegateTo, const EXTRA_DATA& extraData = EXTRA_DATA ());
    };
    template <typename T>
    class DelegatedIterator<T, void> : public Iterator<T> {
    public:
        struct Rep : Iterator<T>::IRep {
            using IteratorRepSharedPtr = typename Iterator<T>::IteratorRepSharedPtr;
            using IRep                 = typename Iterator<T>::IRep;
            Iterator<T> fDelegateTo;
            Rep (const Iterator<T>& delegateTo);
            virtual IteratorRepSharedPtr Clone () const override;
            virtual IteratorOwnerID      GetOwner () const override;
            virtual void                 More (optional<T>* result, bool advance) override;
            virtual bool                 Equals (const IRep* rhs) const override;
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
