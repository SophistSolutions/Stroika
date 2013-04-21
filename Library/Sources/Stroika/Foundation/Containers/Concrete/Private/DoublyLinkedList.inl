/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Private_DoublyLinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Private_DoublyLinkedList_inl_  1


#include    "../../../Debug/Assertions.h"
#include    "../../../Memory/BlockAllocated.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {
                namespace   Private {

                    // class DoubleLink<T>
                    template    <class T>   inline  DoubleLink<T>::DoubleLink (T item, DoubleLink<T>* next) :
                        fItem (item),
                        fNext (next)
                    {
                    }

                    template    <typename   T>  inline  void*   DoubleLink<T>::operator new (size_t size)
                    {
                        return (Memory::BlockAllocated<DoubleLink<T> >::operator new (size));
                    }

                    template    <typename   T>  inline  void    DoubleLink<T>::operator delete (void* p)
                    {
                        Memory::BlockAllocated<DoubleLink<T> >::operator delete (p);
                    }


                    // class DoublyLinkedList<T>
                    template    <typename   T>  DoublyLinkedList<T>::DoublyLinkedList (const DoublyLinkedList<T>& from) :
                        fLength (from.fLength),
                        fFirst (nullptr)
                    {
                        /*
                         *      Copy the link list by keeping a point to the new current and new
                         *  previous, and sliding them along in parallel as we construct the
                         *  new list. Only do this if we have at least one element - then we
                         *  don't have to worry about the head of the list, or nullptr ptrs, etc - that
                         *  case is handled outside, before the loop.
                         */
                        if (from.fFirst != nullptr) {
                            fFirst = new DoubleLink<T> (from.fFirst->fItem, nullptr);
                            DoubleLink<T>*    newCur  =   fFirst;
                            for (const DoubleLink<T>* cur = from.fFirst->fNext; cur != nullptr; cur = cur->fNext) {
                                DoubleLink<T>*    newPrev =   newCur;
                                newCur = new DoubleLink<T> (cur->fItem, nullptr);
                                newPrev->fNext = newCur;
                            }
                        }

                        Invariant ();
                    }

                    template    <class T>   inline  void    DoublyLinkedList<T>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }

                    template    <class T>   inline  DoublyLinkedList<T>::DoublyLinkedList () :
                        fLength (0),
                        fFirst (nullptr)
                    {
                        Invariant ();
                    }

                    template    <class T>   inline  DoublyLinkedList<T>::~DoublyLinkedList ()
                    {
                        /*
                         * This could be a little cheaper - we could avoid setting fLength field,
                         * and fFirst pointer, but we must worry more about codeSize/re-use.
                         * That would involve a new function that COULD NOT BE INLINED.
                         *
                         * < I guess  I could add a hack method - unadvertised - but has to be
                         *   at least protected - and call it here to do what I've mentioned above >
                         */
                        Invariant ();
                        RemoveAll ();
                        Invariant ();
                        Ensure (fLength == 0);
                        Ensure (fFirst == nullptr);
                    }

                    template    <class T>   inline  size_t  DoublyLinkedList<T>::GetLength () const
                    {
                        return (fLength);
                    }

                    template    <class T>   inline  T   DoublyLinkedList<T>::GetFirst () const
                    {
                        Require (fLength > 0);
                        AssertNotNull (fFirst);
                        return (fFirst->fItem);
                    }

                    template    <class T>   inline  void    DoublyLinkedList<T>::Prepend (T item)
                    {
                        Invariant ();
                        fFirst = new DoubleLink<T> (item, fFirst);
                        fLength++;
                        Invariant ();
                    }

                    template    <class T>   inline  void    DoublyLinkedList<T>::RemoveFirst ()
                    {
                        RequireNotNull (fFirst);
                        Invariant ();

                        DoubleLink<T>* victim = fFirst;
                        fFirst = victim->fNext;
                        delete (victim);
                        fLength--;

                        Invariant ();
                    }

                    template    <typename   T>  DoublyLinkedList<T>& DoublyLinkedList<T>::operator= (const DoublyLinkedList<T>& list)
                    {
                        Invariant ();

                        RemoveAll ();

                        /*
                         *      Copy the link list by keeping a point to the new current and new
                         *  previous, and sliding them along in parallel as we construct the
                         *  new list. Only do this if we have at least one element - then we
                         *  don't have to worry about the head of the list, or nullptr ptrs, etc - that
                         *  case is handled outside, before the loop.
                         */
                        if (list.fFirst != nullptr) {
                            fFirst = new DoubleLink<T> (list.fFirst->fItem, nullptr);
                            DoubleLink<T>*    newCur  =   fFirst;
                            for (const DoubleLink<T>* cur = list.fFirst->fNext; cur != nullptr; cur = cur->fNext) {
                                DoubleLink<T>*    newPrev =   newCur;
                                newCur = new DoubleLink<T> (cur->fItem, nullptr);
                                newPrev->fNext = newCur;
                            }
                        }

                        fLength = list.fLength;

                        Invariant ();

                        return (*this);
                    }

                    template    <typename   T>  void    DoublyLinkedList<T>::Remove (T item)
                    {
                        Require (fLength >= 1);

                        Invariant ();

                        if (item == fFirst->fItem) {
                            RemoveFirst ();
                        }
                        else {
                            DoubleLink<T>*    prev    =   nullptr;
                            for (DoubleLink<T>* link = fFirst; link != nullptr; prev = link, link = link->fNext) {
                                if (link->fItem == item) {
                                    AssertNotNull (prev);       // cuz otherwise we would have hit it in first case!
                                    prev->fNext = link->fNext;
                                    delete (link);
                                    fLength--;
                                    break;
                                }
                            }
                        }

                        Invariant ();
                    }

                    template    <typename   T>  bool    DoublyLinkedList<T>::Contains (T item) const
                    {
                        for (const DoubleLink<T>* current = fFirst; current != nullptr; current = current->fNext) {
                            if (current->fItem == item) {
                                return (true);
                            }
                        }
                        return (false);
                    }

                    template    <typename   T>  void    DoublyLinkedList<T>::RemoveAll ()
                    {
                        for (DoubleLink<T>* i = fFirst; i != nullptr;) {
                            DoubleLink<T>*    deleteMe    =   i;
                            i = i->fNext;
                            delete (deleteMe);
                        }
                        fFirst = nullptr;
                        fLength = 0;
                    }

                    template    <typename   T>  T   DoublyLinkedList<T>::GetAt (size_t i) const
                    {
                        Require (i >= 0);
                        Require (i < fLength);
                        const DoubleLink<T>* cur = fFirst;
                        for (; i != 0; cur = cur->fNext, --i) {
                            AssertNotNull (cur);    // cuz i <= fLength
                        }
                        AssertNotNull (cur);        // cuz i <= fLength
                        return (cur->fItem);
                    }

                    template    <typename   T>  void    DoublyLinkedList<T>::SetAt (T item, size_t i)
                    {
                        Require (i >= 0);
                        Require (i < fLength);
                        DoubleLink<T>* cur = fFirst;
                        for (; i != 0; cur = cur->fNext, --i) {
                            AssertNotNull (cur);    // cuz i <= fLength
                        }
                        AssertNotNull (cur);        // cuz i <= fLength
                        cur->fItem = item;
                    }

#if     qDebug
                    template    <typename   T>  void    DoublyLinkedList<T>::Invariant_ () const
                    {
                        /*
                         * Check we are properly linked together.
                         */
                        size_t  counter =   0;
                        for (DoubleLink<T>* i = fFirst; i != nullptr; i = i->fNext) {
                            counter++;
                            Assert (counter <= fLength);    // to this test in the loop so we detect circularities...
                        }
                        Assert (counter == fLength);
                    }
#endif





                    // class DoublyLinkedListIterator<T>
                    template    <class T>   inline  void    DoublyLinkedListIterator<T>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }

                    template    <class T>   inline  DoublyLinkedListIterator<T>::DoublyLinkedListIterator (const DoublyLinkedList<T>& data) :
                        fCurrent (data.fFirst),
                        fSuppressMore (true)
                    {
                    }

                    template    <class T>   inline  DoublyLinkedListIterator<T>::DoublyLinkedListIterator (const DoublyLinkedListIterator<T>& from) :
                        fCurrent (from.fCurrent),
                        fSuppressMore (from.fSuppressMore)
                    {
                    }

                    template    <class T>   inline  DoublyLinkedListIterator<T>&  DoublyLinkedListIterator<T>::operator= (const DoublyLinkedListIterator<T>& rhs)
                    {
                        Invariant ();
                        fCurrent = rhs.fCurrent;
                        fSuppressMore = rhs.fSuppressMore;
                        Invariant ();
                        return (*this);
                    }

                    template    <class T>   inline  bool    DoublyLinkedListIterator<T>::Done () const
                    {
                        Invariant ();
                        return bool (fCurrent == nullptr);
                    }

                    template    <class T>   inline  bool    DoublyLinkedListIterator<T>::More (T* current, bool advance)
                    {
                        Invariant ();

                        if (advance) {
                            /*
                             * We could already be done since after the last Done() call, we could
                             * have done a removeall.
                             */
                            if (not fSuppressMore and fCurrent != nullptr) {
                                fCurrent = fCurrent->fNext;
                            }
                            fSuppressMore = false;
                        }
                        Invariant ();
                        if (current != nullptr and not Done ()) {
                            *current = fCurrent->fItem;
                        }
                        return (not Done ());
                    }

                    template    <class T>   inline  T   DoublyLinkedListIterator<T>::Current () const
                    {
                        Require (not (Done ()));
                        Invariant ();
                        AssertNotNull (fCurrent);
                        return (fCurrent->fItem);
                    }

#if     qDebug
                    template    <typename   T>  void    DoublyLinkedListIterator<T>::Invariant_ () const
                    {
                    }
#endif




                    // class DoublyLinkedList_Patch<T>
                    template    <typename   T>  inline  void    DoublyLinkedList_Patch<T>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
                        InvariantOnIterators_ ();
#endif
                    }

                    template    <typename   T>  inline  DoublyLinkedList_Patch<T>::DoublyLinkedList_Patch () :
                        DoublyLinkedList<T> (),
                        fIterators (nullptr)
                    {
                        Invariant ();
                    }

                    template    <typename   T>  inline  DoublyLinkedList_Patch<T>::DoublyLinkedList_Patch (const DoublyLinkedList_Patch<T>& from) :
                        DoublyLinkedList<T> (from),
                        fIterators (nullptr)    // Don't copy the list of iterators - would be trouble with backpointers!
                        // Could clone but that would do no good, since nobody else would have pointers to them
                    {
                        Invariant ();
                    }

                    template    <typename   T>  inline  DoublyLinkedList_Patch<T>::~DoublyLinkedList_Patch ()
                    {
                        Require (fIterators == nullptr);
                    }

                    template    <typename   T>  inline  bool    DoublyLinkedList_Patch<T>::HasActiveIterators () const
                    {
                        return bool (fIterators != nullptr);
                    }

                    template    <class T>   inline  void    DoublyLinkedList_Patch<T>::PatchViewsAdd (const DoubleLink<T>* link) const
                    {
                        RequireNotNull (link);
                        for (DoublyLinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            v->PatchAdd (link);
                        }
                    }

                    template    <class T>   inline  void    DoublyLinkedList_Patch<T>::PatchViewsRemove (const DoubleLink<T>* link) const
                    {
                        RequireNotNull (link);
                        for (DoublyLinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            v->PatchRemove (link);
                        }
                    }

                    template    <class T>   inline  void    DoublyLinkedList_Patch<T>::PatchViewsRemoveAll () const
                    {
                        for (DoublyLinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            v->PatchRemoveAll ();
                        }
                    }

                    template    <typename   T>  inline  DoublyLinkedList_Patch<T>& DoublyLinkedList_Patch<T>::operator= (const DoublyLinkedList_Patch<T>& rhs)
                    {
                        /*
                         * Don't copy the rhs iterators, and don't do assignments when we have active iterators.
                         * If this is to be supported at some future date, well need to work on our patching.
                         */
                        Assert (not (HasActiveIterators ()));   // cuz copy of DoublyLinkedList does not copy iterators...
                        DoublyLinkedList<T>::operator= (rhs);
                        return (*this);
                    }

                    template    <typename   T>  inline  void    DoublyLinkedList_Patch<T>::Prepend (T item)
                    {
                        Invariant ();
                        DoublyLinkedList<T>::Prepend (item);
                        PatchViewsAdd (this->fFirst);
                        Invariant ();
                    }

					//tmphack - must fix for oduble linked list
                    template    <typename   T>  inline  void    DoublyLinkedList_Patch<T>::Append (T item)
                    {
						if (fLength == 0) {
							Prepend (item);
						}
						else {
							DoubleLink<T>* last = fFirst;
                            for (; last->fNext != nullptr; last = last->fNext)
								;
							Assert (last != nullptr);
							Assert (last->fNext == nullptr);
							last->fNext = new DoubleLink<T> (item, nullptr);
							fLength++;
							PatchViewsAdd (last->fNext);
						}
                    }

                    template    <typename   T>  inline  void    DoublyLinkedList_Patch<T>::RemoveFirst ()
                    {
                        Invariant ();
                        PatchViewsRemove (this->fFirst);
                        DoublyLinkedList<T>::RemoveFirst ();
                        Invariant ();
                    }

                    template    <typename   T>  inline  void    DoublyLinkedList_Patch<T>::RemoveAll ()
                    {
                        Invariant ();
                        DoublyLinkedList<T>::RemoveAll ();
                        PatchViewsRemoveAll ();
                        Invariant ();
                    }

                    template    <typename   T>  void    DoublyLinkedList_Patch<T>::Remove (T item)
                    {
                        Require (this->fLength >= 1);

                        Invariant ();
                        T current;
                        for (DoublyLinkedListMutator_Patch<T> it (*this); it.More (&current, true); ) {
                            if (current == item) {
                                it.RemoveCurrent ();
                                break;
                            }
                        }

                        Invariant ();
                    }


#if     qDebug
                    template    <typename   T>  void    DoublyLinkedList_Patch<T>::Invariant_ () const
                    {
                        DoublyLinkedList<T>::Invariant_ ();
                        /*
                         *      Be sure each iterator points back to us. Thats about all we can test from
                         *  here since we cannot call each iterators Invariant(). That would be
                         *  nice, but sadly when this Invariant_ () is called from DoublyLinkedList<T> the
                         *  iterators themselves may not have been patched, so they'll be out of
                         *  date. Instead, so that in local shadow of Invariant() done in DoublyLinkedList_Patch<T>
                         *  so only called when WE call Invariant().
                         */
                        for (DoublyLinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            Assert (v->fData == this);
                        }
                    }

                    template    <typename   T>  void    DoublyLinkedList_Patch<T>::InvariantOnIterators_ () const
                    {
                        /*
                         *      Only here can we iterate over each iterator and calls its Invariant()
                         *  since now we've completed any needed patching.
                         */
                        for (DoublyLinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            Assert (v->fData == this);
                            v->Invariant ();
                        }
                    }
#endif



                    /*
                    ********************************************************************************
                    ***************************** DoublyLinkedListIterator_Patch<T> **********************
                    ********************************************************************************
                    */
                    template    <class T>
                    inline  DoublyLinkedListIterator_Patch<T>::DoublyLinkedListIterator_Patch (const DoublyLinkedList_Patch<T>& data) :
                        DoublyLinkedListIterator<T> (data),
                        fData (&data),
                        fNext (data.fIterators),
                        fPrev (nullptr)         // means invalid or fData->fFirst == fCurrent ...
                    {
                        const_cast<DoublyLinkedList_Patch<T>*> (&data)->fIterators = this;
                        this->Invariant ();
                    }
                    template    <class T>
                    inline  DoublyLinkedListIterator_Patch<T>::DoublyLinkedListIterator_Patch (const DoublyLinkedListIterator_Patch<T>& from) :
                        DoublyLinkedListIterator<T> (from),
                        fData (from.fData),
                        fNext (from.fData->fIterators),
                        fPrev (from.fPrev)
                    {
                        from.Invariant ();
                        const_cast<DoublyLinkedList_Patch<T>*> (fData)->fIterators = this;
                        this->Invariant ();
                    }
                    template    <class T>
                    inline  DoublyLinkedListIterator_Patch<T>::~DoublyLinkedListIterator_Patch ()
                    {
                        this->Invariant ();
                        AssertNotNull (fData);
                        if (fData->fIterators == this) {
                            const_cast<DoublyLinkedList_Patch<T>*> (fData)->fIterators = fNext;
                        }
                        else {
                            DoublyLinkedListIterator_Patch<T>* v = fData->fIterators;
                            for (; v->fNext != this; v = v->fNext) {
                                AssertNotNull (v);
                                AssertNotNull (v->fNext);
                            }
                            AssertNotNull (v);
                            Assert (v->fNext == this);
                            v->fNext = fNext;
                        }
                    }
                    template    <class T>
                    inline  DoublyLinkedListIterator_Patch<T>&    DoublyLinkedListIterator_Patch<T>::operator= (const DoublyLinkedListIterator_Patch<T>& rhs)
                    {
                        this->Invariant ();

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
                                const_cast<DoublyLinkedList_Patch<T>*>(fData)->fIterators = fNext;
                            }
                            else {
                                DoublyLinkedListIterator_Patch<T>* v = fData->fIterators;
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
                            const_cast<DoublyLinkedList_Patch<T>*> (fData)->fIterators = this;
                        }

                        fData = rhs.fData;
                        fPrev = rhs.fPrev;

                        DoublyLinkedListIterator<T>::operator= (rhs);

                        this->Invariant ();
                        return (*this);
                    }
                    template    <class T>
                    inline  bool    DoublyLinkedListIterator_Patch<T>::More (T* current, bool advance)
                    {
                        this->Invariant ();

                        if (advance) {
                            /*
                             * We could already be done since after the last Done() call, we could
                             * have done a removeall.
                             */
                            if (not this->fSuppressMore and this->fCurrent != nullptr) {
                                fPrev = this->fCurrent;
                                this->fCurrent = this->fCurrent->fNext;

                            }
                            this->fSuppressMore = false;
                        }
                        this->Invariant ();
                        if ((current != nullptr) and (not this->Done ())) {
                            *current = this->fCurrent->fItem;
                        }
                        return (not this->Done ());
                    }
                    template    <class T>
                    inline  void    DoublyLinkedListIterator_Patch<T>::PatchAdd (const DoubleLink<T>* link)
                    {
                        /*
                         *      link is the new link just added. If it was just after current, then
                         *  there is no problem - we will soon hit it. If it was well before current
                         *  (ie before prev) then there is still no problem. If it is the new
                         *  previous, we just adjust our previous.
                         */
                        RequireNotNull (link);
                        if (link->fNext == this->fCurrent) {
                            fPrev = link;
                        }
                    }
                    template    <class T>
                    inline  void    DoublyLinkedListIterator_Patch<T>::PatchRemove (const DoubleLink<T>* link)
                    {
                        RequireNotNull (link);

                        /*
                         *  There are basicly three cases:
                         *
                         *  (1)     We remove the current. In this case, we just advance current to the next
                         *          item (prev is already all set), and set fSuppressMore since we are advanced
                         *          to the next item.
                         *  (2)     We remove our previous. Technically this poses no problems, except then
                         *          our previos pointer is invalid. We could recompute it, but that would
                         *          involve rescanning the list from the beginning - slow. And we probably
                         *          will never need the next pointer (unless we get a remove current call).
                         *          So just set it to nullptr, which conventionally means no valid value.
                         *          It will be recomputed if needed.
                         *  (3)     We are deleting some other value. No probs.
                         */
                        if (this->fCurrent == link) {
                            this->fCurrent = this->fCurrent->fNext;
                            // fPrev remains the same - right now it points to a bad item, since
                            // PatchRemove() called before the actual removal, but right afterwards
                            // it will point to our new fCurrent.
                            this->fSuppressMore = true;         // Since we advanced cursor...
                        }
                        else if (fPrev == link) {
                            fPrev = nullptr;                    // real value recomputed later, if needed
                        }
                    }
                    template    <class T>
                    inline  void    DoublyLinkedListIterator_Patch<T>::PatchRemoveAll ()
                    {
                        this->fCurrent = nullptr;
                        fPrev = nullptr;
                        Ensure (this->Done ());
                    }
#if     qDebug
                    template    <typename   T>
                    void    DoublyLinkedListIterator_Patch<T>::Invariant_ () const
                    {
                        DoublyLinkedListIterator<T>::Invariant_ ();

                        /*
                         *  fPrev could be nullptr, but if it isn't then its next must be fCurrent.
                         */
                        Assert ((fPrev == nullptr) or (fPrev->fNext == this->fCurrent));
                    }
#endif


                    /*
                    ********************************************************************************
                    ****************************** DoublyLinkedListMutator_Patch<T> **********************
                    ********************************************************************************
                    */
                    template    <class T>
                    inline  DoublyLinkedListMutator_Patch<T>::DoublyLinkedListMutator_Patch (DoublyLinkedList_Patch<T>& data) :
                        DoublyLinkedListIterator_Patch<T> ((const DoublyLinkedList_Patch<T>&)data)
                    {
                    }
                    template    <class T>
                    inline  DoublyLinkedListMutator_Patch<T>::DoublyLinkedListMutator_Patch (const DoublyLinkedListMutator_Patch<T>& from) :
                        DoublyLinkedListIterator_Patch<T> ((const DoublyLinkedListIterator_Patch<T>&)from)
                    {
                    }
                    template    <class T>
                    inline  DoublyLinkedListMutator_Patch<T>& DoublyLinkedListMutator_Patch<T>::operator= (DoublyLinkedListMutator_Patch<T>& rhs)
                    {
                        DoublyLinkedListIterator_Patch<T>::operator= ((const DoublyLinkedListIterator_Patch<T>&)rhs);
                        return (*this);
                    }
                    template    <class T>
                    inline  void    DoublyLinkedListMutator_Patch<T>::RemoveCurrent ()
                    {
                        Require (not this->Done ());
                        this->Invariant ();
                        DoubleLink<T>*    victim  = const_cast<DoubleLink<T>*> (this->fCurrent);
                        AssertNotNull (this->fData);
                        this->fData->PatchViewsRemove (victim);
                        Assert (this->fCurrent != victim);              // patching should  have guaranteed this
                        /*
                         *      At this point we need the fPrev pointer. But it may have been lost
                         *  in a patch. If it was, its value will be nullptr (NB: nullptr could also mean
                         *  fCurrent == fData->fFirst). If it is nullptr, recompute. Be careful if it
                         *  is still nullptr, that means update fFirst.
                         */

                        if ((this->fPrev == nullptr) and (this->fData->fFirst != victim)) {
                            AssertNotNull (this->fData->fFirst);    // cuz there must be something to remove current
                            for (this->fPrev = this->fData->fFirst; this->fPrev->fNext != victim; this->fPrev = this->fPrev->fNext) {
                                AssertNotNull (this->fPrev);    // cuz that would mean victim not in DoublyLinkedList!!!
                            }
                        }
                        if (this->fPrev == nullptr) {
                            const_cast<DoublyLinkedList_Patch<T>*> (this->fData)->fFirst = victim->fNext;
                        }
                        else {
                            Assert (this->fPrev->fNext == victim);
                            const_cast<DoubleLink<T>*> (this->fPrev)->fNext = victim->fNext;
                        }
                        const_cast<DoublyLinkedList_Patch<T>*> (this->fData)->fLength--;
                        delete (victim);
                        this->Invariant ();
                        this->fData->Invariant ();  // calls by invariant
                    }
                    template    <class T>
                    inline  void    DoublyLinkedListMutator_Patch<T>::UpdateCurrent (T newValue)
                    {
                        RequireNotNull (this->fCurrent);
                        const_cast<DoubleLink<T>*> (this->fCurrent)->fItem = newValue;
                    }
                    template    <class T>
                    inline  void    DoublyLinkedListMutator_Patch<T>::AddBefore (T newValue)
                    {
                        /*
                         * NB: This code works fine, even if we are done!!!
                         */

                        /*
                         *      At this point we need the fPrev pointer. But it may have been lost
                         *  in a patch. If it was, its value will be nullptr (NB: nullptr could also mean
                         *  fCurrent == fData->fFirst). If it is nullptr, recompute. Be careful if it
                         *  is still nullptr, that means update fFirst.
                         */
                        AssertNotNull (this->fData);
                        if ((this->fPrev == nullptr) and (this->fData->fFirst != nullptr) and (this->fData->fFirst != this->fCurrent)) {
                            for (this->fPrev = this->fData->fFirst; this->fPrev->fNext != this->fCurrent; this->fPrev = this->fPrev->fNext) {
                                AssertNotNull (this->fPrev);    // cuz that would mean fCurrent not in DoublyLinkedList!!!
                            }
                        }
                        if (this->fPrev == nullptr) {
                            Assert (this->fData->fFirst == this->fCurrent);     // could be nullptr, or not...
                            const_cast<DoublyLinkedList_Patch<T>*> (this->fData)->fFirst = new DoubleLink<T> (newValue, this->fData->fFirst);
                            const_cast<DoublyLinkedList_Patch<T>*> (this->fData)->fLength++;
                            this->fData->PatchViewsAdd (this->fData->fFirst);       // Will adjust fPrev
                        }
                        else {
                            Assert (this->fPrev->fNext == this->fCurrent);
                            const_cast<DoubleLink<T>*>(this->fPrev)->fNext = new DoubleLink<T> (newValue, this->fPrev->fNext);
                            const_cast<DoublyLinkedList_Patch<T>*> (this->fData)->fLength++;
                            this->fData->PatchViewsAdd (this->fPrev->fNext);        // Will adjust fPrev
                        }
                        this->fData->Invariant ();  // will call this's Invariant()
                    }
                    template    <class T>
                    inline  void    DoublyLinkedListMutator_Patch<T>::AddAfter (T newValue)
                    {
                        Require (not this->Done ());
                        AssertNotNull (this->fCurrent); // since not done...
                        const_cast<DoubleLink<T>*>(this->fCurrent)->fNext = new DoubleLink<T> (newValue, this->fCurrent->fNext);
                        const_cast<DoublyLinkedList_Patch<T>*> (this->fData)->fLength++;
                        this->fData->PatchViewsAdd (this->fCurrent->fNext);
                    }

                }
            }
        }
    }
}


#endif /* _Stroika_Foundation_Containers_Concrete_Private_DoublyLinkedList_inl_ */
