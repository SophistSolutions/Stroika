/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_inl_
#define _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_inl_   1


#include    "../../../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {


                    /*
                     ********************************************************************************
                     ***************** STLContainerWrapper<T, CONTAINER_OF_T> ***********************
                     ********************************************************************************
                     */
                    template    <typename T, typename CONTAINER_OF_T>
                    inline  STLContainerWrapper<T, CONTAINER_OF_T>::STLContainerWrapper ()
                        : CONTAINER_OF_T ()
                        , fActiveIteratorsListHead_ (nullptr)
                    {
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    inline  STLContainerWrapper<T, CONTAINER_OF_T>::STLContainerWrapper (const STLContainerWrapper<T, CONTAINER_OF_T>& from)
                        : CONTAINER_OF_T (from)
                        , fActiveIteratorsListHead_ (nullptr)
                    {
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    inline  STLContainerWrapper<T, CONTAINER_OF_T>::~STLContainerWrapper ()
                    {
                        Require (fActiveIteratorsListHead_ == nullptr); // cannot destroy container with active iterators
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    inline  STLContainerWrapper<T, CONTAINER_OF_T>& STLContainerWrapper<T, CONTAINER_OF_T>::operator= (const STLContainerWrapper<T, CONTAINER_OF_T>& rhs)
                    {
                        /*
                         * Don't copy the rhs iterators, and don't do assignments when we have active iterators.
                         * If this is to be supported at some future date, well need to work on our patching.
                         */
                        Assert (not (HasActiveIterators ()));   // cuz copy of array does not copy iterators...
                        CONTAINER_OF_T::operator= (rhs);
                        return *this;
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    inline  bool    STLContainerWrapper<T, CONTAINER_OF_T>::HasActiveIterators () const
                    {
                        return fActiveIteratorsListHead_ != nullptr;
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    inline  void    STLContainerWrapper<T, CONTAINER_OF_T>::PatchAfter_insert (typename CONTAINER_OF_T::iterator i) const
                    {
                        for (auto ai = fActiveIteratorsListHead_; ai != nullptr; ai = ai->fNextActiveIterator) {
                            ai->PatchAfter_insert (i);
                        }
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    inline  void    STLContainerWrapper<T, CONTAINER_OF_T>::PatchBefore_erase (typename CONTAINER_OF_T::iterator i) const
                    {
                        for (auto ai = fActiveIteratorsListHead_; ai != nullptr; ai = ai->fNextActiveIterator) {
                            ai->PatchBefore_erase (i);
                        }
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    inline  void    STLContainerWrapper<T, CONTAINER_OF_T>::PatchAfter_clear () const
                    {
                        for (auto ai = fActiveIteratorsListHead_; ai != nullptr; ai = ai->fNextActiveIterator) {
                            ai->PatchAfter_clear ();
                        }
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    inline  void    STLContainerWrapper<T, CONTAINER_OF_T>::PatchAfter_Realloc () const
                    {
                        for (auto ai = fActiveIteratorsListHead_; ai != nullptr; ai = ai->fNextActiveIterator) {
                            ai->PatchAfter_Realloc ();
                        }
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    inline  void    STLContainerWrapper<T, CONTAINER_OF_T>::Invariant () const
                    {
#if     qDebug
                        _Invariant ();
#endif
                    }
#if     qDebug
                    template    <typename T, typename CONTAINER_OF_T>
                    void    STLContainerWrapper<T, CONTAINER_OF_T>::_Invariant () const
                    {
                        for (auto ai = fActiveIteratorsListHead_; ai != nullptr; ai = ai->fNextActiveIterator) {
                            ai->Invariant ();
                        }
                    }
#endif



                    /*
                     ********************************************************************************
                     ******** STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper ***********
                     ********************************************************************************
                     */
                    template    <typename T, typename CONTAINER_OF_T>
                    STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper::IteratorPatchHelper (STLContainerWrapper<T, CONTAINER_OF_T>* data)
                        : fData (data)
                        , fStdIterator (data->begin ())
                        , fNextActiveIterator (data->fActiveIteratorsListHead_)
                        , fSuppressMore (true)
                    {
                        fData->fActiveIteratorsListHead_ = this;
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper::IteratorPatchHelper (const IteratorPatchHelper& from)
                        : fData (from.fData)
                        , fStdIterator (from.fStdIterator)
                        , fNextActiveIterator (from.fData->fActiveIteratorsListHead_)
                        , fSuppressMore (from.fSuppressMore)
                    {
                        fData->fActiveIteratorsListHead_ = this;
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper::~IteratorPatchHelper ()
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
                    template    <typename T, typename CONTAINER_OF_T>
                    typename  STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper& STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper::operator= (const IteratorPatchHelper& rhs)
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
                        }
                        return *this;
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    inline  bool    STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper::Done () const
                    {
                        return fStdIterator == fData->end ();
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    inline  bool    STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper::More (T* current, bool advance)
                    {
                        bool    done    =   Done ();
                        if (advance) {
                            if (not this->fSuppressMore and not done) {
                                this->fStdIterator++;
                            }
                            this->fSuppressMore = false;
                            done = Done ();
                            if ((current != nullptr) and (not done)) {
                                *current = *fStdIterator;
                            }
                        }
                        return (not done);
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    void    STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper::PatchAfter_insert (typename CONTAINER_OF_T::iterator i) const
                    {
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    void    STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper::PatchBefore_erase (typename CONTAINER_OF_T::iterator i) const
                    {
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    void    STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper::PatchAfter_clear () const
                    {
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    void    STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper::PatchAfter_Realloc () const
                    {
                    }
                    template    <typename T, typename CONTAINER_OF_T>
                    inline  void    STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper::Invariant () const
                    {
#if     qDebug
                        _Invariant ();
#endif
                    }
#if     qDebug
                    template    <typename T, typename CONTAINER_OF_T>
                    void    STLContainerWrapper<T, CONTAINER_OF_T>::IteratorPatchHelper::_Invariant () const
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
