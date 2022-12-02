/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_inl_
#define _Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_inl_ 1

#include <algorithm>

#include "../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::DataStructures {

    namespace Private_ {
        template <typename T, typename A>
        constexpr bool IsStdForwardList_ (forward_list<T, A>*)
        {
            return true;
        }
        constexpr bool IsStdForwardList_ (...)
        {
            return false;
        }
        template <typename T>
        constexpr bool IsStdForwardList ()
        {
            return IsStdForwardList_ ((T*)nullptr);
        }
    }

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
        : inherited{forward<EXTRA_ARGS> (args)...}
    {
    }
    template <typename STL_CONTAINER_OF_T>
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::Contains (ArgByValueType<value_type> item) const
    {
        AssertExternallySynchronizedMutex::ReadLock readLock{*this};
        return this->find (item) != this->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    void STLContainerWrapper<STL_CONTAINER_OF_T>::MoveIteratorHereAfterClone (ForwardIterator* pi, const STLContainerWrapper* movedFrom) const
    {
        AssertExternallySynchronizedMutex::ReadLock readLock{*this};
        // TRICKY TODO - BUT MUST DO - MUST MOVE FROM OLD ITER TO NEW
        // only way
        //
        // For STL containers, not sure how to find an equiv new iterator for an old one, but my best guess is to iterate through
        // old for old, and when I match, stop on new
        Require (pi->GetReferredToData () == movedFrom);
        auto                  newI = this->begin ();
        [[maybe_unused]] auto newE = this->end ();
        auto                  oldI = movedFrom->begin ();
        [[maybe_unused]] auto oldE = movedFrom->end ();
        while (oldI != pi->fStdIterator_) {
            Assert (newI != newE);
            Assert (oldI != oldE);
            ++newI;
            ++oldI;
            Assert (newI != newE);
            Assert (oldI != oldE);
        }
        Assert (oldI == pi->fStdIterator_);
        pi->fStdIterator_ = newI;
        pi->fData_        = this;
    }
    template <typename STL_CONTAINER_OF_T>
    template <typename FUNCTION>
    void STLContainerWrapper<STL_CONTAINER_OF_T>::Apply (FUNCTION doToElement) const
    {
        AssertExternallySynchronizedMutex::ReadLock readLock{*this};
        for (auto i = this->begin (); i != this->end (); ++i) {
            (doToElement) (*i);
        }
    }
    template <typename STL_CONTAINER_OF_T>
    template <typename FUNCTION>
    auto STLContainerWrapper<STL_CONTAINER_OF_T>::Find (FUNCTION doToElement) const -> const_iterator
    {
        AssertExternallySynchronizedMutex::ReadLock readLock{*this};
        for (auto i = this->begin (); i != this->end (); ++i) {
            if ((doToElement)(*i)) {
                return i;
            }
        }
        return this->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    template <typename FUNCTION>
    auto STLContainerWrapper<STL_CONTAINER_OF_T>::Find (FUNCTION doToElement) -> iterator
    {
        Debug::AssertExternallySynchronizedMutex::WriteLock writeLock{*this};
        for (auto i = this->begin (); i != this->end (); ++i) {
            if (doToElement (*i)) {
                return i;
            }
        }
        return this->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    template <typename PREDICATE>
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::FindIf (PREDICATE pred) const
    {
        AssertExternallySynchronizedMutex::ReadLock readLock{*this};
        return find_if (this->begin (), this->end (), pred) != this->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::Invariant () const noexcept
    {
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::remove_constness (const_iterator it) -> iterator
    {
        // http://stackoverflow.com/questions/765148/how-to-remove-constness-of-const-iterator
        if constexpr (Private_::IsStdForwardList<STL_CONTAINER_OF_T> ()) {
            return this->erase_after (it, it);
        }
        else {
            return this->erase (it, it);
        }
    }

    /*
     ********************************************************************************
     *********** STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator ***********
     ********************************************************************************
     */
    template <typename STL_CONTAINER_OF_T>
    inline STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::ForwardIterator (const STLContainerWrapper* data, UnderlyingIteratorRep startAt)
        : fData_{data}
        , fStdIterator_{startAt}
    {
        RequireNotNull (data);
    }
    template <typename STL_CONTAINER_OF_T>
    inline STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::ForwardIterator (const STLContainerWrapper* data)
        : ForwardIterator{data, data->cbegin ()}
    {
        RequireNotNull (data);
    }
    template <typename STL_CONTAINER_OF_T>
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::Done () const noexcept
    {
#if qStroika_Foundation_Containers_DataStructures_STLContainerWrapper_IncludeSlowDebugChecks_
        AssertExternallySynchronizedMutex::ReadLock readLock{*fData_};
#endif
        AssertNotNull (fData_);
        return fStdIterator_ == fData_->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::operator++ () noexcept -> ForwardIterator&
    {
        Require (not Done ());
        ++fStdIterator_;
        return *this;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::Current () const -> value_type
    {
        AssertExternallySynchronizedMutex::ReadLock readLock{*fData_};
        AssertNotNull (fData_);
        Require (not Done ());
        return *fStdIterator_;
    }
    template <typename STL_CONTAINER_OF_T>
    inline size_t STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::CurrentIndex () const
    {
        AssertExternallySynchronizedMutex::ReadLock readLock{*fData_};
        AssertNotNull (fData_);
        return fStdIterator_ - fData_->begin ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::GetUnderlyingIteratorRep () const -> UnderlyingIteratorRep
    {
        AssertExternallySynchronizedMutex::ReadLock readLock{*fData_};
        return fStdIterator_;
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::SetUnderlyingIteratorRep (UnderlyingIteratorRep l)
    {
        AssertExternallySynchronizedMutex::ReadLock readLock{*fData_}; // read lock on data, though writing to this iterator
        fStdIterator_ = l;
    }
    template <typename STL_CONTAINER_OF_T>
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::Equals (const typename STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator& rhs) const
    {
        AssertExternallySynchronizedMutex::ReadLock readLock{*fData_};
        return fStdIterator_ == rhs.fStdIterator_;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::GetReferredToData () const -> const STLContainerWrapper*
    {
        return fData_;
    }

}
#endif /* _Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_inl_ */
