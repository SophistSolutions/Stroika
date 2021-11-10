/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
        : fItem{item}
        , fPrev{prev}
        , fNext{next}
    {
    }

    /*
     ********************************************************************************
     ******************************* DoublyLinkedList<T> ****************************
     ********************************************************************************
     */
    template <typename T>
    inline DoublyLinkedList<T>::DoublyLinkedList ()
    {
        Invariant ();
    }
    template <typename T>
    DoublyLinkedList<T>::DoublyLinkedList (const DoublyLinkedList& from)
    {
        /*
         *      @todo - this could be a bit more efficient
         */
        for (const Link* cur = from.fHead_; cur != nullptr; cur = cur->fNext) {
            Append (cur->fItem);
        }
        Invariant ();
    }
    template <typename T>
    inline DoublyLinkedList<T>::~DoublyLinkedList ()
    {
        /*
         * This could be a little cheaper - we could avoid setting fHead_ pointer,
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
        Ensure (fHead_ == nullptr);
        Ensure (fTail_ == nullptr);
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
        shared_lock<const AssertExternallySynchronizedLock> readLock{*this};
        return fHead_ == nullptr;
    }
    template <typename T>
    inline size_t DoublyLinkedList<T>::GetLength () const
    {
        shared_lock<const AssertExternallySynchronizedLock> readLock{*this};
        size_t                                              n = 0;
        for (const Link* i = fHead_; i != nullptr; i = i->fNext) {
            n++;
        }
        return n;
    }
    template <typename T>
    inline T DoublyLinkedList<T>::GetFirst () const
    {
        shared_lock<const AssertExternallySynchronizedLock> readLock{*this};
        RequireNotNull (fHead_);
        return fHead_->fItem;
    }
    template <typename T>
    inline T DoublyLinkedList<T>::GetLast () const
    {
        shared_lock<const AssertExternallySynchronizedLock> readLock{*this};
        RequireNotNull (fTail_); // cannot call Getlast on empty list
        return fTail_->fItem;
    }
    template <typename T>
    inline void DoublyLinkedList<T>::Prepend (ArgByValueType<T> item)
    {
        lock_guard<const AssertExternallySynchronizedLock> writeLock{*this};
        Invariant ();
        fHead_ = new Link{item, nullptr, fHead_};
        if (fHead_->fNext != nullptr) [[LIKELY_ATTR]] {
            // backlink second item to first
            fHead_->fNext->fPrev = fHead_;
        }
        if (fTail_ == nullptr) [[UNLIKELY_ATTR]] {
            // if last is null, list was empty, so first==last now
            fTail_ = fHead_;
        }
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::Append (ArgByValueType<T> item)
    {
        lock_guard<const AssertExternallySynchronizedLock> writeLock{*this};
        Invariant ();
        fTail_ = new Link{item, fTail_, nullptr};
        if (fTail_->fPrev != nullptr) [[LIKELY_ATTR]] {
            // forward link second to last item to its prev
            fTail_->fPrev->fNext = fTail_;
        }
        if (fHead_ == nullptr) [[UNLIKELY_ATTR]] {
            // if head is null, list was empty, so first==last now
            fHead_ = fTail_;
        }
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::RemoveFirst ()
    {
        lock_guard<const AssertExternallySynchronizedLock> writeLock{*this};
        RequireNotNull (fHead_);
        Invariant ();
        Link* victim = fHead_;
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
        fHead_ = victim->fNext; // First points to B
        if (fHead_ == nullptr) {
            Assert (victim == fTail_);
            fTail_ = nullptr;
        }
        else {
            Assert (fHead_->fPrev == victim);
            fHead_->fPrev = nullptr; // B's prev is Nil since it is new first
        }
        delete victim;
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::RemoveLast ()
    {
        lock_guard<const AssertExternallySynchronizedLock> writeLock{*this};
        RequireNotNull (fHead_);
        Invariant ();
        Link* victim = fTail_;
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
        fTail_ = victim->fPrev; // new last item
        if (fTail_ == nullptr) {
            Assert (fHead_ == victim);
            fHead_ = nullptr;
        }
        else {
            Assert (fTail_->fNext == victim);
            fTail_->fNext = nullptr; // B's fNext is Nil since it is new last
        }
        delete victim;
        Invariant ();
    }
    template <typename T>
    auto DoublyLinkedList<T>::operator= (const DoublyLinkedList& rhs) -> DoublyLinkedList&
    {
        lock_guard<const AssertExternallySynchronizedLock> writeLock{*this};
        Invariant ();
        RemoveAll ();
        /*
         *      Copy the linked list by keeping a point to the new current and new
         *  previous, and sliding them along in parallel as we construct the
         *  new list. Only do this if we have at least one element - then we
         *  don't have to worry about the head of the list, or nullptr ptrs, etc - that
         *  case is handled outside, before the loop.
         */
        if (rhs.fHead_ != nullptr) {
            fHead_       = new Link{rhs.fHead_->fItem, nullptr};
            Link* newCur = fHead_;
            for (const Link* cur = rhs.fHead_->fNext; cur != nullptr; cur = cur->fNext) {
                Link* newPrev  = newCur;
                newCur         = new Link{cur->fItem, nullptr};
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
        lock_guard<const AssertExternallySynchronizedLock> writeLock{*this};
        Invariant ();
        if (equalsComparer (item, fHead_->fItem)) {
            RemoveFirst ();
        }
        else {
            Link* prev = nullptr;
            for (Link* link = fHead_; link != nullptr; prev = link, link = link->fNext) {
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
        shared_lock<const AssertExternallySynchronizedLock> readLock{*this};
        for (const Link* current = fHead_; current != nullptr; current = current->fNext) {
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
        shared_lock<const AssertExternallySynchronizedLock> readLock{*this};
        for (const Link* i = fHead_; i != nullptr; i = i->fNext) {
            doToElement (i->fItem);
        }
    }
    template <typename T>
    template <typename FUNCTION>
    inline typename DoublyLinkedList<T>::Link* DoublyLinkedList<T>::FindFirstThat (FUNCTION doToElement) const
    {
        shared_lock<const AssertExternallySynchronizedLock> readLock{*this};
        for (Link* i = fHead_; i != nullptr; i = i->fNext) {
            if ((doToElement) (i->fItem)) {
                return i;
            }
        }
        return nullptr;
    }
    template <typename T>
    void DoublyLinkedList<T>::RemoveAll ()
    {
        lock_guard<const AssertExternallySynchronizedLock> writeLock{*this};
        for (Link* i = fHead_; i != nullptr;) {
            Link* deleteMe = i;
            i              = i->fNext;
            delete deleteMe;
        }
        fHead_ = nullptr;
        fTail_ = nullptr;
    }
    template <typename T>
    T DoublyLinkedList<T>::GetAt (size_t i) const
    {
        shared_lock<const AssertExternallySynchronizedLock> readLock{*this};
        Require (i >= 0);
        Require (i < GetLength ());
        const Link* cur = fHead_;
        for (; i != 0; cur = cur->fNext, --i) {
            AssertNotNull (cur); // cuz i <= fLength
        }
        AssertNotNull (cur); // cuz i <= fLength
        return (cur->fItem);
    }
    template <typename T>
    void DoublyLinkedList<T>::SetAt (size_t i, ArgByValueType<T> item)
    {
        lock_guard<const AssertExternallySynchronizedLock> writeLock{*this};
        Require (i >= 0);
        Require (i < GetLength ());
        Link* cur = fHead_;
        for (; i != 0; cur = cur->fNext, --i) {
            AssertNotNull (cur); // cuz i <= fLength
        }
        AssertNotNull (cur); // cuz i <= fLength
        cur->fItem = item;
    }
    template <typename T>
    inline void DoublyLinkedList<T>::MoveIteratorHereAfterClone (ForwardIterator* pi, const DoublyLinkedList<T>* movedFrom) const
    {
        shared_lock<const AssertExternallySynchronizedLock> readLock{*this};
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
    auto DoublyLinkedList<T>::RemoveAt (const ForwardIterator& i) -> ForwardIterator
    {
        lock_guard<const AssertExternallySynchronizedLock> writeLock{*this};
        Require (not i.Done ());
        this->Invariant ();
        ForwardIterator next = i;
        next.More (nullptr, true);

        Link* victim = const_cast<Link*> (i.fCurrent_);
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
            Assert (fHead_ == victim);
            fHead_ = victim->fNext; // 'C' is now first
            if (fHead_ == nullptr) {
                fTail_ = nullptr;
            }
            else {
                Assert (fHead_->fPrev == victim); // Victim used to be 'C's prev
                fHead_->fPrev = nullptr;          // Now Nil!
            }
        }
        else {
            Assert (victim->fPrev->fNext == victim); // In this case 'A' DOES exist
            victim->fPrev->fNext = victim->fNext;    // Make A point to C
                                                     // Now make 'C' point back to A (careful if 'C' is Nil)
            if (victim->fNext == nullptr) {
                // In this case 'C' does not exist - it is Nil...
                Assert (victim == fTail_);
                fTail_ = victim->fPrev; //  'A' is now last
            }
            else {
                Assert (victim->fNext->fPrev == victim); // Victim used to be 'C's prev
                victim->fNext->fPrev = victim->fPrev;    // Now 'A' is
            }
        }
        delete victim;
        this->Invariant ();
        return next;
    }
    template <typename T>
    inline void DoublyLinkedList<T>::SetAt (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> writeLock{*this};
        Require (not i.Done ());
        this->Invariant ();
        const_cast<Link*> (i.fCurrent_)->fItem = newValue;
        this->Invariant ();
    }
    template <typename T>
    void DoublyLinkedList<T>::AddBefore (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> writeLock{*this};
        /*
         * NB: This code works fine, even if we are done!!!
         */
        this->Invariant ();
        if (i.fCurrent_ == nullptr) {
            /*
             *      NB: If I am past the last item on the list, AddBefore() is equivilent
             *  to Appending to the list.
             */
            Assert (i.Done ());
            Append (newValue);
            Assert (i.Done ()); // what is done, cannot be undone!!!
        }
        else {
            Link* prev = i.fCurrent_->fPrev;
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
                Assert (prev->fNext == i.fCurrent_);
                Link* iteratorCurLink = const_cast<Link*> (i.fCurrent_);
                prev->fNext           = new Link (newValue, prev, iteratorCurLink);
                // Since fCurrent != nullptr from above, we update its prev, and don't have
                // to worry about fTail_.
                iteratorCurLink->fPrev = prev->fNext;
                Assert (i.fCurrent_->fPrev->fPrev == prev); // old prev is two back now...
            }
        }
        this->Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::AddAfter (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> writeLock{*this};
        this->Invariant ();
        Require (not i.Done ());
        AssertNotNull (i.fCurrent_); // since not done...
        Assert (fHead_ != nullptr);
        /*
         *      |        |      |        |
         *      |  CUR   |      |   NEW  |
         *  ... | <-prev |      | <-prev |  ...
         *      | next-> |      | next-> |
         *      |        |      |        |
         */
        Link* iteratorCurLink  = const_cast<Link*> (i.fCurrent_);
        Link* newLink          = new Link (newValue, iteratorCurLink, iteratorCurLink->fNext);
        iteratorCurLink->fNext = newLink;
        if (newLink->fNext != nullptr) {
            newLink->fNext->fPrev = newLink;
        }
        if (newLink->fNext == nullptr) {
            fTail_ = newLink;
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
        shared_lock<const AssertExternallySynchronizedLock> readLock{*this};
#endif
        if (fHead_ != nullptr) {
            Assert (fHead_->fPrev == nullptr);
            if (fHead_->fNext == nullptr) {
                Assert (fHead_ == fTail_);
            }
            else {
                Assert (fHead_->fNext->fPrev == fHead_);
            }
        }
        if (fTail_ != nullptr) {
            Assert (fTail_->fNext == nullptr);
            if (fTail_->fPrev == nullptr) {
                Assert (fHead_ == fTail_);
            }
            else {
                Assert (fTail_->fPrev->fNext == fTail_);
            }
        }
        Assert (fHead_ == nullptr or fHead_->fPrev == nullptr);
        Assert (fTail_ == nullptr or fTail_->fNext == nullptr);

        /*
         * Check we are properly linked together.
         */
        size_t forwardCounter = 0;
        for (Link* i = fHead_; i != nullptr; i = i->fNext) {
            Assert (i->fNext == nullptr or i->fNext->fPrev == i); //  adjacent nodes point at each other
            forwardCounter++;
        }
        size_t backwardCounter{};
        for (Link* i = fTail_; i != nullptr; i = i->fPrev) {
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
    inline DoublyLinkedList<T>::ForwardIterator::ForwardIterator (const DoublyLinkedList* data)
        : fData_{data}
        , fCurrent_{data->fHead_}
    {
    }
    template <typename T>
    inline typename DoublyLinkedList<T>::ForwardIterator& DoublyLinkedList<T>::ForwardIterator::operator= (const ForwardIterator& rhs)
    {
        shared_lock<const AssertExternallySynchronizedLock> readLock{*rhs.fData_};
        Invariant ();
        fData_    = rhs.fData_;
        fCurrent_ = rhs.fCurrent_;
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
        shared_lock<const AssertExternallySynchronizedLock> readLock{*fData_};
        Invariant ();
        return bool (fCurrent_ == nullptr);
    }
    template <typename T>
    inline bool DoublyLinkedList<T>::ForwardIterator::More (T* current, bool advance)
    {
        shared_lock<const AssertExternallySynchronizedLock> readLock{*fData_};
        Invariant ();

        if (advance) {
            /*
             * We could already be done since after the last Done() call, we could
             * have done a removeall.
             */
            if (fCurrent_ != nullptr) {
                fCurrent_ = fCurrent_->fNext;
            }
        }
        Invariant ();
        if (current != nullptr and not Done ()) {
            AssertNotNull (fCurrent_); // because Done() test
            *current = fCurrent_->fItem;
        }
        return not Done ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::ForwardIterator::More (optional<T>* result, bool advance)
    {
        shared_lock<const AssertExternallySynchronizedLock> readLock{*fData_};
        RequireNotNull (result);
        Invariant ();
        if (advance) {
            /*
             * We could already be done since after the last Done() call, we could
             * have done a removeall.
             */
            if (fCurrent_ != nullptr) {
                fCurrent_ = fCurrent_->fNext;
            }
        }
        Invariant ();
        if (this->Done ()) {
            *result = nullopt;
        }
        else {
            AssertNotNull (fCurrent_); // because not done
            *result = fCurrent_->fItem;
        }
    }
    template <typename T>
    inline bool DoublyLinkedList<T>::ForwardIterator::More (nullptr_t, bool advance)
    {
        shared_lock<const AssertExternallySynchronizedLock> readLock{*fData_};
        return More (static_cast<T*> (nullptr), advance);
    }
    template <typename T>
    inline auto DoublyLinkedList<T>::ForwardIterator::operator++ () noexcept -> ForwardIterator&
    {
        Require (not Done ());
        Invariant ();
        Assert (fCurrent_ != nullptr);
        fCurrent_ = fCurrent_->fNext;
        Invariant ();
        return *this;
    }
    template <typename T>
    inline T DoublyLinkedList<T>::ForwardIterator::Current () const
    {
        shared_lock<const AssertExternallySynchronizedLock> readLock{*fData_};
        Require (not Done ());
        Invariant ();
        AssertNotNull (fCurrent_);
        return fCurrent_->fItem;
    }
    template <typename T>
    size_t DoublyLinkedList<T>::ForwardIterator::CurrentIndex () const
    {
        shared_lock<const AssertExternallySynchronizedLock> readLock{*fData_};
        Require (not Done ());
        Invariant ();
        size_t n = 0;
        for (const Link* l = fData_->fHead_; l != this->fCurrent_; l = l->fNext, ++n) {
            AssertNotNull (l);
        }
        return n;
    }
    template <typename T>
    inline auto DoublyLinkedList<T>::ForwardIterator::GetCurrentLink () const -> const Link*
    {
        return fCurrent_;
    }
    template <typename T>
    inline void DoublyLinkedList<T>::ForwardIterator::SetCurrentLink (const Link* l)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*fData_};
        // MUST COME FROM THIS LIST
        // CAN be nullptr
        fCurrent_ = l;
    }
    template <typename T>
    inline bool DoublyLinkedList<T>::ForwardIterator::Equals (const typename DoublyLinkedList<T>::ForwardIterator& rhs) const
    {
        shared_lock<const AssertExternallySynchronizedLock> readLock{*fData_};
        return fCurrent_ == rhs.fCurrent_;
    }
    template <typename T>
    inline void DoublyLinkedList<T>::ForwardIterator::PatchBeforeRemove (const ForwardIterator* adjustmentAt)
    {
        RequireNotNull (adjustmentAt);
        this->Invariant ();
        auto link = adjustmentAt->fCurrent_;
        RequireNotNull (link);

        /*
         *  There are basicly three cases:
         *
         *  (1)     We remove the current. In this case, we just advance current to the next
         *          item (prev is already all set), and set _fSuppressMore since we are advanced
         *          to the next item.
         *  (2)     We remove our previous. Technically this poses no problems, except then
         *          our previos pointer is invalid. We could recompute it, but that would
         *          involve rescanning the list from the beginning - slow. And we probably
         *          will never need the next pointer (unless we get a remove current call).
         *          So just set it to nullptr, which conventionally means no valid value.
         *          It will be recomputed if needed.
         *  (3)     We are deleting some other value. No probs.
         */
        if (this->fCurrent_ == link) {
            this->fCurrent_ = this->fCurrent_->fNext;
            // fPrev remains the same - right now it points to a bad item, since
            // PatchRemove() called before the actual removal, but right afterwards
            // it will point to our new fCurrent_.
            //     this->_fSuppressMore = true; // Since we advanced cursor...
        }
    }
#if qDebug
    template <typename T>
    void DoublyLinkedList<T>::ForwardIterator::Invariant_ () const
    {
    }
#endif

}
#endif /* _Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_inl_ */
