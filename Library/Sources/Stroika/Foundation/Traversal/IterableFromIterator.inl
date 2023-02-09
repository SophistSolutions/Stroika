/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_IterableFromIterator_inl_
#define _Stroika_Foundation_Traversal_IterableFromIterator_inl_

#include "../Debug/Assertions.h"
#include "DelegatedIterator.h"

namespace Stroika::Foundation::Traversal {

#if qDebug
    /*
     ********************************************************************************
     * IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::_IteratorTracker *
     ********************************************************************************
     */
    template <typename T, typename NEW_ITERATOR_REP_TYPE, typename CONTEXT_FOR_EACH_ITERATOR>
    inline IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::_IteratorTracker::~_IteratorTracker ()
    {
        Assert (*fCountRunning == 0);
    }
    template <typename T, typename NEW_ITERATOR_REP_TYPE, typename CONTEXT_FOR_EACH_ITERATOR>
    inline Iterator<T>
    IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::_IteratorTracker::MakeDelegatedIterator (const Iterator<T>& sourceIterator)
    {
        return DelegatedIterator<T, shared_ptr<unsigned int>> (sourceIterator, fCountRunning);
    }
#endif

    /*
     ********************************************************************************
     * IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep *
     ********************************************************************************
     */
    template <typename T, typename NEW_ITERATOR_REP_TYPE, typename CONTEXT_FOR_EACH_ITERATOR>
    template <typename K1, enable_if_t<not is_same_v<K1, void>>*>
    inline IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::_Rep (const _ContextObjectType& contextForEachIterator)
        : _fContextForEachIterator{contextForEachIterator}
    {
    }
    template <typename T, typename NEW_ITERATOR_REP_TYPE, typename CONTEXT_FOR_EACH_ITERATOR>
    template <typename K1, enable_if_t<is_same_v<K1, void>>*>
    inline IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::_Rep ()
    {
    }
    template <typename T, typename NEW_ITERATOR_REP_TYPE, typename CONTEXT_FOR_EACH_ITERATOR>
    Iterator<T> IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::MakeIterator (
        [[maybe_unused]] const shared_ptr<typename Iterable<T>::_IRep>& thisSharedPtr) const
    {
        if constexpr (is_same_v<NEW_ITERATOR_REP_TYPE, void>) {
            return nullptr;
        }
        else {
#if qDebug
            return fIteratorTracker_.MakeDelegatedIterator (Iterator<T>{make_unique<NEW_ITERATOR_REP_TYPE> (_fContextForEachIterator)});
#else
            return Iterator<T>{make_unique<NEW_ITERATOR_REP_TYPE> (_fContextForEachIterator)};
#endif
        }
    }

    /*
     ********************************************************************************
     **************************** MakeIterableFromIterator **************************
     ********************************************************************************
     */
    template <typename T>
    Iterable<T> MakeIterableFromIterator (const Iterator<T>& iterator)
    {
        struct MyIterable_ : public Iterable<T> {
            struct Rep : public IterableFromIterator<T>::_Rep, public Memory::UseBlockAllocationIfAppropriate<Rep> {
#if qDebug
                using inherited = typename IterableFromIterator<T>::_Rep;
                using _IteratorTracker = typename inherited::_IteratorTracker;
#endif
                Iterator<T> fOriginalIterator;
#if qDebug
                mutable _IteratorTracker fIteratorTracker_{};
#endif
                Rep (const Iterator<T>& originalIterator)
                    : fOriginalIterator{originalIterator}
                {
                }
                virtual Iterator<T> MakeIterator ([[maybe_unused]] const shared_ptr<typename Iterable<T>::_IRep>& thisSharedPtr) const override
                {
#if qDebug
                    return fIteratorTracker_.MakeDelegatedIterator (fOriginalIterator);
#else
                    return fOriginalIterator;
#endif
                }
                virtual shared_ptr<typename Iterable<T>::_IRep> Clone () const override { return Memory::MakeSharedPtr<Rep> (*this); }
            };
            MyIterable_ (const Iterator<T>& originalIterator)
                : Iterable<T>{Memory::MakeSharedPtr<Rep> (originalIterator)}
            {
            }
        };
        return MyIterable_{iterator};
    }

}

#endif /* _Stroika_Foundation_Traversal_IterableFromIterator_inl_ */
