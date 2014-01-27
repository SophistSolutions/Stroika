/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
                    ********************** LinkedList<T,TRAITS>::Link ******************************
                    ********************************************************************************
                    */
                    template      <typename  T, typename TRAITS>
                    inline  LinkedList<T, TRAITS>::Link::Link (T item, Link* next)
                        : fItem (item)
                        , fNext (next)
                    {
                    }


                    /*
                    ********************************************************************************
                    ************************** LinkedList<T,TRAITS> ********************************
                    ********************************************************************************
                    */
                    template      <typename  T, typename TRAITS>
                    inline  LinkedList<T, TRAITS>::LinkedList ()
                        : _fHead (nullptr)
                    {
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    LinkedList<T, TRAITS>::LinkedList (const LinkedList<T, TRAITS>& from)
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
                    template      <typename  T, typename TRAITS>
                    inline  LinkedList<T, TRAITS>::~LinkedList ()
                    {
                        /*
                         * This could be a little cheaper - we could avoid setting fLength field,
                         * and _fHead pointer, but we must worry more about codeSize/re-use.
                         * That would involve a new function that COULD NOT BE INLINED.
                         *
                         * < I guess  I could add a hack method - unadvertised - but has to be
                         *   at least protected - and call it here to do what I've mentioned above >
                         */
                        Invariant ();
                        RemoveAll ();
                        Invariant ();
                        Ensure (_fHead == nullptr);
                    }
                    template      <typename  T, typename TRAITS>
                    LinkedList<T, TRAITS>& LinkedList<T, TRAITS>::operator= (const LinkedList<T, TRAITS>& list)
                    {
                        Invariant ();
                        if (this != &list) {
                            RemoveAll ();

                            /*
                             *      Copy the link list by keeping a point to the new current and new
                             *  previous, and sliding them along in parallel as we construct the
                             *  new list. Only do this if we have at least one element - then we
                             *  don't have to worry about the head of the list, or nullptr ptrs, etc - that
                             *  case is handled outside, before the loop.
                             */
                            if (list._fHead != nullptr) {
                                _fHead = new Link (list._fHead->fItem, nullptr);
                                Link*    newCur  =   _fHead;
                                for (const Link* cur = list._fHead->fNext; cur != nullptr; cur = cur->fNext) {
                                    Link*    newPrev =   newCur;
                                    newCur = new Link (cur->fItem, nullptr);
                                    newPrev->fNext = newCur;
                                }
                            }

                        }
                        Invariant ();
                        return *this;
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template      <typename  T, typename TRAITS>
                    inline  bool    LinkedList<T, TRAITS>::IsEmpty () const
                    {
                        return _fHead == nullptr;
                    }
                    template      <typename  T, typename TRAITS>
                    inline  size_t  LinkedList<T, TRAITS>::GetLength () const
                    {
                        size_t  n   =   0;
                        for (const Link* i = _fHead; i != nullptr; i = i->fNext) {
                            n++;
                        }
                        return n;
                    }
                    template      <typename  T, typename TRAITS>
                    inline  T   LinkedList<T, TRAITS>::GetFirst () const
                    {
                        Require (not IsEmpty ());
                        AssertNotNull (_fHead);
                        return _fHead->fItem;
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::Prepend (T item)
                    {
                        Invariant ();

                        _fHead = new Link (item, _fHead);

                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::RemoveFirst ()
                    {
                        Require (not IsEmpty ());
                        AssertNotNull (_fHead);

                        Invariant ();

                        Link* victim = _fHead;
                        _fHead = victim->fNext;
                        delete (victim);

                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::SetAt (const ForwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        Invariant ();
                        i.Invariant ();

                        const_cast<Link*> (i._fCurrent)->fItem = newValue;

                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::AddBefore (const ForwardIterator& i, T newValue)
                    {
                        /*
                         * NB: This code works fine, even if we are done!!!
                         */
                        Invariant ();
                        i.Invariant ();

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

                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::AddAfter (const ForwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        AssertNotNull (i._fCurrent); // since not done...
                        i.Invariant ();
                        const_cast<Link*> (i._fCurrent)->fNext = new Link (newValue, i._fCurrent->fNext);
                    }
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::RemoveAt (const ForwardIterator& i)
                    {
                        Require (not i.Done ());
                        Invariant ();
                        i.Invariant ();

                        Link*    victim  = const_cast<Link*> (i._fCurrent);

                        /*
                         *      At this point we need the fPrev pointer. But it may have been lost
                         *  in a patch. If it was, its value will be nullptr (NB: nullptr could also mean
                         *  _fCurrent == fData->_fHead). If it is nullptr, recompute. Be careful if it
                         *  is still nullptr, that means update _fHead.
                         */
                        Link*       prevLink    =   nullptr;
                        if (this->_fHead != victim) {
                            AssertNotNull (this->_fHead);    // cuz there must be something to remove current
                            for (prevLink = this->_fHead; prevLink->fNext != victim; prevLink = prevLink->fNext) {
                                AssertNotNull (prevLink);    // cuz that would mean victim not in DoublyLinkedList!!!
                            }
                        }
                        if (prevLink == nullptr) {
                            Assert (this->_fHead == victim);
                            this->_fHead = victim->fNext;
                        }
                        else {
                            Assert (prevLink->fNext == victim);
                            prevLink->fNext = victim->fNext;
                        }

                        delete (victim);

                        i.Invariant ();
                        Invariant ();
                    }
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::Remove (T item)
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
                    template      <typename  T, typename TRAITS>
                    T*    LinkedList<T, TRAITS>::Lookup (T item)
                    {
                        for (Link* i = _fHead; i != nullptr; i = i->fNext) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (i->fItem , item)) {
                                return &i->fItem;
                            }
                        }
                        return nullptr;
                    }
                    template      <typename  T, typename TRAITS>
                    const T*    LinkedList<T, TRAITS>::Lookup (T item) const
                    {
                        for (const Link* i = _fHead; i != nullptr; i = i->fNext) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (i->fItem,  item)) {
                                return &i->fItem;
                            }
                        }
                        return nullptr;
                    }
                    template    <typename  T, typename TRAITS>
                    template    <typename FUNCTION>
                    inline  void    LinkedList<T, TRAITS>::Apply (FUNCTION doToElement) const
                    {
                        for (const Link* i = _fHead; i != nullptr; i = i->fNext) {
                            (doToElement) (i->fItem);
                        }
                    }
                    template    <typename  T, typename TRAITS>
                    template    <typename FUNCTION>
                    inline  typename LinkedList<T, TRAITS>::Link*    LinkedList<T, TRAITS>::FindFirstThat (FUNCTION doToElement) const
                    {
                        for (Link* i = _fHead; i != nullptr; i = i->fNext) {
                            if ((doToElement) (i->fItem)) {
                                return i;
                            }
                        }
                        return nullptr;
                    }
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::RemoveAll ()
                    {
                        Invariant ();

                        for (Link* i = _fHead; i != nullptr;) {
                            Link*    deleteMe    =   i;
                            i = i->fNext;
                            delete (deleteMe);
                        }
                        _fHead = nullptr;

                        Invariant ();
                        Ensure (IsEmpty ());
                    }
                    template      <typename  T, typename TRAITS>
                    T   LinkedList<T, TRAITS>::GetAt (size_t i) const
                    {
                        Require (i >= 0);
                        Require (i < GetLength ());
                        const Link* cur = _fHead;
                        for (; i != 0; cur = cur->fNext, --i) {
                            AssertNotNull (cur);    // cuz i <= fLength
                        }
                        AssertNotNull (cur);        // cuz i <= fLength
                        return cur->fItem;
                    }
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::SetAt (T item, size_t i)
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
#if     qDebug
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::Invariant_ () const
                    {
                        /*
                         * Check we are properly linked together.
                         */
                        for (Link* i = _fHead; i != nullptr; i = i->fNext) {
                            // at least make sure no currupted links and no infinite loops
                        }
                    }
#endif


                    /*
                    ********************************************************************************
                    ********************* LinkedList<T,TRAITS>::ForwardIterator ********************
                    ********************************************************************************
                    */
                    template      <typename  T, typename TRAITS>
                    inline  LinkedList<T, TRAITS>::ForwardIterator::ForwardIterator (const LinkedList<T, TRAITS>* data)
                    //: fCachedPrev (nullptr)
                        : _fData (data)
                        , _fCurrent (data->_fHead)
                        , _fSuppressMore (true)
                    {
                        RequireNotNull (data);
                    }
                    template      <typename  T, typename TRAITS>
                    inline  LinkedList<T, TRAITS>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
                    //: fCachedPrev (nullptr)
                        : _fData (from._fData)
                        , _fCurrent (from._fCurrent)
                        , _fSuppressMore (from._fSuppressMore)
                    {
                    }
                    template      <typename  T, typename TRAITS>
                    inline  typename LinkedList<T, TRAITS>::ForwardIterator&  LinkedList<T, TRAITS>::ForwardIterator::operator= (const ForwardIterator& rhs)
                    {
                        Invariant ();
                        _fCurrent = rhs._fCurrent;
                        _fSuppressMore = rhs._fSuppressMore;
                        Invariant ();
                        return (*this);
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::ForwardIterator::Invariant () const
                    {
#if     qDebug
                        Invariant_ ();
#endif
                    }
                    template      <typename  T, typename TRAITS>
                    inline  bool    LinkedList<T, TRAITS>::ForwardIterator::Done () const
                    {
                        Invariant ();
                        return _fCurrent == nullptr;
                    }
                    template      <typename  T, typename TRAITS>
                    inline  bool    LinkedList<T, TRAITS>::ForwardIterator::More (T* current, bool advance)
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
                        return not Done ();
                    }
                    template      <typename  T, typename TRAITS>
                    inline  void    LinkedList<T, TRAITS>::ForwardIterator::More (Memory::Optional<T>* result, bool advance)
                    {
                        RequireNotNull (result);
                        Invariant ();
                        if (advance) {
                            if (_fSuppressMore) {
                                _fSuppressMore = false;
                            }
                            else {
                                /*
                                 * We could already be done since after the last Done() call, we could
                                 * have done a removeall.
                                 */
                                if (_fCurrent != nullptr) {
                                    _fCurrent = _fCurrent->fNext;
                                }
                            }
                        }
                        Assert (not _fSuppressMore);
                        Invariant ();
                        if (this->Done ()) {
                            result->clear ();
                        }
                        else {
                            *result = _fCurrent->fItem;
                        }
                    }
                    template      <typename  T, typename TRAITS>
                    inline  T   LinkedList<T, TRAITS>::ForwardIterator::Current () const
                    {
                        Require (not (Done ()));
                        Invariant ();
                        AssertNotNull (_fCurrent);
                        return _fCurrent->fItem;
                    }
                    template      <typename  T, typename TRAITS>
                    size_t  LinkedList<T, TRAITS>::ForwardIterator::CurrentIndex () const
                    {
                        Require (not (Done ()));
                        RequireNotNull (_fData);
                        RequireNotNull (this->_fCurrent);
                        size_t i = 0;
                        for (const Link* l = _fData->_fHead; l != this->_fCurrent; l = l->fNext, ++i) {
                            AssertNotNull (l);
                        }
                        return i;
                    }
#if     qDebug
                    template      <typename  T, typename TRAITS>
                    void    LinkedList<T, TRAITS>::ForwardIterator::Invariant_ () const
                    {
                    }
#endif


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_DataStructures_LinkedList_inl_ */
