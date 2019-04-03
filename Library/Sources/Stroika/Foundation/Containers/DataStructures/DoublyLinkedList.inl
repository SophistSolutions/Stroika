/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_inl_
#define _Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_inl_ 1

#include "../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::DataStructures {

// Would like to leave on by default but we just added and cannot afford to have debug builds get that slow
#ifndef qStroika_Foundation_Containers_DataStructures_DoublyLinkedList_IncludeSlowDebugChecks_
#define qStroika_Foundation_Containers_DataStructures_DoublyLinkedList_IncludeSlowDebugChecks_ 0
#endif

    /*
     ********************************************************************************
     ***************************** DoublyLinkedList<T>::Link ************************
     ********************************************************************************
     */
    template <typename T>
    inline DoublyLinkedList<T>::Link::Link (ArgByValueType<T> item, Link* prev, Link* next)
        : fItem (item)
        , fPrev (prev)
        , fNext (next)
    {
    }

    /*
     ********************************************************************************
     ********************************* DoublyLinkedList<T> **************************
     ********************************************************************************
     */
    template <typename T>
    inline DoublyLinkedList<T>::DoublyLinkedList ()
    {
        Invariant ();
    }
    template <typename T>
    DoublyLinkedList<T>::DoublyLinkedList (const DoublyLinkedList<T>& from)
    {
        /*
         *      @todo - this could be a bit more efficient
         */
        for (const Link* cur = from._fHead; cur != nullptr; cur = cur->fNext) {
            Append (cur->fItem);
        }
        Invariant ();
    }
    template <typename T>
    inline DoublyLinkedList<T>::~DoublyLinkedList ()
    {
        /*
         * This could be a little cheaper - we could avoid setting _fHead pointer,
         *  but we must worry more about codeSize/re-use.
         *  That would involve a new function that COULD NOT BE INLINED.
         *
         * < I guess  I could add a hack method - unadvertised - but has to be
         *   at least protected - and call it here to do what I've mentioned above >
         */
        Invariant ();
        RemoveAll ();
        Invariant ();
        Ensure (GetLength () == 0);
        Ensure (_fHead == nullptr);
        Ensure (_fTail == nullptr);
    }
    template <typename T>
    inline void DoublyLinkedList<T>::Invariant () const
    {
#if qDebug
        Invariant_ ();
#endif
    }
    template <typename T>
    inline bool DoublyLinkedList<T>::IsEmpty () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _fHead == nullptr;
    }
    template <typename T>
    inline size_t DoublyLinkedList<T>::GetLength () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        size_t                                              n = 0;
        for (const Link* i = _fHead; i != nullptr; i = i->fNext) {
            n++;
        }
        return n;
    }
    template <typename T>
    inline T DoublyLinkedList<T>::GetFirst () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        RequireNotNull (_fHead);
        return _fHead->fItem;
    }
    template <typename T>
    inline T DoublyLinkedList<T>::GetLast () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        RequireNotNull (_fTail); // cannot call Getlast on empty list
        return _fTail->fItem;
    }
    template <typename T>
    inline void DoublyLinkedList<T>::Prepend (ArgByValueType<T> item)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Invariant ();
        _fHead = new Link (item, nullptr, _fHead);
        if (_fHead->fNext != nullptr)
            [[LIKELY_ATTR]]
            {
                // backlink second item to first
                _fHead->fNext->fPrev = _fHead;
            }
        if (_fTail == nullptr)
            [[UNLIKELY_ATTR]]
            {
                // if last is null, list was empty, so first==last now
                _fTail = _fHead;
            }
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::Append (ArgByValueType<T> item)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Invariant ();
        _fTail = new Link (item, _fTail, nullptr);
        if (_fTail->fPrev != nullptr)
            [[LIKELY_ATTR]]
            {
                // forward link second to last item to its prev
                _fTail->fPrev->fNext = _fTail;
            }
        if (_fHead == nullptr)
            [[UNLIKELY_ATTR]]
            {
                // if head is null, list was empty, so first==last now
                _fHead = _fTail;
            }
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::RemoveFirst ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        RequireNotNull (_fHead);
        Invariant ();
        Link* victim = _fHead;
        Assert (victim->fPrev == nullptr); // cuz it was first..
        /*
         * Before:
         *  |        |      |        |      |        |
         *  |    V   |      |    B   |      |   C    |
         *  | <-prev |      | <-prev |      | <-prev | ...
         *  | next-> |      | next-> |      | next-> |
         *  |        |      |        |      |        |
         *
         * After:
         *  |        |      |        |
         *  |   B    |      |    C   |
         *  | <-prev |      | <-prev | ...
         *  | next-> |      | next-> |
         *  |        |      |        |
         */
        _fHead = victim->fNext; // First points to B
        if (_fHead == nullptr) {
            Assert (victim == _fTail);
            _fTail = nullptr;
        }
        else {
            Assert (_fHead->fPrev == victim);
            _fHead->fPrev = nullptr; // B's prev is Nil since it is new first
        }
        delete victim;
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::RemoveLast ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        RequireNotNull (_fHead);
        Invariant ();
        Link* victim = _fTail;
        Assert (victim->fNext == nullptr); // cuz it was last..
        /*
         * Before:
         *      |        |      |        |      |        |
         *      |    A   |      |    B   |      |   V    |
         *  ... | <-prev |      | <-prev |      | <-prev |
         *      | next-> |      | next-> |      | next-> |
         *      |        |      |        |      |        |
         *
         * After:
         *      |        |      |        |
         *      |   A    |      |    B   |
         *  ... | <-prev |      | <-prev |
         *      | next-> |      | next-> |
         *      |        |      |        |
         */
        _fTail = victim->fPrev; // new last item
        if (_fTail == nullptr) {
            Assert (_fHead == victim);
            _fHead = nullptr;
        }
        else {
            Assert (_fTail->fNext == victim);
            _fTail->fNext = nullptr; // B's fNext is Nil since it is new last
        }
        delete victim;
        Invariant ();
    }
    template <typename T>
    DoublyLinkedList<T>& DoublyLinkedList<T>::operator= (const DoublyLinkedList<T>& rhs)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Invariant ();
        RemoveAll ();
        /*
         *      Copy the link list by keeping a point to the new current and new
         *  previous, and sliding them along in parallel as we construct the
         *  new list. Only do this if we have at least one element - then we
         *  don't have to worry about the head of the list, or nullptr ptrs, etc - that
         *  case is handled outside, before the loop.
         */
        if (rhs._fHead != nullptr) {
            _fHead       = new Link (rhs._fHead->fItem, nullptr);
            Link* newCur = _fHead;
            for (const Link* cur = rhs._fHead->fNext; cur != nullptr; cur = cur->fNext) {
                Link* newPrev  = newCur;
                newCur         = new Link (cur->fItem, nullptr);
                newPrev->fNext = newCur;
            }
        }
        Invariant ();
        return *this;
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    void DoublyLinkedList<T>::Remove (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Invariant ();
        if (equalsComparer (item, _fHead->fItem)) {
            RemoveFirst ();
        }
        else {
            Link* prev = nullptr;
            for (Link* link = _fHead; link != nullptr; prev = link, link = link->fNext) {
                if (equalsComparer (link->fItem, item)) {
                    AssertNotNull (prev); // cuz otherwise we would have hit it in first case!
                    prev->fNext = link->fNext;
                    delete (link);
                    break;
                }
            }
        }
        Invariant ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    bool DoublyLinkedList<T>::Contains (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        for (const Link* current = _fHead; current != nullptr; current = current->fNext) {
            if (equalsComparer (current->fItem, item)) {
                return true;
            }
        }
        return false;
    }
    template <typename T>
    template <typename FUNCTION>
    inline void DoublyLinkedList<T>::Apply (FUNCTION doToElement) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        for (const Link* i = _fHead; i != nullptr; i = i->fNext) {
            (doToElement) (i->fItem);
        }
    }
    template <typename T>
    template <typename FUNCTION>
    inline typename DoublyLinkedList<T>::Link* DoublyLinkedList<T>::FindFirstThat (FUNCTION doToElement) const
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
    void DoublyLinkedList<T>::RemoveAll ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        for (Link* i = _fHead; i != nullptr;) {
            Link* deleteMe = i;
            i              = i->fNext;
            delete deleteMe;
        }
        _fHead = nullptr;
        _fTail = nullptr;
    }
    template <typename T>
    T DoublyLinkedList<T>::GetAt (size_t i) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (i >= 0);
        Require (i < GetLength ());
        const Link* cur = _fHead;
        for (; i != 0; cur = cur->fNext, --i) {
            AssertNotNull (cur); // cuz i <= fLength
        }
        AssertNotNull (cur); // cuz i <= fLength
        return (cur->fItem);
    }
    template <typename T>
    void DoublyLinkedList<T>::SetAt (size_t i, ArgByValueType<T> item)
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
    template <typename T>
    inline void DoublyLinkedList<T>::MoveIteratorHereAfterClone (ForwardIterator* pi, const DoublyLinkedList<T>* movedFrom)
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
    void DoublyLinkedList<T>::RemoveAt (const ForwardIterator& i)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (not i.Done ());
        this->Invariant ();
#if 1
        Link* victim = const_cast<Link*> (i._fCurrent);
        AssertNotNull (victim); // cuz not done
        /*
         * Before:
         *      |        |      |        |      |        |
         *      |    A   |      |    V   |      |   C    |
         *  ... | <-prev |      | <-prev |      | <-prev |  ...
         *      | next-> |      | next-> |      | next-> |
         *      |        |      |        |      |        |
         *
         * After:
         *      |        |      |        |
         *      |   A    |      |    C   |
         *  ... | <-prev |      | <-prev | ...
         *      | next-> |      | next-> |
         *      |        |      |        |
         */
        if (victim->fPrev == nullptr) {
            // In this case 'A' does not exist - it is Nil...
            Assert (_fHead == victim);
            _fHead = victim->fNext; // 'C' is now first
            if (_fHead == nullptr) {
                _fTail = nullptr;
            }
            else {
                Assert (_fHead->fPrev == victim); // Victim used to be 'C's prev
                _fHead->fPrev = nullptr;          // Now Nil!
            }
        }
        else {
            Assert (victim->fPrev->fNext == victim); // In this case 'A' DOES exist
            victim->fPrev->fNext = victim->fNext;    // Make A point to C
                                                     // Now make 'C' point back to A (careful if 'C' is Nil)
            if (victim->fNext == nullptr) {
                // In this case 'C' does not exist - it is Nil...
                Assert (victim == _fTail);
                _fTail = victim->fPrev; //  'A' is now last
            }
            else {
                Assert (victim->fNext->fPrev == victim); // Victim used to be 'C's prev
                victim->fNext->fPrev = victim->fPrev;    // Now 'A' is
            }
        }
        delete victim;
#else
        Link* victim = const_cast<Link*> (i._fCurrent);
        /*
         *      At this point we need the fPrev pointer. But it may have been lost
         *  in a patch. If it was, its value will be nullptr (NB: nullptr could also mean
         *  _fCurrent == fData->_fHead). If it is nullptr, recompute. Be careful if it
         *  is still nullptr, that means update _fHead.
         */
        Link* prev = nullptr;
        if (this->_fHead != victim) {
            AssertNotNull (this->_fHead); // cuz there must be something to remove current
            for (prev = this->_fHead; prev->fNext != victim; prev = prev->fNext) {
                AssertNotNull (prev); // cuz that would mean victim not in DoublyLinkedList!!!
            }
        }
        if (prev == nullptr) {
            this->_fHead = victim->fNext;
        }
        else {
            Assert (prev->fNext == victim);
            prev->fNext = victim->fNext;
        }
        delete victim;
#endif
        this->Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::SetAt (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (not i.Done ());
        this->Invariant ();
        const_cast<Link*> (i._fCurrent)->fItem = newValue;
        this->Invariant ();
    }
    template <typename T>
    void DoublyLinkedList<T>::AddBefore (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        /*
         * NB: This code works fine, even if we are done!!!
         */
        this->Invariant ();
        if (i._fCurrent == nullptr) {
            /*
             *      NB: If I am past the last item on the list, AddBefore() is equivilent
             *  to Appending to the list.
             */
            Assert (i.Done ());
            Append (newValue);
            Assert (i.Done ()); // what is done, cannot be undone!!!
        }
        else {
            Link* prev = i._fCurrent->fPrev;
            if (prev == nullptr) {
                Prepend (newValue);
            }
            else {
                /*
                 *      |        |      |        |      |        |
                 *      |   PREV |      |  NEW   |      |   CUR  |
                 *  ... | <-prev |      | <-prev |      | <-prev |  ...
                 *      | next-> |      | next-> |      | next-> |
                 *      |        |      |        |      |        |
                 */
                Assert (prev->fNext == i._fCurrent);
                Link* iteratorCurLink = const_cast<Link*> (i._fCurrent);
                prev->fNext           = new Link (newValue, prev, iteratorCurLink);
                // Since fCurrent != nullptr from above, we update its prev, and don't have
                // to worry about _fTail.
                iteratorCurLink->fPrev = prev->fNext;
                Assert (i._fCurrent->fPrev->fPrev == prev); // old prev is two back now...
            }
        }
        this->Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::AddAfter (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        this->Invariant ();
        Require (not i.Done ());
        AssertNotNull (i._fCurrent); // since not done...
        Assert (_fHead != nullptr);
        /*
         *      |        |      |        |
         *      |  CUR   |      |   NEW  |
         *  ... | <-prev |      | <-prev |  ...
         *      | next-> |      | next-> |
         *      |        |      |        |
         */
        Link* iteratorCurLink  = const_cast<Link*> (i._fCurrent);
        Link* newLink          = new Link (newValue, iteratorCurLink, iteratorCurLink->fNext);
        iteratorCurLink->fNext = newLink;
        if (newLink->fNext != nullptr) {
            newLink->fNext->fPrev = newLink;
        }
        if (newLink->fNext == nullptr) {
            _fTail = newLink;
        }
        else {
            Assert (newLink->fNext->fPrev == newLink); // cuz of params to new Link...
        }
        this->Invariant ();
    }
#if qDebug
    template <typename T>
    void DoublyLinkedList<T>::Invariant_ () const
    {
#if qStroika_Foundation_Containers_DataStructures_DoublyLinkedList_IncludeSlowDebugChecks_
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
#endif
        if (_fHead != nullptr) {
            Assert (_fHead->fPrev == nullptr);
            if (_fHead->fNext == nullptr) {
                Assert (_fHead == _fTail);
            }
            else {
                Assert (_fHead->fNext->fPrev == _fHead);
            }
        }
        if (_fTail != nullptr) {
            Assert (_fTail->fNext == nullptr);
            if (_fTail->fPrev == nullptr) {
                Assert (_fHead == _fTail);
            }
            else {
                Assert (_fTail->fPrev->fNext == _fTail);
            }
        }
        Assert (_fHead == nullptr or _fHead->fPrev == nullptr);
        Assert (_fTail == nullptr or _fTail->fNext == nullptr);

        /*
         * Check we are properly linked together.
         */
        size_t forwardCounter = 0;
        for (Link* i = _fHead; i != nullptr; i = i->fNext) {
            Assert (i->fNext == nullptr or i->fNext->fPrev == i); //  adjacent nodes point at each other
            forwardCounter++;
        }
        size_t backwardCounter{};
        for (Link* i = _fTail; i != nullptr; i = i->fPrev) {
            Assert (i->fPrev == nullptr or i->fPrev->fNext == i); //  adjacent nodes point at each other
            backwardCounter++;
        }
        Assert (forwardCounter == backwardCounter);
    }
#endif

    /*
     ********************************************************************************
     ********************** DoublyLinkedList<T>::ForwardIterator ********************
     ********************************************************************************
     */
    template <typename T>
    inline DoublyLinkedList<T>::ForwardIterator::ForwardIterator (const DoublyLinkedList<T>* data)
        : _fData (data)
        , _fCurrent (data->_fHead)
        , _fSuppressMore (true)
    {
    }
    template <typename T>
    inline DoublyLinkedList<T>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
        : _fData (from._fData)
        , _fCurrent (from._fCurrent)
        , _fSuppressMore (from._fSuppressMore)
    {
    }
    template <typename T>
    inline typename DoublyLinkedList<T>::ForwardIterator& DoublyLinkedList<T>::ForwardIterator::operator= (const ForwardIterator& rhs)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*_fData};
        Invariant ();
        _fData         = rhs._fData;
        _fCurrent      = rhs._fCurrent;
        _fSuppressMore = rhs._fSuppressMore;
        Invariant ();
        return *this;
    }
    template <typename T>
    inline void DoublyLinkedList<T>::ForwardIterator::Invariant () const
    {
#if qDebug
        Invariant_ ();
#endif
    }
    template <typename T>
    inline bool DoublyLinkedList<T>::ForwardIterator::Done () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
        Invariant ();
        return bool (_fCurrent == nullptr);
    }
    template <typename T>
    inline bool DoublyLinkedList<T>::ForwardIterator::More (T* current, bool advance)
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
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
            AssertNotNull (_fCurrent); // because Done() test
            *current = _fCurrent->fItem;
        }
        return (not Done ());
    }
    template <typename T>
    inline void DoublyLinkedList<T>::ForwardIterator::More (optional<T>* result, bool advance)
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
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
            AssertNotNull (_fCurrent); // because not done
            *result = _fCurrent->fItem;
        }
    }
    template <typename T>
    inline bool DoublyLinkedList<T>::ForwardIterator::More (nullptr_t, bool advance)
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
        return More (static_cast<T*> (nullptr), advance);
    }
    template <typename T>
    inline T DoublyLinkedList<T>::ForwardIterator::Current () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
        Require (not(Done ()));
        Invariant ();
        AssertNotNull (_fCurrent);
        return _fCurrent->fItem;
    }
    template <typename T>
    size_t DoublyLinkedList<T>::ForwardIterator::CurrentIndex () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
        Require (not(Done ()));
        Invariant ();
        size_t n = 0;
        for (const Link* l = _fData->_fHead; l != this->_fCurrent; l = l->fNext, ++n) {
            AssertNotNull (l);
        }
        return n;
    }
    template <typename T>
    inline void DoublyLinkedList<T>::ForwardIterator::SetCurrentLink (Link* l)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*_fData};
        // MUUST COME FROM THIS LIST
        // CAN be nullptr
        _fCurrent      = l;
        _fSuppressMore = false;
    }
    template <typename T>
    inline bool DoublyLinkedList<T>::ForwardIterator::Equals (const typename DoublyLinkedList<T>::ForwardIterator& rhs) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
        return _fCurrent == rhs._fCurrent and _fSuppressMore == rhs._fSuppressMore;
    }
#if qDebug
    template <typename T>
    void DoublyLinkedList<T>::ForwardIterator::Invariant_ () const
    {
    }
#endif

}
#endif /* _Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_inl_ */
