/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../SortedSet.h"

#ifndef _Stroika_Foundation_Containers_Concrete_SortedSet_stdset_h_
#define _Stroika_Foundation_Containers_Concrete_SortedSet_stdset_h_

/**
 *  \file
 *
 *  TODO:
 *
 *      @todo   Finish using CONTAINER_LOCK_HELPER_START() - synchronization support
 *              THEN - MAYBE - try todo better, but at least do this as starter
 *      @todo   Could optimize Equals() test for if both sorted, faster way to compare.
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   SortedSet_stdset<T> is an std::set-based concrete implementation of the SortedSet<T> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename T>
                class SortedSet_stdset : public SortedSet<T> {
                private:
                    using inherited = SortedSet<T>;

                public:
                    /**
                     */
                    using InOrderComparerType = typename inherited::InOrderComparerType;

                public:
                    /**
                     *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
                     *  \req IsStrictInOrderComparer<INORDER_COMPARER> ()
                     */
                    SortedSet_stdset ();
                    template <typename INORDER_COMPARER>
                    explicit SortedSet_stdset (const INORDER_COMPARER& inorderComparer);
                    SortedSet_stdset (const SortedSet_stdset& src) = default;
                    SortedSet_stdset (const std::initializer_list<T>& src);
                    SortedSet_stdset (const InOrderComparerType& inOrderComparer, const std::initializer_list<T>& src);
                    template <typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_T>::value && !std::is_convertible<const CONTAINER_OF_T*, const SortedSet_stdset<T>*>::value>::type>
                    SortedSet_stdset (const CONTAINER_OF_T& src);
                    template <typename CONTAINER_OF_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_T>::value && !std::is_convertible<const CONTAINER_OF_T*, const SortedSet_stdset<T>*>::value>::type>
                    SortedSet_stdset (const InOrderComparerType& inOrderComparer, const CONTAINER_OF_T& src);
                    template <typename COPY_FROM_ITERATOR_OF_T>
                    SortedSet_stdset (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
                    template <typename COPY_FROM_ITERATOR_OF_T>
                    SortedSet_stdset (const InOrderComparerType& inOrderComparer, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

                public:
                    /**
                     */
                    nonvirtual SortedSet_stdset& operator= (const SortedSet_stdset& rhs) = default;

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
#include "SortedSet_stdset.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_SortedSet_stdset_h_ */
