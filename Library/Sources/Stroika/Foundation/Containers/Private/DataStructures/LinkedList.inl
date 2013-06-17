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
                    template    <typename   T>
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
                    LinkedList<T>::LinkedList (const LinkedList<T>& from)
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
                    inline  void    LinkedList<T>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template    <typename   T>
                    inline  LinkedList<T>::LinkedList ()
                        : fFirst (nullptr)
                    {
                        Invariant ();
                    }
                    template    <typename   T>
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
                        Ensure (fFirst == nullptr);
                    }
                    template    <typename   T>
                    bool    LinkedList<T>::IsEmpty () const
                    {
                        return fFirst == nullptr;
                    }
                    template    <typename   T>
                    inline  size_t  LinkedList<T>::GetLength () const
                    {
                        size_t  n   =   0;
                        for (const Link* i = fFirst; i != nullptr; i = i->fNext) {
                            n++;
                        }
                        return n;
                    }
                    template    <typename   T>
                    inline  T   LinkedList<T>::GetFirst () const
                    {
                        AssertNotNull (fFirst);
                        return (fFirst->fItem);
                    }
                    template    <typename   T>
                    inline  void    LinkedList<T>::Prepend (T item)
                    {
                        Invariant ();
                        fFirst = new Link (item, fFirst);
                        Invariant ();
                    }
                    template    <typename   T>
                    inline  void    LinkedList<T>::RemoveFirst ()
                    {
                        RequireNotNull (fFirst);
                        Invariant ();

                        Link* victim = fFirst;
                        fFirst = victim->fNext;
                        delete (victim);

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

                        Invariant ();

                        return (*this);
                    }
                    template    <typename   T>
                    void    LinkedList<T>::RemoveAt (const ForwardIterator& it)
                    {
                        Require (not it.Done ());
                        it.Invariant ();
                        Link*    victim  = const_cast<Link*> (it.fCurrent);

                        /*
                         *      At this point we need the fPrev pointer. But it may have been lost
                         *  in a patch. If it was, its value will be nullptr (NB: nullptr could also mean
                         *  fCurrent == fData->fFirst). If it is nullptr, recompute. Be careful if it
                         *  is still nullptr, that means update fFirst.
                         */
                        Link*    prevLink  = it.fCachedPrev;
                        if ((prevLink == nullptr) and (this->fFirst != victim)) {
                            AssertNotNull (this->fFirst);    // cuz there must be something to remove current
                            for (prevLink = this->fFirst; prevLink->fNext != victim; prevLink = prevLink->fNext) {
                                AssertNotNull (prevLink);    // cuz that would mean victim not in LinkedList!!!
                            }
                        }
                        if (prevLink == nullptr) {
                            Assert (this->fFirst == victim);
                            this->fFirst = victim->fNext;
                        }
                        else {
                            Assert (prevLink->fNext == victim);
                            prevLink->fNext = victim->fNext;
                        }

                        delete (victim);

                        it.Invariant ();
                        this->Invariant ();
                    }
                    template    <typename   T>
                    void    LinkedList<T>::Remove (T item)
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
                    }
                    template    <typename   T>
                    T   LinkedList<T>::GetAt (size_t i) const
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
                    void    LinkedList<T>::SetAt (T item, size_t i)
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
                    void    LinkedList<T>::Invariant_ () const
                    {
                        /*
                         * Check we are properly linked together.
                         */
                        for (Link* i = fFirst; i != nullptr; i = i->fNext) {
                            // at least make sure no currupted links and no infinite loops
                        }
                    }
#endif


                    /*
                    ********************************************************************************
                    ********************* LinkedList<T>::ForwardIterator ***************************
                    ********************************************************************************
                    */
                    template    <typename   T>
                    inline  void    LinkedList<T>::ForwardIterator::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template    <typename   T>
                    inline  LinkedList<T>::ForwardIterator::ForwardIterator (const LinkedList<T>& data)
                        : fCachedPrev (nullptr)
                        , fCurrent (data.fFirst)
                        , fSuppressMore (true)
                    {
                    }
                    template    <typename   T>
                    inline  LinkedList<T>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
                        : fCachedPrev (nullptr)
                        , fCurrent (from.fCurrent)
                        , fSuppressMore (from.fSuppressMore)
                    {
                    }
                    template    <typename   T>
                    inline  typename LinkedList<T>::ForwardIterator&  LinkedList<T>::ForwardIterator::operator= (const ForwardIterator& rhs)
                    {
                        Invariant ();
                        fCurrent = rhs.fCurrent;
                        fSuppressMore = rhs.fSuppressMore;
                        Invariant ();
                        return (*this);
                    }
                    template    <typename   T>
                    inline  bool    LinkedList<T>::ForwardIterator::Done () const
                    {
                        Invariant ();
                        return bool (fCurrent == nullptr);
                    }
                    template    <typename   T>
                    inline  bool    LinkedList<T>::ForwardIterator::More (T* current, bool advance)
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
                    inline  T   LinkedList<T>::ForwardIterator::Current () const
                    {
                        Require (not (Done ()));
                        Invariant ();
                        AssertNotNull (fCurrent);
                        return (fCurrent->fItem);
                    }
#if     qDebug
                    template    <typename   T>
                    void    LinkedList<T>::ForwardIterator::Invariant_ () const
                    {
                    }
#endif


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_DataStructures_LinkedList_inl_ */
