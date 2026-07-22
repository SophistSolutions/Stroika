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
     *  \note Satisfies Concepts:
     *      o   regular<RandomAccessIterator<T>>        // implies bidirectional_iterator/totally_ordered, and several APIs available, including point at elements
     *      o   random_access_iterator<RandomAccessIterator<T>>
     *      o   sentinel_for<default_sentinel_t, RandomAccessIterator<T>>
     *      o   constructible_from<Iterator<T>, RandomAccessIterator<T>>);
     *      o   constructible_from<BidirectionalIterator<T>, RandomAccessIterator<T>>);
     */
    template <typename T, typename ITERATOR_TRAITS = Support::DefaultIteratorTraits<T>>
    class RandomAccessIterator : public BidirectionalIterator<T, ITERATOR_TRAITS> {
    private:
        using inherited = BidirectionalIterator<T, ITERATOR_TRAITS>;

        /*
     *   forward type declarations so can be used more easily in this definition
     */
    public:
        using difference_type = typename inherited::difference_type;
        using value_type      = typename inherited::value_type;
        using pointer         = typename inherited::pointer;
        using reference       = typename inherited::reference;

    public:
        /**
         *  \brief  iterator_category = random_access_iterator_tag;
         * 
         *  \note this is used by the concept random_access_iterator, and is used to distinguish from bidirectional_iterator.
         */
        using iterator_category = random_access_iterator_tag;

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
        nonvirtual void Advance (difference_type i);

    public:
        /**
         *  \brief
         *      Calculate the difference between this iterator and another.
         * 
         *  \req both iterators of the same type, and one copied FROM the other at some
         *       point (so implies from the same container), or ONE or both of them can be the special
         *       end iterator (default_sentinel or nullptr). 
         */
        nonvirtual difference_type Difference (const RandomAccessIterator& rhs) const;

    public:
        /**
         * \brief same as Iterator::operator++ () - advances iterator - but returns the subclass iterator type.
         * 
         * The subclass impl is functionaly identical, but hiding the base class implementation needed to satisfy the concepts for random access iterators.
         */
        nonvirtual RandomAccessIterator& operator++ ();
        nonvirtual RandomAccessIterator  operator++ (int);

    public:
        /**
         * The subclass impl is functionaly identical, but hiding the base class implementation needed to satisfy the concepts for random access iterators.
         */
        nonvirtual RandomAccessIterator& operator-- ();
        nonvirtual RandomAccessIterator  operator-- (int);

    public:
        /**
         *  \brief Produce a new iterator adjusted forward by the specified number of positions (note unlike base class i maybe negative).
         * 
         *         Note this hides the inherited operator+ from BiderectionIterator, just to be a bit faster.
         */
        nonvirtual RandomAccessIterator operator+ (difference_type i) const;

    public:
        /**
         *  \brief Produce a new iterator adjusted backward by the specified number of positions (note unlike base class i maybe negative).
         * 
         *  \note this hides the inherited operator- from BiderectionIterator, and is probably a bit faster.
         * 
         *  \pre current offset - i is a valid position in the referenced container.
         */
        nonvirtual RandomAccessIterator operator- (difference_type i) const;

    public:
        /**
         * @brief Advance () this iterator by the specified number of positions (which may be negative, but MUST be in range)
         * 
         * \pre i + current offset is a valid position in the referenced container.
         */
        nonvirtual RandomAccessIterator& operator+= (difference_type i) const;

    public:
        /**
         * @brief Advance (backward) this iterator by the specified number of positions (result MUST be in range)
         * 
         * \pre current offset - i is a valid position in the referenced container.
         */
        nonvirtual RandomAccessIterator& operator-= (difference_type i) const;

    public:
        /**
         * @brief Access the element at the specified index (relative to the current position).
         * 
         * \req this MUST specify a valid position, or its an assertion error.
         * 
         * API required by random_access_iterator concept
         * 
         * @param i 
         * @return const T& 
         */
        nonvirtual const T& operator[] (difference_type i) const;

    public:
        /**
         * @brief compare two iterators by their position in underlying container.
         */
        nonvirtual strong_ordering operator<=> (const RandomAccessIterator& rhs) const;

        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wnon-template-friend\""); //  very tricky to avoid this- tried
    public:
        /**
         * @brief  addition of iterator and int is commutative.
         */
        friend RandomAccessIterator operator+ (difference_type i, const RandomAccessIterator& it);

    public:
        /**
         * @brief  differece of iterator and int is anti-commutative.
         */
        friend RandomAccessIterator operator- (difference_type i, const RandomAccessIterator& it);

    public:
        /**
         * @brief  Difference of two iterators is difference_type (number of elements between them)
         */
        friend difference_type operator- (const RandomAccessIterator& lhs, const RandomAccessIterator& rhs);

        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wnon-template-friend\""); //  very tricky to avoid this- tried

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

    public:
        /**
         *  \brief
         *      Peek at the element at the specified position.
         *  \param i The position to peek at.
         *  \return A reference to the element at the specified position.
         *  \req i produces a valid position in the underlying container.
         * 
         *  \note this API is required to support the random access iterator concept (require ... { __j[__n] } -> same_as<iter_reference_t<_It>>;)...
         */
        virtual const T* PeekAtElement (ptrdiff_t i) const = 0;
    };

    // see Satisfies Concepts
    static_assert (random_access_iterator<RandomAccessIterator<int>>);
    static_assert (regular<RandomAccessIterator<int>>);
    static_assert (sentinel_for<default_sentinel_t, RandomAccessIterator<int>>);
    static_assert (constructible_from<Iterator<int>, RandomAccessIterator<int>>);
    static_assert (constructible_from<BidirectionalIterator<int>, RandomAccessIterator<int>>);

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "RandomAccessIterator.inl"

#endif /*_Stroika_Foundation_Traversal_RandomAccessIterator_h_ */
