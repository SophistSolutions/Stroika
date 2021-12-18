/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_Array_inl_
#define _Stroika_Foundation_Containers_DataStructures_Array_inl_ 1

#include <optional>

#include "../../Debug/Assertions.h"

#include "../Support/ReserveTweaks.h"

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
        reserve (from.GetLength ());

        /*
         *  Construct the new items in-place into the new memory.
         */
        size_t newLength = from.GetLength ();
        if (newLength > 0) {
            T*       lhs = &fItems_[0];
            const T* rhs = &from.fItems_[0];
            T*       end = &fItems_[newLength];
            do {
                new (lhs) T (*rhs++);
            } while (++lhs < end);
        }
        fLength_ = newLength;
        Invariant ();
    }
    template <typename T>
    void Array<T>::InsertAt (size_t index, ArgByValueType<T> item)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
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
    void Array<T>::RemoveAt (size_t index)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
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
        fItems_[--fLength_].T::~T ();
        Invariant ();
    }
    template <typename T>
    void Array<T>::RemoveAll ()
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Invariant ();
        T* p = &fItems_[0];
        for (size_t i = fLength_; i > 0; --i, ++p) {
            p->T::~T ();
        }
        fLength_ = 0;
        Invariant ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    bool Array<T>::Contains (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
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
    template <typename FUNCTION>
    inline void Array<T>::Apply (FUNCTION doToElement) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        const T*                                             i    = &fItems_[0];
        const T*                                             last = &fItems_[fLength_];
        for (; i < last; ++i) {
            doToElement (*i);
        }
    }
    template <typename T>
    template <typename FUNCTION>
    inline size_t Array<T>::Find (FUNCTION doToElement) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        const T*                                             start = &fItems_[0];
        const T*                                             i     = start;
        const T*                                             last  = &fItems_[fLength_];
        for (; i < last; ++i) {
            if (doToElement (*i)) {
                return i - start;
            }
        }
        return last - start;
    }
    template <typename T>
    void Array<T>::reserve (size_t slotsAlloced)
    {
        /*
         *  @todo NOTE - https://stroika.atlassian.net/browse/STK-757 - use realloc sometimes - if constexpr
         */
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (GetLength () <= slotsAlloced);
        Invariant ();
        if (fSlotsAllocated_ != slotsAlloced) {
            if (slotsAlloced == 0) {
                delete[](char*) fItems_;
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
#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
                        Configuration::uninitialized_copy_n_MSFT_BWA (&fItems_[0], n2Copy, newV);
#else
                        uninitialized_copy_n (&fItems_[0], n2Copy, newV);
#endif
                    }
                    catch (...) {
                        delete[](char*) newV;
                        throw;
                    }
                    {
                        T* end = &fItems_[fLength_];
                        for (T* p = &fItems_[0]; p != end; ++p) {
                            p->T::~T ();
                        }
                    }
                    delete[](char*) fItems_;
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
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Invariant ();
        size_t newLength = list.GetLength ();

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
                lhs->T::~T ();
            } while (++lhs < end);
        }
        else if (fLength_ < newLength) {
            T* end = &fItems_[newLength]; // point 1 past last new guy
            Assert (lhs < end);
            do {
                new (lhs) T (*rhs++);
            } while (++lhs < end);
        }
        fLength_ = newLength;
        Invariant ();
        return *this;
    }
    template <typename T>
    void Array<T>::SetLength (size_t newLength, ArgByValueType<T> fillValue)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
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
                new (cur) T (fillValue);
            } while (++cur < end);
        }
        else {
            Assert (cur >= end);
            while (cur-- > end) {
                cur->T::~T ();
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
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
#endif
        Assert ((fSlotsAllocated_ == 0) == (fItems_ == nullptr)); // always free iff slots alloced = 0
        Assert (fLength_ <= fSlotsAllocated_);
    }
#endif
    template <typename T>
    inline Array<T>::~Array ()
    {
        RemoveAll ();
        delete[](char*) fItems_;
    }
    template <typename T>
    inline void Array<T>::MoveIteratorHereAfterClone (IteratorBase* pi, [[maybe_unused]] const Array<T>* movedFrom) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        RequireNotNull (pi);
        RequireNotNull (movedFrom);
        Require (pi->CurrentIndex () <= this->GetLength ());
        Require (pi->fData_ == movedFrom);
        pi->fData_ = this;
    }
    template <typename T>
    inline T Array<T>::GetAt (size_t i) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        Require (i >= 0);
        Require (i < fLength_);
        return fItems_[i];
    }
    template <typename T>
    inline T* Array<T>::PeekAt (size_t i)
    {
        shared_lock<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (i >= 0);
        Require (i < fLength_);
        return &fItems_[i];
    }
    template <typename T>
    inline void Array<T>::SetAt (size_t i, ArgByValueType<T> item)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (i >= 0);
        Require (i < fLength_);
        fItems_[i] = item;
    }
    template <typename T>
    inline T& Array<T>::operator[] (size_t i)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (i >= 0);
        Require (i < fLength_);
        return fItems_[i];
    }
    template <typename T>
    inline T Array<T>::operator[] (size_t i) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        Require (i >= 0);
        Require (i < fLength_);
        return fItems_[i];
    }
    template <typename T>
    inline size_t Array<T>::GetLength () const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        return fLength_;
    }
    template <typename T>
    inline size_t Array<T>::size () const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        return fLength_;
    }
    template <typename T>
    inline size_t Array<T>::capacity () const
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLock{*this};
        return fSlotsAllocated_;
    }
    template <typename T>
    inline void Array<T>::shrink_to_fit ()
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        reserve (GetLength ());
    }
    template <typename T>
    inline void Array<T>::RemoveAt (const ForwardIterator& i)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (not i.Done ());
        this->RemoveAt (i.CurrentIndex ());
    }
    template <typename T>
    inline void Array<T>::RemoveAt (const BackwardIterator& i)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (not i.Done ());
        this->RemoveAt (i.CurrentIndex ());
    }
    template <typename T>
    inline void Array<T>::SetAt (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (not i.Done ());
        SetAt (i.CurrentIndex (), newValue);
    }
    template <typename T>
    inline void Array<T>::SetAt (const BackwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (not i.Done ());
        SetAt (i.CurrentIndex (), newValue);
    }
    template <typename T>
    inline void Array<T>::AddBefore (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        // i CAN BE DONE OR NOT
        InsertAt (i.CurrentIndex (), newValue);
    }
    template <typename T>
    inline void Array<T>::AddBefore (const BackwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        // i CAN BE DONE OR NOT
        InsertAt (i.CurrentIndex (), newValue);
    }
    template <typename T>
    inline void Array<T>::AddAfter (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
        Require (not i.Done ());
        InsertAt (i.CurrentIndex () + 1, newValue);
    }
    template <typename T>
    inline void Array<T>::AddAfter (const BackwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLock{*this};
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
        : fData_{data}
    {
        RequireNotNull (data);
    }
#if qDebug
    template <typename T>
    inline Array<T>::IteratorBase::~IteratorBase ()
    {
        // hack so crash and debug easier
        fData_       = reinterpret_cast<Array<T>*> (-1);
        fCurrentIdx_ = numeric_limits<size_t>::max ();
    }
#endif
    template <typename T>
    inline bool Array<T>::IteratorBase::Equals (const IteratorBase& rhs) const
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*fData_};
        return fCurrentIdx_ == rhs.fCurrentIdx_;
    }
    template <typename T>
    inline size_t Array<T>::IteratorBase::CurrentIndex () const
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*fData_};
        /*
         * NB: This can be called if we are done - if so, it returns GetLength().
         */
        Invariant ();
        return fCurrentIdx_;
    }
    template <typename T>
    inline T Array<T>::IteratorBase::Current () const
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*fData_};
        Invariant ();
        Require (0 <= fCurrentIdx_ and fCurrentIdx_ < fData_->fLength_);
        return (*fData_)[fCurrentIdx_];
    }
    template <typename T>
    inline void Array<T>::IteratorBase::SetIndex (size_t i)
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*fData_};
        Require (i <= fData_->fLength_);
        fCurrentIdx_ = i;
    }
    template <typename T>
    inline auto Array<T>::IteratorBase::GetUnderlyingIteratorRep () const -> UnderlyingIteratorRep
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*fData_};
        /*
         * NB: This can be called if we are done - if so, it returns GetLength().
         */
        Invariant ();
        return fCurrentIdx_;
    }
    template <typename T>
    inline void Array<T>::IteratorBase::SetUnderlyingIteratorRep (UnderlyingIteratorRep i)
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*fData_};
        Require (i <= fData_->fLength_);
        fCurrentIdx_ = i;
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
#if qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*fData_};
#endif
        AssertNotNull (fData_);
        Assert (0 <= fCurrentIdx_ and fCurrentIdx_ <= fData_->fLength_);
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
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*this->fData_};
        this->fCurrentIdx_ = startAt;
        this->Invariant ();
    }
    template <typename T>
    inline Array<T>::ForwardIterator::ForwardIterator (const Array* data)
        : ForwardIterator{data, 0}
    {
    }
    template <typename T>
    inline bool Array<T>::ForwardIterator::Done () const noexcept
    {
#if qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*fData_};
#endif
        this->Invariant ();
        return bool (this->CurrentIndex () == this->fData_->fLength_);
    }
    template <typename T>
    inline auto Array<T>::ForwardIterator::operator++ () noexcept -> ForwardIterator&
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*this->fData_};
        Require (not this->Done ());
        this->Invariant ();
        Assert (this->fCurrentIdx_ < this->fData_->fLength_);
        ++this->fCurrentIdx_;
        this->Invariant ();
        return *this;
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
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*this->fData_};
        this->_fCurrent = startAt;
        this->Invariant ();
    }
    template <typename T>
    inline Array<T>::BackwardIterator::BackwardIterator (const Array* data)
        : BackwardIterator{data->GetLength () == 0 ? data->GetLength () : data->GetLength () - 1} // start on last item or at magic (at end) value
    {
    }
    template <typename T>
    inline bool Array<T>::BackwardIterator::Done () const noexcept
    {
#if qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*fData_};
#endif
        this->Invariant ();
        return bool (this->CurrentIndex () == this->fData_->fLength_); // a little queer/confusing, but in C++ only legal extra address is past end, one before start not legal
    }
    template <typename T>
    inline auto Array<T>::BackwardIterator::operator++ () noexcept -> BackwardIterator&
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec{*this->fData_};
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
#endif /* _Stroika_Foundation_Containers_DataStructures_Array_inl_ */
