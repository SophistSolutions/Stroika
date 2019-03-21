/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_BidirectionalIterator_h_
#define _Stroika_Foundation_Traversal_BidirectionalIterator_h_ 1

#include "../StroikaPreComp.h"

#include <iterator>

#include "../Configuration/Common.h"

#include "Iterator.h"

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
    template <typename T, typename BASE_STD_ITERATOR = iterator<bidirectional_iterator_tag, T>>
    class BidirectionalIterator : public Iterator<T, BASE_STD_ITERATOR> {
    private:
        using inherited = Iterator<T, BASE_STD_ITERATOR>;

    public:
        class IRep;

    public:
        using BidirectionalIteratorRepSharedPtr = typename inherited::template PtrImplementationTemplate<IRep>;

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
        explicit BidirectionalIterator (const BidirectionalIteratorRepSharedPtr& rep);
        BidirectionalIterator (const BidirectionalIterator& from);
        BidirectionalIterator () = delete;

    private:
        /**
         *  Mostly internal type to select a constructor for the special END iterator.
         */
        enum ConstructionFlagForceAtEnd_ {
            ForceAtEnd
        };

    private:
        BidirectionalIterator (ConstructionFlagForceAtEnd_);

    public:
        /**
         *  \brief  Iterators are safely copyable, preserving their current position.
         */
        nonvirtual BidirectionalIterator& operator= (const BidirectionalIterator& rhs) = default;

    public:
        // @todo add operator--

    public:
        /**
         *  \brief
         *      Used by *somecontainer*::end ()
         *
         *  GetEmptyIterator () returns a special iterator which is always empty - always 'at the end'.
         *  This is handy in implementing STL-style 'if (a != b)' style iterator comparisons.
         */
        static BidirectionalIterator GetEmptyIterator ();
    };

    /**
     */
    template <typename T, typename BASE_STD_ITERATOR>
    class BidirectionalIterator<T, BASE_STD_ITERATOR>::IRep : public Iterator<T, BASE_STD_ITERATOR>::IRep {
    protected:
        IRep () = default;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "BidirectionalIterator.inl"

#endif /*_Stroika_Foundation_Traversal_BidirectionalIterator_h_ */
