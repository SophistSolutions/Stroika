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
                    ****************** DoublyLinkedList<T>::DoubleLink *****************************
                    ********************************************************************************
                    */
                    template    <class T>
                    inline  DoublyLinkedList<T>::DoubleLink::DoubleLink (T item, DoubleLink* next)
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
                    DoublyLinkedList<T>::DoublyLinkedList (const DoublyLinkedList<T>& from) :
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
                            fFirst = new DoubleLink (from.fFirst->fItem, nullptr);
                            DoubleLink*    newCur  =   fFirst;
                            for (const DoubleLink* cur = from.fFirst->fNext; cur != nullptr; cur = cur->fNext) {
                                DoubleLink*    newPrev =   newCur;
                                newCur = new DoubleLink (cur->fItem, nullptr);
                                newPrev->fNext = newCur;
                            }
                        }

                        Invariant ();
                    }
                    template    <class T>
                    inline  void    DoublyLinkedList<T>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template    <class T>
                    inline  DoublyLinkedList<T>::DoublyLinkedList () :
                        fLength (0),
                        fFirst (nullptr)
                    {
                        Invariant ();
                    }
                    template    <class T>
                    inline  DoublyLinkedList<T>::~DoublyLinkedList ()
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
                    template    <class T>
                    inline  size_t  DoublyLinkedList<T>::GetLength () const
                    {
                        return (fLength);
                    }
                    template    <class T>
                    inline  T   DoublyLinkedList<T>::GetFirst () const
                    {
                        Require (fLength > 0);
                        AssertNotNull (fFirst);
                        return (fFirst->fItem);
                    }
                    template    <class T>
                    inline  T   DoublyLinkedList<T>::GetLast () const
                    {
                        // TMPHACK - must restore storage of fLast - somehow lost (sterl?) by moving this code from old stroika 1992 code?
                        // maybe irrelevant if we swtich to usting STL list class
                        Require (fLength > 0);
                        return GetAt (fLength - 1);
                    }
                    template    <class T>
                    inline  void    DoublyLinkedList<T>::Prepend (T item)
                    {
                        Invariant ();
                        fFirst = new DoubleLink (item, fFirst);
                        fLength++;
                        Invariant ();
                    }
                    template    <class T>
                    inline  void    DoublyLinkedList<T>::RemoveFirst ()
                    {
                        RequireNotNull (fFirst);
                        Invariant ();

                        DoubleLink* victim = fFirst;
                        fFirst = victim->fNext;
                        delete (victim);
                        fLength--;

                        Invariant ();
                    }
                    template    <class T>
                    inline  void    DoublyLinkedList<T>::RemoveLast ()
                    {
                        RequireNotNull (fFirst);
                        Invariant ();

                        // HACK - SEE OLD 1992 Stroika code - or abandom this impl - and use STL?
                        DoubleLink* i = fFirst;
                        AssertNotNull (i);
                        for (; i->fNext != nullptr; i = i->fNext)
                            ;
                        AssertNotNull (i);
                        DoubleLink* victim = i;
                        if (victim == fFirst) {
                            fFirst = nullptr;
                        }
                        Assert (victim->fNext == nullptr);
                        delete (victim);
                        fLength--;

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
                            fFirst = new DoubleLink (list.fFirst->fItem, nullptr);
                            DoubleLink*    newCur  =   fFirst;
                            for (const DoubleLink* cur = list.fFirst->fNext; cur != nullptr; cur = cur->fNext) {
                                DoubleLink*    newPrev =   newCur;
                                newCur = new DoubleLink (cur->fItem, nullptr);
                                newPrev->fNext = newCur;
                            }
                        }

                        fLength = list.fLength;

                        Invariant ();

                        return (*this);
                    }
                    template    <typename   T>
                    void    DoublyLinkedList<T>::Remove (T item)
                    {
                        Require (fLength >= 1);

                        Invariant ();

                        if (item == fFirst->fItem) {
                            RemoveFirst ();
                        }
                        else {
                            DoubleLink*    prev    =   nullptr;
                            for (DoubleLink* link = fFirst; link != nullptr; prev = link, link = link->fNext) {
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
                    template    <typename   T>
                    bool    DoublyLinkedList<T>::Contains (T item) const
                    {
                        for (const DoubleLink* current = fFirst; current != nullptr; current = current->fNext) {
                            if (current->fItem == item) {
                                return (true);
                            }
                        }
                        return (false);
                    }
                    template    <typename   T>
                    void    DoublyLinkedList<T>::RemoveAll ()
                    {
                        for (DoubleLink* i = fFirst; i != nullptr;) {
                            DoubleLink*    deleteMe    =   i;
                            i = i->fNext;
                            delete (deleteMe);
                        }
                        fFirst = nullptr;
                        fLength = 0;
                    }
                    template    <typename   T>
                    T   DoublyLinkedList<T>::GetAt (size_t i) const
                    {
                        Require (i >= 0);
                        Require (i < fLength);
                        const DoubleLink* cur = fFirst;
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
                        Require (i < fLength);
                        DoubleLink* cur = fFirst;
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
                        size_t  counter =   0;
                        for (DoubleLink* i = fFirst; i != nullptr; i = i->fNext) {
                            counter++;
                            Assert (counter <= fLength);    // to this test in the loop so we detect circularities...
                        }
                        Assert (counter == fLength);
                    }
#endif


                    /*
                    ********************************************************************************
                    ********************** DoublyLinkedListIterator<T> *****************************
                    ********************************************************************************
                    */
                    template    <class T>
                    inline  void    DoublyLinkedListIterator<T>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template    <class T>
                    inline  DoublyLinkedListIterator<T>::DoublyLinkedListIterator (const DoublyLinkedList<T>& data) :
                        fCurrent (data.fFirst),
                        fSuppressMore (true)
                    {
                    }
                    template    <class T>
                    inline  DoublyLinkedListIterator<T>::DoublyLinkedListIterator (const DoublyLinkedListIterator<T>& from) :
                        fCurrent (from.fCurrent),
                        fSuppressMore (from.fSuppressMore)
                    {
                    }
                    template    <class T>
                    inline  DoublyLinkedListIterator<T>&  DoublyLinkedListIterator<T>::operator= (const DoublyLinkedListIterator<T>& rhs)
                    {
                        Invariant ();
                        fCurrent = rhs.fCurrent;
                        fSuppressMore = rhs.fSuppressMore;
                        Invariant ();
                        return (*this);
                    }
                    template    <class T>
                    inline  bool    DoublyLinkedListIterator<T>::Done () const
                    {
                        Invariant ();
                        return bool (fCurrent == nullptr);
                    }
                    template    <class T>
                    inline  bool    DoublyLinkedListIterator<T>::More (T* current, bool advance)
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
                    template    <class T>
                    inline  T   DoublyLinkedListIterator<T>::Current () const
                    {
                        Require (not (Done ()));
                        Invariant ();
                        AssertNotNull (fCurrent);
                        return (fCurrent->fItem);
                    }
#if     qDebug
                    template    <typename   T>
                    void    DoublyLinkedListIterator<T>::Invariant_ () const
                    {
                    }
#endif


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_DataStructures_DoublyLinkedList_inl_ */
