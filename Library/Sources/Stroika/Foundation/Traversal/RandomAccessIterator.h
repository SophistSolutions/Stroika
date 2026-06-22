/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_RandomAccessIterator_h_
#define _Stroika_Foundation_Traversal_RandomAccessIterator_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Traversal/BidirectionalIterator.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Traversal {

    /**
     */
    template <typename T, typename ITERATOR_TRAITS = DefaultIteratorTraits<random_access_iterator_tag, T>>
    class RandomAccessIterator : public BidirectionalIterator<T, ITERATOR_TRAITS> {
    private:
        using inherited = BidirectionalIterator<T, ITERATOR_TRAITS>;

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
         *  \note default construction of RandomAccessIterator means empty (both at start and end).
         * 
         *  \pre RequireNotNull (rep.get ()) for rep-taking CTOR
         * 
         *  \note constructor with argument default_sentinel_t - creates an end iterator
         */
        RandomAccessIterator (const unique_ptr<IRep>& rep) noexcept;
        RandomAccessIterator (unique_ptr<IRep>&& rep) noexcept;
        RandomAccessIterator (RandomAccessIterator&& src) noexcept = default;
        RandomAccessIterator (const RandomAccessIterator& src)     = default;
        constexpr RandomAccessIterator (const default_sentinel_t&) noexcept;
        constexpr RandomAccessIterator (nullptr_t) noexcept;
        constexpr RandomAccessIterator () noexcept;

    public:
        /**
         *  \brief  Iterators are safely copyable, preserving their current position.
         */
        nonvirtual RandomAccessIterator& operator= (RandomAccessIterator&& rhs) noexcept = default;
        nonvirtual RandomAccessIterator& operator= (const RandomAccessIterator& rhs)     = default;

    public:
        /**
         *  \brief
         *      Advance the iterator by the specified number of positions (which may be negative).
         *      \req i is a valid offset for the iterator. This means that if its positive, it never triggers
         *           an advance PAST the end, and if negative, it never triggers an advance before the start.
         */
        nonvirtual void Advance (ptrdiff_t i);

    public:
        /**
         *  \brief
         *      Calculate the difference between this iterator and another.
         * 
         *  \req both iterators of the same type, and one copied FROM the other at some
         *       point (so implies from the same container), or ONE or both of them can be the special
         *       end iterator (default_sentinel or nullptr). 
         */
        nonvirtual ptrdiff_t Difference (const RandomAccessIterator& rhs) const;

    public:
        /**
         *  \brief Produce a new iterator adjusted forward by the specified number of positions (note unlike base class i maybe negative).
         */
        nonvirtual RandomAccessIterator operator+ (ptrdiff_t i) const;

    public:
        /**
         *  \brief Produce a new iterator adjusted backward by the specified number of positions (note unlike base class i maybe negative).
         */
        nonvirtual RandomAccessIterator operator- (ptrdiff_t i) const;

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
     *  \brief
     *      The interface for the internal representation of a RandomAccessIterator.
     * 
     * \note In some sense this adds no new functionality to BidirectionIterator, but the presence
     *       of these virtual Advance and Distance methods adds to code size and is an indicator of
     *       additional functionality.
     */
    template <typename T, typename ITERATOR_TRAITS>
    class RandomAccessIterator<T, ITERATOR_TRAITS>::IRep : public BidirectionalIterator<T, ITERATOR_TRAITS>::IRep {
    protected:
        IRep () = default;

    public:
        /**
         *  \brief
         *      Advance the iterator by the specified number of positions (which CAN be negative).
         */
        virtual void Advance (ptrdiff_t i) = 0;

    public:
        /**
         *  \brief
         *      Calculate the difference between this iterator and another.
         *  \param rhs The other iterator to compare with.
         *  \return The difference between the two iterators.
         *  \note rhs maybe nullptr, and if so, implies the end of the container.
         */
        virtual ptrdiff_t Difference (const IRep* rhs) const = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

//#include    "RandomAccessIterator.inl"

#endif /*_Stroika_Foundation_Traversal_RandomAccessIterator_h_ */
