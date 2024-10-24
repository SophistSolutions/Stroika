/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_h_
#define _Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Configuration/TypeHints.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"

/**
 *  \file
 *
 *  Description:
 *      This module generically wraps STL containers (such as map, vector etc), and facilitates
 *      using them as backends for Stroika containers.
 *
 *  TODO:
 *
 *      @todo   Replace Contains() with Lookup () - as we did for LinkedList<T>
 *
 *      @todo   Redo Contains1 versus Contains using partial template specialization of STLContainerWrapper - easy
 *              cuz such a trivial class. I can use THAT trick to handle the case of forward_list too. And size...
 *
 *      @todo   Add special subclass of ForwardIterator that tracks PREVPTR - and use to cleanup stuff
 *              that uses forward_list code...
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Containers::DataStructures {

    namespace Private_ {
        template <typename T>
        using has_erase_t = decltype (declval<T&> ().erase (begin (declval<T&> ()), end (declval<T&> ())));
        template <typename T>
        constexpr inline bool has_erase_v = Configuration::is_detected_v<has_erase_t, T>;
    }

    /**
     *  Use this to wrap an underlying stl container (like std::vector or stl:list, etc) to adapt
     *  it for Stroika containers.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename STL_CONTAINER_OF_T>
    class STLContainerWrapper : public STL_CONTAINER_OF_T, public Debug::AssertExternallySynchronizedMutex {
    private:
        using inherited = STL_CONTAINER_OF_T;

    public:
        using value_type     = typename STL_CONTAINER_OF_T::value_type;
        using iterator       = typename STL_CONTAINER_OF_T::iterator;
        using const_iterator = typename STL_CONTAINER_OF_T::const_iterator;

    public:
        /**
         *  Basic (mostly internal) element used by ForwardIterator. Abstract name so can be referenced generically across 'DataStructure' objects
         */
        using UnderlyingIteratorRep = const_iterator;

    public:
        /**
         *  pass through CTOR args to underlying container
         */
        template <typename... EXTRA_ARGS>
        STLContainerWrapper (EXTRA_ARGS&&... args);
        constexpr STLContainerWrapper (const STLContainerWrapper&)     = default;
        constexpr STLContainerWrapper (STLContainerWrapper&&) noexcept = default;

    public:
        class ForwardIterator;

    public:
        /*
         *  Support for COW (CopyOnWrite):
         *
         *  Take iterator 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
         *  iterator from 'this' - which points at the same logical position. This requires that this container
         *  was just 'copied' from 'movedFrom' - and is used to produce an equivalent iterator (since iterators are tied to
         *  the container they were iterating over).
         */
        nonvirtual void MoveIteratorHereAfterClone (ForwardIterator* pi, const STLContainerWrapper* movedFrom) const;

    public:
        nonvirtual bool contains (Configuration::ArgByValueType<value_type> item) const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: O(N)
         */
        template <invocable<typename STL_CONTAINER_OF_T::value_type> FUNCTION>
        nonvirtual void Apply (FUNCTION&& doToElement) const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Typical: O(N), but can be less if systematically finding entries near start of container
         */
        template <predicate<typename STL_CONTAINER_OF_T::value_type> FUNCTION>
        nonvirtual iterator Find (FUNCTION&& firstThat);
        template <predicate<typename STL_CONTAINER_OF_T::value_type> FUNCTION>
        nonvirtual const_iterator Find (FUNCTION&& firstThat) const;

    public:
        template <predicate<typename STL_CONTAINER_OF_T::value_type> PREDICATE>
        nonvirtual bool FindIf (PREDICATE&& pred) const;

    public:
        nonvirtual void Invariant () const noexcept;

    public:
        nonvirtual iterator remove_constness (const_iterator it);
    };

    /**
     *      STLContainerWrapper::ForwardIterator is a private utility class designed
     *  to promote source code sharing among the patched iterator implementations.
     *
     *  \note   ForwardIterator takes a const-pointer the the container as argument since this
     *          iterator never MODIFIES the container.
     *
     *          However, it does a const-cast to maintain a non-const pointer since that is needed to
     *          option a non-const iterator pointer, which is needed by some classes that use this, and
     *          there is no zero (or even low for forward_list) cost way to map from const to non const
     *          iterators (needed to perform the update).
     *
     *          @see http://stroika-bugs.sophists.com/browse/STK-538
     */
    template <typename STL_CONTAINER_OF_T>
    class STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator {
    public:
        // stuff STL requires you to set to look like an iterator
        using iterator_category = forward_iterator_tag;
        using value_type        = STLContainerWrapper::value_type;
        using difference_type   = ptrdiff_t;
        using pointer           = const value_type*;
        using reference         = const value_type&;

    public:
        /**
         *  /0 overload: sets iterator to 'end' - sentinel
         *  /1 (data) overload: sets iterator to begin
         *  /2 (data,startAt) overload: sets iterator to startAt
         */
        constexpr ForwardIterator () noexcept = default;
        explicit constexpr ForwardIterator (const STLContainerWrapper* data) noexcept;
        explicit constexpr ForwardIterator (const STLContainerWrapper* data, UnderlyingIteratorRep startAt) noexcept;
        constexpr ForwardIterator (const ForwardIterator&) noexcept = default;
        constexpr ForwardIterator (ForwardIterator&&) noexcept      = default;

    public:
        nonvirtual ForwardIterator& operator= (const ForwardIterator&)     = default;
        nonvirtual ForwardIterator& operator= (ForwardIterator&&) noexcept = default;

    public:
        /**
         *  return true if iterator not Done
         */
        explicit operator bool () const;

    public:
        nonvirtual bool Done () const noexcept;

    public:
        nonvirtual ForwardIterator& operator++ () noexcept;

    public:
        nonvirtual const value_type& operator* () const;

    public:
        nonvirtual const value_type* operator->() const;

    public:
        /**
         */
        nonvirtual size_t CurrentIndex () const;

    public:
        nonvirtual UnderlyingIteratorRep GetUnderlyingIteratorRep () const;

    public:
        nonvirtual void SetUnderlyingIteratorRep (UnderlyingIteratorRep l);

    public:
        nonvirtual bool operator== (const ForwardIterator& rhs) const;

    public:
        /**
         *  For debugging, assert the iterator data matches argument data
         */
        constexpr void AssertDataMatches (const STLContainerWrapper* data) const;

    private:
        const STLContainerWrapper* fData_{nullptr};
        const_iterator             fStdIterator_{};

    private:
        friend class STLContainerWrapper;
    };

    static_assert (ranges::input_range<STLContainerWrapper<vector<int>>>); // smoke test - make sure basic iteration etc should work (allows formattable to work)

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "STLContainerWrapper.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_h_ */
