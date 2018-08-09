/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_Array_inl_
#define _Stroika_Foundation_Containers_DataStructures_Array_inl_ 1

#include <optional>

#include "../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::DataStructures {

// Would like to leave on by default but we just added and cannot afford to have debug builds get that slow
#ifndef qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_
#define qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_ 0
#endif

#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
    namespace PRIVATE_ {
        template <class InputIt, class Size, class ForwardIt>
        ForwardIt uninitialized_copy_n_MSFT_BWA (InputIt first, Size count, ForwardIt d_first)
        {
            using Value       = typename iterator_traits<ForwardIt>::value_type;
            ForwardIt current = d_first;
            try {
                for (; count > 0; ++first, ++current, --count) {
                    ::new (static_cast<void*> (Traversal::Iterator2Pointer (current))) Value (*first);
                }
            }
            catch (...) {
                for (; d_first != current; ++d_first) {
                    d_first->~Value ();
                }
                throw;
            }
            return current;
        }
    }
#endif

    /*
     ********************************************************************************
     *********************************** Array<T> ***********************************
     ********************************************************************************
     */
    template <typename T>
    inline void Array<T>::Invariant () const
    {
#if qDebug
        _Invariant ();
#endif
    }
    template <typename T>
    inline Array<T>::Array ()
        : _fLength (0)
        , _fSlotsAllocated (0)
        , _fItems (0)
    {
    }
    template <typename T>
    Array<T>::Array (const Array<T>& from)
        : _fLength (0)
        , _fSlotsAllocated (0)
        , _fItems (nullptr)
    {
        from.Invariant ();
        SetCapacity (from.GetLength ());

        /*
         *  Construct the new items in-place into the new memory.
         */
        size_t newLength = from.GetLength ();
        if (newLength > 0) {
            T*       lhs = &_fItems[0];
            const T* rhs = &from._fItems[0];
            T*       end = &_fItems[newLength];
            do {
                new (lhs) T (*rhs++);
            } while (++lhs < end);
        }
        _fLength = newLength;
        Invariant ();
    }
    template <typename T>
    void Array<T>::InsertAt (size_t index, ArgByValueType<T> item)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (index >= 0);
        Require (index <= _fLength);
        Invariant ();

        /*
         * Delicate matter so that we assure ctors/dtors/op= called at
         * right time.
         */
        SetLength (_fLength + 1, item); //  Add space for extra item
        size_t oldLength = _fLength - 1;
        if (index < oldLength) {
            /*
                * Slide items down, and add our new entry
                */
            Assert (_fLength >= 2);
            T*     lhs = &_fItems[_fLength - 1];
            T*     rhs = &_fItems[_fLength - 2];
            size_t i   = _fLength - 1;

            for (; i > index; i--) {
                *lhs-- = *rhs--;
            }
            Assert (i == index);
            Assert (lhs == &_fItems[index]);
            *lhs = item;
        }
        Invariant ();
    }
    template <typename T>
    void Array<T>::RemoveAt (size_t index)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (index >= 0);
        Require (index < _fLength);
        Invariant ();

        if (index < _fLength - 1) {
            /*
             * Slide items down.
             */
            T* lhs = &_fItems[index];
            T* rhs = &_fItems[index + 1];
            // We tried getting rid of index var and using ptr compare but
            // did much worse on CFront/MPW Thursday, August 27, 1992 4:12:08 PM
            for (size_t i = _fLength - index - 1; i > 0; i--) {
                *lhs++ = *rhs++;
            }
        }
        _fItems[--_fLength].T::~T ();
        Invariant ();
    }
    template <typename T>
    void Array<T>::RemoveAll ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Invariant ();
        T* p = &_fItems[0];
        for (size_t i = _fLength; i > 0; i--, p++) {
            p->T::~T ();
        }
        _fLength = 0;
        Invariant ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    bool Array<T>::Contains (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Invariant ();
        const T* current = &_fItems[0];
        const T* last    = &_fItems[_fLength];
        for (; current < last; current++) {
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
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        const T*                                            i    = &_fItems[0];
        const T*                                            last = &_fItems[_fLength];
        for (; i < last; i++) {
            (doToElement) (*i);
        }
    }
    template <typename T>
    template <typename FUNCTION>
    inline size_t Array<T>::FindFirstThat (FUNCTION doToElement) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        const T*                                            start = &_fItems[0];
        const T*                                            i     = start;
        const T*                                            last  = &_fItems[_fLength];
        for (; i < last; i++) {
            if ((doToElement) (*i)) {
                return i - start;
            }
        }
        return last - start;
    }
    template <typename T>
    void Array<T>::SetCapacity (size_t slotsAlloced)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (GetLength () <= slotsAlloced);
        Invariant ();
        if (_fSlotsAllocated != slotsAlloced) {
            if (slotsAlloced == 0) {
                delete[](char*) _fItems;
                _fItems = nullptr;
            }
            else {
                /*
                 * We should consider getting rid of use of realloc since it prohibits
                 * internal pointers. For example, we cannot have an array of patchable_arrays.
                 */
                if (_fItems == nullptr) {
                    _fItems = (T*)new char[sizeof (T) * slotsAlloced];
                }
                else {
#if 1
                    // do better, but for now at least do something SAFE
                    // USE SFINAE IsTriviallyCopyable to see which way to do it (if can use realloc).
                    // ALSO - on windoze - use _expand() if avaialble...
                    T* newV = (T*)new char[sizeof (T) * slotsAlloced];
                    try {
                        size_t n2Copy = min (_fSlotsAllocated, slotsAlloced);
#if qCompilerAndStdLib_uninitialized_copy_n_Warning_Buggy
                        PRIVATE_::uninitialized_copy_n_MSFT_BWA (&_fItems[0], n2Copy, newV);
#else
                        uninitialized_copy_n (&_fItems[0], n2Copy, newV);
#endif
                    }
                    catch (...) {
                        delete[](char*) newV;
                        throw;
                    }
                    {
                        T* end = &_fItems[_fLength];
                        for (T* p = &_fItems[0]; p != end; ++p) {
                            p->T::~T ();
                        }
                    }
                    delete[](char*) _fItems;
                    _fItems = newV;
#else
                    _fItems = (T*)realloc (_fItems, sizeof (T) * slotsAlloced);
#endif
                }
            }
            _fSlotsAllocated = slotsAlloced;
        }
        Invariant ();
    }
    template <typename T>
    Array<T>& Array<T>::operator= (const Array<T>& list)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Invariant ();
        size_t newLength = list.GetLength ();

        /*
         *      In case user already set this, we should not unset,
         *  but must be sure we are big enuf. Do this before we store any pointers
         *  cuz it could invalidate them.
         */
        SetCapacity (max (GetCapacity (), newLength));

        /*
         * Copy array elements where both sides where constructed.
         */
        size_t commonLength = Stroika::Foundation::min (_fLength, newLength);
        T*     lhs          = &_fItems[0];
        T*     rhs          = &list._fItems[0];
        for (size_t i = commonLength; i-- > 0;) {
            *lhs++ = *rhs++;
        }

        /*
         * Now if new length smaller, we must destroy entries at the end, and
         * otherwise we must copy in new entries.
         */
        Assert (lhs == &_fItems[commonLength]); // point 1 past first guy to destroy/overwrite
        if (_fLength > newLength) {
            T* end = &_fItems[_fLength]; // point 1 past last old guy
            /*
             * Then we must destruct entries at the end.
             */
            Assert (lhs < end);
            do {
                lhs->T::~T ();
            } while (++lhs < end);
        }
        else if (_fLength < newLength) {
            T* end = &_fItems[newLength]; // point 1 past last new guy
            Assert (lhs < end);
            do {
                new (lhs) T (*rhs++);
            } while (++lhs < end);
        }
        _fLength = newLength;
        Invariant ();
        return *this;
    }
    template <typename T>
    void Array<T>::SetLength (size_t newLength, ArgByValueType<T> fillValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Invariant ();

        /*
         * Safe to grow the memory, but not to shrink it here, since
         * we may need to destruct guys in the shrinking case.
         */
        if (newLength > _fSlotsAllocated) {
            /*
             *      Bump up Slots alloced to be at least big enuf for our
             * new length. We could be minimalistic here, and just bump up
             * exactly, but this function can be expensive because it calls
             * realloc which could cause lots of memory copying. There are two
             * plausible strategies for bumping up memory in big chunks-
             * rounding up, and scaling up.
             *
             *      Rounding up works well at small scales - total memory
             *  waste is small (bounded). It is simple, and it helps speed up
             *  loops like while condition { append (); } considerably.
             *
             *      Scaling up has the advantage that for large n, we get
             *  logn reallocs (rather than n/IncSize in the roundup case).
             *  This is much better long-term large-size performance.
             *  The only trouble with this approach is that in order to keep
             *  memory waste small, we must scale by a small number (here 1.1)
             *  and so we need array sizes > 100 before we start seeing any real
             *  benefit at all. Such cases do happen, but we want to be able to
             *  optimize the much more common, small array cases too.
             *
             *      So the compromise is to use a roundup-like strategy for
             *  small n, and a scaling approach as n gets larger.
             *
             *      Also, we really should be more careful about overflow here...
             *
             *      Some math:
             *          k*n = n + 64/sizeof (T) and so
             *          n = (64/sizeof (T))/(k-1)
             *      If we assume k = 1.1 and sizeof(T) = 4 then n = 160. This is
             *  the value for length where we start scaling up by 10% as opposed to
             *  our arithmetic + 16.
             *
             */
            //SetCapacity (Max (newLength+(64/sizeof (T)), size_t (newLength*1.1)));
            // Based on the above arithmatic, we can take a shortcut...
            SetCapacity ((newLength > 160) ? size_t (newLength * 1.1) : (newLength + (64 / sizeof (T))));
        }
        T* cur = &_fItems[_fLength];  // point 1 past first guy
        T* end = &_fItems[newLength]; // point 1 past last guy
        if (newLength > _fLength) {
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
        _fLength = newLength;
        Invariant ();
    }
#if qDebug
    template <typename T>
    void Array<T>::_Invariant () const
    {
#if qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
#endif
        Assert ((_fSlotsAllocated == 0) == (_fItems == nullptr)); // always free iff slots alloced = 0
        Assert (_fLength <= _fSlotsAllocated);
    }
#endif
    template <typename T>
    inline Array<T>::~Array ()
    {
        RemoveAll ();
        delete[](char*) _fItems;
    }
    template <typename T>
    inline void Array<T>::MoveIteratorHereAfterClone (_ArrayIteratorBase* pi, [[maybe_unused]] const Array<T>* movedFrom)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        RequireNotNull (pi);
        RequireNotNull (movedFrom);
        size_t currentIdx = pi->_fCurrent - pi->_fStart;
        Require (currentIdx <= this->GetLength ());
        Require (pi->_fData == movedFrom);
        pi->_fData    = this;
        pi->_fStart   = &_fItems[0];
        pi->_fEnd     = &this->_fItems[this->GetLength ()];
        pi->_fCurrent = pi->_fStart + currentIdx;
    }
    template <typename T>
    inline T Array<T>::GetAt (size_t i) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (i >= 0);
        Require (i < _fLength);
        return _fItems[i];
    }
    template <typename T>
    inline void Array<T>::SetAt (size_t i, ArgByValueType<T> item)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (i >= 0);
        Require (i < _fLength);
        _fItems[i] = item;
    }
    template <typename T>
    inline T& Array<T>::operator[] (size_t i)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (i >= 0);
        Require (i < _fLength);
        return _fItems[i];
    }
    template <typename T>
    inline T Array<T>::operator[] (size_t i) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Require (i >= 0);
        Require (i < _fLength);
        return _fItems[i];
    }
    template <typename T>
    inline size_t Array<T>::GetLength () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _fLength;
    }
    template <typename T>
    inline size_t Array<T>::GetCapacity () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return _fSlotsAllocated;
    }
    template <typename T>
    inline void Array<T>::Compact ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        SetCapacity (GetLength ());
    }
    template <typename T>
    inline void Array<T>::RemoveAt (const ForwardIterator& i)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (not i.Done ());
        this->RemoveAt (i.CurrentIndex ());
    }
    template <typename T>
    inline void Array<T>::RemoveAt (const BackwardIterator& i)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (not i.Done ());
        this->RemoveAt (i.CurrentIndex ());
    }
    template <typename T>
    inline void Array<T>::SetAt (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (not i.Done ());
        SetAt (i.CurrentIndex (), newValue);
    }
    template <typename T>
    inline void Array<T>::SetAt (const BackwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (not i.Done ());
        SetAt (i.CurrentIndex (), newValue);
    }
    template <typename T>
    inline void Array<T>::AddBefore (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        // i CAN BE DONE OR NOT
        InsertAt (i.CurrentIndex (), newValue);
    }
    template <typename T>
    inline void Array<T>::AddBefore (const BackwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        // i CAN BE DONE OR NOT
        InsertAt (i.CurrentIndex (), newValue);
    }
    template <typename T>
    inline void Array<T>::AddAfter (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (not i.Done ());
        InsertAt (i.CurrentIndex () + 1, newValue);
    }
    template <typename T>
    inline void Array<T>::AddAfter (const BackwardIterator& i, ArgByValueType<T> newValue)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        Require (not i.Done ());
        InsertAt (i.CurrentIndex () + 1, newValue);
    }

    /*
     ********************************************************************************
     ************************* Array<>::_ArrayIteratorBase **************************
     ********************************************************************************
     */
    template <typename T>
    inline Array<T>::_ArrayIteratorBase::_ArrayIteratorBase (const Array<T>* data)
        : _fData (data)
        , _fStart (&data->_fItems[0])
        , _fEnd (&data->_fItems[data->GetLength ()])
        //, _fCurrent ()                           dont initialize - done in subclasses...
        , _fSuppressMore (true) // first time thru - cuz of how used in for loops...
    {
        RequireNotNull (data);
#if qDebug
        _fCurrent = nullptr; // more likely to cause bugs...
#endif
        /*
         * Cannot call invariant () here since _fCurrent not yet setup.
         */
    }
#if qDebug
    template <typename T>
    inline Array<T>::_ArrayIteratorBase::~_ArrayIteratorBase ()
    {
        // hack so crash and debug easier
        _fData    = reinterpret_cast<Array<T>*> (-1);
        _fStart   = reinterpret_cast<T*> (-1);
        _fEnd     = reinterpret_cast<T*> (-1);
        _fCurrent = reinterpret_cast<T*> (-1);
    }
#endif
    template <typename T>
    nonvirtual bool Array<T>::_ArrayIteratorBase::Equals (const typename Array<T>::_ArrayIteratorBase& rhs) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
        return _fCurrent == rhs._fCurrent and _fSuppressMore == rhs._fSuppressMore;
    }
    template <typename T>
    inline bool Array<T>::_ArrayIteratorBase::More (T* current, bool advance)
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
        if (advance) {
            this->_fSuppressMore = false;
        }
        Invariant ();
        if (not Done ()) {
            if (current != nullptr) {
                *current = *_fCurrent;
            }
            return true;
        }
        return false;
    }
    template <typename T>
    inline bool Array<T>::_ArrayIteratorBase::Done () const
    {
#if qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
#endif
        Invariant ();
        return bool(_fCurrent == _fEnd);
    }
    template <typename T>
    inline size_t Array<T>::_ArrayIteratorBase::CurrentIndex () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
        /*
            * NB: This can be called if we are done - if so, it returns GetLength().
            */
        Invariant ();
        return _fCurrent - _fStart;
    }
    template <typename T>
    inline T Array<T>::_ArrayIteratorBase::Current () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
        Invariant ();
        EnsureNotNull (_fCurrent);
        return *_fCurrent;
    }
    template <typename T>
    inline void Array<T>::_ArrayIteratorBase::SetIndex (size_t i)
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
        Require (i <= size_t (_fEnd - _fStart));
        _fCurrent      = _fStart + i;
        _fSuppressMore = false;
    }
    template <typename T>
    inline void Array<T>::_ArrayIteratorBase::Invariant () const
    {
#if qDebug
        _Invariant ();
#endif
    }
#if qDebug
    template <typename T>
    void Array<T>::_ArrayIteratorBase::_Invariant () const
    {
#if qStroika_Foundation_Containers_DataStructures_Array_IncludeSlowDebugChecks_
        shared_lock<const AssertExternallySynchronizedLock> critSec{*_fData};
#endif
        AssertNotNull (_fData);
        Assert (_fStart == _fData->_fItems);
        Assert (size_t (_fEnd - _fStart) == _fData->GetLength ());
        Assert ((_fCurrent >= _fStart) and (_fCurrent <= _fEnd)); // ANSI C requires this is always TRUE
    }
#endif

    /*
     ********************************************************************************
     *************************** Array<T>::ForwardIterator **************************
     ********************************************************************************
     */
    template <typename T>
    inline Array<T>::ForwardIterator::ForwardIterator (const Array<T>* data)
        : inherited (data)
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this->_fData};
        this->_fCurrent = this->_fStart;
        this->Invariant ();
    }
    template <typename T>
    inline bool Array<T>::ForwardIterator::More (T* current, bool advance)
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this->_fData};
        this->Invariant ();
        if (advance) {
            if (not this->_fSuppressMore and not this->Done ()) {
                Assert (this->_fCurrent < this->_fEnd);
                this->_fCurrent++;
            }
        }
        return inherited::More (current, advance);
    }
    template <typename T>
    inline void Array<T>::ForwardIterator::More (optional<T>* result, bool advance)
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this->_fData};
        this->Invariant ();
        if (advance) {
            if (this->_fSuppressMore) {
                this->_fSuppressMore = false;
            }
            else {
                if (not this->Done ()) {
                    Assert (this->_fCurrent < this->_fEnd);
                    this->_fCurrent++;
                }
            }
        }
        this->Invariant ();
        if (this->Done ()) {
            *result = nullopt;
        }
        else {
            *result = *this->_fCurrent;
        }
    }
    template <typename T>
    inline bool Array<T>::ForwardIterator::More (nullptr_t, bool advance)
    {
        return More (static_cast<T*> (nullptr), advance);
    }

    /*
     ********************************************************************************
     **************************** Array<T>::BackwardIterator ************************
     ********************************************************************************
     */
    template <typename T>
    inline Array<T>::BackwardIterator::BackwardIterator (const Array<T>* data)
        : inherited (data)
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this->_fData};
        if (data->GetLength () == 0) {
            this->_fCurrent = this->_fEnd; // magic to indicate done
        }
        else {
            this->_fCurrent = this->_fEnd - 1; // last valid item
        }
        this->Invariant ();
    }
    template <typename T>
    inline bool Array<T>::BackwardIterator::More (T* current, bool advance)
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this->_fData};
        this->Invariant ();
        if (advance) {
            if (not this->_fSuppressMore and not this->Done ()) {
                if (this->_fCurrent == this->_fStart) {
                    this->_fCurrent = this->_fEnd; // magic to indicate done
                    Ensure (this->Done ());
                }
                else {
                    this->_fCurrent--;
                    Ensure (not this->Done ());
                }
            }
        }
        return inherited::More (current, advance);
    }
    template <typename T>
    inline void Array<T>::BackwardIterator::More (optional<T>* result, bool advance)
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this->_fData};
        this->Invariant ();
        if (advance) {
            if (this->_fSuppressMore) {
                this->_fSuppressMore = false;
            }
            else {
                if (not this->Done ()) {
                    if (this->_fCurrent == this->_fStart) {
                        this->_fCurrent = this->_fEnd; // magic to indicate done
                        Ensure (this->Done ());
                    }
                    else {
                        this->_fCurrent--;
                        Ensure (not this->Done ());
                    }
                }
            }
        }
        this->Invariant ();
        if (this->Done ()) {
            *result = nullopt;
        }
        else {
            *result = *this->_fCurrent;
        }
    }
    template <typename T>
    inline bool Array<T>::BackwardIterator::More (nullptr_t, bool advance)
    {
        return More (static_cast<T*> (nullptr), advance);
    }

}
#endif /* _Stroika_Foundation_Containers_DataStructures_Array_inl_ */
