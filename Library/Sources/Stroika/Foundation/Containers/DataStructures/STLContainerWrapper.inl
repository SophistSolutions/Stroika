/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <algorithm>

#include "Stroika/Foundation/Debug/Assertions.h"

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
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::contains (Common::ArgByValueType<value_type> item) const
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
        return this->find (item) != this->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    template <invocable<typename STL_CONTAINER_OF_T::const_iterator, typename STL_CONTAINER_OF_T::const_iterator> POINT_TO_SAME_THING>
    void STLContainerWrapper<STL_CONTAINER_OF_T>::MoveIteratorHereAfterClone (ForwardIterator* pi, const STLContainerWrapper* movedFrom,
                                                                              [[maybe_unused]] POINT_TO_SAME_THING&& pointToSameThingTester) const
        requires (convertible_to<invoke_result_t<POINT_TO_SAME_THING, typename STL_CONTAINER_OF_T::const_iterator, typename STL_CONTAINER_OF_T::const_iterator>, bool>)
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
        // TRICKY TODO - BUT MUST DO - MUST MOVE FROM OLD ITER TO NEW
        // only way
        //
        // For STL containers, not sure how to find an equiv new iterator for an old one, but my best guess is to iterate through
        // old for old, and when I match, stop on new
        pi->AssertDataMatches (movedFrom);
        auto                  newI = this->begin ();
        [[maybe_unused]] auto newE = this->end ();
        auto                  oldI = movedFrom->begin ();
        [[maybe_unused]] auto oldE = movedFrom->end ();
        while (not forward<POINT_TO_SAME_THING> (pointToSameThingTester) (oldI, newI)) {
            Assert (newI != newE);
            Assert (oldI != oldE);
            ++newI;
            ++oldI;
            Assert (newI != newE);
            Assert (oldI != oldE);
        }
        Assert (oldI == pi->_fStdIterator);
        pi->_fStdIterator = newI;
        pi->_fData        = this;
    }
    template <typename STL_CONTAINER_OF_T>
    template <invocable<typename STL_CONTAINER_OF_T::value_type> FUNCTION>
    void STLContainerWrapper<STL_CONTAINER_OF_T>::Apply (FUNCTION&& doToElement) const
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
        for (auto i = this->begin (); i != this->end (); ++i) {
            (doToElement) (*i);
        }
    }
    template <typename STL_CONTAINER_OF_T>
    template <predicate<typename STL_CONTAINER_OF_T ::value_type> FUNCTION>
    auto STLContainerWrapper<STL_CONTAINER_OF_T>::Find (FUNCTION&& firstThat) const -> const_iterator
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
        for (auto i = this->begin (); i != this->end (); ++i) {
            if (firstThat (*i)) {
                return i;
            }
        }
        return this->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    template <predicate<typename STL_CONTAINER_OF_T ::value_type> FUNCTION>
    auto STLContainerWrapper<STL_CONTAINER_OF_T>::Find (FUNCTION&& firstThat) -> iterator
    {
        Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
        for (auto i = this->begin (); i != this->end (); ++i) {
            if (firstThat (*i)) {
                return i;
            }
        }
        return this->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    template <predicate<typename STL_CONTAINER_OF_T::value_type> PREDICATE>
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::FindIf (PREDICATE&& pred) const
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
        return find_if (this->begin (), this->end (), forward<PREDICATE> (pred)) != this->end ();
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
    constexpr STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::ForwardIterator (const STLContainerWrapper* data, UnderlyingIteratorRep startAt) noexcept
        : _fData{data}
        , _fStdIterator{startAt}
    {
        RequireNotNull (data);
    }
    template <typename STL_CONTAINER_OF_T>
    constexpr STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::ForwardIterator (const STLContainerWrapper* data) noexcept
        : ForwardIterator{data, (RequireExpression (data != nullptr), data->cbegin ())}
    {
    }
    template <typename STL_CONTAINER_OF_T>
    inline STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::operator bool () const
    {
        return not AtEnd ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::AtEnd () const noexcept
    {
#if qStroika_Foundation_Containers_DataStructures_STLContainerWrapper_IncludeSlowDebugChecks_
        AssertExternallySynchronizedChecker::ReadContext declareContext{*_fData};
#endif
        AssertNotNull (_fData);
        return _fStdIterator == _fData->end ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::operator++ () noexcept -> ForwardIterator&
    {
        Require (not AtEnd ());
        ++_fStdIterator;
        return *this;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::operator++ (int) noexcept -> ForwardIterator
    {
        ForwardIterator result{*this};
        this->operator++ ();
        return result;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::operator* () const -> const value_type&
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*_fData};
        AssertNotNull (_fData);
        Require (not AtEnd ());
        return *_fStdIterator;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::operator->() const -> const value_type*
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*_fData};
        AssertNotNull (_fData);
        Require (not AtEnd ());
        return &*_fStdIterator;
    }
    template <typename STL_CONTAINER_OF_T>
    inline size_t STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::CurrentIndex () const
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*_fData};
        AssertNotNull (_fData);
        return static_cast<size_t> (std::distance (_fData->begin (), _fStdIterator));
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::GetUnderlyingIteratorRep () const -> UnderlyingIteratorRep
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*_fData};
        return _fStdIterator;
    }
    template <typename STL_CONTAINER_OF_T>
    inline void STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::SetUnderlyingIteratorRep (UnderlyingIteratorRep l)
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*_fData}; // read lock on data, though writing to this iterator
        _fStdIterator = l;
    }
    template <typename STL_CONTAINER_OF_T>
    constexpr void STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::AssertDataMatches ([[maybe_unused]] const STLContainerWrapper* data) const
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        Require (data == _fData);
#endif
    }
    template <typename STL_CONTAINER_OF_T>
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator::operator== (const typename STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator& rhs) const
    {
        AssertExternallySynchronizedChecker::ReadContext declareContext{*_fData};
        return _fStdIterator == rhs._fStdIterator;
    }

    /*
     ********************************************************************************
     ******** STLContainerWrapper<STL_CONTAINER_OF_T>::BidirectionalIterator ********
     ********************************************************************************
     */
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::BidirectionalIterator::operator++ () noexcept -> BidirectionalIterator&
    {
        inherited::operator++ ();
        return *this;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::BidirectionalIterator::operator++ (int) noexcept -> BidirectionalIterator
    {
        BidirectionalIterator result{*this};
        this->operator++ ();
        return result;
    }
    template <typename STL_CONTAINER_OF_T>
    inline bool STLContainerWrapper<STL_CONTAINER_OF_T>::BidirectionalIterator::AtStart () const noexcept
    {
        AssertNotNull (this->_fData);
        return this->_fStdIterator == this->_fData->begin ();
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::BidirectionalIterator::operator-- () noexcept -> BidirectionalIterator&
    {
        Require (not this->AtStart ());
        --this->_fStdIterator;
        return *this;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::BidirectionalIterator::operator-- (int) noexcept -> BidirectionalIterator
    {
        BidirectionalIterator result{*this};
        this->operator-- ();
        return result;
    }

    /*
     ********************************************************************************
     ********* STLContainerWrapper<STL_CONTAINER_OF_T>::RandomAccessIterator ********
     ********************************************************************************
     */
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::RandomAccessIterator::operator++ () noexcept -> RandomAccessIterator&
    {
        inherited::operator++ ();
        return *this;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::RandomAccessIterator::operator++ (int) noexcept -> RandomAccessIterator
    {
        RandomAccessIterator result{*this};
        this->operator++ ();
        return result;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::RandomAccessIterator::operator-- () noexcept -> RandomAccessIterator&
    {
        inherited::operator-- ();
        return *this;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::RandomAccessIterator::operator-- (int) noexcept -> RandomAccessIterator
    {
        RandomAccessIterator result{*this};
        this->operator-- ();
        return result;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::RandomAccessIterator::operator+ (difference_type i) const -> RandomAccessIterator
    {
        RandomAccessIterator result{*this};
        result += i;
        return result;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::RandomAccessIterator::operator- (difference_type i) const -> RandomAccessIterator
    {
        RandomAccessIterator result{*this};
        result -= i;
        return result;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::RandomAccessIterator::operator+= (difference_type i) -> RandomAccessIterator&
    {
        AssertNotNull (this->_fData);
        [[maybe_unused]] difference_type newIdx = static_cast<difference_type> (this->CurrentIndex ()) + i;
        Require (newIdx >= 0 and static_cast<size_t> (newIdx) <= this->_fData->size ());
        this->_fStdIterator += i;
        return *this;
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::RandomAccessIterator::operator-= (difference_type i) -> RandomAccessIterator&
    {
        return this->operator+= (-i);
    }
    template <typename STL_CONTAINER_OF_T>
    inline auto STLContainerWrapper<STL_CONTAINER_OF_T>::RandomAccessIterator::operator[] (difference_type i) const -> const value_type&
    {
        AssertNotNull (this->_fData);
        return *(this->_fStdIterator + i);
    }
    template <typename STL_CONTAINER_OF_T>
    inline strong_ordering STLContainerWrapper<STL_CONTAINER_OF_T>::RandomAccessIterator::operator<=> (const RandomAccessIterator& rhs) const
    {
        return (*this - rhs) <=> 0;
    }

}
