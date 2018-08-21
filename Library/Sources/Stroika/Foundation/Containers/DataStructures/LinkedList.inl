/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
     ********************** LinkedList<T,TRAITS>::Link ******************************
     ********************************************************************************
     */
    template <typename T>
    inline LinkedList<T>::Link::Link (T item, Link* next)
        : fItem (item)
        , fNext (next)
    {
    }

    /*
     ********************************************************************************
     ************************** LinkedList<T,TRAITS> ********************************
     ********************************************************************************
     */
    template <typename T>
    inline LinkedList<T>::LinkedList ()
        : _fHead (nullptr)
    {
        Invariant ();
    }
    template <typename T>
    LinkedList<T>::LinkedList (const LinkedList<T>& from)
        : _fHead (nullptr)
    {
        /*
         *      Copy the link list by keeping a point to the new current and new
         *  previous, and sliding them along in parallel as we construct the
         *  new list. Only do this if we have at least one element - then we
         *  don't have to worry about the head of the list, or nullptr ptrs, etc - that
         *  case is handled outside, before the loop.
         */
        if (from._fHead != nullptr) {
            _fHead       = new Link (from._fHead->fItem, nullptr);
            Link* newCur = _fHead;
            for (const Link* cur = from._fHead->fNext; cur != nullptr; cur = cur->fNext) {
                Link* newPrev  = newCur;
                newCur         = new Link (cur->fItem, nullptr);
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
         * and _fHead pointer, but we must worry more about codeSize/re-use.
         * That would involve a new function that COULD NOT BE INLINED.
         *
         * < I guess  I could add a hack method - unadvertised - but has to be
         *   at least protected - and call it here to do what I've mentioned above >
         */
        Invariant ();
        RemoveAll ();
        Invariant ();
        Ensure (_fHead == nullptr);
    }
    template <typename T>
    LinkedList<T>& LinkedList<T>::operator= (const LinkedList<T>& list)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Invariant ();
        if (this != &list) {
            RemoveAll ();

            /*
             *      Copy the link list by keeping a point to the new current and new
             *  previous, and sliding them along in parallel as we construct the
             *  new list. Only do this if we have at least one element - then we
             *  don't have to worry about the head of the list, or nullptr ptrs, etc - that
             *  case is handled outside, before the loop.
             */
            if (list._fHead != nullptr) {
                _fHead       = new Link (list._fHead->fItem, nullptr);
                Link* newCur = _fHead;
                for (const Link* cur = list._fHead->fNext; cur != nullptr; cur = cur->fNext) {
                    Link* newPrev  = newCur;
                    newCur         = new Link (cur->fItem, nullptr);
                    newPrev->fNext = newCur;
                }
            }
        }
        Invariant ();
        return *this;
    }
    template <typename T>
    inline void LinkedList<T>::Invariant () const
    {
#if qDebug
        Invariant_ ();
#endif
    }
    template <typename T>
    inline void LinkedList<T>::MoveIteratorHereAfterClone (ForwardIterator* pi, const LinkedList<T>* movedFrom)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        // TRICKY TODO - BUT MUST DO - MUST MOVE FROM OLD ITER TO NEW
        // only way
        //
        // For STL containers, not sure how to find an equiv new iterator for an old one, but my best guess is to iterate through
        // old for old, and when I match, stop on new
        Require (pi->_fData == movedFrom);
        auto                  newI = this->_fHead;
        [[maybe_unused]] auto newE = nullptr;
        auto                  oldI = movedFrom->_fHead;
        [[maybe_unused]] auto oldE = nullptr;
        while (oldI != pi->_fCurrent) {
            Assert (newI != newE);
            Assert (oldI != oldE);
            newI = newI->fNext;
            oldI = oldI->fNext;
            Assert (newI != newE);
            Assert (oldI != oldE);
        }
        Assert (oldI == pi->_fCurrent);
        pi->_fCurrent = newI;
        pi->_fData    = this;
    }
    template <typename T>
    inline bool LinkedList<T>::IsEmpty () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _fHead == nullptr;
    }
    template <typename T>
    inline size_t LinkedList<T>::GetLength () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        size_t                                              n = 0;
        for (const Link* i = _fHead; i != nullptr; i = i->fNext) {
            n++;
        }
        return n;
    }
    template <typename T>
    inline T LinkedList<T>::GetFirst () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (not IsEmpty ());
        AssertNotNull (_fHead);
        return _fHead->fItem;
    }
    template <typename T>
    inline void LinkedList<T>::Prepend (T item)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Invariant ();

        _fHead = new Link (item, _fHead);

        Invariant ();
    }
    template <typename T>
    void LinkedList<T>::Append (ArgByValueType<T> item)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        if (this->_fHead == nullptr)
            [[UNLIKELY_ATTR]]
            {
                Prepend (item);
            }
        else {
            Link* last = this->_fHead;
            for (; last->fNext != nullptr; last = last->fNext)
                ;
            Assert (last != nullptr);
            Assert (last->fNext == nullptr);
            last->fNext = new Link (item, nullptr);
        }
    }
    template <typename T>
    inline void LinkedList<T>::RemoveFirst ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (not IsEmpty ());
        AssertNotNull (_fHead);

        Invariant ();

        Link* victim = _fHead;
        _fHead       = victim->fNext;
        delete (victim);

        Invariant ();
    }
    template <typename T>
    inline void LinkedList<T>::SetAt (const ForwardIterator& i, T newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (not i.Done ());
        Invariant ();
        i.Invariant ();

        const_cast<Link*> (i._fCurrent)->fItem = newValue;

        Invariant ();
    }
    template <typename T>
    void LinkedList<T>::AddBefore (const ForwardIterator& i, T newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        /*
         * NB: This code works fine, even if we are done!!!
         */
        Invariant ();
        i.Invariant ();

        Link* prev = nullptr;
        if ((this->_fHead != nullptr) and (this->_fHead != i._fCurrent)) {
            for (prev = this->_fHead; prev->fNext != i._fCurrent; prev = prev->fNext) {
                AssertNotNull (prev); // cuz that would mean _fCurrent not in DoublyLinkedList!!!
            }
        }

        if (prev == nullptr) {
            Assert (this->_fHead == i._fCurrent); // could be nullptr, or not...
            this->_fHead = new Link (newValue, const_cast<Link*> (this->_fHead));
        }
        else {
            Assert (prev->fNext == i._fCurrent);
            prev->fNext = new Link (newValue, prev->fNext);
        }

        Invariant ();
    }
    template <typename T>
    inline void LinkedList<T>::AddAfter (const ForwardIterator& i, T newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (not i.Done ());
        AssertNotNull (i._fCurrent); // since not done...
        i.Invariant ();
        const_cast<Link*> (i._fCurrent)->fNext = new Link (newValue, i._fCurrent->fNext);
    }
    template <typename T>
    void LinkedList<T>::RemoveAt (const ForwardIterator& i)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (not i.Done ());
        Invariant ();
        i.Invariant ();

        Link* victim = const_cast<Link*> (i._fCurrent);

        /*
         *      At this point we need the fPrev pointer. But it may have been lost
         *  in a patch. If it was, its value will be nullptr (NB: nullptr could also mean
         *  _fCurrent == fData->_fHead). If it is nullptr, recompute. Be careful if it
         *  is still nullptr, that means update _fHead.
         */
        Link* prevLink = nullptr;
        if (this->_fHead != victim) {
            AssertNotNull (this->_fHead); // cuz there must be something to remove current
            for (prevLink = this->_fHead; prevLink->fNext != victim; prevLink = prevLink->fNext) {
                AssertNotNull (prevLink); // cuz that would mean victim not in DoublyLinkedList!!!
            }
        }
        if (prevLink == nullptr) {
            Assert (this->_fHead == victim);
            this->_fHead = victim->fNext;
        }
        else {
            Assert (prevLink->fNext == victim);
            prevLink->fNext = victim->fNext;
        }

        delete (victim);

        i.Invariant ();
        Invariant ();
    }
#if 0
    template      <typename  T>
    void    LinkedList<T>::Remove (ArgByValueType<T> item)
    {
        Invariant ();

        if (item == _fHead->fItem) {
            RemoveFirst ();
        }
        else {
            Link*    prev    =   nullptr;
            for (Link* link = _fHead; link != nullptr; prev = link, link = link->fNext) {
                if (link->fItem == item) {
                    AssertNotNull (prev);       // cuz otherwise we would have hit it in first case!
                    prev->fNext = link->fNext;
                    delete (link);
                    break;
                }
            }
        }

        Invariant ();
    }
#endif
    template <typename T>
    template <typename EQUALS_COMPARER>
    T* LinkedList<T>::Lookup (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this}; // lock not shared cuz return mutable ptr
        for (Link* i = _fHead; i != nullptr; i = i->fNext) {
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
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        for (const Link* i = _fHead; i != nullptr; i = i->fNext) {
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
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        for (const Link* i = _fHead; i != nullptr; i = i->fNext) {
            (doToElement) (i->fItem);
        }
    }
    template <typename T>
    template <typename FUNCTION>
    inline typename LinkedList<T>::Link* LinkedList<T>::FindFirstThat (FUNCTION doToElement) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        for (Link* i = _fHead; i != nullptr; i = i->fNext) {
            if ((doToElement) (i->fItem)) {
                return i;
            }
        }
        return nullptr;
    }
    template <typename T>
    void LinkedList<T>::RemoveAll ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Invariant ();

        for (Link* i = _fHead; i != nullptr;) {
            Link* deleteMe = i;
            i              = i->fNext;
            delete (deleteMe);
        }
        _fHead = nullptr;

        Invariant ();
        Ensure (IsEmpty ());
    }
    template <typename T>
    T LinkedList<T>::GetAt (size_t i) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (i >= 0);
        Require (i < GetLength ());
        const Link* cur = _fHead;
        for (; i != 0; cur = cur->fNext, --i) {
            AssertNotNull (cur); // cuz i <= fLength
        }
        AssertNotNull (cur); // cuz i <= fLength
        return cur->fItem;
    }
    template <typename T>
    void LinkedList<T>::SetAt (T item, size_t i)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (i >= 0);
        Require (i < GetLength ());
        Link* cur = _fHead;
        for (; i != 0; cur = cur->fNext, --i) {
            AssertNotNull (cur); // cuz i <= fLength
        }
        AssertNotNull (cur); // cuz i <= fLength
        cur->fItem = item;
    }
#if qDebug
    template <typename T>
    void LinkedList<T>::Invariant_ () const
    {
#if qStroika_Foundation_Containers_DataStructures_LinkedList_IncludeSlowDebugChecks_
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
#endif
        /*
         * Check we are properly linked together.
         */
        for (Link* i = _fHead; i != nullptr; i = i->fNext) {
            // at least make sure no currupted links and no infinite loops
        }
    }
#endif

    /*
     ********************************************************************************
     ********************* LinkedList<T,TRAITS>::ForwardIterator ********************
     ********************************************************************************
     */
    template <typename T>
    inline LinkedList<T>::ForwardIterator::ForwardIterator (const LinkedList<T>* data)
        //: fCachedPrev (nullptr)
        : _fData (data)
        , _fCurrent (data->_fHead)
        , _fSuppressMore (true)
    {
        RequireNotNull (data);
    }
    template <typename T>
    inline LinkedList<T>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
        //: fCachedPrev (nullptr)
        : _fData (from._fData)
        , _fCurrent (from._fCurrent)
        , _fSuppressMore (from._fSuppressMore)
    {
    }
    template <typename T>
    inline typename LinkedList<T>::ForwardIterator& LinkedList<T>::ForwardIterator::operator= (const ForwardIterator& rhs)
    {
        Invariant ();
        _fCurrent      = rhs._fCurrent;
        _fSuppressMore = rhs._fSuppressMore;
        Invariant ();
        return (*this);
    }
    template <typename T>
    inline void LinkedList<T>::ForwardIterator::Invariant () const
    {
#if qDebug
        Invariant_ ();
#endif
    }
    template <typename T>
    inline bool LinkedList<T>::ForwardIterator::Done () const
    {
        Invariant ();
        return _fCurrent == nullptr;
    }
    template <typename T>
    inline bool LinkedList<T>::ForwardIterator::More (T* current, bool advance)
    {
        Invariant ();

        if (advance) {
            /*
             * We could already be done since after the last Done() call, we could
             * have done a removeall.
             */
            if (not _fSuppressMore and _fCurrent != nullptr) {
                _fCurrent = _fCurrent->fNext;
            }
            _fSuppressMore = false;
        }
        Invariant ();
        if (current != nullptr and not Done ()) {
            AssertNotNull (_fCurrent); // because not done
            *current = _fCurrent->fItem;
        }
        return not Done ();
    }
    template <typename T>
    inline void LinkedList<T>::ForwardIterator::More (optional<T>* result, bool advance)
    {
        RequireNotNull (result);
        Invariant ();
        if (advance) {
            if (_fSuppressMore) {
                _fSuppressMore = false;
            }
            else {
                /*
                 * We could already be done since after the last Done() call, we could
                 * have done a removeall.
                 */
                if (_fCurrent != nullptr) {
                    _fCurrent = _fCurrent->fNext;
                }
            }
        }
        Assert (not _fSuppressMore);
        Invariant ();
        if (this->Done ()) {
            *result = nullopt;
        }
        else {
            AssertNotNull (_fCurrent);
            *result = _fCurrent->fItem;
        }
    }
    template <typename T>
    inline bool LinkedList<T>::ForwardIterator::More (nullptr_t, bool advance)
    {
        return More (static_cast<T*> (nullptr), advance);
    }
    template <typename T>
    inline T LinkedList<T>::ForwardIterator::Current () const
    {
        Require (not(Done ()));
        Invariant ();
        AssertNotNull (_fCurrent);
        return _fCurrent->fItem;
    }
    template <typename T>
    size_t LinkedList<T>::ForwardIterator::CurrentIndex () const
    {
        Require (not(Done ()));
        RequireNotNull (_fData);
        RequireNotNull (this->_fCurrent);
        size_t i = 0;
        for (const Link* l = _fData->_fHead; l != this->_fCurrent; l = l->fNext, ++i) {
            AssertNotNull (l);
        }
        return i;
    }
    template <typename T>
    inline void LinkedList<T>::ForwardIterator::SetCurrentLink (Link* l)
    {
        // MUUST COME FROM THIS LIST
        // CAN be nullptr
        _fCurrent      = l;
        _fSuppressMore = false;
    }
    template <typename T>
    inline bool LinkedList<T>::ForwardIterator::Equals (const typename LinkedList<T>::ForwardIterator& rhs) const
    {
        return _fCurrent == rhs._fCurrent and _fSuppressMore == rhs._fSuppressMore;
    }
#if qDebug
    template <typename T>
    void LinkedList<T>::ForwardIterator::Invariant_ () const
    {
    }
#endif
}
#endif /* _Stroika_Foundation_Containers_DataStructures_LinkedList_inl_ */
