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
                    Require (i >= 1);
                    Require (i <= fLength);
                    return (fItems [i-1].fItem);
                }
                template	<class	T>	inline	void	Array<T>::SetAt (T item, size_t i)
                {
                    Require (i >= 1);
                    Require (i <= fLength);
                    fItems [i-1].fItem = item;
                }
                template	<class	T>	inline	T&	Array<T>::operator[] (size_t i)
                {
                    Require (i >= 1);
                    Require (i <= fLength);
                    return (fItems [i-1].fItem);
                }
                template	<class	T>	inline	T	Array<T>::operator[] (size_t i) const
                {
                    Require (i >= 1);
                    Require (i <= fLength);
                    return (fItems [i-1].fItem);
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
                    fSupressMore (true)				// first time thru - cuz of how used in for loops...
                {
                    #if		qDebug
                        fCurrent = 0;	// more likely to cause bugs...(leave the xtra newline cuz of genclass bug...)

                    #endif
                    /*
                     * Cannot call invariant () here since fCurrent not yet setup.
                     */
                }
                template	<typename T>	inline	bool	ArrayIteratorBase<T>::Done () const
                {
                    Invariant ();
                    return bool (fCurrent == fEnd);
                }
                template	<typename T>	inline	size_t	ArrayIteratorBase<T>::CurrentIndex () const
                {
                    /*
                     * NB: This can be called if we are done - if so, it returns GetLength() + 1.
                     */
                    Invariant ();
                    return ((fCurrent-fStart)+1);
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
            #if qGCC_ScopingInTemplateBug
                    ForwardArrayIterator<T>::fCurrent = ForwardArrayIterator<T>::fStart;
                    ForwardArrayIterator<T>::Invariant ();
            #else
                    fCurrent = FfStart;
                    Invariant ();
            #endif
                }
                template	<typename T>	inline	bool	ForwardArrayIterator<T>::More ()
                {
            #if qGCC_ScopingInTemplateBug
                    ForwardArrayIterator<T>::Invariant ();
                    if (not ForwardArrayIterator<T>::fSupressMore and not ForwardArrayIterator<T>::Done ()) {
                        Assert (ForwardArrayIterator<T>::fCurrent < ForwardArrayIterator<T>::fEnd);
                        ForwardArrayIterator<T>::fCurrent++;
                    }
                    ForwardArrayIterator<T>::fSupressMore = false;
                    ForwardArrayIterator<T>::Invariant ();
                    return (not ForwardArrayIterator<T>::Done ());
            #else
                    Invariant ();
                    if (not fSupressMore and not Done ()) {
                        Assert (fCurrent < fEnd);
                        fCurrent++;
                    }
                    fSupressMore = false;
                    Invariant ();
                    return (not Done ());
            #endif
                }


                // Class ForwardArrayMutator<T>
                template	<typename T>	inline	ForwardArrayMutator<T>::ForwardArrayMutator (Array<T>& data) :
                    ForwardArrayIterator<T>((const Array<T>&)data)
                {
            #if qGCC_ScopingInTemplateBug
                    ForwardArrayMutator<T>::Invariant ();
            #else
                    Invariant ();
            #endif
                }
                template	<typename T>	inline	void	ForwardArrayMutator<T>::UpdateCurrent (T newValue)
                {
            #if qGCC_ScopingInTemplateBug
                    ForwardArrayMutator<T>::Invariant ();
                    Require (not ForwardArrayMutator<T>::Done ());
                    AssertNotNull (ForwardArrayMutator<T>::fCurrent);
                    const_cast<ArrayNode<T>*> (ForwardArrayMutator<T>::fCurrent)->fItem = newValue; // not sure how to handle better the (~const)
            #else
                    ((ArrayNode<T>*)ForwardArrayMutator<T>::fCurrent)->fItem = newValue;
                    Invariant ();
                    Require (not Done ());
                    AssertNotNull (fCurrent);
                    ((ArrayNode<T>*)fCurrent)->fItem = newValue;	// not sure how to handle better the (~const)
            #endif
                }


                // Class BackwardArrayIterator<T>
                template	<typename T>	inline	BackwardArrayIterator<T>::BackwardArrayIterator (const Array<T>& data) :
                    ArrayIteratorBase<T>(data)
                {
            #if qGCC_ScopingInTemplateBug
                    if (data.GetLength () == 0) {
                        BackwardArrayIterator<T>::fCurrent = BackwardArrayIterator<T>::fEnd;	// magic to indicate done
                    }
                    else {
                        BackwardArrayIterator<T>::fCurrent = BackwardArrayIterator<T>::fEnd-1;	// last valid item
                    }
                    BackwardArrayIterator<T>::Invariant ();
            #else
                    if (data.GetLength () == 0) {
                        fCurrent = fEnd;	// magic to indicate done
                    }
                    else {
                        fCurrent = fEnd-1;	// last valid item
                    }
                    Invariant ();
            #endif
                }

                template	<typename T>	inline	bool	BackwardArrayIterator<T>::More ()
                {
            #if qGCC_ScopingInTemplateBug
                    BackwardArrayIterator<T>::Invariant ();
                    if (BackwardArrayIterator<T>::fSupressMore) {
                        BackwardArrayIterator<T>::fSupressMore = false;
                        return (not BackwardArrayIterator<T>::Done ());
                    }
                    else {
                        if (BackwardArrayIterator<T>::Done ()) {
                            return (false);
                        }
                        else {
                            if (BackwardArrayIterator<T>::fCurrent == BackwardArrayIterator<T>::fStart) {
                                BackwardArrayIterator<T>::fCurrent = BackwardArrayIterator<T>::fEnd;	// magic to indicate done
                                Ensure (BackwardArrayIterator<T>::Done ());
                                return (false);
                            }
                            else {
                                BackwardArrayIterator<T>::fCurrent--;
                                Ensure (not BackwardArrayIterator<T>::Done ());
                                return (true);
                            }
                        }
                    }
            #else
                    Invariant ();
                    if (fSupressMore) {
                        fSupressMore = false;
                        return (not Done ());
                    }
                    else {
                        if (Done ()) {
                            return (false);
                        }
                        else {
                            if (fCurrent == fStart) {
                                fCurrent = fEnd;	// magic to indicate done
                                Ensure (Done ());
                                return (false);
                            }
                            else {
                                fCurrent--;
                                Ensure (not Done ());
                                return (true);
                            }
                        }
                    }
            #endif
                    AssertNotReached ();	return (false);
                }


                // Class BackwardArrayMutator<T>
                template	<typename T>	inline	BackwardArrayMutator<T>::BackwardArrayMutator (Array<T>& data) :
                    BackwardArrayIterator<T>((const Array<T>&)data)
                {
            #if qGCC_ScopingInTemplateBug
                    BackwardArrayMutator<T>::Invariant ();
            #else
                    Invariant ();
            #endif
                }
                template	<typename T>	inline	void	BackwardArrayMutator<T>::UpdateCurrent (T newValue)
                {
            #if qGCC_ScopingInTemplateBug
                    BackwardArrayMutator<T>::Invariant ();
                    Require (not BackwardArrayMutator<T>::Done ());
                    AssertNotNull (BackwardArrayMutator<T>::fCurrent);
                    const_cast<ArrayNode<T>*> (BackwardArrayMutator<T>::fCurrent)->fItem = newValue;	// not sure how to handle better the (~const)

            #else
                    Invariant ();
                    Require (not Done ());
                    AssertNotNull (fCurrent);
                    const_cast<ArrayNode<T>*> (fCurrent)->fItem = newValue;	// not sure how to handle better the (~const)
            #endif
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
            #if qGCC_ScopingInTemplateBug
                    ArrayIterator_PatchBase<T>::Invariant ();
            #else
                    Invariant ();
            #endif
                }
                template	<typename T>	inline	ArrayIterator_PatchBase<T>::~ArrayIterator_PatchBase ()
                {
            #if qGCC_ScopingInTemplateBug
                    ArrayIterator_PatchBase<T>::Invariant ();
            #else
                    Invariant ();
            #endif
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
            #if qGCC_ScopingInTemplateBug
                    ArrayIterator_PatchBase<T>::Invariant ();
            #else
                    Invariant ();
            #endif

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

            #if qGCC_ScopingInTemplateBug
                    ArrayIterator_PatchBase<T>::Invariant ();
            #else
                    Invariant ();
            #endif
                    return (*this);
                }
                template	<typename T>	inline	void	ArrayIterator_PatchBase<T>::PatchAdd (size_t index)
                {
                    /*
                     *		NB: We cannot call invariants here because this is called after the add
                     *	and the PatchRealloc has not yet happened.
                     */
                    Require (index >= 1);

            #if qGCC_ScopingInTemplateBug
                    ArrayIterator_PatchBase<T>::fEnd++;
            #else
                    fEnd++;
            #endif

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
            #if qGCC_ScopingInTemplateBug
                    Require (index <= (ArrayIterator_PatchBase<T>::fEnd-ArrayIterator_PatchBase<T>::fStart));
                    if (&ArrayIterator_PatchBase<T>::fStart[index-1] <= ArrayIterator_PatchBase<T>::fCurrent) {		// index <= CurrentIndex () - only faster
                                                            // Cannot call CurrentIndex () since invariants
                                                            // might fail at this point
                        ArrayIterator_PatchBase<T>::fCurrent++;
                    }
            #else
                    Require (index <= (fEnd-fStart));
                    if (&fStart[index-1] <= fCurrent) {		// index <= CurrentIndex () - only faster
                                                            // Cannot call CurrentIndex () since invariants
                                                            // might fail at this point
                        fCurrent++;
                    }
            #endif
                }
                template	<typename T>	inline	void	ArrayIterator_PatchBase<T>::PatchRemove (size_t index)
                {
                    Require (index >= 1);
                    Require (index <= fData->GetLength ());

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
                     *		In the case where we are directly hit, just set fSupressMore
                     *	to true. If we are going forwards, are are already pointing where
                     *	we should be (and this works for repeat deletions). If we are
                     *	going backwards, then fSupressMore will be ignored, but for the
                     *	sake of code sharing, its tough to do much about that waste.
                     */
            #if qGCC_ScopingInTemplateBug
                    Assert ((&ArrayIterator_PatchBase<T>::fStart[index-1] <= ArrayIterator_PatchBase<T>::fCurrent) == (index <= ArrayIterator_PatchBase<T>::CurrentIndex ()));		// index <= CurrentIndex () - only faster
                    if (&ArrayIterator_PatchBase<T>::fStart[index-1] < ArrayIterator_PatchBase<T>::fCurrent) {
                        Assert (ArrayIterator_PatchBase<T>::CurrentIndex () >= 2);		// cuz then index would be <= 0, and thats imposible
                        ArrayIterator_PatchBase<T>::fCurrent--;
                    }
                    else if (&ArrayIterator_PatchBase<T>::fStart[index-1] == ArrayIterator_PatchBase<T>::fCurrent) {
                        PatchRemoveCurrent ();
                    }
                    // Decrement at the end since CurrentIndex () calls stuff that asserts (fEnd-fStart) == fData->GetLength ()
                    Assert ((ArrayIterator_PatchBase<T>::fEnd-ArrayIterator_PatchBase<T>::fStart) == fData->GetLength ());		//	since called before remove

                    /*
                     * At this point, fCurrent could be == fEnd - must not lest fCurrent point past!
                     */
                    if (ArrayIterator_PatchBase<T>::fCurrent == ArrayIterator_PatchBase<T>::fEnd) {
                        Assert (ArrayIterator_PatchBase<T>::fCurrent > ArrayIterator_PatchBase<T>::fStart);	// since we are removing something start!=end
                        ArrayIterator_PatchBase<T>::fCurrent--;
                    }
                    ArrayIterator_PatchBase<T>::fEnd--;
            #else
                    Assert ((&fStart[index-1] <= fCurrent) == (index <= CurrentIndex ()));		// index <= CurrentIndex () - only faster
                    if (&fStart[index-1] < fCurrent) {
                        Assert (CurrentIndex () >= 2);		// cuz then index would be <= 0, and thats imposible
                        fCurrent--;
                    }
                    else if (&fStart[index-1] == fCurrent) {
                        PatchRemoveCurrent ();
                    }
                    // Decrement at the end since CurrentIndex () calls stuff that asserts (fEnd-fStart) == fData->GetLength ()
                    Assert ((fEnd-fStart) == fData->GetLength ());		//	since called before remove

                    /*
                     * At this point, fCurrent could be == fEnd - must not lest fCurrent point past!
                     */
                    if (fCurrent == fEnd) {
                        Assert (fCurrent > fStart);	// since we are removing something start!=end
                        fCurrent--;
                    }
                    fEnd--;
            #endif
                }
                template	<typename T>	inline	void	ArrayIterator_PatchBase<T>::PatchRemoveAll ()
                {
                    Require (fData->GetLength () == 0);		//	since called after removeall
            #if qGCC_ScopingInTemplateBug
                    ArrayIterator_PatchBase<T>::fCurrent = fData->fItems;
                    ArrayIterator_PatchBase<T>::fStart = fData->fItems;
                    ArrayIterator_PatchBase<T>::fEnd = fData->fItems;
                    ArrayIterator_PatchBase<T>::fSupressMore = true;
            #else
                    fCurrent = fData->fItems;
                    fStart = fData->fItems;
                    fEnd = fData->fItems;
                    fSupressMore = true;
            #endif
                }
                template	<typename T>	inline	void	ArrayIterator_PatchBase<T>::PatchRealloc ()
                {
                    /*
                     *		NB: We can only call invariant after we've fixed things up, since realloc
                     * has happened by now, but things don't point to the right places yet.
                     */
            #if qGCC_ScopingInTemplateBug
                    if (ArrayIterator_PatchBase<T>::fStart != fData->fItems) {
                        const	ArrayNode<T>*	oldStart	=	ArrayIterator_PatchBase<T>::fStart;
                        ArrayIterator_PatchBase<T>::fStart = fData->fItems;
                        ArrayIterator_PatchBase<T>::fCurrent = fData->fItems + (ArrayIterator_PatchBase<T>::fCurrent-oldStart);
                        ArrayIterator_PatchBase<T>::fEnd = fData->fItems + (ArrayIterator_PatchBase<T>::fEnd-oldStart);
                    }
            #else
                    if (fStart != fData->fItems) {
                        const	ArrayNode<T>*	oldStart	=	fStart;
                        fStart = fData->fItems;
                        fCurrent = fData->fItems + (fCurrent-oldStart);
                        fEnd = fData->fItems + (fEnd-oldStart);
                    }
            #endif
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
            #if qGCC_ScopingInTemplateBug
                    ForwardArrayIterator_Patch<T>::fCurrent = ForwardArrayIterator_Patch<T>::fStart;
                    ForwardArrayIterator_Patch<T>::Invariant ();
            #else
                    fCurrent = fStart;
                    Invariant ();
            #endif
                }
                template	<typename T>	inline	bool	ForwardArrayIterator_Patch<T>::More ()
                {
            #if qGCC_ScopingInTemplateBug
                    ForwardArrayIterator_Patch<T>::Invariant ();
                    if (not ForwardArrayIterator_Patch<T>::fSupressMore and not ForwardArrayIterator_Patch<T>::Done ()) {
                        Assert (ForwardArrayIterator_Patch<T>::fCurrent < ForwardArrayIterator_Patch<T>::fEnd);
                        ForwardArrayIterator_Patch<T>::fCurrent++;
                    }
                    ForwardArrayIterator_Patch<T>::fSupressMore = false;
                    ForwardArrayIterator_Patch<T>::Invariant ();
                    return (not ForwardArrayIterator_Patch<T>::Done ());
            #else
                    Invariant ();
                    if (not fSupressMore and not Done ()) {
                        Assert (fCurrent < fEnd);
                        fCurrent++;
                    }
                    fSupressMore = false;
                    Invariant ();
                    return (not Done ());
            #endif
                }
                template	<typename T>	inline	void	ForwardArrayIterator_Patch<T>::PatchRemoveCurrent ()
                {
            #if qGCC_ScopingInTemplateBug
                    Assert (ForwardArrayIterator_Patch<T>::fCurrent < ForwardArrayIterator_Patch<T>::fEnd);	// cannot remove something past the end
                    ForwardArrayIterator_Patch<T>::fSupressMore = true;
            #else
                    Assert (fCurrent < fEnd);	// cannot remove something past the end
                    fSupressMore = true;
            #endif
                }


                // Class ForwardArrayMutator<T>
                template	<typename T>	inline	ForwardArrayMutator_Patch<T>::ForwardArrayMutator_Patch (Array_Patch<T>& data) :
                    ForwardArrayIterator_Patch<T>((const Array_Patch<T>&)data)
                {
            #if qGCC_ScopingInTemplateBug
                    ForwardArrayMutator_Patch<T>::Invariant ();
            #else
                    Invariant ();
            #endif
                }
                template	<typename T>	inline	void	ForwardArrayMutator_Patch<T>::RemoveCurrent ()
                {
            #if qGCC_ScopingInTemplateBug
                    ForwardArrayMutator_Patch<T>::Invariant ();
                    AssertNotNull (ForwardArrayMutator_Patch<T>::fData);
                    const_cast<Array_Patch<T>*> (ForwardArrayMutator_Patch<T>::fData)->RemoveAt (ForwardArrayMutator_Patch<T>::CurrentIndex ());
                    ForwardArrayMutator_Patch<T>::Invariant ();
            #else
                    Invariant ();
                    AssertNotNull (fData);
                    const_cast<Array_Patch<T>*> (fData)->RemoveAt (CurrentIndex ());
                    Invariant ();
            #endif
                }
                template	<typename T>	inline	void	ForwardArrayMutator_Patch<T>::UpdateCurrent (T newValue)
                {
            #if qGCC_ScopingInTemplateBug
                    ForwardArrayMutator_Patch<T>::Invariant ();
                    Require (not ForwardArrayMutator_Patch<T>::Done ());
                    AssertNotNull (ForwardArrayMutator_Patch<T>::fCurrent);
                    const_cast<ArrayNode<T>*>(ForwardArrayMutator_Patch<T>::fCurrent)->fItem = newValue;
            #else
                    Invariant ();
                    Require (not Done ());
                    AssertNotNull (fCurrent);
                    const_cast<ArrayNode<T>*>(fCurrent)->fItem = newValue;
            #endif
                }
                template	<typename T>	inline	void	ForwardArrayMutator_Patch<T>::AddBefore (T newValue)
                {
                    /*
                     * NB: This can be called if we are done.
                     */
            #if qGCC_ScopingInTemplateBug
                    ForwardArrayMutator_Patch<T>::Invariant ();
                    AssertNotNull (ForwardArrayMutator_Patch<T>::fData);
                    const_cast<Array_Patch<T>*> (ForwardArrayMutator_Patch<T>::fData)->InsertAt (newValue, ForwardArrayMutator_Patch<T>::CurrentIndex ());
                    ForwardArrayMutator_Patch<T>::Invariant ();
            #else
                    Invariant ();
                    AssertNotNull (fData);
                    const_cast<Array_Patch<T>*> (fData)->InsertAt (newValue, CurrentIndex ());
                    Invariant ();
            #endif
                }
                template	<typename T>	inline	void	ForwardArrayMutator_Patch<T>::AddAfter (T newValue)
                {
            #if qGCC_ScopingInTemplateBug
                    Require (not ForwardArrayMutator_Patch<T>::Done ());
                    ForwardArrayMutator_Patch<T>::Invariant ();
                    AssertNotNull (ForwardArrayMutator_Patch<T>::fData);
                    const_cast<Array_Patch<T>*> (ForwardArrayMutator_Patch<T>::fData)->InsertAt (newValue, ForwardArrayMutator_Patch<T>::CurrentIndex ()+1);
                    ForwardArrayMutator_Patch<T>::Invariant ();
            #else
                    Require (not Done ());
                    Invariant ();
                    AssertNotNull (fData);
                    const_cast<Array_Patch<T>*> (fData)->InsertAt (newValue, CurrentIndex ()+1);
                    Invariant ();
            #endif
                }


                // class BackwardArrayIterator_Patch<T>
                template	<typename T>	inline	BackwardArrayIterator_Patch<T>::BackwardArrayIterator_Patch (const Array_Patch<T>& data) :
                    ArrayIterator_PatchBase<T> (data)
                {
            #if qGCC_ScopingInTemplateBug
                    if (data.GetLength () == 0) {
                        BackwardArrayIterator_Patch<T>::fCurrent = BackwardArrayIterator_Patch<T>::fEnd;	// magic to indicate done
                    }
                    else {
                        BackwardArrayIterator_Patch<T>::fCurrent = BackwardArrayIterator_Patch<T>::fEnd-1;	// last valid item
                    }
                    BackwardArrayIterator_Patch<T>::Invariant ();
            #else
                    if (data.GetLength () == 0) {
                        fCurrent = fEnd;	// magic to indicate done
                    }
                    else {
                        fCurrent = fEnd-1;	// last valid item
                    }
                    Invariant ();
            #endif
                }

                // Careful to keep hdr and src copies identical...
                template	<typename T>	inline	bool	BackwardArrayIterator_Patch<T>::More ()
                {
            #if qGCC_ScopingInTemplateBug
                    BackwardArrayIterator_Patch<T>::Invariant ();
                    if (BackwardArrayIterator_Patch<T>::fSupressMore) {
                        BackwardArrayIterator_Patch<T>::fSupressMore = false;
                        return (not BackwardArrayIterator_Patch<T>::Done ());
                    }
                    else {
                        if (BackwardArrayIterator_Patch<T>::Done ()) {
                            return (false);
                        }
                        else {
                            if (BackwardArrayIterator_Patch<T>::fCurrent == BackwardArrayIterator_Patch<T>::fStart) {
                                BackwardArrayIterator_Patch<T>::fCurrent = BackwardArrayIterator_Patch<T>::fEnd;	// magic to indicate done
                                Ensure (BackwardArrayIterator_Patch<T>::Done ());
                                return (false);
                            }
                            else {
                                BackwardArrayIterator_Patch<T>::fCurrent--;
                                Ensure (not BackwardArrayIterator_Patch<T>::Done ());
                                return (true);
                            }
                        }
                    }
                    AssertNotReached ();	return (false);
            #else
                    Invariant ();
                    if (fSupressMore) {
                        fSupressMore = false;
                        return (not Done ());
                    }
                    else {
                        if (Done ()) {
                            return (false);
                        }
                        else {
                            if (fCurrent == fStart) {
                                fCurrent = fEnd;	// magic to indicate done
                                Ensure (Done ());
                                return (false);
                            }
                            else {
                                fCurrent--;
                                Ensure (not Done ());
                                return (true);
                            }
                        }
                    }
                    AssertNotReached ();	return (false);
            #endif
                }

                template	<typename T>	inline	void	BackwardArrayIterator_Patch<T>::PatchRemoveCurrent ()
                {
            #if qGCC_ScopingInTemplateBug
                    if (BackwardArrayIterator_Patch<T>::fCurrent == BackwardArrayIterator_Patch<T>::fStart) {
                        BackwardArrayIterator_Patch<T>::fCurrent = BackwardArrayIterator_Patch<T>::fEnd;	// magic to indicate done
                    }
                    else {
                        Assert (BackwardArrayIterator_Patch<T>::fCurrent > BackwardArrayIterator_Patch<T>::fStart);
                        BackwardArrayIterator_Patch<T>::fCurrent--;
                    }
                    BackwardArrayIterator_Patch<T>::fSupressMore = true;
            #else
                    if (fCurrent == fStart) {
                        fCurrent = fEnd;	// magic to indicate done
                    }
                    else {
                        Assert (fCurrent > fStart);
                        fCurrent--;
                    }
                    fSupressMore = true;
            #endif
                }


                // Class BackwardArrayMutator<T>
                template	<typename T>	inline	BackwardArrayMutator_Patch<T>::BackwardArrayMutator_Patch (Array_Patch<T>& data) :
                    BackwardArrayIterator_Patch<T>((const Array_Patch<T>&)data)
                {
            #if qGCC_ScopingInTemplateBug
                    BackwardArrayIterator_Patch<T>::Invariant ();
            #else
                    Invariant ();
            #endif
                }
                template	<typename T>	inline	void	BackwardArrayMutator_Patch<T>::RemoveCurrent ()
                {
            #if qGCC_ScopingInTemplateBug
                    BackwardArrayIterator_Patch<T>::Invariant ();
                    AssertNotNull (BackwardArrayIterator_Patch<T>::fData);
                    const_cast<Array_Patch<T>*> (BackwardArrayIterator_Patch<T>::fData)->RemoveAt (BackwardArrayIterator_Patch<T>::CurrentIndex ());
            #else
                    Invariant ();
                    AssertNotNull (fData);
                    const_cast<Array_Patch<T>*> (fData)->RemoveAt (CurrentIndex ());
            #endif
                }
                template	<typename T>	inline	void	BackwardArrayMutator_Patch<T>::UpdateCurrent (T newValue)
                {
            #if qGCC_ScopingInTemplateBug
                    BackwardArrayMutator_Patch<T>::Invariant ();
                    Require (not BackwardArrayMutator_Patch<T>::Done ());
                    AssertNotNull (BackwardArrayMutator_Patch<T>::fCurrent);
                    const_cast<ArrayNode<T>*>(BackwardArrayMutator_Patch<T>::fCurrent)->fItem = newValue;
            #else
                    Invariant ();
                    Require (not Done ());
                    AssertNotNull (fCurrent);
                    const_cast<ArrayNode<T>*>(fCurrent)->fItem = newValue;
            #endif
                }
                template	<typename T>	inline	void	BackwardArrayMutator_Patch<T>::AddBefore (T newValue)
                {
            #if qGCC_ScopingInTemplateBug
                    Require (not BackwardArrayMutator_Patch<T>::Done ());
                    BackwardArrayMutator_Patch<T>::Invariant ();
                    AssertNotNull (BackwardArrayMutator_Patch<T>::fData);
                    const_cast<Array_Patch<T>*> (BackwardArrayMutator_Patch<T>::fData)->InsertAt (newValue, BackwardArrayMutator_Patch<T>::CurrentIndex ());
                    BackwardArrayMutator_Patch<T>::Invariant ();
            #else
                    Require (not Done ());
                    Invariant ();
                    AssertNotNull (fData);
                    const_cast<Array_Patch<T>*> (fData)->InsertAt (newValue, CurrentIndex ());
                    Invariant ();
            #endif
                }
                template	<typename T>	inline	void	BackwardArrayMutator_Patch<T>::AddAfter (T newValue)
                {
            #if qGCC_ScopingInTemplateBug
                    /*
                     * NB: This can be called if we are done.
                     */
                    BackwardArrayMutator_Patch<T>::Invariant ();
                    AssertNotNull (BackwardArrayMutator_Patch<T>::fData);
                    const_cast<Array_Patch<T>*>(BackwardArrayMutator_Patch<T>::fData)->InsertAt (newValue, BackwardArrayMutator_Patch<T>::CurrentIndex ()+1);
                    BackwardArrayMutator_Patch<T>::Invariant ();
            #else
                    /*
                     * NB: This can be called if we are done.
                     */
                    Invariant ();
                    AssertNotNull (fData);
                    const_cast<Array_Patch<T>*>(fData)->InsertAt (newValue, CurrentIndex ()+1);
                    Invariant ();
            #endif
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
                Require (index >= 1);
                Require (index <= fLength+1);
                Invariant ();

                /*
                 * Delicate matter so that we assure ctors/dtors/op= called at
                 * right time.
                 */
                SetLength (fLength + 1, item);			//	Add space for extra item

                if (index < fLength) {
                    /*
                     * Slide items down, and add our new entry
                     */
                    ArrayNode<T>*	lhs	=	&fItems [fLength-1];
                    ArrayNode<T>*	rhs	=	&fItems [fLength-2];
                    size_t i = fLength-1;
                    for (; i >= index; i--) {
                        *lhs-- = *rhs--;
                    }
                    Assert (i == index-1);
                    Assert (lhs == &fItems [index-1]);
                    lhs->fItem = item;
                }
                Invariant ();
            }

            template	<typename T>	void	Array<T>::RemoveAt (size_t index)
            {
                Require (index >= 1);
                Require (index <= fLength);
                Invariant ();

                if (index < fLength) {
                    /*
                     * Slide items down.
                     */
                        ArrayNode<T>*	lhs	=	&fItems [index-1];
                        ArrayNode<T>*	rhs	=	&fItems [index];
            // We tried getting rid of index var and using ptr compare but
            // did much worse on CFront/MPW Thursday, August 27, 1992 4:12:08 PM
                    for ( size_t i = fLength - index; i > 0; i--) {
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
                Assert ((fEnd-fStart) == fData->GetLength ());
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
