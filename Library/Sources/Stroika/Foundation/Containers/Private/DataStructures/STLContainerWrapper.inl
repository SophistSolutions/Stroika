/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
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
                        shared_lock<const AssertExternallySynchronizedLock> critSec { *this };
                        return this->find (item) != this->end ();
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T>::MoveIteratorHereAfterClone (IteratorBaseType* pi, const STLContainerWrapper<STL_CONTAINER_OF_T>* movedFrom)
                    {
                        lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
                        // TRICKY TODO - BUT MUST DO - MUST MOVE FROM OLD ITER TO NEW
                        // only way
                        //
                        // For STL containers, not sure how to find an equiv new iterator for an old one, but my best guess is to iterate through
                        // old for old, and when I match, stop on new
                        Require (pi->fData == movedFrom);
                        auto newI = this->begin ();
                        auto newE = this->end ();
                        auto oldI = movedFrom->begin ();
                        auto oldE = movedFrom->end ();
                        while (oldI != pi->fStdIterator) {
                            Assert (newI != newE);
                            Assert (oldI != oldE);
                            newI++;
                            oldI++;
                            Assert (newI != newE);
                            Assert (oldI != oldE);
                        }
                        Assert (oldI == pi->fStdIterator);
                        pi->fStdIterator = newI;
                        pi->fData = this;
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    template    <typename FUNCTION>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T>::Apply (FUNCTION doToElement) const
                    {
                        shared_lock<const AssertExternallySynchronizedLock> critSec { *this };
                        for (auto i = this->begin (); i != this->end (); ++i) {
                            (doToElement) (*i);
                        }
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    template    <typename FUNCTION>
                    inline  typename STL_CONTAINER_OF_T::const_iterator    STLContainerWrapper<STL_CONTAINER_OF_T>::FindFirstThat (FUNCTION doToElement) const
                    {
                        shared_lock<const AssertExternallySynchronizedLock> critSec { *this };
                        for (auto i = this->begin (); i != this->end (); ++i) {
                            if ((doToElement) (*i)) {
                                return i;
                            }
                        }
                        return this->end ();
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    template <typename PREDICATE>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::FindIf (PREDICATE pred) const
                    {
                        shared_lock<const AssertExternallySynchronizedLock> critSec { *this };
                        return std::find_if (this->begin (), this->end (), pred) != this->end ();
                    }


                    /*
                     ********************************************************************************
                     *********** STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator ***********
                     ********************************************************************************
                     */
                    template    <typename STL_CONTAINER_OF_T>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::ForwardIterator (STLContainerWrapper<STL_CONTAINER_OF_T>* data)
                        : fData (data)
                        , fStdIterator (data->begin ())
                    {
                        RequireNotNull (data);
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::ForwardIterator (const ForwardIterator& from)
                        : fData (from.fData)
                        , fStdIterator (from.fStdIterator)
                    {
                        RequireNotNull (fData);
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::Done () const
                    {
                        AssertNotNull (fData);
                        return fStdIterator == fData->end ();
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    template    <typename VALUE_TYPE>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::More (VALUE_TYPE* current, bool advance)
                    {
                        shared_lock<const AssertExternallySynchronizedLock> critSec { *fData };
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
                    template    <typename VALUE_TYPE>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::More (Memory::Optional<VALUE_TYPE>* result, bool advance)
                    {
                        shared_lock<const AssertExternallySynchronizedLock> critSec { *fData };
                        RequireNotNull (result);
                        if (advance) {
                            if (not Done ()) {
                                fStdIterator++;
                            }
                        }
                        if (Done ()) {
                            result->clear ();
                        }
                        else {
                            *result =  *fStdIterator;
                        }
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  size_t    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::CurrentIndex () const
                    {
                        shared_lock<const AssertExternallySynchronizedLock> critSec { *fData };
                        AssertNotNull (fData);
                        return fStdIterator - fData->begin ();
                    }
                    template    <typename STL_CONTAINER_OF_T>
                    inline  void    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::SetCurrentLink (typename CONTAINER_TYPE::const_iterator l)
                    {
                        lock_guard<const AssertExternallySynchronizedLock> critSec { *fData };
                        // MUUST COME FROM THIS stl container
                        // CAN be end ()
                        //
                        // bit of a queer kludge to covnert from const iterator to iterator in STL
                        fStdIterator = fData->erase (l, l);
                    }

                    template    <typename STL_CONTAINER_OF_T>
                    inline  bool    STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::Equals (const typename STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator& rhs) const
                    {
                        shared_lock<const AssertExternallySynchronizedLock> critSec { *fData };
                        return fStdIterator == rhs.fStdIterator;
                    }


                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Private_DataStructures_STLContainerWrapper_inl_ */
