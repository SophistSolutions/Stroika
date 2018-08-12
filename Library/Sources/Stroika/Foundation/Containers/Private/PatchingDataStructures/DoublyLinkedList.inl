/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_inl_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_inl_ 1

#include "../../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::Private::PatchingDataStructures {

    /*
     ********************************************************************************
     ******************** PatchingDataStructures::DoublyLinkedList<T> ***************
     ********************************************************************************
     */
    template <typename T>
    inline DoublyLinkedList<T>::DoublyLinkedList ()
        : inherited ()
    {
        Invariant ();
    }
    template <typename T>
    inline DoublyLinkedList<T>::DoublyLinkedList (DoublyLinkedList<T>* rhs, IteratorOwnerID newOwnerID)
        : inherited (rhs, newOwnerID, (ForwardIterator*)nullptr)
    {
        RequireNotNull (rhs);
        rhs->Invariant ();
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::Invariant () const
    {
#if qDebug
        Invariant_ ();
        InvariantOnIterators_ ();
#endif
    }
    template <typename T>
    inline void DoublyLinkedList<T>::PatchViewsAdd (const Link* link) const
    {
        RequireNotNull (link);
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([link](ForwardIterator* ai) {
            ai->PatchAdd (link);
        });
    }
    template <typename T>
    inline void DoublyLinkedList<T>::PatchViewsRemove (const Link* link) const
    {
        RequireNotNull (link);
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([link](ForwardIterator* ai) {
            ai->PatchRemove (link);
        });
    }
    template <typename T>
    inline void DoublyLinkedList<T>::PatchViewsRemoveAll () const
    {
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([](ForwardIterator* ai) {
            ai->PatchRemoveAll ();
        });
    }
    template <typename T>
    inline void DoublyLinkedList<T>::TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch) const
    {
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([oldI, items2Patch](ForwardIterator* ai) {
            ai->TwoPhaseIteratorPatcherPass1 (oldI, items2Patch);
        });
    }
    template <typename T>
    inline void DoublyLinkedList<T>::TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<ForwardIterator*>* items2Patch, Link* newI) const
    {
        for (size_t i = 0; i < items2Patch->GetSize (); ++i) {
            (*items2Patch)[i]->TwoPhaseIteratorPatcherPass2 (newI);
        }
    }
    template <typename T>
    inline void DoublyLinkedList<T>::Prepend (ArgByValueType<T> item)
    {
        Invariant ();
        inherited::Prepend (item);
        PatchViewsAdd (this->_fHead);
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::Append (ArgByValueType<T> item)
    {
        inherited::Append (item);
        PatchViewsAdd (this->_fTail);
    }
    template <typename T>
    inline void DoublyLinkedList<T>::RemoveFirst ()
    {
        Invariant ();
        PatchViewsRemove (this->_fHead);
        inherited::RemoveFirst ();
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::RemoveAll ()
    {
        Invariant ();
        inherited::RemoveAll ();
        PatchViewsRemoveAll ();
        Invariant ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    inline void DoublyLinkedList<T>::Remove (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer)
    {
        Invariant ();
        T current;
        for (ForwardIterator it (*this); it.More (&current, true);) {
            if (equalsComparer (current, item)) {
                RemoveAt (it);
                break;
            }
        }
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::RemoveAt (const ForwardIterator& i)
    {
        Require (not i.Done ());
        Invariant ();
        Memory::SmallStackBuffer<ForwardIterator*> items2Patch (0);
        TwoPhaseIteratorPatcherPass1 (const_cast<Link*> (i._fCurrent), &items2Patch);
        Link* next = i._fCurrent->fNext;
        this->inherited::RemoveAt (i);
        TwoPhaseIteratorPatcherPass2 (&items2Patch, next);
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::AddBefore (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        Invariant ();
        const Link* prev       = i._fCurrent == nullptr ? nullptr : i._fCurrent->fPrev;
        bool        isPrevNull = (prev == nullptr);
        inherited::AddBefore (i, newValue);
        /// WAG - VERY LIKELY WRONG BELOIW - MUST CLENAUP - LGP -2013-06-17
        if (isPrevNull) {
            this->PatchViewsAdd (this->_fHead); // Will adjust fPrev
        }
        else {
            this->PatchViewsAdd (prev->fNext); // Will adjust fPrev
        }
        Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::AddAfter (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        Invariant ();
        inherited::AddAfter (i, newValue);
        this->PatchViewsAdd (i._fCurrent->fNext);
        Invariant ();
    }
#if qDebug
    template <typename T>
    void DoublyLinkedList<T>::Invariant_ () const
    {
        inherited::Invariant_ ();
        /*
            *      Be sure each iterator points back to us. Thats about all we can test from
            *  here since we cannot call each iterators Invariant(). That would be
            *  nice, but sadly when this Invariant_ () is called from DoublyLinkedList<T> the
            *  iterators themselves may not have been patched, so they'll be out of
            *  date. Instead, so that in local shadow of Invariant() done in DoublyLinkedList<T>
            *  so only called when WE call Invariant().
            */
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([this](ForwardIterator* ai) {
            Assert (ai->_fData == this);
        });
    }
    template <typename T>
    void DoublyLinkedList<T>::InvariantOnIterators_ () const
    {
        /*
            *      Only here can we iterate over each iterator and calls its Invariant()
            *  since now we've completed any needed patching.
            */
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([this](ForwardIterator* ai) {
            Assert (ai->_fData == this);
            ai->Invariant ();
        });
    }
#endif

    /*
     ********************************************************************************
     **** PatchingDataStructures::DoublyLinkedList<T>::ForwardIterator ******
     ********************************************************************************
     */
    template <typename T>
    inline DoublyLinkedList<T>::ForwardIterator::ForwardIterator (IteratorOwnerID ownerID, const DoublyLinkedList<T>* data)
        : inherited_DataStructure (data)
        , inherited_PatchHelper (const_cast<DoublyLinkedList<T>*> (data), ownerID)
    {
        RequireNotNull (data);
        this->Invariant ();
    }
    template <typename T>
    inline DoublyLinkedList<T>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
        : inherited_DataStructure (from)
        , inherited_PatchHelper (from)
    {
        from.Invariant ();
        this->Invariant ();
    }
    template <typename T>
    inline DoublyLinkedList<T>::ForwardIterator::~ForwardIterator ()
    {
        this->Invariant ();
    }
    template <typename T>
    inline void DoublyLinkedList<T>::ForwardIterator::PatchAdd (const Link* link)
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
    template <typename T>
    inline void DoublyLinkedList<T>::ForwardIterator::PatchRemove (const Link* link)
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
         *          So just set it to nullptr, which conventionally means no valid  value.
         *          It will be recomputed if needed. 
         *  (3)     We are deleting some other value. No probs.
         */
        if (this->_fCurrent == link) {
            this->_fCurrent = this->_fCurrent->fNext;
            // fPrev remains the same - right now it points to a bad item, since
            // PatchRemove() called before the actual removal, but right afterwards
            // it will point to our new _fCurrent.
            this->_fSuppressMore = true; // Since we advanced cursor...
        }
#if 0
        else if (fPrev == link) {
            fPrev = nullptr;                    // real value recomputed later, if needed
        }
#endif
    }
    template <typename T>
    inline void DoublyLinkedList<T>::ForwardIterator::PatchRemoveAll ()
    {
        this->_fCurrent = nullptr;
        //                        fPrev = nullptr;
        Ensure (this->Done ());
    }
    template <typename T>
    inline void DoublyLinkedList<T>::ForwardIterator::TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch)
    {
        if (this->_fCurrent == oldI) {
            items2Patch->push_back (this);
        }
    }
    template <typename T>
    inline void DoublyLinkedList<T>::ForwardIterator::TwoPhaseIteratorPatcherPass2 (Link* newI)
    {
        this->_fSuppressMore = true;
        this->_fCurrent      = newI;
    }
#if qDebug
    template <typename T>
    void DoublyLinkedList<T>::ForwardIterator::Invariant_ () const
    {
        inherited_DataStructure::Invariant_ ();
        //inherited_PatchHelper::Invariant_ ();

        /*
         *  fPrev could be nullptr, but if it isn't then its next must be _fCurrent.
         */
        //Assert ((fPrev == nullptr) or (fPrev->fNext == this->_fCurrent));
    }
#endif

}

#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_inl_ */
