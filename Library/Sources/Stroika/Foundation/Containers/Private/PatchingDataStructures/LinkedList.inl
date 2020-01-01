/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_inl_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_inl_ 1

#include "../../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::Private::PatchingDataStructures {

    /*
     ********************************************************************************
     ************************************ LinkedList<T> *****************************
     ********************************************************************************
     */
    template <typename T>
    inline LinkedList<T>::LinkedList ()
        : inherited ()
    {
        Invariant ();
    }
    template <typename T>
    inline LinkedList<T>::LinkedList (LinkedList<T>* rhs, IteratorOwnerID newOwnerID)
        : inherited (rhs, newOwnerID, (ForwardIterator*)nullptr)
    {
        RequireNotNull (rhs);
        rhs->Invariant ();
        Invariant ();
    }
    template <typename T>
    inline void LinkedList<T>::Invariant () const
    {
#if qDebug
        Invariant_ ();
        InvariantOnIterators_ ();
#endif
    }
    template <typename T>
    inline void LinkedList<T>::PatchViewsAdd (const Link* link) const
    {
        RequireNotNull (link);
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([link] (ForwardIterator* ai) {
            ai->PatchAdd (link);
        });
    }
    template <typename T>
    inline void LinkedList<T>::PatchViewsRemove (const Link* link) const
    {
        RequireNotNull (link);
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([link] (ForwardIterator* ai) {
            ai->PatchRemove (link);
        });
    }
    template <typename T>
    inline void LinkedList<T>::PatchViewsRemoveAll () const
    {
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([] (ForwardIterator* ai) {
            ai->PatchRemoveAll ();
        });
    }
    template <typename T>
    inline void LinkedList<T>::TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch) const
    {
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([oldI, items2Patch] (ForwardIterator* ai) {
            ai->TwoPhaseIteratorPatcherPass1 (oldI, items2Patch);
        });
    }
    template <typename T>
    inline void LinkedList<T>::TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<ForwardIterator*>* items2Patch, Link* newI) const
    {
        for (size_t i = 0; i < items2Patch->GetSize (); ++i) {
            (*items2Patch)[i]->TwoPhaseIteratorPatcherPass2 (newI);
        }
    }
    template <typename T>
    inline void LinkedList<T>::Prepend (ArgByValueType<T> item)
    {
        Invariant ();
        inherited::Prepend (item);
        PatchViewsAdd (this->_fHead);
        Invariant ();
    }
    //tmphack - must fix to have in  base class and just hook for patch here
    template <typename T>
    inline void LinkedList<T>::Append (ArgByValueType<T> item)
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
    template <typename T>
    inline void LinkedList<T>::RemoveFirst ()
    {
        Invariant ();
        PatchViewsRemove (this->_fHead);
        inherited::RemoveFirst ();
        Invariant ();
    }
    template <typename T>
    inline void LinkedList<T>::RemoveAll ()
    {
        Invariant ();
        inherited::RemoveAll ();
        PatchViewsRemoveAll ();
        Invariant ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    inline void LinkedList<T>::Remove (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer)
    {
        /*
         *  Base class impl is fine, but doesn't do patching, and doesn't
         *  provide the hooks so I can do the patching from here.
         *
         *  @todo   We may want to correct that (see STL container impl -
         *  returning ptr to next node would do it).
         */
        Invariant ();
        optional<T> current;
        for (ForwardIterator it (Traversal::kUnknownIteratorOwnerID, this); it.More (&current, true), current.has_value ();) {
            if (equalsComparer (*current, item)) {
                this->RemoveAt (it);
                break;
            }
        }
        Invariant ();
    }
    template <typename T>
    inline void LinkedList<T>::RemoveAt (const ForwardIterator& i)
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
    inline void LinkedList<T>::AddBefore (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        Invariant ();

        //tmphack
        const Link* prev = nullptr;
        if ((this->_fHead != nullptr) and (this->_fHead != i._fCurrent)) {
            for (prev = this->_fHead; prev->fNext != i._fCurrent; prev = prev->fNext) {
                AssertNotNull (prev); // cuz that would mean _fCurrent not in DoublyLinkedList!!!
            }
        }

        bool isPrevNull = (prev == nullptr);
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
    inline void LinkedList<T>::AddAfter (const ForwardIterator& i, ArgByValueType<T> newValue)
    {
        Invariant ();
        inherited::AddAfter (i, newValue);
        this->PatchViewsAdd (i._fCurrent->fNext);
        Invariant ();
    }
#if qDebug
    template <typename T>
    void LinkedList<T>::Invariant_ () const
    {
        inherited::Invariant_ ();
        /*
            *      Be sure each iterator points back to us. Thats about all we can test from
            *  here since we cannot call each iterators Invariant(). That would be
            *  nice, but sadly when this Invariant_ () is called from LinkedList<T,TRAITS> the
            *  iterators themselves may not have been patched, so they'll be out of
            *  date. Instead, so that in local shadow of Invariant() done in LinkedList<T>
            *  so only called when WE call Invariant().
            */
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([this] (ForwardIterator* ai) {
            Assert (ai->_fData == this);
        });
    }
    template <typename T>
    void LinkedList<T>::InvariantOnIterators_ () const
    {
        /*
            *      Only here can we iterate over each iterator and calls its Invariant()
            *  since now we've completed any needed patching.
            */
        this->template _ApplyToEachOwnedIterator<ForwardIterator> ([this] (ForwardIterator* ai) {
            Assert (ai->_fData == this);
            ai->Invariant ();
        });
    }
#endif

    /*
     ********************************************************************************
     ************************* LinkedList<T>::ForwardIterator ***********************
     ********************************************************************************
     */
    template <typename T>
    inline LinkedList<T>::ForwardIterator::ForwardIterator (IteratorOwnerID ownerID, const LinkedList<T>* data)
        : inherited_DataStructure (data)
        , inherited_PatchHelper (const_cast<LinkedList<T>*> (data), ownerID)
    //, fPrev (nullptr)         // means invalid or fData->_fHead == _fCurrent ...
    {
        RequireNotNull (data);
        this->Invariant ();
    }
    template <typename T>
    inline LinkedList<T>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
        : inherited_DataStructure (from)
        , inherited_PatchHelper (from)
    //, fPrev (from.fPrev)
    {
        from.Invariant ();
        this->Invariant ();
    }
    template <typename T>
    inline LinkedList<T>::ForwardIterator::~ForwardIterator ()
    {
        this->Invariant ();
    }
    template <typename T>
    inline void LinkedList<T>::ForwardIterator::PatchAdd (const Link* link)
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
    inline void LinkedList<T>::ForwardIterator::PatchRemove (const Link* link)
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
            this->_fSuppressMore = true; // Since we advanced cursor...
        }
        //else if (fPrev == link) {
        //    fPrev = nullptr;                    // real value recomputed later, if needed
        //}
    }
    template <typename T>
    inline void LinkedList<T>::ForwardIterator::PatchRemoveAll ()
    {
        this->_fCurrent = nullptr;
        //fPrev = nullptr;
        Ensure (this->Done ());
    }
    template <typename T>
    inline void LinkedList<T>::ForwardIterator::TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch)
    {
        if (this->_fCurrent == oldI) {
            items2Patch->push_back (this);
        }
    }
    template <typename T>
    inline void LinkedList<T>::ForwardIterator::TwoPhaseIteratorPatcherPass2 (Link* newI)
    {
        this->_fSuppressMore = true;
        this->_fCurrent      = newI;
    }
#if qDebug
    template <typename T>
    void LinkedList<T>::ForwardIterator::Invariant_ () const
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

#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_inl_ */
