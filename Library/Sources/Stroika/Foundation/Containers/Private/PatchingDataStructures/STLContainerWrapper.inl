/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
                     ** PatchingDataStructures::STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER> *****
                     ********************************************************************************
                     */
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::STLContainerWrapper ()
                        : inherited ()
                    {
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::STLContainerWrapper (STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>* rhs, IteratorOwnerID newOwnerID)
                        : inherited (rhs, newOwnerID, (ForwardIterator*)nullptr)
                    {
                        RequireNotNull (rhs);
                        rhs->Invariant ();
                        Invariant ();
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    template    <typename INSERT_VALUE_TYPE>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::insert_toVector_WithPatching (typename STL_CONTAINER_OF_T::iterator i, INSERT_VALUE_TYPE v)
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
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::erase_WithPatching (typename STL_CONTAINER_OF_T::iterator i)
                    {
                        Invariant ();
                        Memory::SmallStackBuffer<ForwardIterator*>   items2Patch (0);
                        TwoPhaseIteratorPatcherPass1 (i, &items2Patch);
                        auto newI = this->erase (i);
                        TwoPhaseIteratorPatcherPass2 (&items2Patch, newI);
                        Invariant ();
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::clear_WithPatching ()
                    {
                        Invariant ();
                        this->clear ();
                        for (auto ai = this->template GetFirstActiveIterator<ForwardIterator> ();  ai != nullptr; ai = ai->template GetNextActiveIterator<ForwardIterator> ()) {
                            ai->TwoPhaseIteratorPatcherPass2 (this->end ());
                        }
                        Invariant ();
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::TwoPhaseIteratorPatcherPass1 (typename STL_CONTAINER_OF_T::iterator oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch) const
                    {
                        for (auto ai = this->template GetFirstActiveIterator<ForwardIterator> (); ai != nullptr; ai = ai->template GetNextActiveIterator<ForwardIterator> ()) {
                            ai->TwoPhaseIteratorPatcherPass1 (oldI, items2Patch);
                        }
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<ForwardIterator*>* items2Patch, typename STL_CONTAINER_OF_T::iterator newI)
                    {
                        for (size_t i = 0; i < items2Patch->GetSize (); ++i) {
                            (*items2Patch)[i]->TwoPhaseIteratorPatcherPass2 (newI);
                        }
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::Invariant () const
                    {
#if     qDebug
                        _Invariant ();
#endif
                    }
#if     qDebug
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    void    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::_Invariant () const
                    {
                        for (auto ai = this->template GetFirstActiveIterator<ForwardIterator> (); ai != nullptr; ai = ai->template GetNextActiveIterator<ForwardIterator> ()) {
                            ai->Invariant ();
                        }
                    }
#endif


                    /*
                     ********************************************************************************
                     * PatchingDataStructures::STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator *
                     ********************************************************************************
                     */
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator::ForwardIterator (IteratorOwnerID ownerID, CONTAINER_TYPE* data)
                        : inherited_DataStructure (data)
                        , inherited_PatchHelper (const_cast<STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>*> (data), ownerID)
                        , fSuppressMore (true)
                    {
                        RequireNotNull (data);
                        this->Invariant ();
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
                        : inherited_DataStructure (from)
                        , inherited_PatchHelper (from)
                        , fSuppressMore (from.fSuppressMore)
                    {
                        this->Invariant ();
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator::~ForwardIterator ()
                    {
                        this->Invariant ();
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  typename  STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator&   STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator::operator= (const ForwardIterator& rhs)
                    {
                        this->Invariant ();
                        inherited_DataStructure::operator= (rhs);
                        inherited_PatchHelper::operator= (rhs);
                        this->Invariant ();
                        return *this;
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    template    <typename VALUE_TYPE>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator::More (VALUE_TYPE* current, bool advance)
                    {
                        if (advance and fSuppressMore) {
                            advance = false;
                            fSuppressMore = false;
                        }
                        return inherited_DataStructure::More (current, advance);
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    template    <typename VALUE_TYPE>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator::More (Memory::Optional<VALUE_TYPE>* result, bool advance)
                    {
                        if (advance and fSuppressMore) {
                            advance = false;
                            fSuppressMore = false;
                        }
                        inherited_DataStructure::More (result, advance);
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator::RemoveCurrent ()
                    {
                        AssertNotNull (this->fData);
                        static_cast<STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>*> (this->fData)->erase_WithPatching (this->fStdIterator);
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator::TwoPhaseIteratorPatcherPass1 (typename STL_CONTAINER_OF_T::iterator oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch)
                    {
                        if (this->fStdIterator == oldI) {
                            items2Patch->push_back (this);
                        }
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator::TwoPhaseIteratorPatcherPass2 (typename STL_CONTAINER_OF_T::iterator newI)
                    {
                        fSuppressMore = true;
                        this->fStdIterator = newI;
                    }
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator::Invariant () const
                    {
#if     qDebug
                        _Invariant ();
#endif
                    }
#if     qDebug
                    template    <typename STL_CONTAINER_OF_T, typename LOCKER>
                    void    STLContainerWrapper<STL_CONTAINER_OF_T, LOCKER>::ForwardIterator::_Invariant () const
                    {
                        AssertNotNull (this->fData);
                        // apparently pointless test is just to force triggering any check code in iterator classes for valid iterators
                        Assert (this->fStdIterator == this->fData->end () or this->fStdIterator != this->fData->end ());
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
