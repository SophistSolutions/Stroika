/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_inl_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_inl_ 1

#include "../../../Debug/Assertions.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Private {
                namespace PatchingDataStructures {

                    /*
                    ********************************************************************************
                    *************** PatchingDataStructures::Array<T,TRAITS> ************************
                    ********************************************************************************
                    */
                    template <typename T, typename TRAITS>
                    inline Array<T, TRAITS>::Array ()
                        : inherited ()
                    {
                        this->Invariant ();
                    }
                    template <typename T, typename TRAITS>
                    inline Array<T, TRAITS>::Array (Array<T, TRAITS>* rhs, IteratorOwnerID newOwnerID)
                        : inherited (rhs, newOwnerID, (_ArrayIteratorBase*)nullptr)
                    {
                        RequireNotNull (rhs);
                        rhs->Invariant ();
                        this->Invariant ();
                    }
                    template <typename T, typename TRAITS>
                    inline Array<T, TRAITS>::~Array ()
                    {
                        this->Invariant ();
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::PatchViewsAdd (size_t index) const
                    {
                        /*
                         *      Must call PatchRealloc before PatchAdd() since the test of currentIndex
                         *  depends on things being properly adjusted.
                         */
                        this->template _ApplyToEachOwnedIterator<_ArrayIteratorBase> ([index](_ArrayIteratorBase* ai) {
                            ai->PatchRealloc ();
                            ai->PatchAdd (index);
                        });
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::PatchViewsRemove (size_t index) const
                    {
                        this->template _ApplyToEachOwnedIterator<_ArrayIteratorBase> ([index](_ArrayIteratorBase* ai) {
                            ai->PatchRemove (index);
                        });
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::PatchViewsRemoveAll () const
                    {
                        this->template _ApplyToEachOwnedIterator<_ArrayIteratorBase> ([](_ArrayIteratorBase* ai) {
                            ai->PatchRemoveAll ();
                        });
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::PatchViewsRealloc () const
                    {
                        this->template _ApplyToEachOwnedIterator<_ArrayIteratorBase> ([](_ArrayIteratorBase* ai) {
                            ai->PatchRealloc ();
                        });
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::SetLength (size_t newLength, T fillValue)
                    {
                        // For now, not sure how to patch the iterators, so just Assert out - fix later ...
                        AssertNotImplemented ();
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::InsertAt (size_t index, T item)
                    {
                        this->Invariant ();
                        inherited::InsertAt (index, item);
                        PatchViewsAdd (index); // PatchRealloc done in PatchViewsAdd()
                        this->Invariant ();
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::RemoveAt (size_t index)
                    {
                        this->Invariant ();
                        PatchViewsRemove (index);
                        inherited::RemoveAt (index);
                        // Dont call PatchViewsRealloc () since removeat does not do a SetCapacity, it
                        // just destructs things.
                        this->Invariant ();
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::RemoveAll ()
                    {
                        this->Invariant ();
                        inherited::RemoveAll ();
                        PatchViewsRemoveAll (); // PatchRealloc not needed cuz removeall just destructs things,
                        // it does not realloc pointers (ie does not call SetCapacity).
                        this->Invariant ();
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::RemoveAt (const ForwardIterator& i)
                    {
                        Require (not i.Done ());
                        RemoveAt (i.CurrentIndex ());
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::RemoveAt (const BackwardIterator& i)
                    {
                        Require (not i.Done ());
                        RemoveAt (i.CurrentIndex ());
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::SetAt (size_t i, T newValue)
                    {
                        inherited::SetAt (i, newValue);
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::SetAt (const ForwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        inherited::SetAt (i.CurrentIndex (), newValue);
                    }
                    template <typename T, typename TRAITS>
                    void Array<T, TRAITS>::SetAt (const BackwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        inherited::SetAt (newValue, i.CurrentIndex ());
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::AddBefore (const ForwardIterator& i, T newValue)
                    {
                        // i CAN BE DONE OR NOT
                        InsertAt (i.CurrentIndex (), newValue);
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::AddBefore (const BackwardIterator& i, T newValue)
                    {
                        // i CAN BE DONE OR NOT
                        InsertAt (i.CurrentIndex (), newValue);
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::AddAfter (const ForwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        InsertAt (i.CurrentIndex () + 1, newValue);
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::AddAfter (const BackwardIterator& i, T newValue)
                    {
                        Require (not i.Done ());
                        InsertAt (i.CurrentIndex () + 1, newValue);
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::SetCapacity (size_t slotsAlloced)
                    {
                        this->Invariant ();
                        inherited::SetCapacity (slotsAlloced);
                        PatchViewsRealloc ();
                        this->Invariant ();
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::Compact ()
                    {
                        this->Invariant ();
                        inherited::Compact ();
                        PatchViewsRealloc ();
                        this->Invariant ();
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::Invariant () const
                    {
#if qDebug
                        _Invariant ();
#endif
                    }
#if qDebug
                    template <typename T, typename TRAITS>
                    void Array<T, TRAITS>::_Invariant () const
                    {
                        inherited::_Invariant ();
                        /*
                         *      Be sure each iterator points back to us. Thats about all we can test from
                         *  here since we cannot call each iterators Invariant(). That would be
                         *  nice, but sadly when this Invariant_ () is called from Array<T,TRAITS> the
                         *  iterators themselves may not have been patched, so they'll be out of
                         *  date. Instead, so that in local shadow of Invariant() done in Array<T,TRAITS>
                         *  so only called when WE call Invariant().
                         */
                        this->template _ApplyToEachOwnedIterator<_ArrayIteratorBase> ([this](_ArrayIteratorBase* ai) {
                            Assert (ai->_fData == this);
                            ai->Invariant ();
                        });
                    }
#endif /*qDebug*/

                    /*
                     ********************************************************************************
                     ********* PatchingDataStructures::Array<T, TRAITS>::_ArrayIteratorBase *********
                     ********************************************************************************
                     */
                    template <typename T, typename TRAITS>
                    inline Array<T, TRAITS>::_ArrayIteratorBase::_ArrayIteratorBase (IteratorOwnerID ownerID, const Array<T, TRAITS>* data)
                        : inherited_DataStructure (data)
                        , inherited_PatchHelper (const_cast<Array<T, TRAITS>*> (data), ownerID)
                    {
                        /*
                         * Cannot call invariant () here since _fCurrent not yet setup.
                         */
                    }
                    template <typename T, typename TRAITS>
                    inline Array<T, TRAITS>::_ArrayIteratorBase::_ArrayIteratorBase (const typename Array<T, TRAITS>::_ArrayIteratorBase& from)
                        : inherited_DataStructure (from)
                        , inherited_PatchHelper (from)
                    {
                        this->Invariant ();
                    }
                    template <typename T, typename TRAITS>
                    inline Array<T, TRAITS>::_ArrayIteratorBase::~_ArrayIteratorBase ()
                    {
                        this->Invariant ();
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::_ArrayIteratorBase::PatchAdd (size_t index)
                    {
                        /*
                         *      NB: We cannot call invariants here because this is called after the add
                         *  and the PatchRealloc has not yet happened.
                         */
                        Require (index >= 0);

                        this->_fEnd++;

                        AssertNotNull (this->_fData);

                        /*
                         *      If we added an item to the right of our cursor, it has no effect
                         *  on our - by index - addressing, and so ignore it. We will eventually
                         *  reach that new item.
                         *
                         *      On the other hand, if we add the item to the left of our cursor,
                         *  things are more complex:
                         *
                         *      If we added an item left of the cursor, then we are now pointing to
                         *  the item before the one we used to, and so incrementing (ie Next)
                         *  would cause us to revisit (in the forwards case, or skip one in the
                         *  reverse case). To correct our index, we must increment it so that
                         *  it.Current () refers to the same entity.
                         *
                         *      Note that this should indeed by <=, since (as opposed to <) since
                         *  if we are a direct hit, and someone tries to insert something at
                         *  the position we are at, the same argument as before applies - we
                         *  would be revisiting, or skipping forwards an item.
                         */

                        Require ((this->_fEnd >= this->_fStart) and (index <= size_t (this->_fEnd - this->_fStart)));
                        if (&this->_fStart[index] <= this->_fCurrent) { // index <= CurrentIndex () - only faster
                            // Cannot call CurrentIndex () since invariants
                            // might fail at this point
                            this->_fCurrent++;
                        }
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::_ArrayIteratorBase::PatchRemove (size_t index)
                    {
                        Require (index >= 0);
                        Require (index < this->_fData->GetLength ());

                        /*
                         *      If we are removing an item from the right of our cursor, it has no effect
                         *  on our - by index - addressing, and so ignore it.
                         *
                         *      On the other hand, if we are removing the item from the left of our cursor,
                         *  things are more complex:
                         *
                         *      If we are removing an item from the left of the cursor, then we are now
                         *  pointing to the item after the one we used to, and so decrementing (ie Next)
                         *  would cause us to skip one. To correct our index, we must decrement it so that
                         *  it.Current () refers to the same entity.
                         *
                         *      In the case where we are directly hit, just set _fSuppressMore
                         *  to true. If we are going forwards, are are already pointing where
                         *  we should be (and this works for repeat deletions). If we are
                         *  going backwards, then _fSuppressMore will be ignored, but for the
                         *  sake of code sharing, its tough to do much about that waste.
                         */
                        Assert ((&this->_fStart[index] <= this->_fCurrent) == (index <= this->CurrentIndex ()));
                        if (&this->_fStart[index] < this->_fCurrent) {
                            Assert (this->CurrentIndex () >= 1);
                            this->_fCurrent--;
                        }
                        else if (&this->_fStart[index] == this->_fCurrent) {
                            PatchRemoveCurrent ();
                        }
                        // Decrement at the end since this->CurrentIndex () calls stuff that asserts (_fEnd-fStart) == this->fData->GetLength ()
                        Assert (size_t (this->_fEnd - this->_fStart) == this->_fData->GetLength ()); //  since called before remove

                        /*
                         * At this point, _fCurrent could be == _fEnd - must not lest _fCurrent point past!
                         */
                        if (this->_fCurrent == this->_fEnd) {
                            Assert (this->_fCurrent > this->_fStart); // since we are removing something start!=end
                            this->_fCurrent--;
                        }
                        this->_fEnd--;
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::_ArrayIteratorBase::PatchRemoveAll ()
                    {
                        Require (this->_fData->GetLength () == 0); //  since called after removeall

                        this->_fCurrent      = this->_fData->_fItems;
                        this->_fStart        = this->_fData->_fItems;
                        this->_fEnd          = this->_fData->_fItems;
                        this->_fSuppressMore = true;
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::_ArrayIteratorBase::PatchRealloc ()
                    {
                        /*
                         *      NB: We can only call invariant after we've fixed things up, since realloc
                         * has happened by now, but things don't point to the right places yet.
                         */
                        if (this->_fStart != this->_fData->_fItems) {
                            const T* oldStart = this->_fStart;
                            this->_fStart     = this->_fData->_fItems;
                            this->_fCurrent   = this->_fData->_fItems + (this->_fCurrent - oldStart);
                            this->_fEnd       = this->_fData->_fItems + (this->_fEnd - oldStart);
                        }
                    }
#if qDebug
                    template <typename T, typename TRAITS>
                    void Array<T, TRAITS>::_ArrayIteratorBase::_Invariant () const
                    {
                        inherited_DataStructure::_Invariant ();
                    }
#endif /*qDebug*/

                    /*
                    ********************************************************************************
                    ********** PatchingDataStructures::Array<T,TRAITS>::ForwardIterator ************
                    ********************************************************************************
                    */
                    template <typename T, typename TRAITS>
                    inline Array<T, TRAITS>::ForwardIterator::ForwardIterator (IteratorOwnerID ownerID, const Array<T, TRAITS>* data)
                        : inherited (ownerID, data)
                    {
                        RequireNotNull (data);
                        this->_fCurrent = this->_fStart;
                        this->Invariant ();
                    }
                    template <typename T, typename TRAITS>
                    inline bool Array<T, TRAITS>::ForwardIterator::More (T* current, bool advance)
                    {
                        this->Invariant ();
                        if (advance) {
                            if (not this->_fSuppressMore and not this->Done ()) {
                                Assert (this->_fCurrent < this->_fEnd);
                                this->_fCurrent++;
                            }
                            this->_fSuppressMore = false;
                        }
                        if ((current != nullptr) and (not this->Done ())) {
                            *current = *this->_fCurrent;
                        }
                        this->Invariant ();
                        return (not this->Done ());
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::ForwardIterator::More (Memory::Optional<T>* result, bool advance)
                    {
                        //// RESTRUCTUIRE SO WE CAN INHERIT IMPL!!!
                        RequireNotNull (result);
                        this->Invariant ();
                        if (advance) {
                            if (not this->_fSuppressMore and not this->Done ()) {
                                Assert (this->_fCurrent < this->_fEnd);
                                this->_fCurrent++;
                            }
                            this->_fSuppressMore = false;
                        }
                        this->Invariant ();
                        if (this->Done ()) {
                            result->clear ();
                        }
                        else {
                            *result = *this->_fCurrent;
                        }
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::ForwardIterator::PatchRemoveCurrent ()
                    {
                        Assert (this->_fCurrent < this->_fEnd); // cannot remove something past the end
                        this->_fSuppressMore = true;
                    }
                    template <typename T, typename TRAITS>
                    inline bool Array<T, TRAITS>::ForwardIterator::More (nullptr_t, bool advance)
                    {
                        return More (static_cast<T*> (nullptr), advance);
                    }

                    /*
                    ********************************************************************************
                    ****** PatchingDataStructures::Array<T,TRAITS>::BackwardIterator ***************
                    ********************************************************************************
                    */
                    template <typename T, typename TRAITS>
                    inline Array<T, TRAITS>::BackwardIterator::BackwardIterator (IteratorOwnerID ownerID, const Array<T, TRAITS>* data)
                        : inherited (ownerID, data)
                    {
                        if (data->GetLength () == 0) {
                            this->_fCurrent = this->_fEnd; // magic to indicate done
                        }
                        else {
                            this->_fCurrent = this->_fEnd - 1; // last valid item
                        }
                        this->Invariant ();
                    }
                    // Careful to keep hdr and src copies identical...
                    template <typename T, typename TRAITS>
                    inline bool Array<T, TRAITS>::BackwardIterator::More (T* current, bool advance)
                    {
                        this->Invariant ();
                        if (advance) {
                            if (this->_fSuppressMore) {
                                this->_fSuppressMore = false;
                                if (not this->Done ()) {
                                    *current = *(this->_fCurrent);
                                }
                                return (not this->Done ());
                            }
                            else {
                                if (this->Done ()) {
                                    return (false);
                                }
                                else {
                                    if (this->_fCurrent == this->fStart) {
                                        this->_fCurrent = this->_fEnd; // magic to indicate done
                                        Ensure (this->Done ());
                                        return (false);
                                    }
                                    else {
                                        this->_fCurrent--;
                                        *current = *(this->_fCurrent);
                                        Ensure (not this->Done ());
                                        return (true);
                                    }
                                }
                            }
                        }
                        return (not this->Done ());
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::BackwardIterator::More (Memory::Optional<T>* result, bool advance)
                    {
                        //// RESTRUCTUIRE SO WE CAN INHERIT IMPL!!!
                        RequireNotNull (result);
                        this->Invariant ();
                        if (advance) {
                            if (this->_fSuppressMore) {
                                this->_fSuppressMore = false;
                            }
                            else {
                                if (not this->Done ()) {
                                    if (this->_fCurrent == this->fStart) {
                                        this->_fCurrent = this->_fEnd; // magic to indicate done
                                        Ensure (this->Done ());
                                    }
                                    else {
                                        this->_fCurrent--;
                                    }
                                }
                            }
                        }
                        this->Invariant ();
                        if (this->Done ()) {
                            result->clear ();
                        }
                        else {
                            *result = *this->_fCurrent;
                        }
                    }
                    template <typename T, typename TRAITS>
                    inline bool Array<T, TRAITS>::BackwardIterator::More (nullptr_t, bool advance)
                    {
                        return More (static_cast<T*> (nullptr), advance);
                    }
                    template <typename T, typename TRAITS>
                    inline void Array<T, TRAITS>::BackwardIterator::PatchRemoveCurrent ()
                    {
                        if (this->_fCurrent == this->_fStart) {
                            this->_fCurrent = this->_fEnd; // magic to indicate done
                        }
                        else {
                            Assert (this->_fCurrent > this->_fStart);
                            this->_fCurrent--;
                        }
                        this->_fSuppressMore = true;
                    }
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_Array_inl_ */
