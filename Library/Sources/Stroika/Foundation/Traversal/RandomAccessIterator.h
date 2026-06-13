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
        // @todo add
        //      advance (ptrdiff_t)
        //      difference (iterator) -> ptrdiff_t
        //      global operator +/- calling above

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
     *
    */
    template <typename T, typename ITERATOR_TRAITS>
    class RandomAccessIterator<T, ITERATOR_TRAITS>::IRep : public BidirectionalIterator<T, ITERATOR_TRAITS>::IRep {
    protected:
        IRep () = default;

    public:
        /**
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
