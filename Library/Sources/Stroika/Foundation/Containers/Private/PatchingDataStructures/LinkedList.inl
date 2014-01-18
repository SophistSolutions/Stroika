/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
                    ****************************** LinkedList<T, TRAITS> ***************************
                    ********************************************************************************
                    */
                    template      <typename  T, typename TRAITS>
                    inline  LinkedList<T, TRAITS>::LinkedList ()
                        : inherited ()
                        , fActiveIteratorsListHead_ (nullptr)
                    {
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    inline  LinkedList<T, TRAITS>::LinkedList (const LinkedList<T, TRAITS>& from)
                        : inherited (from)
                        // Don't copy the list of iterators - would be trouble with backpointers!
                        // Could clone but that would do no good, since nobody else would have pointers to them
                        , fActiveIteratorsListHead_ (nullptr)
                    {
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    inline  LinkedList<T, TRAITS>::~LinkedList ()
                    {
                        Require (fActiveIteratorsListHead_ == nullptr);
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
                        InvariantOnIterators_ ();
#endif
                    }
                    template      <typename  T, typename TRAITS>
                    inline  bool    LinkedList<T, TRAITS>::HasActiveIterators () const
                    {
                        return bool (fActiveIteratorsListHead_ != nullptr);
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::PatchViewsAdd (const Link* link) const
                    {
                        RequireNotNull (link);
                        for (auto v = fActiveIteratorsListHead_; v != nullptr; v = v->_fNextActiveIterator) {
                            v->PatchAdd (link);
                        }
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::PatchViewsRemove (const Link* link) const
                    {
                        RequireNotNull (link);
                        for (auto v = fActiveIteratorsListHead_; v != nullptr; v = v->_fNextActiveIterator) {
                            v->PatchRemove (link);
                        }
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::PatchViewsRemoveAll () const
                    {
                        for (auto v = fActiveIteratorsListHead_; v != nullptr; v = v->_fNextActiveIterator) {
                            v->PatchRemoveAll ();
                        }
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch) const
                    {
                        for (auto ai = fActiveIteratorsListHead_; ai != nullptr; ai = ai->_fNextActiveIterator) {
                            ai->TwoPhaseIteratorPatcherPass1 (oldI, items2Patch);
                        }
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<ForwardIterator*>* items2Patch, Link* newI) const
                    {
                        for (size_t i = 0; i < items2Patch->GetSize (); ++i) {
                            (*items2Patch)[i]->TwoPhaseIteratorPatcherPass2 (newI);
                        }
                    }
                    template      <typename  T, typename TRAITS>
                    inline  LinkedList<T, TRAITS>& LinkedList<T, TRAITS>::operator= (const LinkedList<T, TRAITS>& rhs)
                    {
                        /*
                         * Don't copy the rhs iterators, and don't do assignments when we have active iterators.
                         * If this is to be supported at some future date, well need to work on our patching.
                         */
                        Assert (not (HasActiveIterators ()));   // cuz copy of LinkedList does not copy iterators...
                        inherited::operator= (rhs);
                        return *this;
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::Prepend (T item)
                    {
                        Invariant ();
                        inherited::Prepend (item);
                        PatchViewsAdd (this->_fHead);
                        Invariant ();
                    }
                    //tmphack - must fix to have in  base class and just hook for patch here
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::Append (T item)
                    {
                        if (this->_fHead == nullptr) {
                            Prepend (item);
                        }
                        else {
                            Link* last = this->_fHead;
                            for (; last->fNext != nullptr; last = last->fNext)
                                ;
                            Assert (last != nullptr);
                            Assert (last->fNext == nullptr);
                            last->fNext = new Link (item, nullptr);
                            PatchViewsAdd (last->fNext);
                        }
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::RemoveFirst ()
                    {
                        Invariant ();
                        PatchViewsRemove (this->_fHead);
                        inherited::RemoveFirst ();
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::RemoveAll ()
                    {
                        Invariant ();
                        inherited::RemoveAll ();
                        PatchViewsRemoveAll ();
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::Remove (T item)
                    {
                        using   Traversal::kUnknownIteratorOwnerID;
                        /*
                         *  Base class impl is fine, but doesn't do patching, and doesn't
                         *  provide the hooks so I can do the patching from here.
                         *
                         *  @todo   We may want to correct that (see STL container impl -
                         *  returning ptr to next node would do it).
                         */
                        Invariant ();
                        Memory::Optional<T> current;
                        for (ForwardIterator it (kUnknownIteratorOwnerID, this); it.More (&current, true), current.IsPresent (); ) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (*current, item)) {
                                this->RemoveAt (it);
                                break;
                            }
                        }
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::RemoveAt (const ForwardIterator& i)
                    {
                        Require (not i.Done ());
                        Invariant ();

                        Memory::SmallStackBuffer<ForwardIterator*>   items2Patch (0);
                        TwoPhaseIteratorPatcherPass1 (const_cast<Link*> (i._fCurrent), &items2Patch);
                        Link*   next = i._fCurrent->fNext;
                        this->inherited::RemoveAt (i);
                        TwoPhaseIteratorPatcherPass2 (&items2Patch, next);

                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::AddBefore (const ForwardIterator& i, T newValue)
                    {
                        Invariant ();

                        //tmphack
                        const Link*     prev = nullptr;
                        if ((this->_fHead != nullptr) and (this->_fHead != i._fCurrent)) {
                            for (prev = this->_fHead; prev->fNext != i._fCurrent; prev = prev->fNext) {
                                AssertNotNull (prev);    // cuz that would mean _fCurrent not in DoublyLinkedList!!!
                            }
                        }

                        bool    isPrevNull = (prev == nullptr);
                        inherited::AddBefore (i, newValue);
                        /// WAG - VERY LIKELY WRONG BELOIW - MUST CLENAUP - LGP -2013-06-17
                        if (isPrevNull) {
                            this->PatchViewsAdd (this->_fHead);       // Will adjust fPrev
                        }
                        else {
                            this->PatchViewsAdd (prev->fNext);       // Will adjust fPrev
                        }

                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::AddAfter (const ForwardIterator& i, T newValue)
                    {
                        Invariant ();

                        inherited::AddAfter (i, newValue);
                        this->PatchViewsAdd (i._fCurrent->fNext);

                        Invariant ();
                    }
#if     qDebug
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted)
                    {
                        for (auto v = fActiveIteratorsListHead_; v != nullptr; v = v->_fNextActiveIterator) {
                            Assert (v->fOwnerID != oBeingDeleted);
                        }
                    }
#endif
#if     qDebug
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::Invariant_ () const
                    {
                        inherited::Invariant_ ();
                        /*
                         *      Be sure each iterator points back to us. Thats about all we can test from
                         *  here since we cannot call each iterators Invariant(). That would be
                         *  nice, but sadly when this Invariant_ () is called from LinkedList<T,TRAITS> the
                         *  iterators themselves may not have been patched, so they'll be out of
                         *  date. Instead, so that in local shadow of Invariant() done in LinkedList<T, TRAITS>
                         *  so only called when WE call Invariant().
                         */
                        for (auto v = fActiveIteratorsListHead_; v != nullptr; v = v->_fNextActiveIterator) {
                            Assert (v->_fData == this);
                        }
                    }
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::InvariantOnIterators_ () const
                    {
                        /*
                         *      Only here can we iterate over each iterator and calls its Invariant()
                         *  since now we've completed any needed patching.
                         */
                        for (auto v = fActiveIteratorsListHead_; v != nullptr; v = v->_fNextActiveIterator) {
                            Assert (v->_fData == this);
                            v->Invariant ();
                        }
                    }
#endif


                    /*
                    ********************************************************************************
                    ********************** LinkedList<T, TRAITS>::ForwardIterator ******************
                    ********************************************************************************
                    */
                    template      <typename  T, typename TRAITS>
                    inline  LinkedList<T, TRAITS>::ForwardIterator::ForwardIterator (IteratorOwnerID ownerID, const LinkedList<T, TRAITS>* data)
                        : inherited (data)
                        , fOwnerID (ownerID)
                        , _fNextActiveIterator (data->fActiveIteratorsListHead_)
                        //, fPrev (nullptr)         // means invalid or fData->_fHead == _fCurrent ...
                    {
                        RequireNotNull (data);
                        const_cast<LinkedList<T, TRAITS>*> (data)->fActiveIteratorsListHead_ = this;
                        this->Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    inline  LinkedList<T, TRAITS>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
                        : inherited (from)
                        , fOwnerID (from.fOwnerID)
                        , _fNextActiveIterator (from.GetPatchingContainer_ ().fActiveIteratorsListHead_)
                        //, fPrev (from.fPrev)
                    {
                        from.Invariant ();
                        GetPatchingContainer_ ().fActiveIteratorsListHead_ = this;
                        this->Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    inline  LinkedList<T, TRAITS>::ForwardIterator::~ForwardIterator ()
                    {
                        this->Invariant ();
                        if (GetPatchingContainer_ ().fActiveIteratorsListHead_ == this) {
                            GetPatchingContainer_ ().fActiveIteratorsListHead_ = _fNextActiveIterator;
                        }
                        else {
                            auto    v = GetPatchingContainer_ ().fActiveIteratorsListHead_;
                            for (; v->_fNextActiveIterator != this; v = v->_fNextActiveIterator) {
                                AssertNotNull (v);
                                AssertNotNull (v->_fNextActiveIterator);
                            }
                            AssertNotNull (v);
                            Assert (v->_fNextActiveIterator == this);
                            v->_fNextActiveIterator = _fNextActiveIterator;
                        }
                    }
                    template      <typename  T, typename TRAITS>
                    inline  typename LinkedList<T, TRAITS>::ForwardIterator&    LinkedList<T, TRAITS>::ForwardIterator::operator= (const ForwardIterator& rhs)
                    {
                        this->Invariant ();

                        /*
                         *      If the fData field has not changed, then we can leave alone our iterator linkage.
                         *  Otherwise, we must remove ourselves from the old, and add ourselves to the new.
                         */
                        if (&GetPatchingContainer_ () != &rhs.GetPatchingContainer_ ()) {

                            /*
                             * Remove from old.
                             */
                            if (GetPatchingContainer_ ().fActiveIteratorsListHead_ == this) {
                                GetPatchingContainer_ ().fActiveIteratorsListHead_ = _fNextActiveIterator;
                            }
                            else {
                                auto v = GetPatchingContainer_ ().fActiveIteratorsListHead_;
                                for (; v->_fNextActiveIterator != this; v = v->_fNextActiveIterator) {
                                    AssertNotNull (v);
                                    AssertNotNull (v->_fNextActiveIterator);
                                }
                                AssertNotNull (v);
                                Assert (v->_fNextActiveIterator == this);
                                v->_fNextActiveIterator = _fNextActiveIterator;
                            }

                            /*
                             * Add to new.
                             */
                            _fNextActiveIterator = rhs.GetPatchingContainer_ ().fActiveIteratorsListHead_;
                            GetPatchingContainer_ ().fActiveIteratorsListHead_ = this;
                        }

                        //fPrev = rhs.fPrev;

                        fOwnerID = rhs.fOwnerID;
                        inherited::operator= (rhs);

                        this->Invariant ();
                        return *this;
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::ForwardIterator::PatchAdd (const Link* link)
                    {
                        /*
                         *      link is the new link just added. If it was just after current, then
                         *  there is no problem - we will soon hit it. If it was well before current
                         *  (ie before prev) then there is still no problem. If it is the new
                         *  previous, we just adjust our previous.
                         */
                        RequireNotNull (link);
                        if (link->fNext == this->_fCurrent) {
                            //fPrev = link;
                        }
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::ForwardIterator::PatchRemove (const Link* link)
                    {
                        RequireNotNull (link);

                        /*
                         *  There are basicly three cases:
                         *
                         *  (1)     We remove the current. In this case, we just advance current to the next
                         *          item (prev is already all set), and set _fSuppressMore since we are advanced
                         *          to the next item.
                         *  (2)     We remove our previous. Technically this poses no problems, except then
                         *          our previos pointer is invalid. We could recompute it, but that would
                         *          involve rescanning the list from the beginning - slow. And we probably
                         *          will never need the next pointer (unless we get a remove current call).
                         *          So just set it to nullptr, which conventionally means no valid value.
                         *          It will be recomputed if needed.
                         *  (3)     We are deleting some other value. No probs.
                         */
                        if (this->_fCurrent == link) {
                            this->_fCurrent = this->_fCurrent->fNext;
                            // fPrev remains the same - right now it points to a bad item, since
                            // PatchRemove() called before the actual removal, but right afterwards
                            // it will point to our new _fCurrent.
                            this->_fSuppressMore = true;         // Since we advanced cursor...
                        }
                        //else if (fPrev == link) {
                        //    fPrev = nullptr;                    // real value recomputed later, if needed
                        //}
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::ForwardIterator::PatchRemoveAll ()
                    {
                        this->_fCurrent = nullptr;
                        //fPrev = nullptr;
                        Ensure (this->Done ());
                    }
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::ForwardIterator::TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch)
                    {
                        if (this->_fCurrent == oldI) {
                            items2Patch->push_back (this);
                        }
                    }
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::ForwardIterator::TwoPhaseIteratorPatcherPass2 (Link* newI)
                    {
                        this->_fSuppressMore = true;
                        this->_fCurrent = newI;
                    }
                    template      <typename  T, typename TRAITS>
                    inline  const typename LinkedList<T, TRAITS>::ForwardIterator::ContainerType&  LinkedList<T, TRAITS>::ForwardIterator::GetPatchingContainer_ () const
                    {
                        AssertMember (this->_fData, ContainerType);
                        return *static_cast<const ContainerType*> (this->_fData);
                    }
                    template      <typename  T, typename TRAITS>
                    inline  typename LinkedList<T, TRAITS>::ForwardIterator::ContainerType&    LinkedList<T, TRAITS>::ForwardIterator::GetPatchingContainer_ ()
                    {
                        AssertMember (this->_fData, ContainerType);
                        return *static_cast<ContainerType*> (const_cast<DataStructures::LinkedList<T, TRAITS>*> (this->_fData));
                    }
#if     qDebug
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::ForwardIterator::Invariant_ () const
                    {
                        inherited::Invariant_ ();

                        /*
                         *  fPrev could be nullptr, but if it isn't then its next must be _fCurrent.
                         */
                        //Assert ((fPrev == nullptr) or (fPrev->fNext == this->_fCurrent));
                    }
#endif


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_inl_ */
