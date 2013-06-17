/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_DoublyLinkedList_inl_
#define _Stroika_Foundation_Containers_Private_DataStructures_DoublyLinkedList_inl_  1


#include    "../../../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Private {
                namespace   DataStructures {


                    /*
                    ********************************************************************************
                    ************************ DoublyLinkedList<T>::Link *****************************
                    ********************************************************************************
                    */
                    template    <typename   T>
                    inline  DoublyLinkedList<T>::Link::Link (T item, Link* next)
                        : fItem (item)
                        , fNext (next)
                    {
                    }


                    /*
                    ********************************************************************************
                    ************************* DoublyLinkedList<T> **********************************
                    ********************************************************************************
                    */
                    template    <typename   T>
                    DoublyLinkedList<T>::DoublyLinkedList (const DoublyLinkedList<T>& from)
                        : fFirst (nullptr)
                    {
                        /*
                         *      Copy the link list by keeping a point to the new current and new
                         *  previous, and sliding them along in parallel as we construct the
                         *  new list. Only do this if we have at least one element - then we
                         *  don't have to worry about the head of the list, or nullptr ptrs, etc - that
                         *  case is handled outside, before the loop.
                         */
                        if (from.fFirst != nullptr) {
                            fFirst = new Link (from.fFirst->fItem, nullptr);
                            Link*    newCur  =   fFirst;
                            for (const Link* cur = from.fFirst->fNext; cur != nullptr; cur = cur->fNext) {
                                Link*    newPrev =   newCur;
                                newCur = new Link (cur->fItem, nullptr);
                                newPrev->fNext = newCur;
                            }
                        }

                        Invariant ();
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template    <typename   T>
                    inline  DoublyLinkedList<T>::DoublyLinkedList ()
                        : fFirst (nullptr)
                    {
                        Invariant ();
                    }
                    template    <typename   T>
                    inline  DoublyLinkedList<T>::~DoublyLinkedList ()
                    {
                        /*
                         * This could be a little cheaper - we could avoid setting fFirst pointer,
                         *  but we must worry more about codeSize/re-use.
                         *  That would involve a new function that COULD NOT BE INLINED.
                         *
                         * < I guess  I could add a hack method - unadvertised - but has to be
                         *   at least protected - and call it here to do what I've mentioned above >
                         */
                        Invariant ();
                        RemoveAll ();
                        Invariant ();
                        Ensure (GetLength () == 0);
                        Ensure (fFirst == nullptr);
                    }
                    template    <typename   T>
                    bool    DoublyLinkedList<T>::IsEmpty () const
                    {
                        return fFirst == nullptr;
                    }
                    template    <typename   T>
                    inline  size_t  DoublyLinkedList<T>::GetLength () const
                    {
                        size_t  n   =   0;
                        for (const Link* i = fFirst; i != nullptr; i = i->fNext) {
                            n++;
                        }
                        return n;
                    }
                    template    <typename   T>
                    inline  T   DoublyLinkedList<T>::GetFirst () const
                    {
                        AssertNotNull (fFirst);
                        return (fFirst->fItem);
                    }
                    template    <typename   T>
                    inline  T   DoublyLinkedList<T>::GetLast () const
                    {
                        // TMPHACK - must restore storage of fLast - somehow lost (sterl?) by moving this code from old stroika 1992 code?
                        // maybe irrelevant if we swtich to usting STL list class
                        for (const Link* i = fFirst; i != nullptr; i = i->fNext) {
                            if (i->fNext == nullptr) {
                                return i->fItem;
                            }
                        }
                        RequireNotReached ();
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::Prepend (T item)
                    {
                        Invariant ();
                        fFirst = new Link (item, fFirst);
                        Invariant ();
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::RemoveFirst ()
                    {
                        RequireNotNull (fFirst);
                        Invariant ();

                        Link* victim = fFirst;
                        fFirst = victim->fNext;
                        delete (victim);

                        Invariant ();
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::RemoveLast ()
                    {
                        RequireNotNull (fFirst);
                        Invariant ();

                        Link*   i       = fFirst;
                        Link*   prev    = nullptr;
                        AssertNotNull (i);  // because must be at least one
                        for (; i->fNext != nullptr; prev = i, i = i->fNext)
                            ;
                        AssertNotNull (i);
                        Link* victim = i;
                        Assert (victim->fNext == nullptr);
                        if (victim == fFirst) {
                            fFirst = nullptr;
                            Assert (prev == nullptr);
                        }
                        else {
                            AssertNotNull (prev);
                            Assert (prev->fNext == victim);
                            prev->fNext = nullptr;
                        }
                        delete victim;

                        Invariant ();
                    }
                    template    <typename   T>
                    DoublyLinkedList<T>& DoublyLinkedList<T>::operator= (const DoublyLinkedList<T>& list)
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
                            fFirst = new Link (list.fFirst->fItem, nullptr);
                            Link*    newCur  =   fFirst;
                            for (const Link* cur = list.fFirst->fNext; cur != nullptr; cur = cur->fNext) {
                                Link*    newPrev =   newCur;
                                newCur = new Link (cur->fItem, nullptr);
                                newPrev->fNext = newCur;
                            }
                        }

                        Invariant ();

                        return (*this);
                    }
                    template    <typename   T>
                    void    DoublyLinkedList<T>::Remove (T item)
                    {
                        Invariant ();

                        if (item == fFirst->fItem) {
                            RemoveFirst ();
                        }
                        else {
                            Link*    prev    =   nullptr;
                            for (Link* link = fFirst; link != nullptr; prev = link, link = link->fNext) {
                                if (link->fItem == item) {
                                    AssertNotNull (prev);       // cuz otherwise we would have hit it in first case!
                                    prev->fNext = link->fNext;
                                    delete (link);
                                    break;
                                }
                            }
                        }

                        Invariant ();
                    }
                    template    <typename   T>
                    bool    DoublyLinkedList<T>::Contains (T item) const
                    {
                        for (const Link* current = fFirst; current != nullptr; current = current->fNext) {
                            if (current->fItem == item) {
                                return (true);
                            }
                        }
                        return (false);
                    }
                    template    <typename   T>
                    void    DoublyLinkedList<T>::RemoveAll ()
                    {
                        for (Link* i = fFirst; i != nullptr;) {
                            Link*    deleteMe    =   i;
                            i = i->fNext;
                            delete (deleteMe);
                        }
                        fFirst = nullptr;
                    }
                    template    <typename   T>
                    T   DoublyLinkedList<T>::GetAt (size_t i) const
                    {
                        Require (i >= 0);
                        Require (i < GetLength ());
                        const Link* cur = fFirst;
                        for (; i != 0; cur = cur->fNext, --i) {
                            AssertNotNull (cur);    // cuz i <= fLength
                        }
                        AssertNotNull (cur);        // cuz i <= fLength
                        return (cur->fItem);
                    }
                    template    <typename   T>
                    void    DoublyLinkedList<T>::SetAt (T item, size_t i)
                    {
                        Require (i >= 0);
                        Require (i < GetLength ());
                        Link* cur = fFirst;
                        for (; i != 0; cur = cur->fNext, --i) {
                            AssertNotNull (cur);    // cuz i <= fLength
                        }
                        AssertNotNull (cur);        // cuz i <= fLength
                        cur->fItem = item;
                    }
#if     qDebug
                    template    <typename   T>
                    void    DoublyLinkedList<T>::Invariant_ () const
                    {
                        /*
                         * Check we are properly linked together.
                         */
                        for (Link* i = fFirst; i != nullptr; i = i->fNext) {
                            // just check no invalid pointers - or loops - at least can walk link listed
                        }
                    }
#endif


                    /*
                    ********************************************************************************
                    *********** DoublyLinkedList<T>::DoublyLinkedListIterator **********************
                    ********************************************************************************
                    */
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::DoublyLinkedListIterator::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template    <typename   T>
                    inline  DoublyLinkedList<T>::DoublyLinkedListIterator::DoublyLinkedListIterator (const DoublyLinkedList<T>& data)
                        : fCurrent (data.fFirst)
                        , fSuppressMore (true)
                    {
                    }
                    template    <typename   T>
                    inline  DoublyLinkedList<T>::DoublyLinkedListIterator::DoublyLinkedListIterator (const DoublyLinkedListIterator& from)
                        : fCurrent (from.fCurrent)
                        , fSuppressMore (from.fSuppressMore)
                    {
                    }
                    template    <typename   T>
                    inline  typename DoublyLinkedList<T>::DoublyLinkedListIterator&  DoublyLinkedList<T>::DoublyLinkedListIterator::operator= (const DoublyLinkedListIterator& rhs)
                    {
                        Invariant ();
                        fCurrent = rhs.fCurrent;
                        fSuppressMore = rhs.fSuppressMore;
                        Invariant ();
                        return *this;
                    }
                    template    <typename   T>
                    inline  bool    DoublyLinkedList<T>::DoublyLinkedListIterator::Done () const
                    {
                        Invariant ();
                        return bool (fCurrent == nullptr);
                    }
                    template    <typename   T>
                    inline  bool    DoublyLinkedList<T>::DoublyLinkedListIterator::More (T* current, bool advance)
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
                    template    <typename   T>
                    inline  T   DoublyLinkedList<T>::DoublyLinkedListIterator::Current () const
                    {
                        Require (not (Done ()));
                        Invariant ();
                        AssertNotNull (fCurrent);
                        return (fCurrent->fItem);
                    }
#if     qDebug
                    template    <typename   T>
                    void    DoublyLinkedList<T>::DoublyLinkedListIterator::Invariant_ () const
                    {
                    }
#endif


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_DataStructures_DoublyLinkedList_inl_ */
