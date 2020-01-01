/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_RandomAccessIterator_h_
#define _Stroika_Foundation_Traversal_RandomAccessIterator_h_ 1

#include "../StroikaPreComp.h"

#include <iterator>

#include "../Configuration/Common.h"

#include "BidirectionalIterator.h"

/**
 *
 *  \file
 *              ****VERY ROUGH UNUSABLE DRAFT
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 */

namespace Stroika::Foundation::Traversal {

    /**
     */
    template <typename T, typename BASE_STD_ITERATOR = iterator<random_access_iterator_tag, T>>
    class RandomAccessIterator : public BidirectionalIterator<T, BASE_STD_ITERATOR> {
    private:
        using inherited = Iterator<T, BASE_STD_ITERATOR>;

    public:
        class IRep;

    public:
        using RandomAccessIteratorRepSharedPtr = typename inherited::template PtrImplementationTemplate<IRep>;

    public:
        /**
         *  \brief
         *      This overload is usually not called directly. Instead, iterators are
         *      usually created from a container (eg. Bag<T>::begin()).
         *
         *  Iterators are safely copyable, preserving their current position.
         *
         *  \req RequireNotNull (rep.get ())
         */
        explicit RandomAccessIterator (const RandomAccessIteratorRepSharedPtr& rep);
        RandomAccessIterator (const RandomAccessIterator& from);
        RandomAccessIterator () = delete;

    private:
        /**
         *  Mostly internal type to select a constructor for the special END iterator.
         */
        enum ConstructionFlagForceAtEnd_ {
            ForceAtEnd
        };

    private:
        RandomAccessIterator (ConstructionFlagForceAtEnd_);

    public:
        /**
         *  \brief  Iterators are safely copyable, preserving their current position.
         */
        nonvirtual RandomAccessIterator& operator= (const RandomAccessIterator& rhs);

    public:
        /**
         *  \brief
         *      Used by *somecontainer*::end ()
         *
         *  GetEmptyIterator () returns a special iterator which is always empty - always 'at the end'.
         *  This is handy in implementing STL-style 'if (a != b)' style iterator comparisons.
         */
        static RandomAccessIterator GetEmptyIterator ();

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
        nonvirtual const IRep& GetRep () const;
    };

    /**
     *
    */
    template <typename T, typename BASE_STD_ITERATOR>
    class RandomAccessIterator<T, BASE_STD_ITERATOR>::IRep : public BidirectionalIterator<T, BASE_STD_ITERATOR>::IRep {
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
