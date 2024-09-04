/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <optional>

#include "Stroika/Foundation/Containers/Support/ReserveTweaks.h"
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Containers::DataStructures {

// Would like to leave on by default but we just added and cannot afford to have debug builds get that slow
#ifndef qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_
#define qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_ 0
#endif

    /*
     ********************************************************************************
     *********************************** Array<T> ***********************************
     ********************************************************************************
     */
    template <typename T>
    inline void Array<T>::Invariant () const noexcept
    {
#if qDebug
        Invariant_ ();
#endif
    }
    template <typename T>
    Array<T>::Array (const Array& from)
    {
        from.Invariant ();
        reserve (from.size ());

        /*
         *  Construct the new items in-place into the new memory.
         */
        size_t newLength = from.size ();
        if (newLength > 0) {
            T*       lhs = &fItems_[0];
            const T* rhs = &from.fItems_[0];
            T*       end = &fItems_[newLength];
            do {
                new (lhs) T{*rhs++};
            } while (++lhs < end);
        }
        fLength_ = newLength;
        Invariant ();
    }
    template <typename T>
    Array<T>::Array (Array&& from)
        : fItems_{move (from.fItems_)}
        , fLength_{from.fLength_}
    {
        Invariant ();
        from.fItems_  = nullptr;
        from.fLength_ = 0;
        from.Invariant ();
    }
    template <typename T>
    void Array<T>::InsertAt (size_t index, ArgByValueType<T> item)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (index >= 0);
        Require (index <= fLength_);
        Invariant ();

        /*
         * Delicate matter so that we assure ctors/dtors/op= called at
         * right time.
         */
        size_t oldLength = fLength_;
        SetLength (oldLength + 1, item); //  Add space for extra item
        if (index < oldLength) {
            /*
             * Slide items down, and add our new entry
             */
            Assert (fLength_ >= 2);
            T*     lhs = &fItems_[fLength_ - 1];
            T*     rhs = &fItems_[fLength_ - 2];
            size_t i   = fLength_ - 1;

            for (; i > index; --i) {
                *lhs-- = *rhs--;
            }
            Assert (i == index);
            Assert (lhs == &fItems_[index]);
            *lhs = item;
        }
        Invariant ();
    }
    template <typename T>
    inline void Array<T>::push_back (ArgByValueType<T> item)
    {
        InsertAt (this->size (), item);
    }
    template <typename T>
    void Array<T>::RemoveAt (size_t index)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (index >= 0);
        Require (index < fLength_);
        Invariant ();
        if (index < fLength_ - 1) {
            /*
             * Slide items down.
             */
            T* lhs = &fItems_[index];
            T* rhs = &fItems_[index + 1];
            // We tried getting rid of index var and using ptr compare but
            // did much worse on CFront/MPW Thursday, August 27, 1992 4:12:08 PM
            for (size_t i = fLength_ - index - 1; i > 0; i--) {
                *lhs++ = *rhs++;
            }
        }
        destroy_at (&fItems_[--fLength_]);
        Invariant ();
    }
    template <typename T>
    void Array<T>::RemoveAll ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Invariant ();
        T* p = &fItems_[0];
        for (size_t i = fLength_; i > 0; --i, ++p) {
            destroy_at (p);
        }
        fLength_ = 0;
        Invariant ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    bool Array<T>::Contains (ArgByValueType<T> item, EQUALS_COMPARER&& equalsComparer) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Invariant ();
        const T* current = &fItems_[0];
        const T* last    = &fItems_[fLength_];
        for (; current < last; ++current) {
            if (equalsComparer (*current, item)) {
                return true;
            }
        }
        return false;
    }
    template <typename T>
    template <invocable<T> FUNCTION>
    inline void Array<T>::Apply (FUNCTION&& doToElement, Execution::SequencePolicy seq) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        const T*                                              start = &fItems_[0];
        const T*                                              end   = &fItems_[fLength_];
        switch (seq) {
            case Execution::SequencePolicy::eSeq:
                std::for_each (start, end, forward<FUNCTION> (doToElement));
                break;
            default:
#if __cpp_lib_execution >= 201603L
                std::for_each (execution::par, start, end, forward<FUNCTION> (doToElement));
#else
                std::for_each (start, end, forward<FUNCTION> (doToElement));
#endif
                break;
        }
    }
    template <typename T>
    inline auto Array<T>::begin () const -> ForwardIterator
    {
        return ForwardIterator{this, 0};
    }
    template <typename T>
    constexpr auto Array<T>::end () const -> ForwardIterator
    {
        return ForwardIterator{};
    }
    template <typename T>
    template <predicate<T> FUNCTION>
    auto Array<T>::Find (FUNCTION&& firstThat) const -> ForwardIterator
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        const T*                                              start = &fItems_[0];
        const T*                                              i     = start;
        const T*                                              last  = &fItems_[fLength_];
        for (; i < last; ++i) {
            if (firstThat (*i)) {
                return ForwardIterator{this, static_cast<size_t> (i - start)};
            }
        }
        return end ();
    }
    template <typename T>
    void Array<T>::reserve (size_t slotsAlloced)
    {
        /*
         *  @todo NOTE - http://stroika-bugs.sophists.com/browse/STK-757 - use realloc sometimes - if constexpr
         */
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (size () <= slotsAlloced);
        Invariant ();
        if (fSlotsAllocated_ != slotsAlloced) {
            if (slotsAlloced == 0) {
                delete[] (char*)fItems_;
                fItems_ = nullptr;
            }
            else {
                /*
                 * We should consider getting rid of use of realloc since it prohibits
                 * internal pointers. For example, we cannot have an array of patchable_arrays.
                 */
                if (fItems_ == nullptr) {
                    fItems_ = (T*)new char[sizeof (T) * slotsAlloced];
                }
                else {
#if 1
                    // do better, but for now at least do something SAFE
                    // USE SFINAE IsTriviallyCopyable to see which way to do it (if can use realloc).
                    // ALSO - on windoze - use _expand() if avaialble...
                    T* newV = (T*)new char[sizeof (T) * slotsAlloced];
                    try {
                        size_t n2Copy = fLength_;
                        uninitialized_copy_n (&fItems_[0], n2Copy, newV);
                    }
                    catch (...) {
                        delete[] (char*)newV;
                        throw;
                    }
                    {
                        T* end = &fItems_[fLength_];
                        for (T* p = &fItems_[0]; p != end; ++p) {
                            destroy_at (p);
                        }
                    }
                    delete[] (char*)fItems_;
                    fItems_ = newV;
#else
                    fItems_ = (T*)realloc (fItems_, sizeof (T) * slotsAlloced);
#endif
                }
            }
            fSlotsAllocated_ = slotsAlloced;
        }
        Invariant ();
    }
    template <typename T>
    auto Array<T>::operator= (const Array& list) -> Array&
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Invariant ();
        size_t newLength = list.size ();

        /*
         *      In case user already set this, we should not unset,
         *  but must be sure we are big enuf. Do this before we store any pointers
         *  cuz it could invalidate them.
         */
        reserve (max (capacity (), newLength));

        /*
         * Copy array elements where both sides where constructed.
         */
        size_t commonLength = Stroika::Foundation::min (fLength_, newLength);
        T*     lhs          = &fItems_[0];
        T*     rhs          = &list.fItems_[0];
        for (size_t i = commonLength; i-- > 0;) {
            *lhs++ = *rhs++;
        }

        /*
         * Now if new length smaller, we must destroy entries at the end, and
         * otherwise we must copy in new entries.
         */
        Assert (lhs == &fItems_[commonLength]); // point 1 past first guy to destroy/overwrite
        if (fLength_ > newLength) {
            T* end = &fItems_[fLength_]; // point 1 past last old guy
            /*
             * Then we must destruct entries at the end.
             */
            Assert (lhs < end);
            do {
                destroy_at (lhs);
            } while (++lhs < end);
        }
        else if (fLength_ < newLength) {
            T* end = &fItems_[newLength]; // point 1 past last new guy
            Assert (lhs < end);
            do {
                new (lhs) T{*rhs++};
            } while (++lhs < end);
        }
        fLength_ = newLength;
        Invariant ();
        return *this;
    }
    template <typename T>
    void Array<T>::SetLength (size_t newLength, ArgByValueType<T> fillValue)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Invariant ();

        /*
         * Safe to grow the memory, but not to shrink it here, since
         * we may need to destruct guys in the shrinking case.
         */
        if (newLength > fSlotsAllocated_) {
            /*
             *      Bump up Slots alloced to be at least big enuf for our
             * new length. We could be minimalistic here, and just bump up
             * exactly, but this function can be expensive because it calls
             * realloc which could cause lots of memory copying. There are two
             * plausible strategies for bumping up memory in big chunks-
             * rounding up, and scaling up.
             */
            reserve (Support::ReserveTweaks::GetScaledUpCapacity (newLength, sizeof (T)));
        }
        T* cur = &fItems_[fLength_];  // point 1 past first guy
        T* end = &fItems_[newLength]; // point 1 past last guy
        if (newLength > fLength_) {
            Assert (cur < end);
            do {
                new (cur) T{fillValue};
            } while (++cur < end);
        }
        else {
            Assert (cur >= end);
            while (cur-- > end) {
                destroy_at (cur);
            }
        }
        fLength_ = newLength;
        Invariant ();
    }
#if qDebug
    template <typename T>
    void Array<T>::Invariant_ () const noexcept
    {
#if qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
#endif
        Assert ((fSlotsAllocated_ == 0) == (fItems_ == nullptr)); // always free iff slots alloced = 0
        Assert (fLength_ <= fSlotsAllocated_);
    }
#endif
    template <typename T>
    inline Array<T>::~Array ()
    {
        RemoveAll (); // call destructors on elements
        delete[] (char*)fItems_;
    }
    template <typename T>
    inline void Array<T>::MoveIteratorHereAfterClone (IteratorBase* pi, [[maybe_unused]] const Array<T>* movedFrom) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        RequireNotNull (pi);
        RequireNotNull (movedFrom);
        Require (pi->CurrentIndex () <= this->size ());
        Require (pi->_fData == movedFrom);
        pi->_fData = this;
    }
    template <typename T>
    inline T Array<T>::GetAt (size_t i) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (i >= 0);
        Require (i < fLength_);
        return fItems_[i];
    }
    template <typename T>
    inline T* Array<T>::PeekAt (size_t i)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (i >= 0);
        Require (i < fLength_);
        return &fItems_[i];
    }
    template <typename T>
    inline const T* Array<T>::PeekAt (size_t i) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (i >= 0);
        Require (i < fLength_);
        return &fItems_[i];
    }
    template <typename T>
    inline void Array<T>::SetAt (size_t i, ArgByValueType<T> item)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (i >= 0);
        Require (i < fLength_);
        fItems_[i] = item;
    }
    template <typename T>
    inline T& Array<T>::operator[] (size_t i)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (i >= 0);
        Require (i < fLength_);
        return fItems_[i];
    }
    template <typename T>
    inline T Array<T>::operator[] (size_t i) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Require (i >= 0);
        Require (i < fLength_);
        return fItems_[i];
    }
    template <typename T>
    inline size_t Array<T>::size () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return fLength_;
    }
    template <typename T>
    inline bool Array<T>::empty () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return fLength_ == 0;
    }
    template <typename T>
    inline size_t Array<T>::capacity () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return fSlotsAllocated_;
    }
    template <typename T>
    inline void Array<T>::shrink_to_fit ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        reserve (size ());
    }
    template <typename T>
    inline void Array<T>::Remove (const ForwardIterator& i)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (not i.Done ());
        Require (i._fData == this); // assure iterator not stale
        this->RemoveAt (i.CurrentIndex ());
    }
    template <typename T>
    inline auto Array<T>::erase (const ForwardIterator& i) -> ForwardIterator
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (not i.Done ());
        Require (i._fData == this); // assure iterator not stale
        size_t idx = i.CurrentIndex ();
        this->RemoveAt (idx);
        if (idx == this->fLength_) {
            return ForwardIterator{};
        }
        else {
            return ForwardIterator{this, idx};
        }
    }
    template <typename T>
    inline void Array<T>::Remove (const BackwardIterator& i)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (not i.Done ());
        this->RemoveAt (i.CurrentIndex ());
    }
    template <typename T>
    inline void Array<T>::SetAt (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (i._fData == this); // assure iterator not stale
        Require (not i.Done ());
        SetAt (i.CurrentIndex (), newValue);
    }
    template <typename T>
    inline void Array<T>::SetAt (const BackwardIterator& i, ArgByValueType<T> newValue)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (not i.Done ());
        SetAt (i.CurrentIndex (), newValue);
    }
    template <typename T>
    inline void Array<T>::AddBefore (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (i._fData == this); // assure iterator not stale
        // i CAN BE DONE OR NOT
        InsertAt (i.CurrentIndex (), newValue);
    }
    template <typename T>
    inline void Array<T>::AddBefore (const BackwardIterator& i, ArgByValueType<T> newValue)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (i._fData == this); // assure iterator not stale
        // i CAN BE DONE OR NOT
        InsertAt (i.CurrentIndex (), newValue);
    }
    template <typename T>
    inline void Array<T>::AddAfter (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (i._fData == this); // assure iterator not stale
        Require (not i.Done ());
        InsertAt (i.CurrentIndex () + 1, newValue);
    }
    template <typename T>
    inline void Array<T>::AddAfter (const BackwardIterator& i, ArgByValueType<T> newValue)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Require (not i.Done ());
        InsertAt (i.CurrentIndex () + 1, newValue);
    }

    /*
     ********************************************************************************
     ****************************** Array<>::IteratorBase ***************************
     ********************************************************************************
     */
    template <typename T>
    inline Array<T>::IteratorBase::IteratorBase (const Array* data)
        : _fData{data}
    {
        RequireNotNull (data);
    }
#if qDebug
    template <typename T>
    inline Array<T>::IteratorBase::~IteratorBase ()
    {
#if qDebug
        // hack so crash and debug easier
        _fData       = reinterpret_cast<Array<T>*> (-1);
        _fCurrentIdx = numeric_limits<size_t>::max ();
#endif
    }
#endif
    template <typename T>
    inline bool Array<T>::IteratorBase::operator== (const IteratorBase& rhs) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*_fData};
        Require (_fData == rhs._fData);
        return _fCurrentIdx == rhs._fCurrentIdx;
    }
    template <typename T>
    inline size_t Array<T>::IteratorBase::CurrentIndex () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*_fData};
        /*
         * NB: This can be called if we are done - if so, it returns size().
         */
        Invariant ();
        return _fCurrentIdx;
    }
    template <typename T>
    inline const T& Array<T>::IteratorBase::operator* () const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*_fData};
        Invariant ();
        RequireNotNull (_fData);
        Require (0 <= _fCurrentIdx and _fCurrentIdx < _fData->fLength_);
        return _fData->fItems_[_fCurrentIdx];
    }
    template <typename T>
    inline const T* Array<T>::IteratorBase::operator->() const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*_fData};
        Invariant ();
        RequireNotNull (_fData);
        Require (0 <= _fCurrentIdx and _fCurrentIdx < _fData->fLength_);
        return _fData->PeekAt (_fCurrentIdx);
    }
    template <typename T>
    inline void Array<T>::IteratorBase::SetIndex (size_t i)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*_fData};
        RequireNotNull (_fData);
        Require (i <= _fData->fLength_);
        _fCurrentIdx = i;
    }
    template <typename T>
    inline auto Array<T>::IteratorBase::GetUnderlyingIteratorRep () const -> UnderlyingIteratorRep
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*_fData};
        /*
         * NB: This can be called if we are done - if so, it returns size().
         */
        Invariant ();
        return _fCurrentIdx;
    }
    template <typename T>
    inline void Array<T>::IteratorBase::SetUnderlyingIteratorRep (UnderlyingIteratorRep i)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{*_fData};
        RequireNotNull (_fData);
        Require (i <= _fData->fLength_);
        _fCurrentIdx = i;
    }
    template <typename T>
    constexpr void Array<T>::IteratorBase::AssertDataMatches (const Array* data) const
    {
#if qDebug
        Require (data == _fData);
#endif
    }
    template <typename T>
    inline void Array<T>::IteratorBase::Invariant () const noexcept
    {
#if qDebug
        Invariant_ ();
#endif
    }
#if qDebug
    template <typename T>
    void Array<T>::IteratorBase::Invariant_ () const noexcept
    {
        Assert (0 <= _fCurrentIdx);
        if (_fData != nullptr) {
#if qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*_fData};
#endif
            Assert (_fCurrentIdx <= _fData->fLength_);
        }
    }
#endif

    /*
     ********************************************************************************
     *************************** Array<T>::ForwardIterator **************************
     ********************************************************************************
     */
    template <typename T>
    inline Array<T>::ForwardIterator::ForwardIterator (const Array* data, UnderlyingIteratorRep startAt)
        : inherited{data}
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this->_fData};
        this->_fCurrentIdx = startAt;
        this->Invariant ();
    }
    template <typename T>
    constexpr Array<T>::ForwardIterator::ForwardIterator (ForwardIterator&& src) noexcept
    {
        this->_fData       = -src._fData;
        this->_fCurrentIdx = src._fCurrentIdx;
        src._fData         = nullptr;
    }
    template <typename T>
    inline Array<T>::ForwardIterator::operator bool () const
    {
        return not Done ();
    }
    template <typename T>
    inline bool Array<T>::ForwardIterator::Done () const noexcept
    {
        if (this->_fData == nullptr) {
            Assert (this->_fCurrentIdx == 0);
            return true;
        }
#if qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*_fData};
#endif
        this->Invariant ();
        return this->CurrentIndex () == this->_fData->fLength_;
    }
    template <typename T>
    inline auto Array<T>::ForwardIterator::operator++ () noexcept -> ForwardIterator&
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this->_fData};
        Require (not this->Done ());
        this->Invariant ();
        Assert (this->_fCurrentIdx < this->_fData->fLength_);
        ++this->_fCurrentIdx;
        this->Invariant ();
        return *this;
    }
    template <typename T>
    inline auto Array<T>::ForwardIterator::operator++ (int) noexcept -> ForwardIterator
    {
        ForwardIterator result = *this;
        this->operator++ ();
        return result;
    }

    /*
     ********************************************************************************
     **************************** Array<T>::BackwardIterator ************************
     ********************************************************************************
     */
    template <typename T>
    inline Array<T>::BackwardIterator::BackwardIterator (const Array* data, UnderlyingIteratorRep startAt)
        : inherited{data}
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this->_fData};
        this->_fCurrent = startAt;
        this->Invariant ();
    }
    template <typename T>
    inline Array<T>::BackwardIterator::BackwardIterator (const Array* data)
        : BackwardIterator{data->size () == 0 ? data->size () : data->size () - 1} // start on last item or at magic (at end) value
    {
    }
    template <typename T>
    inline bool Array<T>::BackwardIterator::Done () const noexcept
    {
#if qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*_fData};
#endif
        this->Invariant ();
        return bool (this->CurrentIndex () == this->_fData->fLength_); // a little queer/confusing, but in C++ only legal extra address is past end, one before start not legal
    }
    template <typename T>
    inline auto Array<T>::BackwardIterator::operator++ () noexcept -> BackwardIterator&
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this->_fData};
        Require (not this->Done ());
        this->Invariant ();
        if (this->_fCurrent == this->_fStart) {
            this->_fCurrent = this->_fEnd; // magic to indicate done
            Ensure (this->Done ());
        }
        else {
            this->_fCurrent--;
            Ensure (not this->Done ());
        }
        this->Invariant ();
        return *this;
    }

}
