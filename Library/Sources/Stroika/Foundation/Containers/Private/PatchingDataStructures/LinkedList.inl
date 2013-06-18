/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_inl_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_inl_  1


#include    "../../../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Private {
                namespace   PatchingDataStructures {


                    /*
                    ********************************************************************************
                    ***************************** LinkedList_Patch<T> ******************************
                    ********************************************************************************
                    */
                    template    <typename   T>
                    inline  void    LinkedList_Patch<T>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
                        InvariantOnIterators_ ();
#endif
                    }
                    template    <typename   T>
                    inline  LinkedList_Patch<T>::LinkedList_Patch ()
                        : inherited ()
                        , fIterators (nullptr)
                    {
                        Invariant ();
                    }
                    template    <typename   T>
                    inline  LinkedList_Patch<T>::LinkedList_Patch (const LinkedList_Patch<T>& from)
                        : inherited (from)
                        , fIterators (nullptr)    // Don't copy the list of iterators - would be trouble with backpointers!
                        // Could clone but that would do no good, since nobody else would have pointers to them
                    {
                        Invariant ();
                    }
                    template    <typename   T>
                    inline  LinkedList_Patch<T>::~LinkedList_Patch ()
                    {
                        Require (fIterators == nullptr);
                    }
                    template    <typename   T>
                    inline  bool    LinkedList_Patch<T>::HasActiveIterators () const
                    {
                        return bool (fIterators != nullptr);
                    }
                    template    <typename   T>
                    inline  void    LinkedList_Patch<T>::PatchViewsAdd (const Link* link) const
                    {
                        RequireNotNull (link);
                        for (LinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            v->PatchAdd (link);
                        }
                    }
                    template    <typename   T>
                    inline  void    LinkedList_Patch<T>::PatchViewsRemove (const Link* link) const
                    {
                        RequireNotNull (link);
                        for (LinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            v->PatchRemove (link);
                        }
                    }
                    template    <typename   T>
                    inline  void    LinkedList_Patch<T>::PatchViewsRemoveAll () const
                    {
                        for (LinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            v->PatchRemoveAll ();
                        }
                    }
                    template    <typename   T>
                    inline  void    LinkedList_Patch<T>::TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<LinkedListIterator_Patch<T>*>* items2Patch) const
                    {
                        for (auto ai = fIterators; ai != nullptr; ai = ai->fNext) {
                            ai->TwoPhaseIteratorPatcherPass1 (oldI, items2Patch);
                        }
                    }
                    template    <typename   T>
                    inline  void    LinkedList_Patch<T>::TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<LinkedListIterator_Patch<T>*>* items2Patch, Link* newI)
                    {
                        for (size_t i = 0; i < items2Patch->GetSize (); ++i) {
                            (*items2Patch)[i]->TwoPhaseIteratorPatcherPass2 (newI);
                        }
                    }
                    template    <typename   T>
                    inline  LinkedList_Patch<T>& LinkedList_Patch<T>::operator= (const LinkedList_Patch<T>& rhs)
                    {
                        /*
                         * Don't copy the rhs iterators, and don't do assignments when we have active iterators.
                         * If this is to be supported at some future date, well need to work on our patching.
                         */
                        Assert (not (HasActiveIterators ()));   // cuz copy of LinkedList does not copy iterators...
                        inherited::operator= (rhs);
                        return (*this);
                    }
                    template    <typename   T>
                    inline  void    LinkedList_Patch<T>::Prepend (T item)
                    {
                        Invariant ();
                        inherited::Prepend (item);
                        PatchViewsAdd (this->fFirst);
                        Invariant ();
                    }
                    //tmphack - must fix to have in  base class and just hook for patch here
                    template    <typename   T>
                    inline  void    LinkedList_Patch<T>::Append (T item)
                    {
                        if (this->fFirst == nullptr) {
                            Prepend (item);
                        }
                        else {
                            Link* last = this->fFirst;
                            for (; last->fNext != nullptr; last = last->fNext)
                                ;
                            Assert (last != nullptr);
                            Assert (last->fNext == nullptr);
                            last->fNext = new Link (item, nullptr);
                            PatchViewsAdd (last->fNext);
                        }
                    }
                    template    <typename   T>
                    inline  void    LinkedList_Patch<T>::RemoveFirst ()
                    {
                        Invariant ();
                        PatchViewsRemove (this->fFirst);
                        inherited::RemoveFirst ();
                        Invariant ();
                    }
                    template    <typename   T>
                    inline  void    LinkedList_Patch<T>::RemoveAll ()
                    {
                        Invariant ();
                        inherited::RemoveAll ();
                        PatchViewsRemoveAll ();
                        Invariant ();
                    }
                    template    <typename   T>
                    void    LinkedList_Patch<T>::Remove (T item)
                    {
                        Invariant ();
                        T current;
                        for (LinkedListIterator_Patch<T> it (*this); it.More (&current, true); ) {
                            if (current == item) {
                                this->RemoveAt (it);
                                break;
                            }
                        }
                        Invariant ();
                    }
                    template    <typename   T>
                    void    LinkedList_Patch<T>::RemoveAt (const ForwardIterator& i)
                    {
                        Require (not i.Done ());
                        Invariant ();
                        Memory::SmallStackBuffer<LinkedListIterator_Patch<T>*>   items2Patch (0);
                        TwoPhaseIteratorPatcherPass1 (const_cast<Link*> (i.fCurrent), &items2Patch);
                        Link*   next = i.fCurrent->fNext;
                        this->inherited::RemoveAt (i);
                        TwoPhaseIteratorPatcherPass2 (&items2Patch, next);
                        Invariant ();
                    }
                    template    <typename   T>
                    void    LinkedList_Patch<T>::AddBefore (const ForwardIterator& i, T newValue)
                    {
                        Invariant ();

                        //tmphack
                        const Link*     prev = nullptr;
                        if ((this->fFirst != nullptr) and (this->fFirst != i.fCurrent)) {
                            for (prev = this->fFirst; prev->fNext != i.fCurrent; prev = prev->fNext) {
                                AssertNotNull (prev);    // cuz that would mean _fCurrent not in DoublyLinkedList!!!
                            }
                        }

                        bool    isPrevNull = (prev == nullptr);
                        inherited::AddBefore (i, newValue);
                        /// WAG - VERY LIKELY WRONG BELOIW - MUST CLENAUP - LGP -2013-06-17
                        if (isPrevNull) {
                            this->PatchViewsAdd (this->fFirst);       // Will adjust fPrev
                        }
                        else {
                            this->PatchViewsAdd (prev->fNext);       // Will adjust fPrev
                        }
                        Invariant ();
                    }
                    template    <typename   T>
                    void    LinkedList_Patch<T>::AddAfter (const ForwardIterator& i, T newValue)
                    {
                        Invariant ();
                        inherited::AddAfter (i, newValue);
                        this->PatchViewsAdd (i.fCurrent->fNext);
                        Invariant ();
                    }
#if     qDebug
                    template    <typename   T>
                    void    LinkedList_Patch<T>::Invariant_ () const
                    {
                        inherited::Invariant_ ();
                        /*
                         *      Be sure each iterator points back to us. Thats about all we can test from
                         *  here since we cannot call each iterators Invariant(). That would be
                         *  nice, but sadly when this Invariant_ () is called from LinkedList<T> the
                         *  iterators themselves may not have been patched, so they'll be out of
                         *  date. Instead, so that in local shadow of Invariant() done in LinkedList_Patch<T>
                         *  so only called when WE call Invariant().
                         */
                        for (LinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            Assert (v->fData == this);
                        }
                    }
                    template    <typename   T>
                    void    LinkedList_Patch<T>::InvariantOnIterators_ () const
                    {
                        /*
                         *      Only here can we iterate over each iterator and calls its Invariant()
                         *  since now we've completed any needed patching.
                         */
                        for (LinkedListIterator_Patch<T>* v = fIterators; v != nullptr; v = v->fNext) {
                            Assert (v->fData == this);
                            v->Invariant ();
                        }
                    }
#endif


                    /*
                    ********************************************************************************
                    ***************************** LinkedListIterator_Patch<T> **********************
                    ********************************************************************************
                    */
                    template    <typename   T>
                    inline  LinkedListIterator_Patch<T>::LinkedListIterator_Patch (const LinkedList_Patch<T>& data)
                        : inherited (data)
                        , fData (&data)
                        , fNext (data.fIterators)
                        , fPrev (nullptr)         // means invalid or fData->fFirst == fCurrent ...
                    {
                        const_cast<LinkedList_Patch<T>*> (&data)->fIterators = this;
                        this->Invariant ();
                    }
                    template    <typename   T>
                    inline  LinkedListIterator_Patch<T>::LinkedListIterator_Patch (const LinkedListIterator_Patch<T>& from)
                        : inherited (from)
                        , fData (from.fData)
                        , fNext (from.fData->fIterators)
                        , fPrev (from.fPrev)
                    {
                        from.Invariant ();
                        const_cast<LinkedList_Patch<T>*> (fData)->fIterators = this;
                        this->Invariant ();
                    }
                    template    <typename   T>
                    inline  LinkedListIterator_Patch<T>::~LinkedListIterator_Patch ()
                    {
                        this->Invariant ();
                        AssertNotNull (fData);
                        if (fData->fIterators == this) {
                            const_cast<LinkedList_Patch<T>*> (fData)->fIterators = fNext;
                        }
                        else {
                            LinkedListIterator_Patch<T>* v = fData->fIterators;
                            for (; v->fNext != this; v = v->fNext) {
                                AssertNotNull (v);
                                AssertNotNull (v->fNext);
                            }
                            AssertNotNull (v);
                            Assert (v->fNext == this);
                            v->fNext = fNext;
                        }
                    }
                    template    <typename   T>
                    inline  LinkedListIterator_Patch<T>&    LinkedListIterator_Patch<T>::operator= (const LinkedListIterator_Patch<T>& rhs)
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
                                const_cast<LinkedList_Patch<T>*>(fData)->fIterators = fNext;
                            }
                            else {
                                LinkedListIterator_Patch<T>* v = fData->fIterators;
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
                            const_cast<LinkedList_Patch<T>*> (fData)->fIterators = this;
                        }

                        fData = rhs.fData;
                        fPrev = rhs.fPrev;

                        inherited::operator= (rhs);

                        this->Invariant ();
                        return (*this);
                    }
                    template    <typename   T>
                    inline  bool    LinkedListIterator_Patch<T>::More (T* current, bool advance)
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
                    template    <typename   T>
                    inline  void    LinkedListIterator_Patch<T>::PatchAdd (const Link* link)
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
                    template    <typename   T>
                    inline  void    LinkedListIterator_Patch<T>::PatchRemove (const Link* link)
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
                    template    <typename   T>
                    inline  void    LinkedListIterator_Patch<T>::PatchRemoveAll ()
                    {
                        this->fCurrent = nullptr;
                        fPrev = nullptr;
                        Ensure (this->Done ());
                    }
                    template    <typename T>
                    void    LinkedListIterator_Patch<T>::TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<LinkedListIterator_Patch<T>*>* items2Patch)
                    {
                        if (this->fCurrent == oldI) {
                            items2Patch->push_back (this);
                        }
                    }
                    template    <typename T>
                    void    LinkedListIterator_Patch<T>::TwoPhaseIteratorPatcherPass2 (Link* newI)
                    {
                        this->fSuppressMore = true;
                        this->fCurrent = newI;
                    }
#if     qDebug
                    template    <typename   T>
                    void    LinkedListIterator_Patch<T>::Invariant_ () const
                    {
                        inherited::Invariant_ ();

                        /*
                         *  fPrev could be nullptr, but if it isn't then its next must be fCurrent.
                         */
                        Assert ((fPrev == nullptr) or (fPrev->fNext == this->fCurrent));
                    }
#endif


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_inl_ */
