/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_inl_
#define _Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_inl_ 1

#include <algorithm>

#include "../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::DataStructures {

// Would like to leave on by default but we just added and cannot afford to have debug builds get that slow
#ifndef qStroika_Foundation_Containers_DataStructures_STLContainerWrapper_IncludeSlowDebugChecks_
#define qStroika_Foundation_Containers_DataStructures_STLContainerWrapper_IncludeSlowDebugChecks_ 0
#endif

    /*
     ********************************************************************************
     ******************* STLContainerWrapper<STL_CONTAINER_OF_T> ********************
     ********************************************************************************
     */
    template <typename STL_CONTAINER_OF_T>
    template <typename... EXTRA_ARGS>
    inline STLContainerWrapper<STL_CONTAINER_OF_T>::STLContainerWrapper (EXTRA_ARGS&&... args)
        : inherited (forward<EXTRA_ARGS> (args)...)
    {
    }
    template <typename STL_CONTAINER_OF_T>
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::Contains (ArgByValueType<value_type> item) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return this->find (item) != this->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    void STLContainerWrapper<STL_CONTAINER_OF_T>::MoveIteratorHereAfterClone (ForwardIterator* pi, const STLContainerWrapper<STL_CONTAINER_OF_T>* movedFrom)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
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
        pi->fData        = this;
    }
    template <typename STL_CONTAINER_OF_T>
    template <typename FUNCTION>
    void STLContainerWrapper<STL_CONTAINER_OF_T>::Apply (FUNCTION doToElement) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        for (auto i = this->begin (); i != this->end (); ++i) {
            (doToElement) (*i);
        }
    }
    template <typename STL_CONTAINER_OF_T>
    template <typename FUNCTION>
    typename STL_CONTAINER_OF_T::const_iterator STLContainerWrapper<STL_CONTAINER_OF_T>::FindFirstThat (FUNCTION doToElement) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        for (auto i = this->begin (); i != this->end (); ++i) {
            if ((doToElement) (*i)) {
                return i;
            }
        }
        return this->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    template <typename FUNCTION>
    typename STL_CONTAINER_OF_T::iterator STLContainerWrapper<STL_CONTAINER_OF_T>::FindFirstThat (FUNCTION doToElement)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        for (auto i = this->begin (); i != this->end (); ++i) {
            if ((doToElement) (*i)) {
                return i;
            }
        }
        return this->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    template <typename PREDICATE>
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::FindIf (PREDICATE pred) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return find_if (this->begin (), this->end (), pred) != this->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::Invariant () const
    {
    }
    template <typename STL_CONTAINER_OF_T>
    inline typename STL_CONTAINER_OF_T::iterator STLContainerWrapper<STL_CONTAINER_OF_T>::remove_constness (STL_CONTAINER_OF_T& c, typename STL_CONTAINER_OF_T::const_iterator it)
    {
        return remove_constness_<STL_CONTAINER_OF_T> (c, it);
    }
    template <typename STL_CONTAINER_OF_T>
    template <typename CHECK_>
    inline typename STL_CONTAINER_OF_T::iterator STLContainerWrapper<STL_CONTAINER_OF_T>::remove_constness_ (STL_CONTAINER_OF_T& c, typename STL_CONTAINER_OF_T::const_iterator it, enable_if_t<Private_::has_erase<CHECK_>::value>*)
    {
        // http://stackoverflow.com/questions/765148/how-to-remove-constness-of-const-iterator
        return c.erase (it, it);
    }
    template <typename STL_CONTAINER_OF_T>
    template <typename CHECK_>
    inline typename STL_CONTAINER_OF_T::iterator STLContainerWrapper<STL_CONTAINER_OF_T>::remove_constness_ (STL_CONTAINER_OF_T& c, typename STL_CONTAINER_OF_T::const_iterator it, enable_if_t<!Private_::has_erase<CHECK_>::value>*)
    {
        // if erase trick doesn't work - this will - but at a horid cost
        typename STL_CONTAINER_OF_T::iterator i (c.begin ());
        return advance (i, distance (i, it));
    }

    /*
     ********************************************************************************
     *********** STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator ***********
     ********************************************************************************
     */
    template <typename STL_CONTAINER_OF_T>
    inline STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::ForwardIterator (const STLContainerWrapper<STL_CONTAINER_OF_T>* data)
        : fData (const_cast<STLContainerWrapper<STL_CONTAINER_OF_T>*> (data))
        , fStdIterator ((const_cast<STLContainerWrapper<STL_CONTAINER_OF_T>*> (data))->begin ())
    {
        RequireNotNull (data);
    }
    template <typename STL_CONTAINER_OF_T>
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::Done () const
    {
#if qStroika_Foundation_Containers_DataStructures_STLContainerWrapper_IncludeSlowDebugChecks_
        shared_lock<const AssertExternallySynchronizedLock> critSec{*fData};
#endif
        AssertNotNull (fData);
        return fStdIterator == fData->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    template <typename VALUE_TYPE>
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::More (VALUE_TYPE* current, bool advance)
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*fData};
        if (advance and fSuppressMore) {
            advance       = false;
            fSuppressMore = false;
        }
        bool done = Done ();
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
    template <typename STL_CONTAINER_OF_T>
    template <typename VALUE_TYPE>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::More (optional<VALUE_TYPE>* result, bool advance)
    {
        RequireNotNull (result);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*fData};
        if (advance and fSuppressMore) {
            advance       = false;
            fSuppressMore = false;
        }
        if (advance) {
            if (not Done ()) {
                fStdIterator++;
            }
        }
        if (Done ()) {
            *result = nullopt;
        }
        else {
            *result = *fStdIterator;
        }
    }
    template <typename STL_CONTAINER_OF_T>
    inline size_t STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::CurrentIndex () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*fData};
        AssertNotNull (fData);
        return fStdIterator - fData->begin ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::SetCurrentLink (typename CONTAINER_TYPE::iterator l)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*fData};
        // MUUST COME FROM THIS stl container
        // CAN be end ()
        //
        // bit of a queer kludge to covnert from const iterator to iterator in STL
        fStdIterator = l;
        //fStdIterator = fData->erase (l, l);
        //fStdIterator = STLContainerWrapper<STL_CONTAINER_OF_T>::remove_constness (*fData, l);  --not sure why didnt compile...
        fSuppressMore = false;
    }
    template <typename STL_CONTAINER_OF_T>
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::Equals (const typename STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator& rhs) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*fData};
        return fStdIterator == rhs.fStdIterator;
    }

}
#endif /* _Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_inl_ */
