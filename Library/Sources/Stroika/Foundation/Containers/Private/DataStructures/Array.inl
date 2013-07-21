/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_Array_inl_
#define _Stroika_Foundation_Containers_Private_DataStructures_Array_inl_   1


#include    "../../../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {


                    /*
                    ********************************************************************************
                    **************************** Array<T,TRAITS> ***********************************
                    ********************************************************************************
                    */
                    template      <typename  T, typename TRAITS>
                    inline  void    Array<T, TRAITS>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template      <typename  T, typename TRAITS>
                    inline  Array<T, TRAITS>::Array ()
                        : _fLength (0)
                        , _fSlotsAllocated (0)
                        , _fItems (0)
                    {
                    }
                    template      <typename  T, typename TRAITS>
                    Array<T, TRAITS>::Array (const Array<T, TRAITS>& from)
                        : _fLength (0)
                        , _fSlotsAllocated (0)
                        , _fItems (nullptr)
                    {
                        from.Invariant ();
                        SetCapacity (from.GetLength ());

                        /*
                         *  Construct the new items in-place into the new memory.
                         */
                        size_t  newLength       =   from.GetLength ();
                        if (newLength > 0) {
                            T*   lhs =   &_fItems[0];
                            const   T*   rhs =   &from._fItems[0];
                            T*   end =   &_fItems[newLength];
                            do {
                                new (lhs) T (*rhs++);
                            }
                            while (++lhs < end);
                        }
                        _fLength = newLength;
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    void    Array<T, TRAITS>::InsertAt (size_t index, T item)
                    {
                        Require (index >= 0);
                        Require (index <= _fLength);
                        Invariant ();

                        /*
                         * Delicate matter so that we assure ctors/dtors/op= called at
                         * right time.
                         */
                        SetLength (_fLength + 1, item);          //  Add space for extra item
                        size_t  oldLength = _fLength - 1;
                        if (index < oldLength) {
                            /*
                             * Slide items down, and add our new entry
                             */
                            Assert (_fLength >= 2);
                            T*   lhs =   &_fItems [_fLength - 1];
                            T*   rhs =   &_fItems [_fLength - 2];
                            size_t i = _fLength - 1;

                            for (; i > index; i--) {
                                *lhs-- = *rhs--;
                            }
                            Assert (i == index);
                            Assert (lhs == &_fItems [index]);
                            *lhs = item;
                        }
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    void    Array<T, TRAITS>::RemoveAt (size_t index)
                    {
                        Require (index >= 0);
                        Require (index < _fLength);
                        Invariant ();

                        if (index < _fLength - 1) {
                            /*
                             * Slide items down.
                             */
                            T*   lhs =   &_fItems [index];
                            T*   rhs =   &_fItems [index + 1];
                            // We tried getting rid of index var and using ptr compare but
                            // did much worse on CFront/MPW Thursday, August 27, 1992 4:12:08 PM
                            for ( size_t i = _fLength - index - 1; i > 0; i--) {

                                *lhs++ = *rhs++;
                            }
                        }
                        _fItems [--_fLength].T::~T ();
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    void    Array<T, TRAITS>::RemoveAll ()
                    {
                        Invariant ();
                        T*   p   =   &_fItems[0];
                        for ( size_t i = _fLength; i > 0; i--, p++) {
                            p->T::~T ();
                        }
                        _fLength = 0;
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    bool    Array<T, TRAITS>::Contains (T item) const
                    {
                        Invariant ();
                        if (_fLength > 0) {
                            const   T*   current =   &_fItems [0];
                            const   T*   last    =   &_fItems [_fLength - 1];  // safe to -1 since _fLength>0
                            for (; current <= last; current++) {
                                if (TRAITS::EqualsCompareFunctionType::Equals (*current, item)) {
                                    return true;
                                }
                            }
                        }
                        return false;
                    }
                    template      <typename  T, typename TRAITS>
                    void    Array<T, TRAITS>::SetCapacity (size_t slotsAlloced)
                    {
                        Require (GetLength () <= slotsAlloced);
                        Invariant ();
                        if (_fSlotsAllocated != slotsAlloced) {
                            if (slotsAlloced == 0) {
                                delete (char*)_fItems;
                                _fItems = nullptr;
                            }
                            else {
                                /*
                                 * We should consider getting rid of use of realloc since it prohibits
                                 * internal pointers. For example, we cannot have an array of patchable_arrays.
                                 */
                                if (_fItems == nullptr) {
                                    _fItems = (T*) new char [sizeof (T) * slotsAlloced];
                                }
                                else {
                                    _fItems = (T*) realloc (_fItems, sizeof (T) * slotsAlloced);
                                }
                            }
                            _fSlotsAllocated = slotsAlloced;
                        }
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    Array<T, TRAITS>& Array<T, TRAITS>::operator= (const Array<T, TRAITS>& list)
                    {
                        Invariant ();
                        size_t  newLength       =   list.GetLength ();

                        /*
                         *      In case user already set this, we should not unset,
                         *  but must be sure we are big enuf. Do this before we store any pointers
                         *  cuz it could invalidate them.
                         */
                        SetCapacity (max (GetCapacity (), newLength));

                        /*
                         * Copy array elements where both sides where constructed.
                         */
                        size_t  commonLength    =   Stroika::Foundation::min (_fLength, newLength);
                        T*   lhs =   &_fItems[0];
                        T*   rhs =   &list._fItems[0];
                        for ( size_t i = commonLength; i-- > 0; ) {
                            *lhs++ = *rhs++;
                        }

                        /*
                         * Now if new length smaller, we must destroy entries at the end, and
                         * otherwise we must copy in new entries.
                         */
                        Assert (lhs == &_fItems[commonLength]);                      // point 1 past first guy to destroy/overwrite
                        if (_fLength > newLength) {
                            T*   end =   &_fItems[_fLength];   // point 1 past last old guy
                            /*
                             * Then we must destruct entries at the end.
                             */
                            Assert (lhs < end);
                            do {
                                lhs->T::~T ();
                            }
                            while (++lhs < end);
                        }
                        else if (_fLength < newLength) {
                            T*   end =   &_fItems[newLength]; // point 1 past last new guy
                            Assert (lhs < end);
                            do {
                                new (lhs) T (*rhs++);
                            }
                            while (++lhs < end);
                        }
                        _fLength = newLength;
                        Invariant ();
                        return *this;
                    }
                    template      <typename  T, typename TRAITS>
                    void    Array<T, TRAITS>::SetLength (size_t newLength, T fillValue)
                    {
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
                        T*   cur =   &_fItems[_fLength];   // point 1 past first guy
                        T*   end =   &_fItems[newLength]; // point 1 past last guy
                        if (newLength > _fLength) {
                            Assert (cur < end);
                            do {
                                new (cur) T (fillValue);
                            }
                            while (++cur < end);
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
#if     qDebug
                    template      <typename  T, typename TRAITS>
                    void    Array<T, TRAITS>::Invariant_ () const
                    {
                        Assert ((_fSlotsAllocated == 0) == (_fItems == nullptr));     // always free iff slots alloced = 0
                        Assert (_fLength <= _fSlotsAllocated);
                    }
#endif
                    template      <typename  T, typename TRAITS>
                    inline  Array<T, TRAITS>::~Array ()
                    {
                        RemoveAll ();
                        delete (char*)_fItems;
                    }
                    template      <typename  T, typename TRAITS>
                    inline  T   Array<T, TRAITS>::GetAt (size_t i) const
                    {
                        Require (i >= 0);
                        Require (i < _fLength);
                        return _fItems [i];
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    Array<T, TRAITS>::SetAt (size_t i, T item)
                    {
                        Require (i >= 0);
                        Require (i < _fLength);
                        _fItems [i] = item;
                    }
                    template      <typename  T, typename TRAITS>
                    inline  T&  Array<T, TRAITS>::operator[] (size_t i)
                    {
                        Require (i >= 0);
                        Require (i < _fLength);
                        return _fItems [i];
                    }
                    template      <typename  T, typename TRAITS>
                    inline  T   Array<T, TRAITS>::operator[] (size_t i) const
                    {
                        Require (i >= 0);
                        Require (i < _fLength);
                        return _fItems [i];
                    }
                    template      <typename  T, typename TRAITS>
                    inline  size_t  Array<T, TRAITS>::GetLength () const
                    {
                        return _fLength;
                    }
                    template      <typename  T, typename TRAITS>
                    inline  size_t  Array<T, TRAITS>::GetCapacity () const
                    {
                        return _fSlotsAllocated;
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    Array<T, TRAITS>::Compact ()
                    {
                        SetCapacity (GetLength ());
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    Array<T, TRAITS>::RemoveAt (const ForwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        this->RemoveAt (i.CurrentIndex ());
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    Array<T, TRAITS>::RemoveAt (const BackwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        this->RemoveAt (i.CurrentIndex ());
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    Array<T, TRAITS>::SetAt (const ForwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        SetAt (i.CurrentIndex (), newValue);
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    Array<T, TRAITS>::SetAt (const BackwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        SetAt (i.CurrentIndex (), newValue);
                    }
                    template      <typename  T, typename TRAITS>
                    void    Array<T, TRAITS>::AddBefore (const ForwardIterator& i, T newValue)
                    {
                        // i CAN BE DONE OR NOT
                        InsertAt (i.CurrentIndex (), newValue);
                    }
                    template      <typename  T, typename TRAITS>
                    void    Array<T, TRAITS>::AddBefore (const BackwardIterator& i, T newValue)
                    {
                        // i CAN BE DONE OR NOT
                        InsertAt (i.CurrentIndex (), newValue);
                    }
                    template      <typename  T, typename TRAITS>
                    void    Array<T, TRAITS>::AddAfter (const ForwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        InsertAt (i.CurrentIndex () + 1, newValue);
                    }
                    template      <typename  T, typename TRAITS>
                    void    Array<T, TRAITS>::AddAfter (const BackwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        InsertAt (i.CurrentIndex () + 1, newValue);
                    }


                    /*
                    ********************************************************************************
                    ***************** Array<T,TRAITS>::_ArrayIteratorBase **************************
                    ********************************************************************************
                    */
#if     qDebug
                    template      <typename  T, typename TRAITS>
                    void    Array<T, TRAITS>::_ArrayIteratorBase::Invariant_ () const
                    {
                        AssertNotNull (_fData);
                        Assert (_fStart == _fData->_fItems);
                        Assert (size_t (_fEnd - _fStart) == _fData->GetLength ());
                        Assert ((_fCurrent >= _fStart) and (_fCurrent <= _fEnd));   // ANSI C requires this is always TRUE
                    }
#endif
                    template      <typename  T, typename TRAITS>
                    inline  void    Array<T, TRAITS>::_ArrayIteratorBase::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template      <typename  T, typename TRAITS>
                    inline  Array<T, TRAITS>::_ArrayIteratorBase::_ArrayIteratorBase (const Array<T, TRAITS>& data) :
#if     qDebug
                        _fData (&data),
#endif
                        _fStart (&data._fItems[0]),
                        _fEnd (&data._fItems[data.GetLength ()]),
                        //_fCurrent ()                           dont initialize - done in subclasses...
                        _fSuppressMore (true)                // first time thru - cuz of how used in for loops...
                    {
#if     qDebug
                        _fCurrent = nullptr; // more likely to cause bugs...
#endif
                        /*
                         * Cannot call invariant () here since _fCurrent not yet setup.
                         */
                    }
                    template      <typename  T, typename TRAITS>
                    bool    Array<T, TRAITS>::_ArrayIteratorBase::More (T* current, bool advance)
                    {
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

                        return (false);
                    }
                    template      <typename  T, typename TRAITS>
                    inline  bool    Array<T, TRAITS>::_ArrayIteratorBase::Done () const
                    {
                        Invariant ();
                        return bool (_fCurrent == _fEnd);
                    }
                    template      <typename  T, typename TRAITS>
                    inline  size_t  Array<T, TRAITS>::_ArrayIteratorBase::CurrentIndex () const
                    {
                        /*
                         * NB: This can be called if we are done - if so, it returns GetLength().
                         */
                        Invariant ();
                        return _fCurrent - _fStart;
                    }
                    template      <typename  T, typename TRAITS>
                    inline  T       Array<T, TRAITS>::_ArrayIteratorBase::Current () const
                    {
                        Invariant ();
						EnsureNotNull (_fCurrent);
                        return *_fCurrent;
                    }


                    /*
                    ********************************************************************************
                    *********************** Array<T,TRAITS>::ForwardIterator ***********************
                    ********************************************************************************
                    */
                    template      <typename  T, typename TRAITS>
                    inline  Array<T, TRAITS>::ForwardIterator::ForwardIterator (const Array<T, TRAITS>& data)
                        : inherited (data)
                    {
                        this->_fCurrent = this->_fStart;
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    inline  bool    Array<T, TRAITS>::ForwardIterator::More (T* current, bool advance)
                    {
                        Invariant ();
                        if (advance) {
                            if (not this->_fSuppressMore and not this->Done ()) {
                                Assert (this->_fCurrent < this->_fEnd);
                                this->_fCurrent++;
                            }
                        }
                        return (inherited::More (current, advance));
                    }


                    /*
                    ********************************************************************************
                    ********************** Array<T,TRAITS>::BackwardIterator ***********************
                    ********************************************************************************
                    */
                    template      <typename  T, typename TRAITS>
                    inline  Array<T, TRAITS>::BackwardIterator::BackwardIterator (const Array<T, TRAITS>& data)
                        : inherited (data)
                    {
                        if (data.GetLength () == 0) {
                            this->_fCurrent = this->_fEnd;    // magic to indicate done
                        }
                        else {
                            this->_fCurrent = this->_fEnd - 1; // last valid item
                        }
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    inline  bool    Array<T, TRAITS>::BackwardIterator::More (T* current, bool advance)
                    {
                        Invariant ();
                        if (advance) {
                            if (not this->_fSuppressMore and not this->Done ()) {
                                if (this->_fCurrent == this->_fStart) {
                                    this->_fCurrent = this->_fEnd;    // magic to indicate done
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


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_DataStructures_Array_inl_ */
