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
                        : _fHead (nullptr)
                    {
                        Invariant ();
                    }
                    template    <typename   T>
                    DoublyLinkedList<T>::DoublyLinkedList (const DoublyLinkedList<T>& from)
                        : _fHead (nullptr)
                    {
                        /*
                         *      Copy the link list by keeping a point to the new current and new
                         *  previous, and sliding them along in parallel as we construct the
                         *  new list. Only do this if we have at least one element - then we
                         *  don't have to worry about the head of the list, or nullptr ptrs, etc - that
                         *  case is handled outside, before the loop.
                         */
                        if (from._fHead != nullptr) {
                            _fHead = new Link (from._fHead->fItem, nullptr);
                            Link*    newCur  =   _fHead;
                            for (const Link* cur = from._fHead->fNext; cur != nullptr; cur = cur->fNext) {
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
                        Ensure (_fHead == nullptr);
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
                        return _fHead == nullptr;
                    }
                    template    <typename   T>
                    inline  size_t  DoublyLinkedList<T>::GetLength () const
                    {
                        size_t  n   =   0;
                        for (const Link* i = _fHead; i != nullptr; i = i->fNext) {
                            n++;
                        }
                        return n;
                    }
                    template    <typename   T>
                    inline  T   DoublyLinkedList<T>::GetFirst () const
                    {
                        AssertNotNull (_fHead);
                        return _fHead->fItem;
                    }
                    template    <typename   T>
                    inline  T   DoublyLinkedList<T>::GetLast () const
                    {
                        // TMPHACK - must restore storage of fLast - somehow lost (sterl?) by moving this code from old stroika 1992 code?
                        // maybe irrelevant if we swtich to usting STL list class
                        for (const Link* i = _fHead; i != nullptr; i = i->fNext) {
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
                        _fHead = new Link (item, _fHead);
                        Invariant ();
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::RemoveFirst ()
                    {
                        RequireNotNull (_fHead);
                        Invariant ();

                        Link* victim = _fHead;
                        _fHead = victim->fNext;
                        delete (victim);

                        Invariant ();
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::RemoveLast ()
                    {
                        RequireNotNull (_fHead);
                        Invariant ();

                        Link*   i       =   _fHead;
                        Link*   prev    =   nullptr;
                        AssertNotNull (i);  // because must be at least one
                        for (; i->fNext != nullptr; prev = i, i = i->fNext)
                            ;
                        AssertNotNull (i);
                        Link* victim = i;
                        Assert (victim->fNext == nullptr);
                        if (victim == _fHead) {
                            _fHead = nullptr;
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
                        if (rhs._fHead != nullptr) {
                            _fHead = new Link (rhs._fHead->fItem, nullptr);
                            Link*    newCur  =   _fHead;
                            for (const Link* cur = rhs._fHead->fNext; cur != nullptr; cur = cur->fNext) {
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

                        if (item == _fHead->fItem) {
                            RemoveFirst ();
                        }
                        else {
                            Link*    prev    =   nullptr;
                            for (Link* link = _fHead; link != nullptr; prev = link, link = link->fNext) {
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
                        for (const Link* current = _fHead; current != nullptr; current = current->fNext) {
                            if (current->fItem == item) {
                                return true;
                            }
                        }
                        return false;
                    }
                    template    <typename   T>
                    void    DoublyLinkedList<T>::RemoveAll ()
                    {
                        for (Link* i = _fHead; i != nullptr;) {
                            Link*    deleteMe    =   i;
                            i = i->fNext;
                            delete (deleteMe);
                        }
                        _fHead = nullptr;
                    }
                    template    <typename   T>
                    T   DoublyLinkedList<T>::GetAt (size_t i) const
                    {
                        Require (i >= 0);
                        Require (i < GetLength ());
                        const Link* cur = _fHead;
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
                        Link* cur = _fHead;
                        for (; i != 0; cur = cur->fNext, --i) {
                            AssertNotNull (cur);    // cuz i <= fLength
                        }
                        AssertNotNull (cur);        // cuz i <= fLength
                        cur->fItem = item;
                    }
                    template    <typename   T>
                    void    DoublyLinkedList<T>::RemoveAt (const ForwardIterator& i)
                    {
                        Require (not i.Done ());
                        this->Invariant ();

                        Link*    victim  = const_cast<Link*> (i._fCurrent);

                        /*
                         *      At this point we need the fPrev pointer. But it may have been lost
                         *  in a patch. If it was, its value will be nullptr (NB: nullptr could also mean
                         *  _fCurrent == fData->fFirst). If it is nullptr, recompute. Be careful if it
                         *  is still nullptr, that means update fFirst.
                         */
                        Link*       prev    =   nullptr;
                        if (this->_fHead != victim) {
                            AssertNotNull (this->_fHead);    // cuz there must be something to remove current
                            for (prev = this->_fHead; prev->fNext != victim; prev = prev->fNext) {
                                AssertNotNull (prev);    // cuz that would mean victim not in DoublyLinkedList!!!
                            }
                        }
                        if (prev == nullptr) {
                            this->_fHead = victim->fNext;
                        }
                        else {
                            Assert (prev->fNext == victim);
                            prev->fNext = victim->fNext;
                        }
                        delete (victim);
                        this->Invariant ();
                    }
                    template    <typename   T>
                    void    DoublyLinkedList<T>::SetAt (const ForwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        this->Invariant ();
                        const_cast<Link*> (i._fCurrent)->fItem = newValue;
                        this->Invariant ();
                    }
                    template    <typename   T>
                    void    DoublyLinkedList<T>::AddBefore (const ForwardIterator& i, T newValue)
                    {
                        /*
                         * NB: This code works fine, even if we are done!!!
                         */
                        this->Invariant ();

                        Link*     prev = nullptr;
                        if ((this->_fHead != nullptr) and (this->_fHead != i._fCurrent)) {
                            for (prev = this->_fHead; prev->fNext != i._fCurrent; prev = prev->fNext) {
                                AssertNotNull (prev);    // cuz that would mean _fCurrent not in DoublyLinkedList!!!
                            }
                        }

                        if (prev == nullptr) {
                            Assert (this->_fHead == i._fCurrent);     // could be nullptr, or not...
                            this->_fHead = new Link (newValue, const_cast<Link*> (this->_fHead));
                        }
                        else {
                            Assert (prev->fNext == i._fCurrent);
                            prev->fNext = new Link (newValue, prev->fNext);
                        }
                        this->Invariant ();
                    }
                    template    <typename   T>
                    void    DoublyLinkedList<T>::AddAfter (const ForwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        AssertNotNull (i._fCurrent); // since not done...
                        const_cast<Link*> (i._fCurrent)->fNext = new Link (newValue, i._fCurrent->fNext);
                    }
#if     qDebug
                    template    <typename   T>
                    void    DoublyLinkedList<T>::Invariant_ () const
                    {
                        /*
                         * Check we are properly linked together.
                         */
                        for (Link* i = _fHead; i != nullptr; i = i->fNext) {
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
                    inline  DoublyLinkedList<T>::ForwardIterator::ForwardIterator (const DoublyLinkedList<T>& data)
                        : _fData (&data)
                        , _fCurrent (data._fHead)
                        , _fSuppressMore (true)
                    {
                    }
                    template    <typename   T>
                    inline  DoublyLinkedList<T>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
                        : _fData (from._fData)
                        , _fCurrent (from._fCurrent)
                        , _fSuppressMore (from._fSuppressMore)
                    {
                    }
                    template    <typename   T>
                    inline  typename DoublyLinkedList<T>::ForwardIterator&  DoublyLinkedList<T>::ForwardIterator::operator= (const ForwardIterator& rhs)
                    {
                        Invariant ();
                        _fData = rhs._fData;
                        _fCurrent = rhs._fCurrent;
                        _fSuppressMore = rhs._fSuppressMore;
                        Invariant ();
                        return *this;
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::ForwardIterator::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template    <typename   T>
                    inline  bool    DoublyLinkedList<T>::ForwardIterator::Done () const
                    {
                        Invariant ();
                        return bool (_fCurrent == nullptr);
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
                            if (not _fSuppressMore and _fCurrent != nullptr) {
                                _fCurrent = _fCurrent->fNext;
                            }
                            _fSuppressMore = false;
                        }
                        Invariant ();
                        if (current != nullptr and not Done ()) {
                            *current = _fCurrent->fItem;
                        }
                        return (not Done ());
                    }
                    template    <typename   T>
                    inline  T   DoublyLinkedList<T>::ForwardIterator::Current () const
                    {
                        Require (not (Done ()));
                        Invariant ();
                        AssertNotNull (_fCurrent);
                        return _fCurrent->fItem;
                    }
                    template    <typename   T>
                    size_t DoublyLinkedList<T>::ForwardIterator::CurrentIndex () const
                    {
                        Require (not (Done ()));
                        Invariant ();
                        size_t n = 0;
                        for (const Link* l = _fData->_fHead; l != this->_fCurrent; l = l->fNext, ++n) {
                            AssertNotNull (l);
                        }
                        return n;
                    }
                    template    <typename   T>
                    inline    typename DoublyLinkedList<T>::Link*   DoublyLinkedList<T>::ForwardIterator::_GetFirstDataLink (DoublyLinkedList<T>* data)
                    {
                        RequireNotNull (data);
                        return data->_fHead;
                    }
                    template    <typename   T>
                    inline const typename DoublyLinkedList<T>::Link* DoublyLinkedList<T>::ForwardIterator::_GetFirstDataLink (const DoublyLinkedList<T>* data)
                    {
                        RequireNotNull (data);
                        return data->_fHead;
                    }
                    template    <typename   T>
                    inline  void    DoublyLinkedList<T>::ForwardIterator::_SetFirstDataLink (DoublyLinkedList<T>* data, Link* newFirstLink)
                    {
                        RequireNotNull (data);
                        data->_fHead = newFirstLink;
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
