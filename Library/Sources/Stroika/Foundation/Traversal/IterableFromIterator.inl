/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "DelegatedIterator.h"
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Traversal {

#if qStroika_Foundation_Debug_AssertionsChecked
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
    inline IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::_Rep (const _ContextObjectType& contextForEachIterator)
        requires (same_as<void, CONTEXT_FOR_EACH_ITERATOR>)
        : _fContextForEachIterator{contextForEachIterator}
    {
    }
    template <typename T, typename NEW_ITERATOR_REP_TYPE, typename CONTEXT_FOR_EACH_ITERATOR>
    Iterator<T> IterableFromIterator<T, NEW_ITERATOR_REP_TYPE, CONTEXT_FOR_EACH_ITERATOR>::_Rep::MakeIterator () const
    {
        if constexpr (same_as<NEW_ITERATOR_REP_TYPE, void>) {
            AssertNotReached ();
            return nullptr;
        }
        else {
#if qStroika_Foundation_Debug_AssertionsChecked
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
#if qStroika_Foundation_Debug_AssertionsChecked
                using inherited        = typename IterableFromIterator<T>::_Rep;
                using _IteratorTracker = typename inherited::_IteratorTracker;
#endif
                Iterator<T> fOriginalIterator;
#if qStroika_Foundation_Debug_AssertionsChecked
                mutable _IteratorTracker fIteratorTracker_{};
#endif
                Rep (const Iterator<T>& originalIterator)
                    : fOriginalIterator{originalIterator}
                {
                }
                virtual Iterator<T> MakeIterator () const override
                {
#if qStroika_Foundation_Debug_AssertionsChecked
                    return fIteratorTracker_.MakeDelegatedIterator (fOriginalIterator);
#else
                    return fOriginalIterator;
#endif
                }
                virtual shared_ptr<typename Iterable<T>::_IRep> Clone () const override
                {
                    return Memory::MakeSharedPtr<Rep> (*this);
                }
            };
            MyIterable_ (const Iterator<T>& originalIterator)
                : Iterable<T>{Memory::MakeSharedPtr<Rep> (originalIterator)}
            {
            }
        };
        return MyIterable_{iterator};
    }

}
