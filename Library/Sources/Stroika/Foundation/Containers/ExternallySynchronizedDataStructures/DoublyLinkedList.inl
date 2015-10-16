/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_ExternallySynchronizedDataStructures_DoublyLinkedList_inl_
#define _Stroika_Foundation_Containers_ExternallySynchronizedDataStructures_DoublyLinkedList_inl_  1


#include    "../../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   ExternallySynchronizedDataStructures {


                /*
                ********************************************************************************
                ********************* DoublyLinkedList<T, TRAITS>::Link ************************
                ********************************************************************************
                */
                template      <typename  T, typename TRAITS>
                inline  DoublyLinkedList<T, TRAITS>::Link::Link (T item, Link* next)
                    : fItem (item)
                    , fNext (next)
                {
                }


                /*
                ********************************************************************************
                ************************* DoublyLinkedList<T, TRAITS> **************************
                ********************************************************************************
                */
                template      <typename  T, typename TRAITS>
                inline  DoublyLinkedList<T, TRAITS>::DoublyLinkedList ()
                    : _fHead (nullptr)
                {
                    Invariant ();
                }
                template      <typename  T, typename TRAITS>
                DoublyLinkedList<T, TRAITS>::DoublyLinkedList (const DoublyLinkedList<T, TRAITS>& from)
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
                inline  DoublyLinkedList<T, TRAITS>::~DoublyLinkedList ()
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
                template      <typename  T, typename TRAITS>
                inline  void    DoublyLinkedList<T, TRAITS>::Invariant () const
                {
#if     qDebug
                    Invariant_ ();
#endif
                }
                template      <typename  T, typename TRAITS>
                bool    DoublyLinkedList<T, TRAITS>::IsEmpty () const
                {
                    return _fHead == nullptr;
                }
                template      <typename  T, typename TRAITS>
                inline  size_t  DoublyLinkedList<T, TRAITS>::GetLength () const
                {
                    size_t  n   =   0;
                    for (const Link* i = _fHead; i != nullptr; i = i->fNext) {
                        n++;
                    }
                    return n;
                }
                template      <typename  T, typename TRAITS>
                inline  T   DoublyLinkedList<T, TRAITS>::GetFirst () const
                {
                    RequireNotNull (_fHead);
                    return _fHead->fItem;
                }
                template      <typename  T, typename TRAITS>
                inline  T   DoublyLinkedList<T, TRAITS>::GetLast () const
                {
                    RequireNotNull (_fHead);        // cannot call Getlast on empty list
                    // TMPHACK - must restore storage of fLast - somehow lost (sterl?) by moving this code from old stroika 1992 code?
                    // maybe irrelevant if we swtich to usting STL list class
                    for (const Link* i = _fHead; true; i = i->fNext) {
                        AssertNotNull (i);
                        if (i->fNext == nullptr) {
                            return i->fItem;
                        }
                    }
                }
                template      <typename  T, typename TRAITS>
                inline  void    DoublyLinkedList<T, TRAITS>::Prepend (T item)
                {
                    Invariant ();
                    _fHead = new Link (item, _fHead);
                    Invariant ();
                }
                template      <typename  T, typename TRAITS>
                inline  void    DoublyLinkedList<T, TRAITS>::RemoveFirst ()
                {
                    RequireNotNull (_fHead);
                    Invariant ();

                    Link* victim = _fHead;
                    _fHead = victim->fNext;
                    delete victim;

                    Invariant ();
                }
                template      <typename  T, typename TRAITS>
                inline  void    DoublyLinkedList<T, TRAITS>::RemoveLast ()
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
                template      <typename  T, typename TRAITS>
                DoublyLinkedList<T, TRAITS>& DoublyLinkedList<T, TRAITS>::operator= (const DoublyLinkedList<T, TRAITS>& rhs)
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
                template      <typename  T, typename TRAITS>
                void    DoublyLinkedList<T, TRAITS>::Remove (T item)
                {
                    Invariant ();

                    if (TRAITS::EqualsCompareFunctionType::Equals (item, _fHead->fItem)) {
                        RemoveFirst ();
                    }
                    else {
                        Link*    prev    =   nullptr;
                        for (Link* link = _fHead; link != nullptr; prev = link, link = link->fNext) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (link->fItem, item)) {
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
                bool    DoublyLinkedList<T, TRAITS>::Contains (ArgByValueType<T> item) const
                {
                    for (const Link* current = _fHead; current != nullptr; current = current->fNext) {
                        if (TRAITS::EqualsCompareFunctionType::Equals (current->fItem, item)) {
                            return true;
                        }
                    }
                    return false;
                }
                template    <typename  T, typename TRAITS>
                template    <typename FUNCTION>
                inline  void    DoublyLinkedList<T, TRAITS>::Apply (FUNCTION doToElement) const
                {
                    for (const Link* i = _fHead; i != nullptr; i = i->fNext) {
                        (doToElement) (i->fItem);
                    }
                }
                template    <typename  T, typename TRAITS>
                template    <typename FUNCTION>
                inline  typename DoublyLinkedList<T, TRAITS>::Link*    DoublyLinkedList<T, TRAITS>::FindFirstThat (FUNCTION doToElement) const
                {
                    for (Link* i = _fHead; i != nullptr; i = i->fNext) {
                        if ((doToElement) (i->fItem)) {
                            return i;
                        }
                    }
                    return nullptr;
                }
                template      <typename  T, typename TRAITS>
                void    DoublyLinkedList<T, TRAITS>::RemoveAll ()
                {
                    for (Link* i = _fHead; i != nullptr;) {
                        Link*    deleteMe    =   i;
                        i = i->fNext;
                        delete (deleteMe);
                    }
                    _fHead = nullptr;
                }
                template      <typename  T, typename TRAITS>
                T   DoublyLinkedList<T, TRAITS>::GetAt (size_t i) const
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
                template      <typename  T, typename TRAITS>
                void    DoublyLinkedList<T, TRAITS>::SetAt (size_t i, T item)
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
                template      <typename  T, typename TRAITS>
                inline  void    DoublyLinkedList<T, TRAITS>::MoveIteratorHereAfterClone (IteratorBaseType* pi, const DoublyLinkedList<T, TRAITS>* movedFrom)
                {
                    // TRICKY TODO - BUT MUST DO - MUST MOVE FROM OLD ITER TO NEW
                    // only way
                    //
                    // For STL containers, not sure how to find an equiv new iterator for an old one, but my best guess is to iterate through
                    // old for old, and when I match, stop on new
                    Require (pi->_fData == movedFrom);
                    auto newI = this->_fHead;
                    auto newE = nullptr;
                    auto oldI = movedFrom->_fHead;
                    auto oldE = nullptr;
                    while (oldI != pi->_fCurrent) {
                        Assert (newI != newE);
                        Assert (oldI != oldE);
                        newI = newI->fNext;
                        oldI = oldI->fNext;
                        Assert (newI != newE);
                        Assert (oldI != oldE);
                    }
                    Assert (oldI == pi->_fCurrent);
                    pi->_fCurrent = newI;
                    pi->_fData = this;
                }
                template      <typename  T, typename TRAITS>
                void    DoublyLinkedList<T, TRAITS>::RemoveAt (const ForwardIterator& i)
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
                    delete victim;
                    this->Invariant ();
                }
                template      <typename  T, typename TRAITS>
                void    DoublyLinkedList<T, TRAITS>::SetAt (const ForwardIterator& i, ArgByValueType<T> newValue)
                {
                    Require (not i.Done ());
                    this->Invariant ();
                    const_cast<Link*> (i._fCurrent)->fItem = newValue;
                    this->Invariant ();
                }
                template      <typename  T, typename TRAITS>
                void    DoublyLinkedList<T, TRAITS>::AddBefore (const ForwardIterator& i, ArgByValueType<T> newValue)
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
                template      <typename  T, typename TRAITS>
                void    DoublyLinkedList<T, TRAITS>::AddAfter (const ForwardIterator& i, ArgByValueType<T> newValue)
                {
                    Require (not i.Done ());
                    AssertNotNull (i._fCurrent); // since not done...
                    const_cast<Link*> (i._fCurrent)->fNext = new Link (newValue, i._fCurrent->fNext);
                }
#if     qDebug
                template      <typename  T, typename TRAITS>
                void    DoublyLinkedList<T, TRAITS>::Invariant_ () const
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
                **************** DoublyLinkedList<T, TRAITS>::ForwardIterator ******************
                ********************************************************************************
                */
                template      <typename  T, typename TRAITS>
                inline  DoublyLinkedList<T, TRAITS>::ForwardIterator::ForwardIterator (const DoublyLinkedList<T, TRAITS>* data)
                    : _fData (data)
                    , _fCurrent (data->_fHead)
                    , _fSuppressMore (true)
                {
                }
                template      <typename  T, typename TRAITS>
                inline  DoublyLinkedList<T, TRAITS>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
                    : _fData (from._fData)
                    , _fCurrent (from._fCurrent)
                    , _fSuppressMore (from._fSuppressMore)
                {
                }
                template      <typename  T, typename TRAITS>
                inline  typename DoublyLinkedList<T, TRAITS>::ForwardIterator&  DoublyLinkedList<T, TRAITS>::ForwardIterator::operator= (const ForwardIterator& rhs)
                {
                    Invariant ();
                    _fData = rhs._fData;
                    _fCurrent = rhs._fCurrent;
                    _fSuppressMore = rhs._fSuppressMore;
                    Invariant ();
                    return *this;
                }
                template      <typename  T, typename TRAITS>
                inline  void    DoublyLinkedList<T, TRAITS>::ForwardIterator::Invariant () const
                {
#if     qDebug
                    Invariant_ ();
#endif
                }
                template      <typename  T, typename TRAITS>
                inline  bool    DoublyLinkedList<T, TRAITS>::ForwardIterator::Done () const
                {
                    Invariant ();
                    return bool (_fCurrent == nullptr);
                }
                template      <typename  T, typename TRAITS>
                inline  bool    DoublyLinkedList<T, TRAITS>::ForwardIterator::More (T* current, bool advance)
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
                        AssertNotNull (_fCurrent);  // because Done() test
                        *current = _fCurrent->fItem;
                    }
                    return (not Done ());
                }
                template      <typename  T, typename TRAITS>
                inline  void    DoublyLinkedList<T, TRAITS>::ForwardIterator::More (Memory::Optional<T>* result, bool advance)
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
                        AssertNotNull (_fCurrent);  // because not done
                        *result = _fCurrent->fItem;
                    }
                }
                template      <typename  T, typename TRAITS>
                inline  bool    DoublyLinkedList<T, TRAITS>::ForwardIterator::More (nullptr_t, bool advance)
                {
                    return More (static_cast<T*> (nullptr), advance);
                }
                template      <typename  T, typename TRAITS>
                inline  T   DoublyLinkedList<T, TRAITS>::ForwardIterator::Current () const
                {
                    Require (not (Done ()));
                    Invariant ();
                    AssertNotNull (_fCurrent);
                    return _fCurrent->fItem;
                }
                template      <typename  T, typename TRAITS>
                size_t  DoublyLinkedList<T, TRAITS>::ForwardIterator::CurrentIndex () const
                {
                    Require (not (Done ()));
                    Invariant ();
                    size_t n = 0;
                    for (const Link* l = _fData->_fHead; l != this->_fCurrent; l = l->fNext, ++n) {
                        AssertNotNull (l);
                    }
                    return n;
                }
                template      <typename  T, typename TRAITS>
                inline  void    DoublyLinkedList<T, TRAITS>::ForwardIterator::SetCurrentLink (Link* l)
                {
                    // MUUST COME FROM THIS LIST
                    // CAN be nullptr
                    _fCurrent = l;
                    _fSuppressMore = false;
                }
                template      <typename  T, typename TRAITS>
                inline  bool    DoublyLinkedList<T, TRAITS>::ForwardIterator::Equals (const typename DoublyLinkedList<T, TRAITS>::ForwardIterator& rhs) const
                {
                    return _fCurrent == rhs._fCurrent and _fSuppressMore == rhs._fSuppressMore;
                }
                template      <typename  T, typename TRAITS>
                inline    typename DoublyLinkedList<T, TRAITS>::Link*   DoublyLinkedList<T, TRAITS>::ForwardIterator::_GetFirstDataLink (DoublyLinkedList<T, TRAITS>* data)
                {
                    RequireNotNull (data);
                    return data->_fHead;
                }
                template      <typename  T, typename TRAITS>
                inline const typename DoublyLinkedList<T, TRAITS>::Link* DoublyLinkedList<T, TRAITS>::ForwardIterator::_GetFirstDataLink (const DoublyLinkedList<T, TRAITS>* data)
                {
                    RequireNotNull (data);
                    return data->_fHead;
                }
                template      <typename  T, typename TRAITS>
                inline  void    DoublyLinkedList<T, TRAITS>::ForwardIterator::_SetFirstDataLink (DoublyLinkedList<T, TRAITS>* data, Link* newFirstLink)
                {
                    RequireNotNull (data);
                    data->_fHead = newFirstLink;
                }
#if     qDebug
                template      <typename  T, typename TRAITS>
                void    DoublyLinkedList<T, TRAITS>::ForwardIterator::Invariant_ () const
                {
                }
#endif



            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_ExternallySynchronizedDataStructures_DoublyLinkedList_inl_ */
