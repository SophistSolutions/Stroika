/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_Array_inl_
#define	_Stroika_Foundation_Containers_Array_inl_	1


#include "../../Debug/Assertions.h"



namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {
                using namespace Private;

                template	<class	T>	inline	ArrayNode<T>::ArrayNode (T item) :
                    fItem (item)
                {
                }

                template	<class	T>	inline	ArrayNode<T>::~ArrayNode ()
                {
                }


                // class	Array<T>
                template	<class	T>	inline	void	Array<T>::Invariant () const
                {
            #if		qDebug
                    Invariant_ ();
            #endif
                }

                template	<class	T>	inline	Array<T>::Array () :
                    fLength (0),
                    fSlotsAllocated (0),
                    fItems (0)
                {
                }

                template	<class	T>	inline	Array<T>::~Array ()
                {
                    RemoveAll ();
                    delete (char*)fItems;
                }

                template	<class	T>	inline	T	Array<T>::GetAt (size_t i) const
                {
                    Require (i >= 0);
                    Require (i < fLength);
                    return (fItems [i].fItem);
                }

                template	<class	T>	inline	void	Array<T>::SetAt (T item, size_t i)
                {
                    Require (i >= 0);
                    Require (i < fLength);
                    fItems [i].fItem = item;
                }

                template	<class	T>	inline	T&	Array<T>::operator[] (size_t i)
                {
                    Require (i >= 0);
                    Require (i < fLength);
                    return (fItems [i].fItem);
                }

                template	<class	T>	inline	T	Array<T>::operator[] (size_t i) const
                {
                    Require (i >= 0);
                    Require (i < fLength);
                    return (fItems [i].fItem);
                }

                template	<class	T>	inline	size_t	Array<T>::GetLength () const
                {
                    return (fLength);
                }

                template	<class	T>	inline	size_t	Array<T>::GetSlotsAlloced () const
                {
                    return (fSlotsAllocated);
                }

                template	<class	T>	inline	void	Array<T>::Compact ()
                {
                    SetSlotsAlloced (GetLength ());
                }


                // Class ArrayIteratorBase<T>
                template	<typename T>	inline	void	ArrayIteratorBase<T>::Invariant () const
                {
            #if		qDebug
                    Invariant_ ();
            #endif
                }
                template	<typename T>	inline	ArrayIteratorBase<T>::ArrayIteratorBase (const Array<T>& data) :
            #if		qDebug
                    fData (&data),
            #endif
                    fStart (&data.fItems[0]),
                    fEnd (&data.fItems[data.GetLength ()]),
                    //fCurrent ()							dont initialize - done in subclasses...
                    fSuppressMore (true)				// first time thru - cuz of how used in for loops...
                {
                    #if		qDebug
                        fCurrent = nullptr;	// more likely to cause bugs...(leave the xtra newline cuz of genclass bug...)
                    #endif
                    /*
                     * Cannot call invariant () here since fCurrent not yet setup.
                     */
                }
                template	<typename T>	bool	ArrayIteratorBase<T>::More (T* current, bool advance)
                {
                	if (advance) {
						this->fSuppressMore = false;
                	}
                    Invariant ();
                    if (not Done ()) {
                    	if (current != nullptr) {
							*current = fCurrent->fItem;
                    	}
                        return true;
                    }

                    return (false);
                }
                template	<typename T>	inline	bool	ArrayIteratorBase<T>::Done () const
                {
                    Invariant ();
                    return bool (fCurrent == fEnd);
                }

                template	<typename T>	inline	size_t	ArrayIteratorBase<T>::CurrentIndex () const
                {
                    /*
                     * NB: This can be called if we are done - if so, it returns GetLength().
                     */
                    Invariant ();
                    return ((fCurrent-fStart));
                }

                template	<typename T>	inline	T		ArrayIteratorBase<T>::Current () const
                {
                    Ensure (fData->GetAt (CurrentIndex ()) == fCurrent->fItem);
                    Invariant ();
                    return (fCurrent->fItem);
                }


                // Class ForwardArrayIterator<T>
                template	<typename T>	inline	ForwardArrayIterator<T>::ForwardArrayIterator (const Array<T>& data) :
                    ArrayIteratorBase<T>(data)
                {
                    this->fCurrent = this->fStart;
                    Invariant ();
                }

                template	<typename T>	inline	bool	ForwardArrayIterator<T>::More (T* current, bool advance)
                {
                    Invariant ();
                    if (advance) {
						if (not this->fSuppressMore and not Done ()) {
							Assert (this->fCurrent < this->fEnd);
							this->fCurrent++;
						}
                    }

                    return (inherited::More (current, advance));
                }

                template	<typename T>	inline	bool	ForwardArrayIterator<T>::Done () const
                {
                    return (inherited::Done ());
                }

                template	<typename T>	inline	void	ForwardArrayIterator<T>::Invariant () const
                {
                    inherited::Invariant ();
                }

                // Class ForwardArrayMutator<T>
                template	<typename T>	inline	ForwardArrayMutator<T>::ForwardArrayMutator (Array<T>& data) :
                    ForwardArrayIterator<T>((const Array<T>&)data)
                {
                    Invariant ();
                }

                template	<typename T>	inline	void	ForwardArrayMutator<T>::UpdateCurrent (T newValue)
                {
                    Invariant ();
                    Require (not Done ());
                    AssertNotNull (this->fCurrent);
                    const_cast<ArrayNode<T>*> (this->fCurrent)->fItem = newValue;
                }

                template	<typename T>	inline	bool	ForwardArrayMutator<T>::More (T* current, bool advance)
                {
                    return (inherited::More (current, advance));
                }

                template	<typename T>	inline	bool	ForwardArrayMutator<T>::Done () const
                {
                    return (inherited::Done ());
                }

                template	<typename T>	inline	void	ForwardArrayMutator<T>::Invariant () const
                {
                    inherited::Invariant ();
                }

                // Class BackwardArrayIterator<T>
                template	<typename T>	inline	BackwardArrayIterator<T>::BackwardArrayIterator (const Array<T>& data) :
                    ArrayIteratorBase<T>(data)
                {
                    if (data.GetLength () == 0) {
                        this->fCurrent = this->fEnd;	// magic to indicate done
                    }
                    else {
                        this->fCurrent = this->fEnd-1;	// last valid item
                    }
                    Invariant ();
                }

                template	<typename T>	inline	bool	BackwardArrayIterator<T>::More (T* current, bool advance)
                {
                    Invariant ();
                    if (advance) {
						if (not this->fSuppressMore and not Done ()) {
							if (this->fCurrent == this->fStart) {
								this->fCurrent = this->fEnd;	// magic to indicate done
								Ensure (Done ());
							}
							else {
								this->fCurrent--;
								Ensure (not Done ());
							}
						}
                    }
                    return (inherited::More (current, advance));
                }

                template	<typename T>	inline	bool	BackwardArrayIterator<T>::Done () const
                {
                    return (inherited::Done ());
                }

                template	<typename T>	inline	void	BackwardArrayIterator<T>::Invariant () const
                {
                    inherited::Invariant ();
                }

                // Class BackwardArrayMutator<T>
                template	<typename T>	inline	BackwardArrayMutator<T>::BackwardArrayMutator (Array<T>& data) :
                    BackwardArrayIterator<T>((const Array<T>&)data)
                {
                    Invariant ();
                }
                template	<typename T>	inline	void	BackwardArrayMutator<T>::UpdateCurrent (T newValue)
                {
                    Invariant ();
                    Require (not Done ());
                    AssertNotNull (this->fCurrent);
                    const_cast<ArrayNode<T>*> (this->fCurrent)->fItem = newValue;	// not sure how to handle better the (~const)
                }

                template	<typename T>	inline	bool	BackwardArrayMutator<T>::More (T* current, bool advance)
                {
                    return (inherited::More (current, advance));
                }

                template	<typename T>	inline	bool	BackwardArrayMutator<T>::Done () const
                {
                    return (inherited::Done ());
                }

                template	<typename T>	inline	void	BackwardArrayMutator<T>::Invariant () const
                {
                    inherited::Invariant ();
                }

                /*
                 **************************** Patching code ****************************
                 */

                // Class ArrayIterator_PatchBase<T>
                template	<typename T>	inline	ArrayIterator_PatchBase<T>::ArrayIterator_PatchBase (const Array_Patch<T>& data) :
                    ArrayIteratorBase<T>(data),
                    fData (&data),
                    fNext (data.fIterators)
                {
                    const_cast <Array_Patch<T>*> (fData)->fIterators = this;

                    /*
                     * Cannot call invariant () here since fCurrent not yet setup.
                     */
                }

                template	<typename T>	inline	ArrayIterator_PatchBase<T>::ArrayIterator_PatchBase (const ArrayIterator_PatchBase<T>& from) :
                    ArrayIteratorBase<T>(from),
                    fData (from.fData),
                    fNext (from.fData->fIterators)
                {
                    RequireNotNull (fData);
                    const_cast <Array_Patch<T>*> (fData)->fIterators = this;
                    Invariant ();
                }

                template	<typename T>	inline	ArrayIterator_PatchBase<T>::~ArrayIterator_PatchBase ()
                {
                    Invariant ();
                    AssertNotNull (fData);
                    if (fData->fIterators == this) {
                        const_cast <Array_Patch<T>*> (fData)->fIterators = fNext;
                    }
                    else {
                        ArrayIterator_PatchBase<T>* v = fData->fIterators;
                        for (; v->fNext != this; v = v->fNext) {
                            AssertNotNull (v);
                            AssertNotNull (v->fNext);
                        }
                        AssertNotNull (v);
                        Assert (v->fNext == this);
                        v->fNext = fNext;
                    }
                }

                template	<typename T>	inline	ArrayIterator_PatchBase<T>&	ArrayIterator_PatchBase<T>::operator= (const ArrayIterator_PatchBase<T>& rhs)
                {
                    Invariant ();

                    /*
                     *		If the fData field has not changed, then we can leave alone our iterator linkage.
                     *	Otherwise, we must remove ourselves from the old, and add ourselves to the new.
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
                            ArrayIterator_PatchBase<T>* v = fData->fIterators;
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


                    ArrayIteratorBase<T>::operator=(rhs);


                    Invariant ();

                    return (*this);
                }

                template	<typename T>	inline	void	ArrayIterator_PatchBase<T>::Invariant () const
                {
                    inherited::Invariant ();
                }

                template	<typename T>	inline  size_t	ArrayIterator_PatchBase<T>::CurrentIndex () const
                {
                    return (inherited::CurrentIndex ());
                }

                template	<typename T>	inline	void	ArrayIterator_PatchBase<T>::PatchAdd (size_t index)
                {
                    /*
                     *		NB: We cannot call invariants here because this is called after the add
                     *	and the PatchRealloc has not yet happened.
                     */
                    Require (index >= 0);

                    this->fEnd++;

                    AssertNotNull (fData);

                    /*
                     *		If we added an item to the right of our cursor, it has no effect
                     *	on our - by index - addressing, and so ignore it. We will eventually
                     *	reach that new item.
                     *
                     *		On the other hand, if we add the item to the left of our cursor,
                     *	things are more complex:
                     *
                     *		If we added an item left of the cursor, then we are now pointing to
                     *	the item before the one we used to, and so incrementing (ie Next)
                     *	would cause us to revisit (in the forwards case, or skip one in the
                     *	reverse case). To correct our index, we must increment it so that
                     *	it.Current () refers to the same entity.
                     *
                     *		Note that this should indeed by <=, since (as opposed to <) since
                     *	if we are a direct hit, and someone tries to insert something at
                     *	the position we are at, the same argument as before applies - we
                     *	would be revisiting, or skipping forwards an item.
                     */

                    Require ((this->fEnd >= this->fStart) and (index <= size_t (this->fEnd-this->fStart)));
                    if (&this->fStart[index] <= this->fCurrent) {		// index <= CurrentIndex () - only faster
                                                            // Cannot call CurrentIndex () since invariants
                                                            // might fail at this point
                        this->fCurrent++;
                    }
                }

                template	<typename T>	inline	void	ArrayIterator_PatchBase<T>::PatchRemove (size_t index)
                {
                    Require (index >= 0);
                    Require (index < fData->GetLength ());

                    /*
                     *		If we are removing an item from the right of our cursor, it has no effect
                     *	on our - by index - addressing, and so ignore it.
                     *
                     *		On the other hand, if we are removing the item from the left of our cursor,
                     *	things are more complex:
                     *
                     *		If we are removing an item from the left of the cursor, then we are now
                     *	pointing to the item after the one we used to, and so decrementing (ie Next)
                     *	would cause us to skip one. To correct our index, we must decrement it so that
                     *	it.Current () refers to the same entity.
                     *
                     *		In the case where we are directly hit, just set fSuppressMore
                     *	to true. If we are going forwards, are are already pointing where
                     *	we should be (and this works for repeat deletions). If we are
                     *	going backwards, then fSuppressMore will be ignored, but for the
                     *	sake of code sharing, its tough to do much about that waste.
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
                    Assert (size_t (this->fEnd-this->fStart) == fData->GetLength ());		//	since called before remove

                    /*
                     * At this point, fCurrent could be == fEnd - must not lest fCurrent point past!
                     */
                    if (this->fCurrent == this->fEnd) {
                        Assert (this->fCurrent > this->fStart);	// since we are removing something start!=end
                        this->fCurrent--;
                    }
                    this->fEnd--;
                }
                template	<typename T>	inline	void	ArrayIterator_PatchBase<T>::PatchRemoveAll ()
                {
                    Require (fData->GetLength () == 0);		//	since called after removeall

                    this->fCurrent = fData->fItems;
                    this->fStart = fData->fItems;
                    this->fEnd = fData->fItems;
                    this->fSuppressMore = true;
                }
                template	<typename T>	inline	void	ArrayIterator_PatchBase<T>::PatchRealloc ()
                {
                    /*
                     *		NB: We can only call invariant after we've fixed things up, since realloc
                     * has happened by now, but things don't point to the right places yet.
                     */
                    if (this->fStart != fData->fItems) {
                        const	ArrayNode<T>*	oldStart	=	this->fStart;
                        this->fStart = fData->fItems;
                        this->fCurrent = fData->fItems + (this->fCurrent-oldStart);
                        this->fEnd = fData->fItems + (this->fEnd-oldStart);
                    }
                }


                // class Array_Patch<T>
                template	<class	T>	inline	void	Array_Patch<T>::Invariant () const
                {
            #if		qDebug
                    Invariant_ ();
                    InvariantOnIterators_ ();
            #endif
                }

                template	<class	T>	inline	Array_Patch<T>::Array_Patch () :
                    Array<T> (),
                    fIterators (0)
                {
                    Invariant ();
                }

                template	<class	T>	inline	Array_Patch<T>::Array_Patch (const Array_Patch<T>& from) :
                    Array<T> (from),
                    fIterators (0)	// Don't copy the list of iterators - would be trouble with backpointers!
                                        // Could clone but that would do no good, since nobody else would have pointers to them
                {
                    Invariant ();
                }

                template	<class	T>	inline	Array_Patch<T>::~Array_Patch ()
                {
                    Require (fIterators == 0);
                    Invariant ();
                }

                template	<class	T>	inline	bool	Array_Patch<T>::HasActiveIterators () const
                {
                    return bool (fIterators != 0);
                }

                template	<typename T>	inline	void	Array_Patch<T>::PatchViewsAdd (size_t index) const
                {
                    /*
                     *		Must call PatchRealloc before PatchAdd() since the test of currentIndex
                     *	depends on things being properly adjusted.
                     */
                    for (ArrayIterator_PatchBase<T>* v = fIterators; v != 0; v = v->fNext) {
                        v->PatchRealloc ();
                        v->PatchAdd (index);
                    }
                }

                template	<typename T>	inline	void	Array_Patch<T>::PatchViewsRemove (size_t index) const
                {
                    for (ArrayIterator_PatchBase<T>* v = fIterators; v != 0; v = v->fNext) {
                        v->PatchRemove (index);
                    }
                }

                template	<typename T>	inline	void	Array_Patch<T>::PatchViewsRemoveAll () const
                {
                    for (ArrayIterator_PatchBase<T>* v = fIterators; v != 0; v = v->fNext) {
                        v->PatchRemoveAll ();
                    }
                }

                template	<typename T>	inline	void	Array_Patch<T>::PatchViewsRealloc () const
                {
                    for (ArrayIterator_PatchBase<T>* v = fIterators; v != 0; v = v->fNext) {
                        v->PatchRealloc ();
                    }
                }

                template	<class	T>	inline	Array_Patch<T>& Array_Patch<T>::operator= (const Array_Patch<T>& rhs)
                {
                    /*
                     * Don't copy the rhs iterators, and don't do assignments when we have active iterators.
                     * If this is to be supported at some future date, well need to work on our patching.
                     */
                    Assert (not (HasActiveIterators ()));	// cuz copy of array does not copy iterators...
                    Invariant ();
                    Array<T>::operator= (rhs);
                    Invariant ();
                    return (*this);
                }

                template	<class	T>	inline	void	Array_Patch<T>::SetLength (size_t newLength, T fillValue)
                {
                    // For now, not sure how to patch the iterators, so just Assert out - fix later ...
                    AssertNotReached ();
                }

                template	<class	T>	inline	void	Array_Patch<T>::InsertAt (T item, size_t index)
                {
                    Invariant ();
                    Array<T>::InsertAt (item, index);
                    PatchViewsAdd (index);			// PatchRealloc done in PatchViewsAdd()
                    Invariant ();
                }

                template	<class	T>	inline	void	Array_Patch<T>::RemoveAt (size_t index)
                {
                    Invariant ();
                    PatchViewsRemove (index);
                    Array<T>::RemoveAt (index);
                    // Dont call PatchViewsRealloc () since removeat does not do a setslotsalloced, it
                    // just destructs things.
                    Invariant ();
                }

                template	<class	T>	inline	void	Array_Patch<T>::RemoveAll ()
                {
                    Invariant ();
                    Array<T>::RemoveAll ();
                    PatchViewsRemoveAll ();		// PatchRealloc not needed cuz removeall just destructs things,
                                                // it does not realloc pointers (ie does not call setslotsalloced).
                    Invariant ();
                }

                template	<class	T>	inline	void	Array_Patch<T>::SetSlotsAlloced (size_t slotsAlloced)
                {
                    Invariant ();
                    Array<T>::SetSlotsAlloced (slotsAlloced);
                    PatchViewsRealloc ();
                    Invariant ();
                }

                template	<class	T>	inline	void	Array_Patch<T>::Compact ()
                {
                    Invariant ();
                    Array<T>::Compact ();
                    PatchViewsRealloc ();
                    Invariant ();
                }



                // class ForwardArrayIterator_Patch<T>
                template	<typename T>	inline	ForwardArrayIterator_Patch<T>::ForwardArrayIterator_Patch (const Array_Patch<T>& data) :
                    ArrayIterator_PatchBase<T> (data)
                {
                    this->fCurrent = this->fStart;
                    Invariant ();
                }

                template	<typename T>	inline	bool	ForwardArrayIterator_Patch<T>::More (T* current, bool advance)
                {
                    Invariant ();
                    if (advance) {
						if (not this->fSuppressMore and not Done ()) {
							Assert ( this->fCurrent <  this->fEnd);
							 this->fCurrent++;
						}

						this->fSuppressMore = false;
						if ((current != nullptr) and (not Done ())) {
							*current = (*this->fCurrent).fItem;
						}
                    }
                    Invariant ();
                    return (not Done ());
                }

                template	<typename T>	inline	void	ForwardArrayIterator_Patch<T>::PatchRemoveCurrent ()
                {
                    Assert ( this->fCurrent <  this->fEnd);	// cannot remove something past the end
                    this->fSuppressMore = true;
                }


                template	<typename T>	inline	void	ForwardArrayIterator_Patch<T>::Invariant () const
                {
                    inherited::Invariant ();
                }

                template	<typename T>	inline	bool	ForwardArrayIterator_Patch<T>::Done () const
                {
                    return (inherited::Done ());
                }

                // Class ForwardArrayMutator<T>
                template	<typename T>	inline	ForwardArrayMutator_Patch<T>::ForwardArrayMutator_Patch (Array_Patch<T>& data) :
                    ForwardArrayIterator_Patch<T>((const Array_Patch<T>&)data)
                {
                    Invariant ();
                }

                template	<typename T>	inline	void	ForwardArrayMutator_Patch<T>::RemoveCurrent ()
                {
                    Invariant ();
                    AssertNotNull (this->fData);
                    const_cast<Array_Patch<T>*> (this->fData)->RemoveAt (CurrentIndex ());
                    Invariant ();
                }

                template	<typename T>	inline	void	ForwardArrayMutator_Patch<T>::UpdateCurrent (T newValue)
                {
                    Invariant ();
                    Require (not Done ());
                    AssertNotNull (this->fCurrent);
                    const_cast<ArrayNode<T>*>(this->fCurrent)->fItem = newValue;
                }

                template	<typename T>	inline	void	ForwardArrayMutator_Patch<T>::AddBefore (T newValue)
                {
                    /*
                     * NB: This can be called if we are done.
                     */
                    Invariant ();
                    AssertNotNull (this->fData);
                    const_cast<Array_Patch<T>*> (this->fData)->InsertAt (newValue, CurrentIndex ());
                    Invariant ();
                }

                template	<typename T>	inline	void	ForwardArrayMutator_Patch<T>::AddAfter (T newValue)
                {
                    Require (not Done ());
                    Invariant ();
                    AssertNotNull (this->fData);
                    const_cast<Array_Patch<T>*> (this->fData)->InsertAt (newValue, CurrentIndex ()+1);
                    Invariant ();
                }

                template	<typename T>	inline	void	ForwardArrayMutator_Patch<T>::Invariant () const
                {
                    inherited::Invariant ();
                }

                template	<typename T>	inline	bool	ForwardArrayMutator_Patch<T>::Done () const
                {
                    return (inherited::Done ());
                }

                template	<typename T>	inline	size_t	ForwardArrayMutator_Patch<T>::CurrentIndex () const
                {
                    return (inherited::CurrentIndex ());
                }

                // class BackwardArrayIterator_Patch<T>
                template	<typename T>	inline	BackwardArrayIterator_Patch<T>::BackwardArrayIterator_Patch (const Array_Patch<T>& data) :
                    ArrayIterator_PatchBase<T> (data)
                {
                    if (data.GetLength () == 0) {
                        this->fCurrent = this->fEnd;	// magic to indicate done
                    }
                    else {
                        this->fCurrent = this->fEnd-1;	// last valid item
                    }
                    Invariant ();
                }

                // Careful to keep hdr and src copies identical...
                template	<typename T>	inline	bool	BackwardArrayIterator_Patch<T>::More (T* current, bool advance)
                {
                    Invariant ();
                    if (advance) {
						if (this->fSuppressMore) {
							this->fSuppressMore = false;
							if (not Done ()) {
								*current = *(this->fCurrent);
							}
							return (not Done ());
						}
						else {
							if (Done ()) {
								return (false);
							}
							else {
								if (this->fCurrent == this->fStart) {
									this->fCurrent = this->fEnd;	// magic to indicate done
									Ensure (Done ());
									return (false);
								}
								else {
									this->fCurrent--;
									*current = *(this->fCurrent);
									Ensure (not Done ());
									return (true);
								}
							}
						}
                    }
                    return (not Done ());
                }

                template	<typename T>	inline	void	BackwardArrayIterator_Patch<T>::PatchRemoveCurrent ()
                {
                    if (this->fCurrent == this->fStart) {
                        this->fCurrent = this->fEnd;	// magic to indicate done
                    }
                    else {
                        Assert (this->fCurrent > this->fStart);
                        this->fCurrent--;
                    }
                    this->fSuppressMore = true;
                }


                // Class BackwardArrayMutator<T>
                template	<typename T>	inline	BackwardArrayMutator_Patch<T>::BackwardArrayMutator_Patch (Array_Patch<T>& data) :
                    BackwardArrayIterator_Patch<T>((const Array_Patch<T>&)data)
                {
                    Invariant ();
                }
                template	<typename T>	inline	void	BackwardArrayMutator_Patch<T>::RemoveCurrent ()
                {
                    Invariant ();
                    AssertNotNull (this->fData);
                    const_cast<Array_Patch<T>*> (this->fData)->RemoveAt (CurrentIndex ());
                }
                template	<typename T>	inline	void	BackwardArrayMutator_Patch<T>::UpdateCurrent (T newValue)
                {
                    Invariant ();
                    Require (not Done ());
                    AssertNotNull (this->fCurrent);
                    const_cast<ArrayNode<T>*>(this->fCurrent)->fItem = newValue;
                }
                template	<typename T>	inline	void	BackwardArrayMutator_Patch<T>::AddBefore (T newValue)
                {
                    Require (not Done ());
                    Invariant ();
                    AssertNotNull (this->fData);
                    const_cast<Array_Patch<T>*> (this->fData)->InsertAt (newValue, CurrentIndex ());
                    Invariant ();
                }
                template	<typename T>	inline	void	BackwardArrayMutator_Patch<T>::AddAfter (T newValue)
                {
                    /*
                     * NB: This can be called if we are done.
                     */
                    Invariant ();
                    AssertNotNull (this->fData);
                    const_cast<Array_Patch<T>*>(this->fData)->InsertAt (newValue, CurrentIndex ()+1);
                    Invariant ();
                }

                template	<typename T>	inline	void	BackwardArrayMutator_Patch<T>::Invariant () const
                {
                    inherited::Invariant ();
                }

                template	<typename T>	inline	bool	BackwardArrayMutator_Patch<T>::Done () const
                {
                    return (inherited::Done ());
                }

                template	<typename T>	inline	size_t	BackwardArrayMutator_Patch<T>::CurrentIndex () const
                {
                    return (inherited::CurrentIndex ());
                }

            /*
             ********************************************************************************
             ************************************* Array ************************************
             ********************************************************************************
             */

            template	<typename T>	Array<T>::Array (const Array<T>& from) :
                fLength (0),
                fSlotsAllocated (0),
                fItems (Nil)
            {
                from.Invariant ();
                SetSlotsAlloced (from.GetSlotsAlloced ());

                /*
                 *	Construct the new items in-place into the new memory.
                 */
                    size_t	newLength		=	from.GetLength ();
                if (newLength > 0) {
                                ArrayNode<T>*	lhs	=	&fItems[0];
                        const	ArrayNode<T>*	rhs	=	&from.fItems[0];
                                ArrayNode<T>*	end	=	&fItems[newLength];
                    do {
                            new (lhs) ArrayNode<T> (*rhs++);
                    } while (++lhs < end);
                }
                fLength = newLength;
                Invariant ();
            }

            template	<typename T>	void	Array<T>::InsertAt (T item, size_t index)
            {
                Require (index >= 0);
                Require (index <= fLength);
                Invariant ();

                /*
                 * Delicate matter so that we assure ctors/dtors/op= called at
                 * right time.
                 */
                SetLength (fLength + 1, item);			//	Add space for extra item
                size_t  oldLength = fLength -1;
                if (index < oldLength) {
                    /*
                     * Slide items down, and add our new entry
                     */
                    Assert (fLength >= 2);
                    ArrayNode<T>*	lhs	=	&fItems [fLength-1];
                    ArrayNode<T>*	rhs	=	&fItems [fLength-2];
                    size_t i = fLength-1;

                    for (; i > index; i--) {
                        *lhs-- = *rhs--;
                    }
                    Assert (i == index);
                    Assert (lhs == &fItems [index]);
                    lhs->fItem = item;
                }
                Invariant ();
            }

            template	<typename T>	void	Array<T>::RemoveAt (size_t index)
            {
                Require (index >= 0);
                Require (index < fLength);
                Invariant ();

                if (index < fLength-1) {
                    /*
                     * Slide items down.
                     */
                        ArrayNode<T>*	lhs	=	&fItems [index];
                        ArrayNode<T>*	rhs	=	&fItems [index+1];
            // We tried getting rid of index var and using ptr compare but
            // did much worse on CFront/MPW Thursday, August 27, 1992 4:12:08 PM
                    for ( size_t i = fLength - index -1; i > 0; i--) {

                        *lhs++ = *rhs++;
                    }
                }
                fItems [--fLength].ArrayNode<T>::~ArrayNode ();
                Invariant ();
            }

            template	<typename T>	void	Array<T>::RemoveAll ()
            {
                Invariant ();
                ArrayNode<T>*	p	=	&fItems[0];
                for ( size_t i = fLength; i > 0; i--, p++) {
                    p->ArrayNode<T>::~ArrayNode ();
                }
                fLength = 0;
                Invariant ();
            }

            template	<typename T>	bool	Array<T>::Contains (T item) const
            {
                Invariant ();
                if (fLength > 0) {
                    const	ArrayNode<T>*	current	=	&fItems [0];
                    const	ArrayNode<T>*	last	=	&fItems [fLength-1];	// safe to -1 since fLength>0
                    for (; current <= last; current++) {
                        if (current->fItem == item) {
                            return (true);
                        }
                    }
                }
                return (false);
            }

            template	<typename T>	void	Array<T>::SetSlotsAlloced (size_t slotsAlloced)
            {
                Require (GetLength () <= slotsAlloced);
                Invariant ();
                if (fSlotsAllocated != slotsAlloced) {
                    if (slotsAlloced == 0) {
                        delete (char*)fItems;
                        fItems = Nil;
                    }
                    else {
                        /*
                         * We should consider getting rid of use of realloc since it prohibits
                         * internal pointers. For example, we cannot have an array of patchable_arrays.
                         */
                        if (fItems == Nil) {
                            fItems = (ArrayNode<T>*) new char [sizeof (ArrayNode<T>) * slotsAlloced];
                        }
                        else {
                            fItems = (ArrayNode<T>*) realloc (fItems, sizeof (ArrayNode<T>) * slotsAlloced);
                        }
                    }
                    fSlotsAllocated = slotsAlloced;
                }
                Invariant ();
            }

            template	<typename T>	Array<T>& Array<T>::operator= (const Array<T>& list)
            {
                Invariant ();
                size_t	newLength		=	list.GetLength ();

                /*
                 *		In case user already set this, we should not unset,
                 *	but must be sure we are big enuf. Do this before we store any pointers
                 *	cuz it could invalidate them.
                 */
                SetSlotsAlloced (Max (GetSlotsAlloced (), newLength));

                /*
                 * Copy array elements where both sides where constructed.
                 */
                size_t	commonLength	=	Stroika::Foundation::min (fLength, newLength);
                ArrayNode<T>*	lhs	=	&fItems[0];
                ArrayNode<T>*	rhs	=	&list.fItems[0];
                for ( size_t i = commonLength; i-- > 0; ) {
                    *lhs++ = *rhs++;
                }

                /*
                 * Now if new length smaller, we must destroy entries at the end, and
                 * otherwise we must copy in new entries.
                 */
                Assert (lhs == &fItems[commonLength]);						// point 1 past first guy to destroy/overwrite
                if (fLength > newLength) {
                    ArrayNode<T>*	end	=	&fItems[fLength];	// point 1 past last old guy
                    /*
                     * Then we must destruct entries at the end.
                     */
                    Assert (lhs < end);
                    do {
                        lhs->ArrayNode<T>::~ArrayNode ();
                    } while (++lhs < end);
                }
                else if (fLength < newLength) {
                        ArrayNode<T>*	end	=	&fItems[newLength];	// point 1 past last new guy
                    Assert (lhs < end);
                    do {
                        new (lhs) ArrayNode<T> (*rhs++);
                    } while (++lhs < end);
                }
                fLength = newLength;
                Invariant ();
                return (*this);
            }

            template	<typename T>	void	Array<T>::SetLength (size_t newLength, T fillValue)
            {
                Invariant ();

                /*
                 * Safe to grow the memory, but not to shrink it here, since
                 * we may need to destruct guys in the shrinking case.
                 */
                if (newLength > fSlotsAllocated) {
                    /*
                     *		Bump up Slots alloced to be at least big enuf for our
                     * new length. We could be minimalistic here, and just bump up
                     * exactly, but this function can be expensive because it calls
                     * realloc which could cause lots of memory copying. There are two
                     * plausible strategies for bumping up memory in big chunks-
                     * rounding up, and scaling up.
                     *
                     *		Rounding up works well at small scales - total memory
                     *	waste is small (bounded). It is simple, and it helps speed up
                     *	loops like while condition { append (); } considerably.
                     *
                     *		Scaling up has the advantage that for large n, we get
                     *	logn reallocs (rather than n/IncSize in the roundup case).
                     *	This is much better long-term large-size performance.
                     *	The only trouble with this approach is that in order to keep
                     *	memory waste small, we must scale by a small number (here 1.1)
                     *	and so we need array sizes > 100 before we start seeing any real
                     *	benefit at all. Such cases do happen, but we want to be able to
                     *	optimize the much more common, small array cases too.
                     *
                     *		So the compromise is to use a roundup-like strategy for
                     *	small n, and a scaling approach as n gets larger.
                     *
                     *		Also, we really should be more careful about overflow here...
                     *
                     *		Some math:
                     *			k*n = n + 64/sizeof (T) and so
                     *			n = (64/sizeof (T))/(k-1)
                     *		If we assume k = 1.1 and sizeof(T) = 4 then n = 160. This is
                     *	the value for length where we start scaling up by 10% as opposed to
                     *	our arithmetic + 16.
                     *
                     */
                    //SetSlotsAlloced (Max (newLength+(64/sizeof (T)), size_t (newLength*1.1)));
                    // Based on the above arithmatic, we can take a shortcut...
                    SetSlotsAlloced ((newLength > 160)? size_t (newLength*1.1): (newLength+(64/sizeof (T))));
                }
                    ArrayNode<T>*	cur	=	&fItems[fLength];	// point 1 past first guy
                    ArrayNode<T>*	end	=	&fItems[newLength];	// point 1 past last guy
                if (newLength > fLength) {
                    Assert (cur < end);
                    do {
                        new (cur) ArrayNode<T> (fillValue);
                    } while (++cur < end);
                }
                else {
                    Assert (cur >= end);
                    while (cur-- > end) {
                        cur->ArrayNode<T>::~ArrayNode ();
                    }
                }
                fLength = newLength;
                Invariant ();
            }

            #if		qDebug
            template	<typename T>	void	Array<T>::Invariant_ () const
            {
                Assert ((fSlotsAllocated == 0) == (fItems == Nil));		// always free iff slots alloced = 0
                Assert (fLength <= fSlotsAllocated);
            }
            #endif




            /*
             ********************************************************************************
             ******************************** ArrayIteratorBase *****************************
             ********************************************************************************
             */
            #if		qDebug
            template	<typename T>	void	ArrayIteratorBase<T>::Invariant_ () const
            {
                AssertNotNull (fData);
                Assert (fStart == fData->fItems);
                Assert (size_t (fEnd-fStart) == fData->GetLength ());
                Assert ((fCurrent >= fStart) and (fCurrent <= fEnd));	// ANSI C requires this is always TRUE
            }
            #endif

            /*
             ********************************************************************************
             ******************************** Array_Patch ***********************************
             ********************************************************************************
             */
            #if		qDebug
            template	<typename T>	void	Array_Patch<T>::Invariant_ () const
            {
                Array<T>::Invariant_ ();

                /*
                 *		Be sure each iterator points back to us. Thats about all we can test from
                 *	here since we cannot call each iterators Invariant(). That would be
                 *	nice, but sadly when this Invariant_ () is called from Array<T> the
                 *	iterators themselves may not have been patched, so they'll be out of
                 *	date. Instead, so that in local shadow of Invariant() done in Array_Patch<T>
                 *	so only called when WE call Invariant().
                 */
                for (ArrayIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
                    Assert (v->fData == this);
                }
            }

            template	<typename T>	void	Array_Patch<T>::InvariantOnIterators_ () const
            {
                /*
                 *		Only here can we iterate over each iterator and calls its Invariant()
                 *	since now we've completed any needed patching.
                 */
                for (ArrayIterator_PatchBase<T>* v = fIterators; v != Nil; v = v->fNext) {
                    Assert (v->fData == this);
                    v->Invariant ();
                }
            }
            #endif	/*qDebug*/




            /*
             ********************************************************************************
             ******************************* ArrayIterator_PatchBase ************************
             ********************************************************************************
             */
            #if		qDebug
            template	<typename T>	void	ArrayIterator_PatchBase<T>::Invariant_ () const
            {
                ArrayIteratorBase<T>::Invariant_ ();
                Assert (fData == ArrayIteratorBase<T>::fData);
            }
            #endif	/*qDebug*/


		}
	}
}


#endif /* _Stroika_Foundation_Containers_Array_inl_ */
