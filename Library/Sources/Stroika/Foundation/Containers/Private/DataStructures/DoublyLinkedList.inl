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
                    inline  DoublyLinkedList<T>::DoublyLinkedList ()
                        : _fFirst (nullptr)
                    {
                        Invariant ();
                    }
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
                        Ensure (_fFirst == nullptr);
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template    <typename   T>
                    bool    DoublyLinkedList<T>::IsEmpty () const
                    {
                        return _fFirst == nullptr;
                    }
                    template    <typename   T>
                    inline  size_t  DoublyLinkedList<T>::GetLength () const
                    {
                        size_t  n   =   0;
                        for (const Link* i = _fFirst; i != nullptr; i = i->fNext) {
                            n++;
                        }
                        return n;
                    }
                    template    <typename   T>
                    inline  T   DoublyLinkedList<T>::GetFirst () const
                    {
                        AssertNotNull (_fFirst);
                        return _fFirst->fItem;
                    }
                    template    <typename   T>
                    inline  T   DoublyLinkedList<T>::GetLast () const
                    {
                        // TMPHACK - must restore storage of fLast - somehow lost (sterl?) by moving this code from old stroika 1992 code?
                        // maybe irrelevant if we swtich to usting STL list class
                        for (const Link* i = _fFirst; i != nullptr; i = i->fNext) {
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
                        _fFirst = new Link (item, _fFirst);
                        Invariant ();
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::RemoveFirst ()
                    {
                        RequireNotNull (_fFirst);
                        Invariant ();

                        Link* victim = _fFirst;
                        _fFirst = victim->fNext;
                        delete (victim);

                        Invariant ();
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::RemoveLast ()
                    {
                        RequireNotNull (_fFirst);
                        Invariant ();

                        Link*   i       =   _fFirst;
                        Link*   prev    =   nullptr;
                        AssertNotNull (i);  // because must be at least one
                        for (; i->fNext != nullptr; prev = i, i = i->fNext)
                            ;
                        AssertNotNull (i);
                        Link* victim = i;
                        Assert (victim->fNext == nullptr);
                        if (victim == _fFirst) {
                            _fFirst = nullptr;
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
                    DoublyLinkedList<T>& DoublyLinkedList<T>::operator= (const DoublyLinkedList<T>& rhs)
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
                        if (rhs._fFirst != nullptr) {
                            _fFirst = new Link (rhs._fFirst->fItem, nullptr);
                            Link*    newCur  =   _fFirst;
                            for (const Link* cur = rhs._fFirst->fNext; cur != nullptr; cur = cur->fNext) {
                                Link*    newPrev =   newCur;
                                newCur = new Link (cur->fItem, nullptr);
                                newPrev->fNext = newCur;
                            }
                        }

                        Invariant ();

                        return *this;
                    }
                    template    <typename   T>
                    void    DoublyLinkedList<T>::Remove (T item)
                    {
                        Invariant ();

                        if (item == _fFirst->fItem) {
                            RemoveFirst ();
                        }
                        else {
                            Link*    prev    =   nullptr;
                            for (Link* link = _fFirst; link != nullptr; prev = link, link = link->fNext) {
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
                        for (const Link* current = _fFirst; current != nullptr; current = current->fNext) {
                            if (current->fItem == item) {
                                return true;
                            }
                        }
                        return false;
                    }
                    template    <typename   T>
                    void    DoublyLinkedList<T>::RemoveAll ()
                    {
                        for (Link* i = _fFirst; i != nullptr;) {
                            Link*    deleteMe    =   i;
                            i = i->fNext;
                            delete (deleteMe);
                        }
                        _fFirst = nullptr;
                    }
                    template    <typename   T>
                    T   DoublyLinkedList<T>::GetAt (size_t i) const
                    {
                        Require (i >= 0);
                        Require (i < GetLength ());
                        const Link* cur = _fFirst;
                        for (; i != 0; cur = cur->fNext, --i) {
                            AssertNotNull (cur);    // cuz i <= fLength
                        }
                        AssertNotNull (cur);        // cuz i <= fLength
                        return (cur->fItem);
                    }
                    template    <typename   T>
                    void    DoublyLinkedList<T>::SetAt (size_t i, T item)
                    {
                        Require (i >= 0);
                        Require (i < GetLength ());
                        Link* cur = _fFirst;
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
                        for (Link* i = _fFirst; i != nullptr; i = i->fNext) {
                            // just check no invalid pointers - or loops - at least can walk link listed
                        }
                    }
#endif


                    /*
                    ********************************************************************************
                    ******************** DoublyLinkedList<T>::ForwardIterator **********************
                    ********************************************************************************
                    */
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::ForwardIterator::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template    <typename   T>
                    inline  DoublyLinkedList<T>::ForwardIterator::ForwardIterator (const DoublyLinkedList<T>& data)
                        : fCurrent (data._fFirst)
                        , fSuppressMore (true)
                    {
                    }
                    template    <typename   T>
                    inline  DoublyLinkedList<T>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
                        : fCurrent (from.fCurrent)
                        , fSuppressMore (from.fSuppressMore)
                    {
                    }
                    template    <typename   T>
                    inline  typename DoublyLinkedList<T>::ForwardIterator&  DoublyLinkedList<T>::ForwardIterator::operator= (const ForwardIterator& rhs)
                    {
                        Invariant ();
                        fCurrent = rhs.fCurrent;
                        fSuppressMore = rhs.fSuppressMore;
                        Invariant ();
                        return *this;
                    }
                    template    <typename   T>
                    inline  bool    DoublyLinkedList<T>::ForwardIterator::Done () const
                    {
                        Invariant ();
                        return bool (fCurrent == nullptr);
                    }
                    template    <typename   T>
                    inline  bool    DoublyLinkedList<T>::ForwardIterator::More (T* current, bool advance)
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
                    inline  T   DoublyLinkedList<T>::ForwardIterator::Current () const
                    {
                        Require (not (Done ()));
                        Invariant ();
                        AssertNotNull (fCurrent);
                        return (fCurrent->fItem);
                    }
                    template    <typename   T>
                    inline    typename DoublyLinkedList<T>::Link*   DoublyLinkedList<T>::ForwardIterator::_GetFirstDataLink (DoublyLinkedList<T>* data)
                    {
                        RequireNotNull (data);
                        return data->_fFirst;
                    }
                    template    <typename   T>
                    inline const typename DoublyLinkedList<T>::Link* DoublyLinkedList<T>::ForwardIterator::_GetFirstDataLink (const DoublyLinkedList<T>* data)
                    {
                        RequireNotNull (data);
                        return data->_fFirst;
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::ForwardIterator::_SetFirstDataLink (DoublyLinkedList<T>* data, Link* newFirstLink)
                    {
                        RequireNotNull (data);
                        data->_fFirst = newFirstLink;
                    }
#if     qDebug
                    template    <typename   T>
                    void    DoublyLinkedList<T>::ForwardIterator::Invariant_ () const
                    {
                    }
#endif


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_DataStructures_DoublyLinkedList_inl_ */
