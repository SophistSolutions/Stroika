/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_inl_
#define _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_inl_   1

#include    <algorithm>

#include    "../../../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {


                    /*
                     ********************************************************************************
                     ******************* STLContainerWrapper<STL_CONTAINER_OF_T> ********************
                     ********************************************************************************
                     */
                    template    <typename STL_CONTAINER_OF_T>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::Contains (value_type item) const
                    {
                        return this->find (item) != this->end ();
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::Contains1 (value_type item) const
                    {
                        return std::find (this->begin (), this->end (), item) != this->end ();
                    }


                    /*
                     ********************************************************************************
                     ******** STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator *********
                     ********************************************************************************
                     */
                    template    <typename STL_CONTAINER_OF_T>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::BasicForwardIterator (STLContainerWrapper<STL_CONTAINER_OF_T>* data)
                        : fData (data)
                        , fStdIterator (data->begin ())
                    {
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::BasicForwardIterator (const BasicForwardIterator& from)
                        : fData (from.fData)
                        , fStdIterator (from.fStdIterator)
                    {
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::Done () const
                    {
                        return fStdIterator == fData->end ();
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    template    <typename VALUE_TYPE>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::More (VALUE_TYPE* current, bool advance)
                    {
                        bool    done    =   Done ();
                        if (advance) {
                            if (not done) {
                                fStdIterator++;
                                done = Done ();
                            }
                        }
                        if ((current != nullptr) and (not done)) {
                            *current = *fStdIterator;
                        }
                        return not done;
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  size_t    STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::CurrentIndex () const
                    {
                        return fStdIterator - fData->begin ();
                    }


                    /*
                     ********************************************************************************
                     ************** Patching::STLContainerWrapper<STL_CONTAINER_OF_T> ***************
                     ********************************************************************************
                     */
                    template    <typename STL_CONTAINER_OF_T>
                    inline  Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::STLContainerWrapper ()
                        : inherited ()
                        , fActiveIteratorsListHead_ (nullptr)
                    {
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::STLContainerWrapper (const STLContainerWrapper<STL_CONTAINER_OF_T>& from)
                        : inherited (from)
                        , fActiveIteratorsListHead_ (nullptr)
                    {
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::~STLContainerWrapper ()
                    {
                        Require (not HasActiveIterators ()); // cannot destroy container with active iterators
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  Patching::STLContainerWrapper<STL_CONTAINER_OF_T>&   Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::operator= (const Patching::STLContainerWrapper<STL_CONTAINER_OF_T>& rhs)
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
                    inline  bool    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::HasActiveIterators () const
                    {
                        return fActiveIteratorsListHead_ != nullptr;
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    template    <typename INSERT_VALUE_TYPE>
                    void    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::insert_toVector_WithPatching (typename STL_CONTAINER_OF_T::iterator i, INSERT_VALUE_TYPE v)
                    {
                        Invariant ();
                        insert (i, v);
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
                    void    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::erase_WithPatching (typename STL_CONTAINER_OF_T::iterator i)
                    {
                        Invariant ();
                        Memory::SmallStackBuffer<BasicForwardIterator*>   items2Patch (0);
                        TwoPhaseIteratorPatcherPass1 (i, &items2Patch);
                        auto newI = this->erase (i);
                        TwoPhaseIteratorPatcherPass2 (&items2Patch, newI);
                        Invariant ();
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    void    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::clear_WithPatching ()
                    {
                        Invariant ();
                        this->clear ();
                        for (auto ai = fActiveIteratorsListHead_; ai != nullptr; ai = ai->fNextActiveIterator) {
                            ai->TwoPhaseIteratorPatcherPass2 (this->end ());
                        }
                        Invariant ();
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  void    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::TwoPhaseIteratorPatcherPass1 (typename STL_CONTAINER_OF_T::iterator oldI, Memory::SmallStackBuffer<BasicForwardIterator*>* items2Patch) const
                    {
                        for (auto ai = fActiveIteratorsListHead_; ai != nullptr; ai = ai->fNextActiveIterator) {
                            ai->TwoPhaseIteratorPatcherPass1 (oldI, items2Patch);
                        }
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  void    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<BasicForwardIterator*>* items2Patch, typename STL_CONTAINER_OF_T::iterator newI)
                    {
                        for (size_t i = 0; i < items2Patch->GetSize (); ++i) {
                            (*items2Patch)[i]->TwoPhaseIteratorPatcherPass2 (newI);
                        }
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  void    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::Invariant () const
                    {
#if     qDebug
                        _Invariant ();
#endif
                    }
#if     qDebug
                    template    <typename STL_CONTAINER_OF_T>
                    void    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::_Invariant () const
                    {
                        for (auto ai = fActiveIteratorsListHead_; ai != nullptr; ai = ai->fNextActiveIterator) {
                            ai->Invariant ();
                        }
                    }
#endif


                    /*
                     ********************************************************************************
                     **** Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator ***
                     ********************************************************************************
                     */
                    template    <typename STL_CONTAINER_OF_T>
                    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::BasicForwardIterator (CONTAINER_TYPE* data)
                        : inherited (data)
                        , fData (data)
                        , fNextActiveIterator (data->fActiveIteratorsListHead_)
                        , fSuppressMore (true)
                    {
                        RequireNotNull (data);
                        fData->fActiveIteratorsListHead_ = this;
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::BasicForwardIterator (const BasicForwardIterator& from)
                        : inherited (from)
                        , fData (from.fData)
                        , fNextActiveIterator (from.fData->fActiveIteratorsListHead_)
                        , fSuppressMore (from.fSuppressMore)
                    {
                        RequireNotNull (fData);
                        fData->fActiveIteratorsListHead_ = this;
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::~BasicForwardIterator ()
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
                    typename  Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator&   Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::operator= (const BasicForwardIterator& rhs)
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
                    inline  bool    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::More (VALUE_TYPE* current, bool advance)
                    {
                        if (advance and fSuppressMore) {
                            advance = false;
                            fSuppressMore = false;
                        }
                        return inherited::More (current, advance);
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  void    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::RemoveCurrent ()
                    {
                        AssertNotNull (fData);
                        fData->erase_WithPatching (this->fStdIterator);
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    void    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::TwoPhaseIteratorPatcherPass1 (typename STL_CONTAINER_OF_T::iterator oldI, Memory::SmallStackBuffer<BasicForwardIterator*>* items2Patch)
                    {
                        if (this->fStdIterator == oldI) {
                            items2Patch->push_back (this);
                        }
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    void    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::TwoPhaseIteratorPatcherPass2 (typename STL_CONTAINER_OF_T::iterator newI)
                    {
                        fSuppressMore = true;
                        this->fStdIterator = newI;
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  void    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::Invariant () const
                    {
#if     qDebug
                        _Invariant ();
#endif
                    }
#if     qDebug
                    template    <typename STL_CONTAINER_OF_T>
                    void    Patching::STLContainerWrapper<STL_CONTAINER_OF_T>::BasicForwardIterator::_Invariant () const
                    {
                        AssertNotNull (fData);
                        // Find some way to check iterator traits to see if random access and do this test if possible
                        //Assert ((fCurrent >= fStart) and (fCurrent <= fEnd));   // ANSI C requires this is always TRUE
                    }
#endif


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_inl_ */
