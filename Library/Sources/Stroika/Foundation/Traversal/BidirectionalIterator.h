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
 *              ****VERY ROUGH UNUSABLE DRAFT
 *
 *  \note Code-Status:  <a href="Code-Status.md#Draft">Draft</a>
 *
 */

namespace Stroika::Foundation::Traversal {

    /**
     */
    template <typename T, typename ITERATOR_TRAITS = DefaultIteratorTraits<bidirectional_iterator_tag, T>>
    class BidirectionalIterator : public Iterator<T, ITERATOR_TRAITS> {
    private:
        using inherited = Iterator<T, ITERATOR_TRAITS>;

    public:
        class IRep;

    public:
        /**
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
        nonvirtual IRep&       GetRep ();
        nonvirtual const IRep& GetRep () const;
    };

    /**
     */
    template <typename T, typename ITERATOR_TRAITS>
    class BidirectionalIterator<T, ITERATOR_TRAITS>::IRep : public Iterator<T, ITERATOR_TRAITS>::IRep {
    protected:
        IRep () = default;

    public:
        virtual unique_ptr<IRep> Clone () const = 0;
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
