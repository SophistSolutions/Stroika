/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../SortedCollection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Correctly implement override of Iterator<T>::IRep::Equals()
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief SortedCollection_LinkedList<T> is an LinkedList-based concrete implementation of the SortedCollection<T> container pattern.
                 *
                 *  \note   \em Performance
                 *      SortedCollection_LinkedList<T> is a compact, and reasonable implementation of Collections, so long as the Collection remains quite small
                 *      (empty or just a few entires). Thats really quite common!
                 *
                 *      However, add, contains tests, and removes are O(N) - so quite slow - when the Collection grows.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 */
                template <typename T>
                class SortedCollection_LinkedList : public SortedCollection<T> {
                private:
                    using inherited = SortedCollection<T>;

                public:
                    /**
                     *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
                     */
                    SortedCollection_LinkedList ();
                    template <typename INORDER_COMPARER>
                    explicit SortedCollection_LinkedList (const INORDER_COMPARER& inorderComparer);
                    SortedCollection_LinkedList (const T* start, const T* end);
                    SortedCollection_LinkedList (const SortedCollection<T>& src);
                    SortedCollection_LinkedList (const SortedCollection_LinkedList& src) noexcept = default;
                    SortedCollection_LinkedList (SortedCollection_LinkedList&& src) noexcept      = default;

                public:
                    /**
                     */
                    nonvirtual SortedCollection_LinkedList& operator= (const SortedCollection_LinkedList& rhs) = default;

                private:
                    class IImplRepBase_;
                    template <typename INORDER_COMPARER>
                    class Rep_;

                private:
                    nonvirtual void AssertRepValidType_ () const;
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedCollection_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_h_ */
