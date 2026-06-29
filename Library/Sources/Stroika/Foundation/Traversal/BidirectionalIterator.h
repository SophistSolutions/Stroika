/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_BidirectionalIterator_h_
#define _Stroika_Foundation_Traversal_BidirectionalIterator_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Traversal/Iterator.h"

/**
 *
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 *
 */

namespace Stroika::Foundation::Traversal {

    /**
     *  \brief A BidirectionalIterator is an Iterator that can be moved both forward and backward.
     * 
     * BidirectionalIterator allows backing up (if not AtStart), and then moving forward again
     * (if not AtEnd - as a base class Iterator).
     * 
     *  \note Satisfies Concepts:
     *      o   regular<BidirectionalIterator<T>>        // implies copyable/movable/equality_comparable
     *      o   bidirectional_iterator<BidirectionalIterator<T>>
     *      o   sentinel_for<default_sentinel_t, BidirectionalIterator<T>>
     */
    template <typename T, typename ITERATOR_TRAITS = Support::DefaultIteratorTraits<bidirectional_iterator_tag, T>>
    class BidirectionalIterator : public Iterator<T, ITERATOR_TRAITS> {
    private:
        using inherited = Iterator<T, ITERATOR_TRAITS>;

    public:
        class IRep;

    public:
        /**
         *  \brief This overload is usually not called directly. Instead, iterators are
         *         usually created from a container (eg. Sequence<T>{}.begin()).
         *
         *  Iterators are safely copyable, preserving their current position.
         *
         *  CTOR overload taking nullptr - is the same as GetEmptyIterator ()
         *
         *  \note default construction of BidirectionalIterator means empty (both at start and end).
         * 
         *  \pre RequireNotNull (rep.get ()) for rep-taking CTOR
         * 
         *  \note constructor with argument default_sentinel_t - creates an end iterator
         */
        BidirectionalIterator (const unique_ptr<IRep>& rep) noexcept;
        BidirectionalIterator (unique_ptr<IRep>&& rep) noexcept;
        BidirectionalIterator (BidirectionalIterator&& src) noexcept = default;
        BidirectionalIterator (const BidirectionalIterator& src)     = default;
        constexpr BidirectionalIterator (const default_sentinel_t&) noexcept;
        constexpr BidirectionalIterator (nullptr_t) noexcept;
        constexpr BidirectionalIterator () noexcept;

    public:
        /**
         *  \brief  Iterators are safely copyable, preserving their current position.
         */
        nonvirtual BidirectionalIterator& operator= (BidirectionalIterator&& rhs) noexcept = default;
        nonvirtual BidirectionalIterator& operator= (const BidirectionalIterator& rhs)     = default;

    public:
        /**
         * 
         */
        nonvirtual bool AtStart () const;

    public:
        /**
         * \brief same as Iterator::operator++ () - advances iterator - but returns the subclass iterator type.
         * 
         * The subclass impl is functionaly identical, but hiding the base class implementation needed to satisfy the concepts for bidirectional iterators.
         */
        nonvirtual BidirectionalIterator& operator++ ();
        nonvirtual BidirectionalIterator  operator++ (int);

    public:
        /**
         * \pre not AtStart ()
         */
        nonvirtual BidirectionalIterator& operator-- ();
        nonvirtual BidirectionalIterator  operator-- (int);

    public:
        /**
         *  \brief Move the iterator back by the specified number of positions.
         *
         *  \pre i >= 0
         */
        nonvirtual BidirectionalIterator operator- (ptrdiff_t i) const;

    public:
        /**
         *  \brief
         *      Get a reference to the IRep owned by the iterator. This is an implementation detail,
         *      mainly intended for implementors.
         *
         *  Get a reference to the IRep owned by the iterator.
         *  This is an implementation detail, mainly intended for implementors.
         */
        nonvirtual IRep& GetRep ();

    public:
        /**
         *  \brief
         *      Get a reference to the IRep owned by the iterator. This is an implementation detail,
         *      mainly intended for implementors.
         *
         *  Get a reference to the IRep owned by the iterator.
         *  This is an implementation detail, mainly intended for implementors.
         */
        nonvirtual const IRep& ConstGetRep () const;
    };

    /**
     * \brief An Iterator<T> that also supports going backwards.
     */
    template <typename T, typename ITERATOR_TRAITS>
    class BidirectionalIterator<T, ITERATOR_TRAITS>::IRep : public Iterator<T, ITERATOR_TRAITS>::IRep {
    private:
        using inherited = typename Iterator<T, ITERATOR_TRAITS>::IRep;

    protected:
        IRep () = default;

    public:
        /**
         *  \brief return true iff the iterator is at the start of its range. You can only backup further if NOT AtStart ()
         * 
         *  \note - if the range is empty, its BOTH AtEnd () and AtStart ()
         */
        virtual bool AtStart () const = 0;

    public:
        /**
         *  \brief Move iterator one position back, closer to start. Return the current value of T (which must always be valid).
         * 
         *  \pre not AtStart ()
         */
        virtual T Back () = 0;
    };

    // see Satisfies Concepts
    //      @todo would be nice to include these tests generically as part of template declaration, but cannot figure out how
    //      to get that working (probably due to when incomplete types evaluated) --LGP 2024-08-21
    static_assert (bidirectional_iterator<BidirectionalIterator<int>>);
    static_assert (regular<BidirectionalIterator<int>>);
    static_assert (sentinel_for<default_sentinel_t, BidirectionalIterator<int>>);

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "BidirectionalIterator.inl"

#endif /*_Stroika_Foundation_Traversal_BidirectionalIterator_h_ */
