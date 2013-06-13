/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_inl_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_inl_   1


#include    "../../../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   PatchingDataStructures {



                    //// GET RID OF MUTATOR CODE SOON!!!
                    /*
                     *      ForwardArrayMutator_Patch<T> is the same as ForwardIterator<T> but
                     *  adds the ability to update the contents of the array as you go along.
                     */
                    template    <typename T>
                    class  Array_Patch<T>::ForwardArrayMutator_Patch : public ForwardIterator {
                    private:
                        typedef ForwardIterator   inherited;

                    public:
                        ForwardArrayMutator_Patch (Array_Patch<T>& data);

                    public:
                        nonvirtual  void    RemoveCurrent ();
                        nonvirtual  void    UpdateCurrent (T newValue);
                        nonvirtual  void    AddBefore (T item);             //  NB: Can be called if done
                        nonvirtual  void    AddAfter (T item);
                    };

                    /*
                     *      BackwardArrayMutator_Patch<T> is the same as BackwardIterator<T> but
                     *  adds the ability to update the contents of the array as you go along.
                     */
                    template    <typename T>
                    class  Array_Patch<T>::BackwardArrayMutator_Patch : public Array_Patch<T>::BackwardIterator {
                    private:
                        typedef typename Array_Patch<T>::BackwardIterator    inherited;

                    public:
                        BackwardArrayMutator_Patch (Array_Patch<T>& data);

                    public:
                        nonvirtual  void    RemoveCurrent ();
                        nonvirtual  void    UpdateCurrent (T newValue);
                        nonvirtual  void    AddBefore (T item);
                        nonvirtual  void    AddAfter (T item);              //  NB: Can be called if done
                    };




                    /*
                    ********************************************************************************
                    ******************** Array_Patch<T>::_ArrayIteratorBase ************************
                    ********************************************************************************
                    */
                    template    <typename T>
                    inline  Array_Patch<T>::_ArrayIteratorBase::_ArrayIteratorBase (const Array_Patch<T>& data)
                        : inherited (data)
                        , fData (&data)
                        , fNext (data.fIterators)
                    {
                        const_cast <Array_Patch<T>*> (fData)->fIterators = this;
                        /*
                         * Cannot call invariant () here since fCurrent not yet setup.
                         */
                    }
                    template    <typename T>
                    inline  Array_Patch<T>::_ArrayIteratorBase::_ArrayIteratorBase (const typename Array_Patch<T>::_ArrayIteratorBase& from)
                        : inherited (from)
                        , fData (from.fData)
                        , fNext (from.fData->fIterators)
                    {
                        RequireNotNull (fData);
                        const_cast <Array_Patch<T>*> (fData)->fIterators = this;
                        Invariant ();
                    }
                    template    <typename T>
                    inline  Array_Patch<T>::_ArrayIteratorBase::~_ArrayIteratorBase ()
                    {
                        Invariant ();
                        AssertNotNull (fData);
                        if (fData->fIterators == this) {
                            const_cast <Array_Patch<T>*> (fData)->fIterators = fNext;
                        }
                        else {
                            _ArrayIteratorBase* v = fData->fIterators;
                            for (; v->fNext != this; v = v->fNext) {
                                AssertNotNull (v);
                                AssertNotNull (v->fNext);
                            }
                            AssertNotNull (v);
                            Assert (v->fNext == this);
                            v->fNext = fNext;
                        }
                    }
                    template    <typename T>
                    inline  typename Array_Patch<T>::_ArrayIteratorBase& Array_Patch<T>::_ArrayIteratorBase::operator= (const typename Array_Patch<T>::_ArrayIteratorBase& rhs)
                    {
                        Invariant ();

                        /*
                         *      If the fData field has not changed, then we can leave alone our iterator linkage.
                         *  Otherwise, we must remove ourselves from the old, and add ourselves to the new.
                         */
                        if (fData != rhs.fData) {
                            AssertNotNull (fData);
                            AssertNotNull (rhs.fData);

                            /*
                             * Remove from old.
                             */
                            if (fData->fIterators == this) {
                                //(~const)
                                ((Array_Patch<T>*)fData)->fIterators = fNext;
                            }
                            else {
                                _ArrayIteratorBase* v = fData->fIterators;
                                for (; v->fNext != this; v = v->fNext) {
                                    AssertNotNull (v);
                                    AssertNotNull (v->fNext);
                                }
                                AssertNotNull (v);
                                Assert (v->fNext == this);
                                v->fNext = fNext;
                            }

                            /*
                             * Add to new.
                             */
                            fData = rhs.fData;
                            fNext = rhs.fData->fIterators;
                            //(~const)
                            ((Array_Patch<T>*)fData)->fIterators = this;
                        }

                        inherited::operator=(rhs);

                        Invariant ();

                        return (*this);
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::_ArrayIteratorBase::Invariant () const
                    {
                        inherited::Invariant ();
                    }
                    template    <typename T>
                    inline  size_t  Array_Patch<T>::_ArrayIteratorBase::CurrentIndex () const
                    {
                        return (inherited::CurrentIndex ());
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::_ArrayIteratorBase::PatchAdd (size_t index)
                    {
                        /*
                         *      NB: We cannot call invariants here because this is called after the add
                         *  and the PatchRealloc has not yet happened.
                         */
                        Require (index >= 0);

                        this->fEnd++;

                        AssertNotNull (fData);

                        /*
                         *      If we added an item to the right of our cursor, it has no effect
                         *  on our - by index - addressing, and so ignore it. We will eventually
                         *  reach that new item.
                         *
                         *      On the other hand, if we add the item to the left of our cursor,
                         *  things are more complex:
                         *
                         *      If we added an item left of the cursor, then we are now pointing to
                         *  the item before the one we used to, and so incrementing (ie Next)
                         *  would cause us to revisit (in the forwards case, or skip one in the
                         *  reverse case). To correct our index, we must increment it so that
                         *  it.Current () refers to the same entity.
                         *
                         *      Note that this should indeed by <=, since (as opposed to <) since
                         *  if we are a direct hit, and someone tries to insert something at
                         *  the position we are at, the same argument as before applies - we
                         *  would be revisiting, or skipping forwards an item.
                         */

                        Require ((this->fEnd >= this->fStart) and (index <= size_t (this->fEnd - this->fStart)));
                        if (&this->fStart[index] <= this->fCurrent) {       // index <= CurrentIndex () - only faster
                            // Cannot call CurrentIndex () since invariants
                            // might fail at this point
                            this->fCurrent++;
                        }
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::_ArrayIteratorBase::PatchRemove (size_t index)
                    {
                        Require (index >= 0);
                        Require (index < fData->GetLength ());

                        /*
                         *      If we are removing an item from the right of our cursor, it has no effect
                         *  on our - by index - addressing, and so ignore it.
                         *
                         *      On the other hand, if we are removing the item from the left of our cursor,
                         *  things are more complex:
                         *
                         *      If we are removing an item from the left of the cursor, then we are now
                         *  pointing to the item after the one we used to, and so decrementing (ie Next)
                         *  would cause us to skip one. To correct our index, we must decrement it so that
                         *  it.Current () refers to the same entity.
                         *
                         *      In the case where we are directly hit, just set fSuppressMore
                         *  to true. If we are going forwards, are are already pointing where
                         *  we should be (and this works for repeat deletions). If we are
                         *  going backwards, then fSuppressMore will be ignored, but for the
                         *  sake of code sharing, its tough to do much about that waste.
                         */
                        Assert ((&this->fStart[index] <= this->fCurrent) == (index <= CurrentIndex ()));
                        if (&this->fStart[index] < this->fCurrent) {
                            Assert (CurrentIndex () >= 1);
                            this->fCurrent--;
                        }
                        else if (&this->fStart[index] == this->fCurrent) {
                            PatchRemoveCurrent ();
                        }
                        // Decrement at the end since CurrentIndex () calls stuff that asserts (fEnd-fStart) == fData->GetLength ()
                        Assert (size_t (this->fEnd - this->fStart) == fData->GetLength ());     //  since called before remove

                        /*
                         * At this point, fCurrent could be == fEnd - must not lest fCurrent point past!
                         */
                        if (this->fCurrent == this->fEnd) {
                            Assert (this->fCurrent > this->fStart); // since we are removing something start!=end
                            this->fCurrent--;
                        }
                        this->fEnd--;
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::_ArrayIteratorBase::PatchRemoveAll ()
                    {
                        Require (fData->GetLength () == 0);     //  since called after removeall

                        this->fCurrent = fData->_fItems;
                        this->fStart = fData->_fItems;
                        this->fEnd = fData->_fItems;
                        this->fSuppressMore = true;
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::_ArrayIteratorBase::PatchRealloc ()
                    {
                        /*
                         *      NB: We can only call invariant after we've fixed things up, since realloc
                         * has happened by now, but things don't point to the right places yet.
                         */
                        if (this->fStart != fData->_fItems) {
                            const   T*   oldStart    =   this->fStart;
                            this->fStart = fData->_fItems;
                            this->fCurrent = fData->_fItems + (this->fCurrent - oldStart);
                            this->fEnd = fData->_fItems + (this->fEnd - oldStart);
                        }
                    }
#if     qDebug
                    template    <typename T>
                    void    Array_Patch<T>::_ArrayIteratorBase::Invariant_ () const
                    {
                        inherited::Invariant_ ();
                        Assert (fData == inherited::fData);
                    }
#endif  /*qDebug*/


                    /*
                    ********************************************************************************
                    ******************************* Array_Patch<T> *********************************
                    ********************************************************************************
                    */
                    template    <typename T>
                    inline  void    Array_Patch<T>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
                        InvariantOnIterators_ ();
#endif
                    }
                    template    <typename T>
                    inline  Array_Patch<T>::Array_Patch ()
                        : inherited ()
                        , fIterators (0)
                    {
                        Invariant ();
                    }
                    template    <typename T>
                    inline  Array_Patch<T>::Array_Patch (const Array_Patch<T>& from) :
                        inherited (from),
                        fIterators (0)  // Don't copy the list of iterators - would be trouble with backpointers!
                        // Could clone but that would do no good, since nobody else would have pointers to them
                    {
                        Invariant ();
                    }
                    template    <typename T>
                    inline  Array_Patch<T>::~Array_Patch ()
                    {
                        Require (fIterators == 0);
                        Invariant ();
                    }
                    template    <typename T>
                    inline  bool    Array_Patch<T>::HasActiveIterators () const
                    {
                        return bool (fIterators != 0);
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::PatchViewsAdd (size_t index) const
                    {
                        /*
                         *      Must call PatchRealloc before PatchAdd() since the test of currentIndex
                         *  depends on things being properly adjusted.
                         */
                        for (_ArrayIteratorBase* v = fIterators; v != 0; v = v->fNext) {
                            v->PatchRealloc ();
                            v->PatchAdd (index);
                        }
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::PatchViewsRemove (size_t index) const
                    {
                        for (_ArrayIteratorBase* v = fIterators; v != 0; v = v->fNext) {
                            v->PatchRemove (index);
                        }
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::PatchViewsRemoveAll () const
                    {
                        for (_ArrayIteratorBase* v = fIterators; v != 0; v = v->fNext) {
                            v->PatchRemoveAll ();
                        }
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::PatchViewsRealloc () const
                    {
                        for (_ArrayIteratorBase* v = fIterators; v != 0; v = v->fNext) {
                            v->PatchRealloc ();
                        }
                    }
                    template    <typename T>
                    inline  Array_Patch<T>& Array_Patch<T>::operator= (const Array_Patch<T>& rhs)
                    {
                        /*
                         * Don't copy the rhs iterators, and don't do assignments when we have active iterators.
                         * If this is to be supported at some future date, well need to work on our patching.
                         */
                        Assert (not (HasActiveIterators ()));   // cuz copy of array does not copy iterators...
                        Invariant ();
                        inherited::operator= (rhs);
                        Invariant ();
                        return (*this);
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::SetLength (size_t newLength, T fillValue)
                    {
                        // For now, not sure how to patch the iterators, so just Assert out - fix later ...
                        AssertNotReached ();
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::InsertAt (T item, size_t index)
                    {
                        Invariant ();
                        inherited::InsertAt (item, index);
                        PatchViewsAdd (index);          // PatchRealloc done in PatchViewsAdd()
                        Invariant ();
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::RemoveAt (size_t index)
                    {
                        Invariant ();
                        PatchViewsRemove (index);
                        inherited::RemoveAt (index);
                        // Dont call PatchViewsRealloc () since removeat does not do a SetCapacity, it
                        // just destructs things.
                        Invariant ();
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::RemoveAll ()
                    {
                        Invariant ();
                        inherited::RemoveAll ();
                        PatchViewsRemoveAll ();     // PatchRealloc not needed cuz removeall just destructs things,
                        // it does not realloc pointers (ie does not call SetCapacity).
                        Invariant ();
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::SetCapacity (size_t slotsAlloced)
                    {
                        Invariant ();
                        inherited::SetCapacity (slotsAlloced);
                        PatchViewsRealloc ();
                        Invariant ();
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::Compact ()
                    {
                        Invariant ();
                        inherited::Compact ();
                        PatchViewsRealloc ();
                        Invariant ();
                    }
#if     qDebug
                    template    <typename T>
                    void    Array_Patch<T>::Invariant_ () const
                    {
                        inherited::Invariant_ ();

                        /*
                         *      Be sure each iterator points back to us. Thats about all we can test from
                         *  here since we cannot call each iterators Invariant(). That would be
                         *  nice, but sadly when this Invariant_ () is called from Array<T> the
                         *  iterators themselves may not have been patched, so they'll be out of
                         *  date. Instead, so that in local shadow of Invariant() done in Array_Patch<T>
                         *  so only called when WE call Invariant().
                         */
                        for (_ArrayIteratorBase* v = fIterators; v != nullptr; v = v->fNext) {
                            Assert (v->fData == this);
                        }
                    }
                    template    <typename T>
                    void    Array_Patch<T>::InvariantOnIterators_ () const
                    {
                        /*
                         *      Only here can we iterate over each iterator and calls its Invariant()
                         *  since now we've completed any needed patching.
                         */
                        for (_ArrayIteratorBase* v = fIterators; v != nullptr; v = v->fNext) {
                            Assert (v->fData == this);
                            v->Invariant ();
                        }
                    }
#endif  /*qDebug*/


                    /*
                    ********************************************************************************
                    **************** Array_Patch<T>::ForwardIterator ********************
                    ********************************************************************************
                    */
                    template    <typename T>
                    inline  Array_Patch<T>::ForwardIterator::ForwardIterator (const Array_Patch<T>& data) :
                        inherited (data)
                    {
                        this->fCurrent = this->fStart;
                        this->Invariant ();
                    }
                    template    <typename T>
                    inline  bool    Array_Patch<T>::ForwardIterator::More (T* current, bool advance)
                    {
                        this->Invariant ();
                        if (advance) {
                            if (not this->fSuppressMore and not this->Done ()) {
                                Assert ( this->fCurrent <  this->fEnd);
                                this->fCurrent++;
                            }

                            this->fSuppressMore = false;
                            if ((current != nullptr) and (not this->Done ())) {
                                *current = (*this->fCurrent);
                            }
                        }
                        this->Invariant ();
                        return (not this->Done ());
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::ForwardIterator::PatchRemoveCurrent ()
                    {
                        Assert ( this->fCurrent <  this->fEnd); // cannot remove something past the end
                        this->fSuppressMore = true;
                    }


                    /*
                    ********************************************************************************
                    ************************* ForwardArrayMutator_Patch<T> *************************
                    ********************************************************************************
                    */
                    template    <typename T>
                    inline  Array_Patch<T>::ForwardArrayMutator_Patch::ForwardArrayMutator_Patch (Array_Patch<T>& data) :
                        inherited ((const Array_Patch<T>&)data)
                    {
                        this->Invariant ();
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::ForwardArrayMutator_Patch::RemoveCurrent ()
                    {
                        this->Invariant ();
                        AssertNotNull (this->fData);
                        const_cast<Array_Patch<T>*> (this->fData)->RemoveAt (this->CurrentIndex ());
                        this->Invariant ();
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::ForwardArrayMutator_Patch::UpdateCurrent (T newValue)
                    {
                        this->Invariant ();
                        Require (not this->Done ());
                        AssertNotNull (this->fCurrent);
                        *const_cast<T*>(this->fCurrent) = newValue;
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::ForwardArrayMutator_Patch::AddBefore (T newValue)
                    {
                        /*
                         * NB: This can be called if we are done.
                         */
                        this->Invariant ();
                        AssertNotNull (this->fData);
                        const_cast<Array_Patch<T>*> (this->fData)->InsertAt (newValue, this->CurrentIndex ());
                        this->Invariant ();
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::ForwardArrayMutator_Patch::AddAfter (T newValue)
                    {
                        Require (not this->Done ());
                        this->Invariant ();
                        AssertNotNull (this->fData);
                        const_cast<Array_Patch<T>*> (this->fData)->InsertAt (newValue, this->CurrentIndex () + 1);
                        this->Invariant ();
                    }


                    /*
                    ********************************************************************************
                    ********************* Array_Patch<T>::BackwardIterator *************************
                    ********************************************************************************
                    */
                    template    <typename T>
                    inline  Array_Patch<T>::BackwardIterator::BackwardIterator (const Array_Patch<T>& data)
                        : inherited (data)
                    {
                        if (data.GetLength () == 0) {
                            this->fCurrent = this->fEnd;    // magic to indicate done
                        }
                        else {
                            this->fCurrent = this->fEnd - 1; // last valid item
                        }
                        this->Invariant ();
                    }
                    // Careful to keep hdr and src copies identical...
                    template    <typename T>
                    inline  bool    Array_Patch<T>::BackwardIterator::More (T* current, bool advance)
                    {
                        this->Invariant ();
                        if (advance) {
                            if (this->fSuppressMore) {
                                this->fSuppressMore = false;
                                if (not this->Done ()) {
                                    *current = *(this->fCurrent);
                                }
                                return (not this->Done ());
                            }
                            else {
                                if (this->Done ()) {
                                    return (false);
                                }
                                else {
                                    if (this->fCurrent == this->fStart) {
                                        this->fCurrent = this->fEnd;    // magic to indicate done
                                        Ensure (this->Done ());
                                        return (false);
                                    }
                                    else {
                                        this->fCurrent--;
                                        *current = *(this->fCurrent);
                                        Ensure (not this->Done ());
                                        return (true);
                                    }
                                }
                            }
                        }
                        return (not this->Done ());
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::BackwardIterator::PatchRemoveCurrent ()
                    {
                        if (this->fCurrent == this->fStart) {
                            this->fCurrent = this->fEnd;    // magic to indicate done
                        }
                        else {
                            Assert (this->fCurrent > this->fStart);
                            this->fCurrent--;
                        }
                        this->fSuppressMore = true;
                    }


                    /*
                    ********************************************************************************
                    ************************ BackwardArrayMutator_Patch<T> *************************
                    ********************************************************************************
                    */
                    template    <typename T>
                    inline  Array_Patch<T>::BackwardArrayMutator_Patch::BackwardArrayMutator_Patch (Array_Patch<T>& data) :
                        inherited ((const Array_Patch<T>&)data)
                    {
                        this->Invariant ();
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::BackwardArrayMutator_Patch::RemoveCurrent ()
                    {
                        this->Invariant ();
                        AssertNotNull (this->fData);
                        const_cast<Array_Patch<T>*> (this->fData)->RemoveAt (this->CurrentIndex ());
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::BackwardArrayMutator_Patch::UpdateCurrent (T newValue)
                    {
                        this->Invariant ();
                        Require (not this->Done ());
                        AssertNotNull (this->fCurrent);
                        *const_cast<T*>(this->fCurrent) = newValue;
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::BackwardArrayMutator_Patch::AddBefore (T newValue)
                    {
                        Require (not this->Done ());
                        this->Invariant ();
                        AssertNotNull (this->fData);
                        const_cast<Array_Patch<T>*> (this->fData)->InsertAt (newValue, this->CurrentIndex ());
                        this->Invariant ();
                    }
                    template    <typename T>
                    inline  void    Array_Patch<T>::BackwardArrayMutator_Patch::AddAfter (T newValue)
                    {
                        /*
                         * NB: This can be called if we are done.
                         */
                        this->Invariant ();
                        AssertNotNull (this->fData);
                        const_cast<Array_Patch<T>*>(this->fData)->InsertAt (newValue, this->CurrentIndex () + 1);
                        this->Invariant ();
                    }


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_inl_ */
