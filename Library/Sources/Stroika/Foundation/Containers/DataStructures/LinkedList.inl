/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <optional>

#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Containers::DataStructures {

// Would like to leave on by default but we just added and cannot afford to have debug builds get that slow
#ifndef qStroika_Foundation_Containers_DataStructures_LinkedList_IncludeSlowDebugChecks_
#define qStroika_Foundation_Containers_DataStructures_LinkedList_IncludeSlowDebugChecks_ 0
#endif

    /*
     ********************************************************************************
     ************************ LinkedList<T,TRAITS>::Link_ ***************************
     ********************************************************************************
     */
    template <typename T>
    constexpr LinkedList<T>::Link_::Link_ (ArgByValueType<T> item, Link_* next)
        : fItem{item}
        , fNext{next}
    {
    }

    /*
     ********************************************************************************
     **************************** LinkedList<T,TRAITS> ******************************
     ********************************************************************************
     */
    template <typename T>
    inline LinkedList<T>::LinkedList ()
    {
        Invariant ();
    }
    template <typename T>
    inline LinkedList<T>::LinkedList (LinkedList&& src) noexcept
        : fHead_{src.fHead_}
        , fLength_{src.fLength_}
    {
        src.fHead_   = nullptr;
        src.fLength_ = 0;
        Invariant ();
        src.Invariant ();
    }
    template <typename T>
    LinkedList<T>::LinkedList (const LinkedList& src)
    {
        /*
         *      Copy the link list by keeping a pointer to the new current and new
         *  previous, and sliding them along in parallel as we construct the
         *  new list. Only do this if we have at least one element - then we
         *  don't have to worry about the head of the list, or nullptr ptrs, etc - that
         *  case is handled outside, before the loop.
         */
        // NB: fLength_ is bumped as each link is attached, NOT assigned from src at the end. A
        // 'new Link_' can throw - the allocation, or T's copy CTOR, since Link_ holds a T by value -
        // and counting incrementally keeps fLength_ equal to the links actually present if it does.
        //
        // The try/catch is REQUIRED, and is not the same case as operator=: a CTOR that throws leaves an
        // object that never finished constructing, so ~LinkedList () is never run for it, so nothing else
        // will ever free the links built so far. Each new Link_ is created with a null fNext and only
        // linked in afterwards, so the chain is always well formed at the throw point and clear () can
        // walk it.
        try {
            if (src.fHead_ != nullptr) {
                fHead_ = new Link_{src.fHead_->fItem, nullptr};
                ++fLength_;
                Link_* newCur = fHead_;
                for (const Link_* cur = src.fHead_->fNext; cur != nullptr; cur = cur->fNext) {
                    Link_* newPrev = newCur;
                    newCur         = new Link_{cur->fItem, nullptr};
                    newPrev->fNext = newCur;
                    ++fLength_;
                }
            }
        }
        catch (...) {
            clear ();
            throw;
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
        clear ();
        Invariant ();
        Ensure (fHead_ == nullptr);
    }
    template <typename T>
    auto LinkedList<T>::operator= (const LinkedList& rhs) -> LinkedList&
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
        Invariant ();
        if (this != &rhs) {
            clear ();
            /*
             *      Copy the link list by keeping a point to the new current and new
             *  previous, and sliding them along in parallel as we construct the
             *  new list. Only do this if we have at least one element - then we
             *  don't have to worry about the head of the list, or nullptr ptrs, etc - that
             *  case is handled outside, before the loop.
             */
            // count as we link (see the note in the copy CTOR): if a 'new Link_' throws partway, this
            // object SURVIVES the failed assignment, so fLength_ must still match the links built so far
            if (rhs.fHead_ != nullptr) {
                fHead_ = new Link_{rhs.fHead_->fItem, nullptr};
                ++fLength_;
                Link_* newCur = fHead_;
                for (const Link_* cur = rhs.fHead_->fNext; cur != nullptr; cur = cur->fNext) {
                    Link_* newPrev = newCur;
                    newCur         = new Link_{cur->fItem, nullptr};
                    newPrev->fNext = newCur;
                    ++fLength_;
                }
            }
        }
        Invariant ();
        return *this;
    }
    template <typename T>
    inline void LinkedList<T>::Invariant () const noexcept
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        Invariant_ ();
#endif
    }
    template <typename T>
    inline void LinkedList<T>::MoveIteratorHereAfterClone (ForwardIterator* pi, const LinkedList* movedFrom) const
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
        // TRICKY TODO - BUT MUST DO - MUST MOVE FROM OLD ITER TO NEW
        // only way
        //
        // For STL containers, not sure how to find an equiv new iterator for an old one, but my best guess is to iterate through
        // old for old, and when I match, stop on new
#if qStroika_Foundation_Debug_AssertionsChecked
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
#if qStroika_Foundation_Debug_AssertionsChecked
        pi->fData_ = this;
#endif
    }
    template <typename T>
    inline auto LinkedList<T>::begin () const -> ForwardIterator
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
        return ForwardIterator{this};
    }
    template <typename T>
    constexpr auto LinkedList<T>::end () const noexcept -> ForwardIterator
    {
        return ForwardIterator{};
    }
    template <typename T>
    inline bool LinkedList<T>::empty () const
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
        return fHead_ == nullptr;
    }
    template <typename T>
    inline size_t LinkedList<T>::size () const
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
        return fLength_;
    }
    template <typename T>
    inline optional<T> LinkedList<T>::GetFirst () const
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
        return fHead_ == nullptr ? optional<T>{} : fHead_->fItem;
    }
    template <typename T>
    inline void LinkedList<T>::push_front (ArgByValueType<T> item)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
        Invariant ();
        fHead_ = new Link_{item, fHead_};
        ++fLength_;
        Invariant ();
    }
    template <typename T>
    template <Memory::ISpanOfT<T> SPAN_T>
    void LinkedList<T>::push_front (const SPAN_T& copyFrom)
    {
        // push_front in reverse order cuz push_front reverses traversal order, and two wrongs make a right
        for (auto ri = copyFrom.rbegin (); ri != copyFrom.rend (); ++ri) {
            push_front (*ri);
        }
    }
    template <typename T>
    void LinkedList<T>::push_back (ArgByValueType<T> item)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
        if (this->fHead_ == nullptr) [[unlikely]] {
            push_front (item);
        }
        else {
            Link_* last = this->fHead_;
            for (; last->fNext != nullptr; last = last->fNext)
                ;
            Assert (last != nullptr);
            Assert (last->fNext == nullptr);
            last->fNext = new Link_{item, nullptr};
            ++fLength_;
        }
    }
    template <typename T>
    template <Memory::ISpanOfT<T> SPAN_T>
    void LinkedList<T>::push_back (const SPAN_T& copyFrom)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
        Link_* last = this->fHead_; // Compute last once, and re-use for each item appended
        if (last != nullptr) {
            for (; last->fNext != nullptr; last = last->fNext)
                ;
        }
        for (const auto& i : copyFrom) {
            if (last == nullptr) [[unlikely]] {
                // list was empty, so the element we just prepended is also the LAST one - track it, or
                // every subsequent element takes this branch too and the span comes out REVERSED
                push_front (i);
                last = fHead_;
            }
            else {
                Assert (last->fNext == nullptr); // really we are last
                last->fNext = new Link_{i, nullptr};
                ++fLength_;
                last = last->fNext; // for next item in span
            }
        }
    }
    template <typename T>
    inline void LinkedList<T>::RemoveFirst ()
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
        Require (not empty ());
        AssertNotNull (fHead_);
        Invariant ();
        Link_* victim = fHead_;
        fHead_        = victim->fNext;
        delete victim;
        --fLength_;
        Invariant ();
    }
    template <typename T>
    inline T* LinkedList<T>::PeekAt (const ForwardIterator& i)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
#if qStroika_Foundation_Debug_AssertionsChecked
        Require (i.fData_ == this); // assure iterator not stale
#endif
        Require (not i.AtEnd ());
        Invariant ();
        i.Invariant ();
        return &const_cast<Link_*> (i.fCurrent_)->fItem;
    }
    template <typename T>
    inline void LinkedList<T>::SetAt (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
        Require (not i.AtEnd ());
#if qStroika_Foundation_Debug_AssertionsChecked
        Require (i.fData_ == this); // assure iterator not stale
#endif
        Invariant ();
        i.Invariant ();
        const_cast<Link_*> (i.fCurrent_)->fItem = newValue;
        Invariant ();
    }
    template <typename T>
    void LinkedList<T>::AddBefore (const ForwardIterator& i, ArgByValueType<T> item)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
#if qStroika_Foundation_Debug_AssertionsChecked
        Require (i.fData_ == this); // assure iterator not stale
#endif
        /*
         * NB: This code works fine, even if 'i' is AtEnd ()
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
            this->fHead_ = new Link_{item, this->fHead_};
        }
        else {
            Assert (prev->fNext == i.fCurrent_);
            prev->fNext = new Link_{item, prev->fNext};
        }
        ++fLength_;

        Invariant ();
    }
    template <typename T>
    void LinkedList<T>::AddBefore (const ForwardIterator& i, ArgByValueType<T> item, ForwardIterator* newLinkCreatedAt)
    {
        RequireNotNull (newLinkCreatedAt);
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
#if qStroika_Foundation_Debug_AssertionsChecked
        Require (i.fData_ == this); // assure iterator not stale
#endif
        /*
         * NB: This code works fine, even if 'i' is AtEnd ()
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
            this->fHead_      = new Link_{item, this->fHead_};
            *newLinkCreatedAt = ForwardIterator{this, this->fHead_};
        }
        else {
            Assert (prev->fNext == i.fCurrent_);
            prev->fNext       = new Link_{item, prev->fNext};
            *newLinkCreatedAt = ForwardIterator{this, prev->fNext};
        }
        ++fLength_;

        Invariant ();
    }
    template <typename T>
    inline void LinkedList<T>::AddAfter (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
        Require (not i.AtEnd ());
#if qStroika_Foundation_Debug_AssertionsChecked
        Require (i.fData_ == this); // assure iterator not stale
#endif
        AssertNotNull (i.fCurrent_); // since not AtEnd...
        i.Invariant ();
        const_cast<Link_*> (i.fCurrent_)->fNext = new Link_{newValue, i.fCurrent_->fNext};
        ++fLength_;
    }
    template <typename T>
    inline auto LinkedList<T>::erase (const ForwardIterator& i) -> ForwardIterator
    {
        ForwardIterator next = i;
        ++next;
        Remove (i);
        next.Invariant ();
        return next;
    }
    template <typename T>
    void LinkedList<T>::Remove (const ForwardIterator& i)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
#if qStroika_Foundation_Debug_AssertionsChecked
        Require (i.fData_ == this); // assure iterator not stale
#endif
        Require (not i.AtEnd ());
        Invariant ();
        i.Invariant ();

        const Link_* victim = i.fCurrent_;

        /*
         *      At this point we need the prev pointer (so so we can adjust its 'next'). 
         *  Since the links go in one direction, we must start at the head, and find the item
         *  pointing to the 'victim'.
         */
        Link_* prevLink = nullptr;
        if (this->fHead_ != victim) {
            auto potentiallyPrevLink = this->fHead_;
            AssertNotNull (potentiallyPrevLink); // cuz there must be something to remove current
            for (; potentiallyPrevLink->fNext != victim; potentiallyPrevLink = potentiallyPrevLink->fNext) {
                AssertNotNull (potentiallyPrevLink); // cuz that would mean victim not in LinkedList!!!
            }
            prevLink = potentiallyPrevLink;
        }
        Assert (prevLink == nullptr or prevLink->fNext == victim);
        if (prevLink == nullptr) {
            Require (this->fHead_ == victim); // If this ever happened, it would mean the argument link to be removed from
                                              // this list was not actually in this list! Caller error - serious bug (corruption?)
            this->fHead_ = victim->fNext;
        }
        else {
            Assert (prevLink->fNext == victim); // because of how we computed prevLink above, this must be true
            prevLink->fNext = victim->fNext;
        }

        delete victim;
        --fLength_;
        Invariant ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    void LinkedList<T>::Remove (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
        Invariant ();
        /*
         *  Base class impl is fine, but doesn't do patching, and doesn't
         *  provide the hooks so I can do the patching from here.
         *
         *  @todo   We may want to correct that (see STL container impl -
         *  returning ptr to next node would do it).
         */
        for (ForwardIterator it{this}; not it.AtEnd (); ++it) {
            if (equalsComparer (*it, item)) {
                this->Remove (it);
                break;
            }
        }
        Invariant ();
    }
    template <typename T>
    template <invocable<T> FUNCTION>
    inline void LinkedList<T>::Apply (FUNCTION&& doToElement) const
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
        for (const Link_* i = fHead_; i != nullptr; i = i->fNext) {
            doToElement (i->fItem);
        }
    }
    template <typename T>
    template <predicate<T> FUNCTION>
    inline auto LinkedList<T>::Find (FUNCTION&& firstThat) const -> UnderlyingIteratorRep
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
        for (Link_* i = fHead_; i != nullptr; i = i->fNext) {
            if (firstThat (i->fItem)) {
                return i;
            }
        }
        return nullptr;
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    T* LinkedList<T>::Find (ArgByValueType<T> item, EQUALS_COMPARER&& equalsComparer)
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this}; // lock not shared cuz return mutable ptr
        for (Link_* i = fHead_; i != nullptr; i = i->fNext) {
            if (forward<EQUALS_COMPARER> (equalsComparer) (i->fItem, item)) {
                return &i->fItem;
            }
        }
        return nullptr;
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    const T* LinkedList<T>::Find (ArgByValueType<T> item, EQUALS_COMPARER&& equalsComparer) const
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
        for (const Link_* i = fHead_; i != nullptr; i = i->fNext) {
            if (forward<EQUALS_COMPARER> (equalsComparer) (i->fItem, item)) {
                return &i->fItem;
            }
        }
        return nullptr;
    }
    template <typename T>
    void LinkedList<T>::clear ()
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
        Invariant ();
        for (Link_* i = fHead_; i != nullptr;) {
            Link_* deleteMe = i;
            i               = i->fNext;
            delete deleteMe;
        }
        fHead_   = nullptr;
        fLength_ = 0;
        Invariant ();
        Ensure (empty ());
    }
    template <typename T>
    T LinkedList<T>::GetAt (size_t i) const
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
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
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
        Require (i >= 0);
        Require (i < size ());
        Link_* cur = fHead_;
        for (; i != 0; cur = cur->fNext, --i) {
            AssertNotNull (cur); // cuz i <= fLength
        }
        AssertNotNull (cur); // cuz i <= fLength
        cur->fItem = item;
    }
#if qStroika_Foundation_Debug_AssertionsChecked
    template <typename T>
    void LinkedList<T>::Invariant_ () const noexcept
    {
#if qStroika_Foundation_Containers_DataStructures_LinkedList_IncludeSlowDebugChecks_
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
#endif
        /*
         * Check we are properly linked together.
         */
        size_t n = 0;
        for (Link_* i = fHead_; i != nullptr; i = i->fNext) {
            // at least make sure no corrupted links and no infinite loops
            ++n;
        }
        // the cached length must agree with the links, or some mutator failed to maintain it
        Assert (n == fLength_);
    }
#endif

    /*
     ********************************************************************************
     ************************* LinkedList<T>::ForwardIterator ***********************
     ********************************************************************************
     */
    template <typename T>
    constexpr LinkedList<T>::ForwardIterator::ForwardIterator ([[maybe_unused]] const LinkedList* data, UnderlyingIteratorRep startAt) noexcept
        : fCurrent_{startAt}
#if qStroika_Foundation_Debug_AssertionsChecked
        , fData_{data}
#endif
    {
        RequireNotNull (data);
    }
    template <typename T>
    constexpr LinkedList<T>::ForwardIterator::ForwardIterator (const LinkedList* data) noexcept
        : ForwardIterator{data, (RequireExpression (data != nullptr), data->fHead_)}
    {
        RequireNotNull (data);
    }
    template <typename T>
    inline void LinkedList<T>::ForwardIterator::Invariant () const noexcept
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        Invariant_ ();
#endif
    }
    template <typename T>
    inline LinkedList<T>::ForwardIterator::operator bool () const
    {
        return not AtEnd ();
    }
    template <typename T>
    inline bool LinkedList<T>::ForwardIterator::AtEnd () const noexcept
    {
        Invariant ();
        return fCurrent_ == nullptr;
    }
    template <typename T>
    inline auto LinkedList<T>::ForwardIterator::operator++ () noexcept -> ForwardIterator&
    {
        Require (not AtEnd ());
        Invariant ();
        Assert (fCurrent_ != nullptr);
        fCurrent_ = fCurrent_->fNext;
        Invariant ();
        return *this;
    }
    template <typename T>
    inline auto LinkedList<T>::ForwardIterator::operator++ (int) noexcept -> ForwardIterator
    {
        ForwardIterator result = *this;
        this->operator++ ();
        return result;
    }
    template <typename T>
    inline T LinkedList<T>::ForwardIterator::operator* () const
    {
        Require (not(AtEnd ()));
        Invariant ();
        AssertNotNull (fCurrent_);
        return fCurrent_->fItem;
    }
    template <typename T>
    inline const T* LinkedList<T>::ForwardIterator::operator->() const
    {
        Require (not(AtEnd ()));
        Invariant ();
        AssertNotNull (fCurrent_);
        return &fCurrent_->fItem;
    }
    template <typename T>
    size_t LinkedList<T>::ForwardIterator::CurrentIndex (const LinkedList* data) const
    {
        Require (not AtEnd ());
#if qStroika_Foundation_Debug_AssertionsChecked
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
    constexpr void LinkedList<T>::ForwardIterator::AssertDataMatches ([[maybe_unused]] const LinkedList* data) const
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        Require (data == fData_);
#endif
    }
    template <typename T>
    inline bool LinkedList<T>::ForwardIterator::operator== (const ForwardIterator& rhs) const
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        Require (fData_ == nullptr or rhs.fData_ == nullptr or fData_ == rhs.fData_); // fData_==null for end sentinel case
#endif
        return fCurrent_ == rhs.fCurrent_;
    }
#if qStroika_Foundation_Debug_AssertionsChecked
    template <typename T>
    void LinkedList<T>::ForwardIterator::Invariant_ () const noexcept
    {
    }
#endif

}
