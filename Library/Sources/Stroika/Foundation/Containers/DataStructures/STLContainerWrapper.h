/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_h_
#define _Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_h_

#include "../../StroikaPreComp.h"

#include "../../Configuration/Common.h"
#include "../../Configuration/Concepts.h"
#include "../../Debug/AssertExternallySynchronizedMutex.h"

#include "../Common.h"

/**
 *  \file
 *
 *  Description:
 *      This module genericly wraps STL containers (such as map, vector etc), and facilitates
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
 *
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

    public:
        class ForwardIterator;

    public:
        /*
         *  Take iteartor 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
         *  iterator from 'this' - which points at the same logical position. This requires that this container
         *  was just 'copied' from 'movedFrom' - and is used to produce an eqivilennt iterator (since iterators are tied to
         *  the container they were iterating over).
         */
        nonvirtual void MoveIteratorHereAfterClone (ForwardIterator* pi, const STLContainerWrapper* movedFrom) const;

    public:
        nonvirtual bool Contains (ArgByValueType<value_type> item) const;

    public:
        /**
         *  \note Complexity:
         *      Always: O(N)
         */
        template <typename FUNCTION>
        nonvirtual void Apply (FUNCTION doToElement) const;

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Typical: O(N), but can be less if systematically finding entries near start of container
         */
        template <typename FUNCTION>
        nonvirtual iterator Find (FUNCTION doToElement);
        template <typename FUNCTION>
        nonvirtual const_iterator Find (FUNCTION doToElement) const;

    public:
        template <typename PREDICATE>
        nonvirtual bool FindIf (PREDICATE pred) const;

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
     *          @see https://stroika.atlassian.net/browse/STK-538
     */
    template <typename STL_CONTAINER_OF_T>
    class STLContainerWrapper<STL_CONTAINER_OF_T>::ForwardIterator {
    public:
        /**
         */
        explicit ForwardIterator (const STLContainerWrapper* data);
        explicit ForwardIterator (const STLContainerWrapper* data, UnderlyingIteratorRep startAt);
        explicit ForwardIterator (const ForwardIterator& from) = default;

    public:
        nonvirtual bool Done () const noexcept;

    public:
        nonvirtual ForwardIterator& operator++ () noexcept;

    public:
        nonvirtual value_type Current () const;

    public:
        /**
         * Only legal to call if underlying iterator is random_access
         */
        nonvirtual size_t CurrentIndex () const;

    public:
        nonvirtual UnderlyingIteratorRep GetUnderlyingIteratorRep () const;

    public:
        nonvirtual void SetUnderlyingIteratorRep (UnderlyingIteratorRep l);

    public:
        nonvirtual bool Equals (const ForwardIterator& rhs) const;

    public:
        nonvirtual const STLContainerWrapper* GetReferredToData () const;

    private:
        const STLContainerWrapper* fData_;
        const_iterator             fStdIterator_;

    private:
        friend class STLContainerWrapper;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "STLContainerWrapper.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_STLContainerWrapper_h_ */
