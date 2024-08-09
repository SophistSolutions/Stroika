/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Containers::DataStructures {

// Would like to leave on by default but we just added and cannot afford to have debug builds get that slow
#ifndef qStroika_Foundation_Containers_DataStructures_DoublyLinkedList_IncludeSlowDebugChecks_
#define qStroika_Foundation_Containers_DataStructures_DoublyLinkedList_IncludeSlowDebugChecks_ 0
#endif

    /*
     ********************************************************************************
     ***************************** DoublyLinkedList<T>::Link_ ***********************
     ********************************************************************************
     */
    template <typename T>
    constexpr DoublyLinkedList<T>::Link_::Link_ (ArgByValueType<T> item, Link_* prev, Link_* next)
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
        for (const Link_* cur = from.fHead_; cur != nullptr; cur = cur->fNext) {
            push_back (cur->fItem);
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
        Ensure (size () == 0);
        Ensure (fHead_ == nullptr);
        Ensure (fTail_ == nullptr);
    }
    template <typename T>
    inline void DoublyLinkedList<T>::Invariant () const noexcept
    {
#if qDebug
        Invariant_ ();
#endif
    }
    template <typename T>
    inline bool DoublyLinkedList<T>::empty () const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return fHead_ == nullptr;
    }
    template <typename T>
    inline size_t DoublyLinkedList<T>::size () const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        size_t                                         n = 0;
        for (const Link_* i = fHead_; i != nullptr; i = i->fNext) {
            ++n;
        }
        return n;
    }
    template <typename T>
    inline T DoublyLinkedList<T>::GetFirst () const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        RequireNotNull (fHead_);
        return fHead_->fItem;
    }
    template <typename T>
    inline T DoublyLinkedList<T>::GetLast () const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        RequireNotNull (fTail_); // cannot call Getlast on empty list
        return fTail_->fItem;
    }
    template <typename T>
    inline void DoublyLinkedList<T>::push_front (ArgByValueType<T> item)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Invariant ();
        fHead_ = new Link_{item, nullptr, fHead_};
        if (fHead_->fNext != nullptr) [[likely]] {
            // backlink second item to first
            fHead_->fNext->fPrev = fHead_;
        }
        if (fTail_ == nullptr) [[unlikely]] {
            // if last is null, list was empty, so first==last now
            fTail_ = fHead_;
        }
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::push_back (ArgByValueType<T> item)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Invariant ();
        fTail_ = new Link_{item, fTail_, nullptr};
        if (fTail_->fPrev != nullptr) [[likely]] {
            // forward link second to last item to its prev
            fTail_->fPrev->fNext = fTail_;
        }
        if (fHead_ == nullptr) [[unlikely]] {
            // if head is null, list was empty, so first==last now
            fHead_ = fTail_;
        }
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::RemoveFirst ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        RequireNotNull (fHead_);
        Invariant ();
        Link_* victim = fHead_;
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
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        RequireNotNull (fHead_);
        Invariant ();
        Link_* victim = fTail_;
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
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
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
            fHead_        = new Link_{rhs.fHead_->fItem, nullptr};
            Link_* newCur = fHead_;
            for (const Link_* cur = rhs.fHead_->fNext; cur != nullptr; cur = cur->fNext) {
                Link_* newPrev = newCur;
                newCur         = new Link_{cur->fItem, nullptr};
                newPrev->fNext = newCur;
            }
        }
        Invariant ();
        return *this;
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    void DoublyLinkedList<T>::Remove (ArgByValueType<T> item, EQUALS_COMPARER&& equalsComparer)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Invariant ();
        if (equalsComparer (item, fHead_->fItem)) {
            RemoveFirst ();
        }
        else {
            Link_* prev = nullptr;
            for (Link_* link = fHead_; link != nullptr; prev = link, link = link->fNext) {
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
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        for (const Link_* current = fHead_; current != nullptr; current = current->fNext) {
            if (equalsComparer (current->fItem, item)) {
                return true;
            }
        }
        return false;
    }
    template <typename T>
    template <typename FUNCTION>
    inline void DoublyLinkedList<T>::Apply (FUNCTION&& doToElement) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        for (const Link_* i = fHead_; i != nullptr; i = i->fNext) {
            doToElement (i->fItem);
        }
    }
    template <typename T>
    template <typename FUNCTION>
    inline auto DoublyLinkedList<T>::Find (FUNCTION&& firstThat) const -> UnderlyingIteratorRep
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        for (Link_* i = fHead_; i != nullptr; i = i->fNext) {
            if (firstThat (i->fItem)) {
                return i;
            }
        }
        return nullptr;
    }
    template <typename T>
    void DoublyLinkedList<T>::RemoveAll ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        for (Link_* i = fHead_; i != nullptr;) {
            Link_* deleteMe = i;
            i               = i->fNext;
            delete deleteMe;
        }
        fHead_ = nullptr;
        fTail_ = nullptr;
    }
    template <typename T>
    T DoublyLinkedList<T>::GetAt (size_t i) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (i >= 0);
        Require (i < size ());
        const Link_* cur = fHead_;
        for (; i != 0; cur = cur->fNext, --i) {
            AssertNotNull (cur); // cuz i <= fLength
        }
        AssertNotNull (cur); // cuz i <= fLength
        return (cur->fItem);
    }
    template <typename T>
    void DoublyLinkedList<T>::SetAt (size_t i, ArgByValueType<T> item)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (i >= 0);
        Require (i < size ());
        Link_* cur = fHead_;
        for (; i != 0; cur = cur->fNext, --i) {
            AssertNotNull (cur); // cuz i <= fLength
        }
        AssertNotNull (cur); // cuz i <= fLength
        cur->fItem = item;
    }
    template <typename T>
    inline void DoublyLinkedList<T>::MoveIteratorHereAfterClone (ForwardIterator* pi, const DoublyLinkedList<T>* movedFrom) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        // TRICKY TODO - BUT MUST DO - MUST MOVE FROM OLD ITER TO NEW
        // only way
        //
        // For STL containers, not sure how to find an equiv new iterator for an old one, but my best guess is to iterate through
        // old for old, and when I match, stop on new
#if qDebug
        Require (pi->fData_ == movedFrom);
#endif
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
#if qDebug
        pi->fData_ = this;
#endif
    }
    template <typename T>
    inline auto DoublyLinkedList<T>::begin () const -> ForwardIterator
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return ForwardIterator{this};
    }
    template <typename T>
    constexpr auto DoublyLinkedList<T>::end () const noexcept -> ForwardIterator
    {
        return ForwardIterator{};
    }
    template <typename T>
    auto DoublyLinkedList<T>::RemoveAt (const ForwardIterator& i) -> ForwardIterator
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (not i.Done ());
#if qDebug
        Require (i.fData_ == this); // assure iterator not stale
#endif
        this->Invariant ();
        ForwardIterator next = i;
        ++next;

        const Link_* victim = i.fCurrent_;
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
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (not i.Done ());
#if qDebug
        Require (i.fData_ == this); // assure iterator not stale
#endif
        this->Invariant ();
        const_cast<Link_*> (i.fCurrent_)->fItem = newValue;
        this->Invariant ();
    }
    template <typename T>
    void DoublyLinkedList<T>::AddBefore (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
#if qDebug
        Require (i.fData_ == this); // assure iterator not stale
#endif
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
            push_back (newValue);
            Assert (i.Done ()); // what is done, cannot be undone!!!
        }
        else {
            Link_* prev = i.fCurrent_->fPrev;
            if (prev == nullptr) {
                push_front (newValue);
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
                Link_* iteratorCurLink = const_cast<Link_*> (i.fCurrent_);
                prev->fNext            = new Link_{newValue, prev, iteratorCurLink};
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
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
#if qDebug
        Require (i.fData_ == this); // assure iterator not stale
#endif
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
        Link_* iteratorCurLink = const_cast<Link_*> (i.fCurrent_);
        Link_* newLink         = new Link_{newValue, iteratorCurLink, iteratorCurLink->fNext};
        iteratorCurLink->fNext = newLink;
        if (newLink->fNext != nullptr) {
            newLink->fNext->fPrev = newLink;
        }
        if (newLink->fNext == nullptr) {
            fTail_ = newLink;
        }
        else {
            Assert (newLink->fNext->fPrev == newLink); // cuz of params to new Link_...
        }
        this->Invariant ();
    }
#if qDebug
    template <typename T>
    void DoublyLinkedList<T>::Invariant_ () const noexcept
    {
#if qStroika_Foundation_Containers_DataStructures_DoublyLinkedList_IncludeSlowDebugChecks_
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
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
        for (Link_* i = fHead_; i != nullptr; i = i->fNext) {
            Assert (i->fNext == nullptr or i->fNext->fPrev == i); //  adjacent nodes point at each other
            ++forwardCounter;
        }
        size_t backwardCounter{};
        for (Link_* i = fTail_; i != nullptr; i = i->fPrev) {
            Assert (i->fPrev == nullptr or i->fPrev->fNext == i); //  adjacent nodes point at each other
            ++backwardCounter;
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
    constexpr DoublyLinkedList<T>::ForwardIterator::ForwardIterator ([[maybe_unused]] const DoublyLinkedList* data, UnderlyingIteratorRep startAt) noexcept
        : fCurrent_{startAt}
#if qDebug
        , fData_{data}
#endif
    {
    }
    template <typename T>
    constexpr DoublyLinkedList<T>::ForwardIterator::ForwardIterator (const DoublyLinkedList* data) noexcept
        : ForwardIterator{data, (RequireExpression (data != nullptr), data->fHead_)}
    {
    }
    template <typename T>
    inline void DoublyLinkedList<T>::ForwardIterator::Invariant () const noexcept
    {
#if qDebug
        Invariant_ ();
#endif
    }
    template <typename T>
    inline DoublyLinkedList<T>::ForwardIterator::operator bool () const
    {
        return not Done ();
    }
    template <typename T>
    inline bool DoublyLinkedList<T>::ForwardIterator::Done () const noexcept
    {
#if qDebug
        if (fData_ != nullptr) {
            AssertExternallySynchronizedMutex::ReadContext declareContext{*fData_};
            Invariant ();
        }
#endif
        return fCurrent_ == nullptr;
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
    inline auto DoublyLinkedList<T>::ForwardIterator::operator++ (int) noexcept -> ForwardIterator
    {
        ForwardIterator result = *this;
        this->operator++ ();
        return result;
    }
    template <typename T>
    inline const T& DoublyLinkedList<T>::ForwardIterator::operator* () const
    {
#if qDebug
        RequireNotNull (fData_);
        AssertExternallySynchronizedMutex::ReadContext declareContext{*fData_};
#endif
        Require (not Done ());
        Invariant ();
        AssertNotNull (fCurrent_);
        return fCurrent_->fItem;
    }
    template <typename T>
    inline const T* DoublyLinkedList<T>::ForwardIterator::operator->() const
    {
#if qDebug
        RequireNotNull (fData_);
        AssertExternallySynchronizedMutex::ReadContext declareContext{*fData_};
#endif
        Require (not Done ());
        Invariant ();
        AssertNotNull (fCurrent_);
        return &fCurrent_->fItem;
    }
    template <typename T>
    size_t DoublyLinkedList<T>::ForwardIterator::CurrentIndex (const DoublyLinkedList* data) const
    {
        Require (not Done ());
#if qDebug
        Require (data == fData_);
        RequireNotNull (fData_);
#endif
        RequireNotNull (this->fCurrent_);
        size_t i = 0;
        for (const Link_* l = data->fHead_;; l = l->fNext, ++i) {
            AssertNotNull (l);
            if (l == fCurrent_) [[unlikely]] {
                return i;
            }
        }
        AssertNotReached ();
        return i;
    }
    template <typename T>
    inline auto DoublyLinkedList<T>::ForwardIterator::GetUnderlyingIteratorRep () const -> UnderlyingIteratorRep
    {
        return fCurrent_;
    }
    template <typename T>
    inline void DoublyLinkedList<T>::ForwardIterator::SetUnderlyingIteratorRep (UnderlyingIteratorRep l)
    {
#if qDebug
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*fData_};
#endif
        // MUST COME FROM THIS LIST
        // CAN be nullptr
        fCurrent_ = l;
    }
    template <typename T>
    inline bool DoublyLinkedList<T>::ForwardIterator::operator== (const ForwardIterator& rhs) const
    {
        return fCurrent_ == rhs.fCurrent_;
    }
#if qDebug
    template <typename T>
    void DoublyLinkedList<T>::ForwardIterator::Invariant_ () const noexcept
    {
    }
#endif

}
