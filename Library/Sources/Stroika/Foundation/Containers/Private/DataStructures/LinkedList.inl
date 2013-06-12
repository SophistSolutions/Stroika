/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_LinkedList_inl_
#define _Stroika_Foundation_Containers_Private_DataStructures_LinkedList_inl_  1


#include    "../../../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Private {
                namespace   DataStructures {


                    /*
                    ********************************************************************************
                    *********************** LinkedList<T>::Link ************************************
                    ********************************************************************************
                    */
                    template    <class T>
                    inline  LinkedList<T>::Link::Link (T item, Link* next)
                        : fItem (item)
                        , fNext (next)
                    {
                    }


                    /*
                    ********************************************************************************
                    ****************************** LinkedList<T> ***********************************
                    ********************************************************************************
                    */
                    template    <typename   T>
                    LinkedList<T>::LinkedList (const LinkedList<T>& from) :
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
                    template    <class T>
                    inline  void    LinkedList<T>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template    <class T>
                    inline  LinkedList<T>::LinkedList () :
                        fLength (0),
                        fFirst (nullptr)
                    {
                        Invariant ();
                    }
                    template    <class T>
                    inline  LinkedList<T>::~LinkedList ()
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
                    inline  size_t  LinkedList<T>::GetLength () const
                    {
                        return (fLength);
                    }
                    template    <class T>
                    inline  T   LinkedList<T>::GetFirst () const
                    {
                        Require (fLength > 0);
                        AssertNotNull (fFirst);
                        return (fFirst->fItem);
                    }
                    template    <class T>
                    inline  void    LinkedList<T>::Prepend (T item)
                    {
                        Invariant ();
                        fFirst = new Link (item, fFirst);
                        fLength++;
                        Invariant ();
                    }
                    template    <class T>
                    inline  void    LinkedList<T>::RemoveFirst ()
                    {
                        RequireNotNull (fFirst);
                        Invariant ();

                        Link* victim = fFirst;
                        fFirst = victim->fNext;
                        delete (victim);
                        fLength--;

                        Invariant ();
                    }
                    template    <typename   T>
                    LinkedList<T>& LinkedList<T>::operator= (const LinkedList<T>& list)
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

                        fLength = list.fLength;

                        Invariant ();

                        return (*this);
                    }
                    template    <typename   T>
                    void    LinkedList<T>::Remove (T item)
                    {
                        Require (fLength >= 1);

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
                                    fLength--;
                                    break;
                                }
                            }
                        }

                        Invariant ();
                    }
                    template    <typename   T>
                    bool    LinkedList<T>::Contains (T item) const
                    {
                        for (const Link* current = fFirst; current != nullptr; current = current->fNext) {
                            if (current->fItem == item) {
                                return (true);
                            }
                        }
                        return (false);
                    }
                    template    <typename   T>
                    void    LinkedList<T>::RemoveAll ()
                    {
                        for (Link* i = fFirst; i != nullptr;) {
                            Link*    deleteMe    =   i;
                            i = i->fNext;
                            delete (deleteMe);
                        }
                        fFirst = nullptr;
                        fLength = 0;
                    }
                    template    <typename   T>
                    T   LinkedList<T>::GetAt (size_t i) const
                    {
                        Require (i >= 0);
                        Require (i < fLength);
                        const Link* cur = fFirst;
                        for (; i != 0; cur = cur->fNext, --i) {
                            AssertNotNull (cur);    // cuz i <= fLength
                        }
                        AssertNotNull (cur);        // cuz i <= fLength
                        return (cur->fItem);
                    }
                    template    <typename   T>
                    void    LinkedList<T>::SetAt (T item, size_t i)
                    {
                        Require (i >= 0);
                        Require (i < fLength);
                        Link* cur = fFirst;
                        for (; i != 0; cur = cur->fNext, --i) {
                            AssertNotNull (cur);    // cuz i <= fLength
                        }
                        AssertNotNull (cur);        // cuz i <= fLength
                        cur->fItem = item;
                    }
#if     qDebug
                    template    <typename   T>
                    void    LinkedList<T>::Invariant_ () const
                    {
                        /*
                         * Check we are properly linked together.
                         */
                        size_t  counter =   0;
                        for (Link* i = fFirst; i != nullptr; i = i->fNext) {
                            counter++;
                            Assert (counter <= fLength);    // to this test in the loop so we detect circularities...
                        }
                        Assert (counter == fLength);
                    }
#endif


                    /*
                    ********************************************************************************
                    *************************** LinkedListIterator<T> ******************************
                    ********************************************************************************
                    */
                    template    <class T>
                    inline  void    LinkedListIterator<T>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template    <class T>
                    inline  LinkedListIterator<T>::LinkedListIterator (const LinkedList<T>& data) :
                        fCurrent (data.fFirst),
                        fSuppressMore (true)
                    {
                    }
                    template    <class T>
                    inline  LinkedListIterator<T>::LinkedListIterator (const LinkedListIterator<T>& from) :
                        fCurrent (from.fCurrent),
                        fSuppressMore (from.fSuppressMore)
                    {
                    }
                    template    <class T>
                    inline  LinkedListIterator<T>&  LinkedListIterator<T>::operator= (const LinkedListIterator<T>& rhs)
                    {
                        Invariant ();
                        fCurrent = rhs.fCurrent;
                        fSuppressMore = rhs.fSuppressMore;
                        Invariant ();
                        return (*this);
                    }
                    template    <class T>
                    inline  bool    LinkedListIterator<T>::Done () const
                    {
                        Invariant ();
                        return bool (fCurrent == nullptr);
                    }
                    template    <class T>
                    inline  bool    LinkedListIterator<T>::More (T* current, bool advance)
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
                    inline  T   LinkedListIterator<T>::Current () const
                    {
                        Require (not (Done ()));
                        Invariant ();
                        AssertNotNull (fCurrent);
                        return (fCurrent->fItem);
                    }
#if     qDebug
                    template    <typename   T>
                    void    LinkedListIterator<T>::Invariant_ () const
                    {
                    }
#endif


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_DataStructures_LinkedList_inl_ */
