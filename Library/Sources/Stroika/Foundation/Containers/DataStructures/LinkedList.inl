/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_LinkedList_inl_
#define _Stroika_Foundation_Containers_DataStructures_LinkedList_inl_ 1

#include <optional>

#include "../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::DataStructures {

// Would like to leave on by default but we just added and cannot afford to have debug builds get that slow
#ifndef qStroika_Foundation_Containers_DataStructures_LinkedList_IncludeSlowDebugChecks_
#define qStroika_Foundation_Containers_DataStructures_LinkedList_IncludeSlowDebugChecks_ 0
#endif

    /*
     ********************************************************************************
     ********************** LinkedList<T,TRAITS>::Link_ *****************************
     ********************************************************************************
     */
    template <typename T>
    inline LinkedList<T>::Link_::Link_ (ArgByValueType<T> item, Link_* next)
        : fItem{item}
        , fNext{next}
    {
    }

    /*
     ********************************************************************************
     ************************** LinkedList<T,TRAITS> ********************************
     ********************************************************************************
     */
    template <typename T>
    inline LinkedList<T>::LinkedList ()
    {
        Invariant ();
    }
    template <typename T>
    LinkedList<T>::LinkedList (const LinkedList& from)
    {
        /*
         *      Copy the link list by keeping a point to the new current and new
         *  previous, and sliding them along in parallel as we construct the
         *  new list. Only do this if we have at least one element - then we
         *  don't have to worry about the head of the list, or nullptr ptrs, etc - that
         *  case is handled outside, before the loop.
         */
        if (from.fHead_ != nullptr) {
            fHead_        = new Link_{from.fHead_->fItem, nullptr};
            Link_* newCur = fHead_;
            for (const Link_* cur = from.fHead_->fNext; cur != nullptr; cur = cur->fNext) {
                Link_* newPrev = newCur;
                newCur         = new Link_{cur->fItem, nullptr};
                newPrev->fNext = newCur;
            }
        }
        Invariant ();
    }
    template <typename T>
    inline LinkedList<T>::~LinkedList ()
    {
        /*
         * This could be a little cheaper - we could avoid setting fLength field,
         * and fHead_ pointer, but we must worry more about codeSize/re-use.
         * That would involve a new function that COULD NOT BE INLINED.
         *
         * < I guess  I could add a hack method - unadvertised - but has to be
         *   at least protected - and call it here to do what I've mentioned above >
         */
        Invariant ();
        RemoveAll ();
        Invariant ();
        Ensure (fHead_ == nullptr);
    }
    template <typename T>
    auto LinkedList<T>::operator= (const LinkedList& rhs) -> LinkedList&
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Invariant ();
        if (this != &rhs) {
            RemoveAll ();
            /*
             *      Copy the link list by keeping a point to the new current and new
             *  previous, and sliding them along in parallel as we construct the
             *  new list. Only do this if we have at least one element - then we
             *  don't have to worry about the head of the list, or nullptr ptrs, etc - that
             *  case is handled outside, before the loop.
             */
            if (rhs.fHead_ != nullptr) {
                fHead_        = new Link_{rhs.fHead_->fItem, nullptr};
                Link_* newCur = fHead_;
                for (const Link_* cur = rhs.fHead_->fNext; cur != nullptr; cur = cur->fNext) {
                    Link_* newPrev = newCur;
                    newCur         = new Link_{cur->fItem, nullptr};
                    newPrev->fNext = newCur;
                }
            }
        }
        Invariant ();
        return *this;
    }
    template <typename T>
    inline void LinkedList<T>::Invariant () const noexcept
    {
#if qDebug
        Invariant_ ();
#endif
    }
    template <typename T>
    inline void LinkedList<T>::MoveIteratorHereAfterClone (ForwardIterator* pi, const LinkedList* movedFrom) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        // TRICKY TODO - BUT MUST DO - MUST MOVE FROM OLD ITER TO NEW
        // only way
        //
        // For STL containers, not sure how to find an equiv new iterator for an old one, but my best guess is to iterate through
        // old for old, and when I match, stop on new
        Require (pi->fData_ == movedFrom);
        auto                  newI = this->fHead_;
        [[maybe_unused]] auto newE = nullptr;
        auto                  oldI = movedFrom->fHead_;
        [[maybe_unused]] auto oldE = nullptr;
        while (oldI != pi->fCurrent_) {
            Assert (newI != newE);
            Assert (oldI != oldE);
            newI = newI->fNext;
            oldI = oldI->fNext;
            Assert (newI != newE);
            Assert (oldI != oldE);
        }
        Assert (oldI == pi->fCurrent_);
        pi->fCurrent_ = newI;
        pi->fData_    = this;
    }
    template <typename T>
    inline bool LinkedList<T>::empty () const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        return fHead_ == nullptr;
    }
    template <typename T>
    inline size_t LinkedList<T>::size () const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        size_t                                               n = 0;
        for (const Link_* i = fHead_; i != nullptr; i = i->fNext) {
            ++n;
        }
        return n;
    }
    template <typename T>
    inline T LinkedList<T>::GetFirst () const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        Require (not empty ());
        AssertNotNull (fHead_);
        return fHead_->fItem;
    }
    template <typename T>
    inline void LinkedList<T>::Prepend (ArgByValueType<T> item)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Invariant ();
        fHead_ = new Link_{item, fHead_};
        Invariant ();
    }
    template <typename T>
    void LinkedList<T>::Append (ArgByValueType<T> item)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        if (this->fHead_ == nullptr) [[UNLIKELY_ATTR]] {
            Prepend (item);
        }
        else {
            Link_* last = this->fHead_;
            for (; last->fNext != nullptr; last = last->fNext)
                ;
            Assert (last != nullptr);
            Assert (last->fNext == nullptr);
            last->fNext = new Link_{item, nullptr};
        }
    }
    template <typename T>
    inline void LinkedList<T>::RemoveFirst ()
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (not empty ());
        AssertNotNull (fHead_);
        Invariant ();
        Link_* victim = fHead_;
        fHead_        = victim->fNext;
        delete victim;
        Invariant ();
    }
    template <typename T>
    inline T* LinkedList<T>::PeekAt (const ForwardIterator& i)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (not i.Done ());
        Invariant ();
        i.Invariant ();
        return &const_cast<Link_*> (i.fCurrent_)->fItem;
    }
    template <typename T>
    inline void LinkedList<T>::SetAt (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (not i.Done ());
        Invariant ();
        i.Invariant ();
        const_cast<Link_*> (i.fCurrent_)->fItem = newValue;
        Invariant ();
    }
    template <typename T>
    void LinkedList<T>::AddBefore (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        /*
         * NB: This code works fine, even if 'i' is Done ()
         */
        Invariant ();
        i.Invariant ();

        Link_* prev = nullptr;
        if ((this->fHead_ != nullptr) and (this->fHead_ != i.fCurrent_)) {
            for (prev = this->fHead_; prev->fNext != i.fCurrent_; prev = prev->fNext) {
                AssertNotNull (prev); // cuz that would mean fCurrent_ not in LinkedList!!!
            }
        }

        if (prev == nullptr) {
            Assert (this->fHead_ == i.fCurrent_); // could be nullptr, or not...
            this->fHead_ = new Link_{newValue, this->fHead_};
        }
        else {
            Assert (prev->fNext == i.fCurrent_);
            prev->fNext = new Link_ (newValue, prev->fNext);
        }

        Invariant ();
    }
    template <typename T>
    inline void LinkedList<T>::AddAfter (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (not i.Done ());
        AssertNotNull (i.fCurrent_); // since not done...
        i.Invariant ();
        const_cast<Link_*> (i.fCurrent_)->fNext = new Link_{newValue, i.fCurrent_->fNext};
    }
    template <typename T>
    auto LinkedList<T>::RemoveAt (const ForwardIterator& i) -> ForwardIterator
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (not i.Done ());
        Invariant ();
        i.Invariant ();

        ForwardIterator next = i;
        ++next;

        Link_* victim = const_cast<Link_*> (i.fCurrent_);

        /*
         *      At this point we need the fPrev pointer. But it may have been lost
         *  in a patch. If it was, its value will be nullptr (NB: nullptr could also mean
         *  fCurrent_ == fData->fHead_). If it is nullptr, recompute. Be careful if it
         *  is still nullptr, that means update fHead_.
         */
        Link_* prevLink = nullptr;
        if (this->fHead_ != victim) {
            AssertNotNull (this->fHead_); // cuz there must be something to remove current
            for (prevLink = this->fHead_; prevLink->fNext != victim; prevLink = prevLink->fNext) {
                AssertNotNull (prevLink); // cuz that would mean victim not in DoublyLinkedList!!!
            }
        }
        if (prevLink == nullptr) {
            Assert (this->fHead_ == victim);
            this->fHead_ = victim->fNext;
        }
        else {
            Assert (prevLink->fNext == victim);
            prevLink->fNext = victim->fNext;
        }

        delete victim;

        i.Invariant ();
        Invariant ();
        return next;
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    void LinkedList<T>::Remove (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Invariant ();
        /*
         *  Base class impl is fine, but doesn't do patching, and doesn't
         *  provide the hooks so I can do the patching from here.
         *
         *  @todo   We may want to correct that (see STL container impl -
         *  returning ptr to next node would do it).
         */
        for (ForwardIterator it{this}; not it.Done (); ++it) {
            if (equalsComparer (it.Current (), item)) {
                this->RemoveAt (it);
                break;
            }
        }
        Invariant ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    T* LinkedList<T>::Lookup (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this}; // lock not shared cuz return mutable ptr
        for (Link_* i = fHead_; i != nullptr; i = i->fNext) {
            if (equalsComparer (i->fItem, item)) {
                return &i->fItem;
            }
        }
        return nullptr;
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    const T* LinkedList<T>::Lookup (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        for (const Link_* i = fHead_; i != nullptr; i = i->fNext) {
            if (equalsComparer (i->fItem, item)) {
                return &i->fItem;
            }
        }
        return nullptr;
    }
    template <typename T>
    template <typename FUNCTION>
    inline void LinkedList<T>::Apply (FUNCTION doToElement) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        for (const Link_* i = fHead_; i != nullptr; i = i->fNext) {
            doToElement (i->fItem);
        }
    }
    template <typename T>
    template <typename FUNCTION>
    inline auto LinkedList<T>::Find (FUNCTION doToElement) const -> UnderlyingIteratorRep
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        for (Link_* i = fHead_; i != nullptr; i = i->fNext) {
            if (doToElement (i->fItem)) {
                return i;
            }
        }
        return nullptr;
    }
    template <typename T>
    void LinkedList<T>::RemoveAll ()
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Invariant ();
        for (Link_* i = fHead_; i != nullptr;) {
            Link_* deleteMe = i;
            i               = i->fNext;
            delete deleteMe;
        }
        fHead_ = nullptr;
        Invariant ();
        Ensure (empty ());
    }
    template <typename T>
    T LinkedList<T>::GetAt (size_t i) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        Require (i >= 0);
        Require (i < size ());
        const Link_* cur = fHead_;
        for (; i != 0; cur = cur->fNext, --i) {
            AssertNotNull (cur); // cuz i <= fLength
        }
        AssertNotNull (cur); // cuz i <= fLength
        return cur->fItem;
    }
    template <typename T>
    void LinkedList<T>::SetAt (T item, size_t i)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (i >= 0);
        Require (i < size ());
        Link_* cur = fHead_;
        for (; i != 0; cur = cur->fNext, --i) {
            AssertNotNull (cur); // cuz i <= fLength
        }
        AssertNotNull (cur); // cuz i <= fLength
        cur->fItem = item;
    }
#if qDebug
    template <typename T>
    void LinkedList<T>::Invariant_ () const noexcept
    {
#if qStroika_Foundation_Containers_DataStructures_LinkedList_IncludeSlowDebugChecks_
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
#endif
        /*
         * Check we are properly linked together.
         */
        for (Link_* i = fHead_; i != nullptr; i = i->fNext) {
            // at least make sure no currupted links and no infinite loops
        }
    }
#endif

    /*
     ********************************************************************************
     ************************* LinkedList<T>::ForwardIterator ***********************
     ********************************************************************************
     */
    template <typename T>
    inline LinkedList<T>::ForwardIterator::ForwardIterator (const LinkedList* data, UnderlyingIteratorRep startAt)
        : fData_{data}
        , fCurrent_{startAt}
    {
        RequireNotNull (data);
    }
    template <typename T>
    inline LinkedList<T>::ForwardIterator::ForwardIterator (const LinkedList* data)
        : ForwardIterator{data, data->fHead_}
    {
        RequireNotNull (data);
    }
    template <typename T>
    inline typename LinkedList<T>::ForwardIterator& LinkedList<T>::ForwardIterator::operator= (const ForwardIterator& rhs)
    {
        Invariant ();
        fData_    = rhs.fData_;
        fCurrent_ = rhs.fCurrent_;
        Invariant ();
        return *this;
    }
    template <typename T>
    inline void LinkedList<T>::ForwardIterator::Invariant () const noexcept
    {
#if qDebug
        Invariant_ ();
#endif
    }
    template <typename T>
    inline bool LinkedList<T>::ForwardIterator::Done () const noexcept
    {
        Invariant ();
        return fCurrent_ == nullptr;
    }
    template <typename T>
    inline auto LinkedList<T>::ForwardIterator::operator++ () noexcept -> ForwardIterator&
    {
        Require (not Done ());
        Invariant ();
        Assert (fCurrent_ != nullptr);
        fCurrent_ = fCurrent_->fNext;
        Invariant ();
        return *this;
    }
    template <typename T>
    inline T LinkedList<T>::ForwardIterator::Current () const
    {
        Require (not(Done ()));
        Invariant ();
        AssertNotNull (fCurrent_);
        return fCurrent_->fItem;
    }
    template <typename T>
    size_t LinkedList<T>::ForwardIterator::CurrentIndex () const
    {
        Require (not(Done ()));
        RequireNotNull (fData_);
        RequireNotNull (this->fCurrent_);
        size_t i = 0;
        for (const Link_* l = fData_->fHead_; l != this->fCurrent_; l = l->fNext, ++i) {
            AssertNotNull (l);
        }
        return i;
    }
    template <typename T>
    inline auto LinkedList<T>::ForwardIterator::GetUnderlyingIteratorRep () const -> UnderlyingIteratorRep
    {
        return fCurrent_;
    }
    template <typename T>
    inline void LinkedList<T>::ForwardIterator::SetUnderlyingIteratorRep (const UnderlyingIteratorRep l)
    {
        // MUUST COME FROM THIS LIST
        // CAN be nullptr
        fCurrent_ = l;
    }
    template <typename T>
    inline bool LinkedList<T>::ForwardIterator::Equals (const typename LinkedList<T>::ForwardIterator& rhs) const
    {
        return fCurrent_ == rhs.fCurrent_;
    }
#if qDebug
    template <typename T>
    void LinkedList<T>::ForwardIterator::Invariant_ () const noexcept
    {
    }
#endif

}
#endif /* _Stroika_Foundation_Containers_DataStructures_LinkedList_inl_ */
