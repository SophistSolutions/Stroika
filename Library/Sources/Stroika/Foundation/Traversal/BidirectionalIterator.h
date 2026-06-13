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
     * BidirectionalIterator allows backing up (if not IsAtStart), and then moving forward again
     * (if not IsAtEnd - as a base class Iterator).
     */
    template <typename T, typename ITERATOR_TRAITS = DefaultIteratorTraits<bidirectional_iterator_tag, T>>
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
         * \pre not IsAtStart ()
         */
        nonvirtual BidirectionalIterator& operator-- ();
        nonvirtual BidirectionalIterator  operator-- (int);

    public:
        /*
         * 
         */
        nonvirtual BidirectionalIterator operator- (int i) const;

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
     */
    template <typename T, typename ITERATOR_TRAITS>
    class BidirectionalIterator<T, ITERATOR_TRAITS>::IRep : public Iterator<T, ITERATOR_TRAITS>::IRep {
    private:
        using inherited = typename Iterator<T, ITERATOR_TRAITS>::IRep;

    protected:
        IRep () = default;

    public:
        /**
         *  \brief
         *      Create a copy of the IRep.
         *
         *  \retval unique_ptr<IRep> - A new IRep instance that is a copy of this one.
         */
        virtual unique_ptr<inherited> Clone () const = 0;

        /**
         *  \brief
         *      Check if the iterator is at the beginning of the range.
         */
         virtual bool AtStart () const = 0;

        /**
         *  \brief
         *      Check if the iterator is at the end of the range.
         */
         virtual bool AtEnd () const = 0;

        /**
         *  \like More () - but going backwards. Use More (..., false) to get the current value without moving.
         */
        virtual void Back (optional<T>* result) = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "BidirectionalIterator.inl"

#endif /*_Stroika_Foundation_Traversal_BidirectionalIterator_h_ */
