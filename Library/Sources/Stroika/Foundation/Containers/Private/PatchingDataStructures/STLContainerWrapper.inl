/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_STLContainerWrapper_inl_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_STLContainerWrapper_inl_   1

#include    <algorithm>

#include    "../../../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   PatchingDataStructures {


                    /*
                     ********************************************************************************
                     ******************* STLContainerWrapper<STL_CONTAINER_OF_T> ********************
                     ********************************************************************************
                     */
                    template    <typename STL_CONTAINER_OF_T>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T>::STLContainerWrapper ()
                        : inherited ()
                        , fActiveIteratorsListHead_ (nullptr)
                    {
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T>::STLContainerWrapper (const STLContainerWrapper<STL_CONTAINER_OF_T>& from)
                        : inherited (from)
                        , fActiveIteratorsListHead_ (nullptr)
                    {
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T>::~STLContainerWrapper ()
                    {
                        Require (not HasActiveIterators ()); // cannot destroy container with active iterators
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T>&   STLContainerWrapper<STL_CONTAINER_OF_T>::operator= (const STLContainerWrapper<STL_CONTAINER_OF_T>& rhs)
                    {
                        /*
                         * Don't copy the rhs iterators, and don't do assignments when we have active iterators.
                         * If this is to be supported at some future date, well need to work on our patching.
                         */
                        Require (not HasActiveIterators ()); // cannot overwrite container with active iterators
                        inherited::operator= (rhs);
                        return *this;
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::HasActiveIterators () const
                    {
                        return fActiveIteratorsListHead_ != nullptr;
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    template    <typename INSERT_VALUE_TYPE>
                    void    STLContainerWrapper<STL_CONTAINER_OF_T>::insert_toVector_WithPatching (typename STL_CONTAINER_OF_T::iterator i, INSERT_VALUE_TYPE v)
                    {
                        Invariant ();
                        this->insert (i, v);
                        /// WRONG!!!! - must be more sophisticated. Must map all iteraotrs to OFFSETS, and then replace with new iteartors based on old offsets!!!
#if 0
                        // USE TwoPhaseIteratorPatcherPass1 - to captch all - and capture their offsets - need new overload - adn then new overload to patch back...
                        for (auto ai = fActiveIteratorsListHead_; ai != nullptr; ai = ai->fNextActiveIterator) {
                            ai->PatchAfter_insert (i);
                        }
#endif
                        Invariant ();
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    void    STLContainerWrapper<STL_CONTAINER_OF_T>::erase_WithPatching (typename STL_CONTAINER_OF_T::iterator i)
                    {
                        Invariant ();
                        Memory::SmallStackBuffer<ForwardIterator*>   items2Patch (0);
                        TwoPhaseIteratorPatcherPass1 (i, &items2Patch);
                        auto newI = this->erase (i);
                        TwoPhaseIteratorPatcherPass2 (&items2Patch, newI);
                        Invariant ();
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    void    STLContainerWrapper<STL_CONTAINER_OF_T>::clear_WithPatching ()
                    {
                        Invariant ();
                        this->clear ();
                        for (auto ai = fActiveIteratorsListHead_; ai != nullptr; ai = ai->fNextActiveIterator) {
                            ai->TwoPhaseIteratorPatcherPass2 (this->end ());
                        }
                        Invariant ();
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T>::TwoPhaseIteratorPatcherPass1 (typename STL_CONTAINER_OF_T::iterator oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch) const
                    {
                        for (auto ai = fActiveIteratorsListHead_; ai != nullptr; ai = ai->fNextActiveIterator) {
                            ai->TwoPhaseIteratorPatcherPass1 (oldI, items2Patch);
                        }
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T>::TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<ForwardIterator*>* items2Patch, typename STL_CONTAINER_OF_T::iterator newI)
                    {
                        for (size_t i = 0; i < items2Patch->GetSize (); ++i) {
                            (*items2Patch)[i]->TwoPhaseIteratorPatcherPass2 (newI);
                        }
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T>::Invariant () const
                    {
#if     qDebug
                        _Invariant ();
#endif
                    }
#if     qDebug
                    template    <typename STL_CONTAINER_OF_T>
                    void    STLContainerWrapper<STL_CONTAINER_OF_T>::_Invariant () const
                    {
                        for (auto ai = fActiveIteratorsListHead_; ai != nullptr; ai = ai->fNextActiveIterator) {
                            ai->Invariant ();
                        }
                    }
#endif


                    /*
                     ********************************************************************************
                     ************ STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator **********
                     ********************************************************************************
                     */
                    template    <typename STL_CONTAINER_OF_T>
                    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::ForwardIterator (CONTAINER_TYPE* data)
                        : inherited (data)
                        , fData (data)
                        , fNextActiveIterator (data->fActiveIteratorsListHead_)
                        , fSuppressMore (true)
                    {
                        RequireNotNull (data);
                        fData->fActiveIteratorsListHead_ = this;
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
                        : inherited (from)
                        , fData (from.fData)
                        , fNextActiveIterator (from.fData->fActiveIteratorsListHead_)
                        , fSuppressMore (from.fSuppressMore)
                    {
                        RequireNotNull (fData);
                        fData->fActiveIteratorsListHead_ = this;
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::~ForwardIterator ()
                    {
                        AssertNotNull (fData);
                        if (fData->fActiveIteratorsListHead_ == this) {
                            fData->fActiveIteratorsListHead_ = fNextActiveIterator;
                        }
                        else {
                            auto v = fData->fActiveIteratorsListHead_;
                            for (; v->fNextActiveIterator != this; v = v->fNextActiveIterator) {
                                AssertNotNull (v);
                                AssertNotNull (v->fNextActiveIterator);
                            }
                            AssertNotNull (v);
                            Assert (v->fNextActiveIterator == this);
                            v->fNextActiveIterator = fNextActiveIterator;
                        }
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    typename  STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator&   STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::operator= (const ForwardIterator& rhs)
                    {
                        /*
                         *      If the fData field has not changed, then we can leave alone our iterator linkage.
                         *  Otherwise, we must remove ourselves from the old, and add ourselves to the new.
                         */
                        if (fData != rhs.fData) {
                            AssertNotNull (fData);
                            AssertNotNull (rhs.fData);

                            /*
                             * Remove from old.
                             */
                            if (fData->fIterators == this) {
                                fData->fIterators = fNextActiveIterator;
                            }
                            else {
                                auto v = fData->fIterators;
                                for (; v->fNextActiveIterator != this; v = v->fNextActiveIterator) {
                                    AssertNotNull (v);
                                    AssertNotNull (v->fNextActiveIterator);
                                }
                                AssertNotNull (v);
                                Assert (v->fNextActiveIterator == this);
                                v->fNextActiveIterator = fNextActiveIterator;
                            }

                            /*
                             * Add to new.
                             */
                            fData = rhs.fData;
                            fNextActiveIterator = rhs.fData->fIterators;
                            fData->fIterators = this;
                            inherited::operator= (rhs);
                        }
                        return *this;
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    template    <typename VALUE_TYPE>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::More (VALUE_TYPE* current, bool advance)
                    {
                        if (advance and fSuppressMore) {
                            advance = false;
                            fSuppressMore = false;
                        }
                        return inherited::More (current, advance);
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    template    <typename VALUE_TYPE>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::More (Memory::Optional<VALUE_TYPE>* result, bool advance)
                    {
                        if (advance and fSuppressMore) {
                            advance = false;
                            fSuppressMore = false;
                        }
                        inherited::More (result, advance);
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::RemoveCurrent ()
                    {
                        AssertNotNull (fData);
                        fData->erase_WithPatching (this->fStdIterator);
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    void    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::TwoPhaseIteratorPatcherPass1 (typename STL_CONTAINER_OF_T::iterator oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch)
                    {
                        if (this->fStdIterator == oldI) {
                            items2Patch->push_back (this);
                        }
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    void    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::TwoPhaseIteratorPatcherPass2 (typename STL_CONTAINER_OF_T::iterator newI)
                    {
                        fSuppressMore = true;
                        this->fStdIterator = newI;
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::Invariant () const
                    {
#if     qDebug
                        _Invariant ();
#endif
                    }
#if     qDebug
                    template    <typename STL_CONTAINER_OF_T>
                    void    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::_Invariant () const
                    {
                        AssertNotNull (fData);
                        // apparently pointless test is just to force triggering any check code in iterator classes for valid iterators
                        Assert (this->fStdIterator == fData->end () or this->fStdIterator != fData->end ());
                        // Find some way to check iterator traits to see if random access and do this test if possible
                        //Assert ((fCurrent >= fStart) and (fCurrent <= fEnd));   // ANSI C requires this is always TRUE
                    }
#endif


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_PatchingDataStructures_STLContainerWrapper_inl_ */
