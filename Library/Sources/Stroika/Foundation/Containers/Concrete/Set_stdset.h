/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Set.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Set_stdset_h_
#define _Stroika_Foundation_Containers_Concrete_Set_stdset_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   Set_stdset<KEY_TYPE, VALUE_TYPE, TRAITS> is an std::set-based concrete implementation of the Set<T> container pattern.
                 *
                 *  \note   \em Implemenation   Just indirect to SortedSet_stdset<>, which in turn mostly indirects to std::set<T>
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename T>
                class Set_stdset : public Set<T> {
                private:
                    using inherited = Set<T>;

                public:
                    /**
                     */
                    using EqualityComparerType = typename inherited::EqualityComparerType;

                public:
                    /**
                     *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
                     */
                    Set_stdset ();
                    template <typename INORDER_COMPARER>
                    explicit Set_stdset (const INORDER_COMPARER& inorderComparer);
                    Set_stdset (const Set_stdset& src) = default;
                    Set_stdset (const std::initializer_list<T>& src);
                    Set_stdset (const EqualityComparerType& equalsComparer, const std::initializer_list<T>& src);
                    template <typename CONTAINER_OF_T, typename ENABLE_IF = enable_if_t<Configuration::has_beginend<CONTAINER_OF_T>::value and !is_convertible_v<const CONTAINER_OF_T*, const Set_stdset<T>*>>>
                    Set_stdset (const CONTAINER_OF_T& src);
                    template <typename CONTAINER_OF_T, typename ENABLE_IF = enable_if_t<Configuration::has_beginend<CONTAINER_OF_T>::value and !is_convertible_v<const CONTAINER_OF_T*, const Set_stdset<T>*>>>
                    Set_stdset (const EqualityComparerType& equalsComparer, const CONTAINER_OF_T& src);
                    template <typename COPY_FROM_ITERATOR_T>
                    Set_stdset (COPY_FROM_ITERATOR_T start, COPY_FROM_ITERATOR_T end);
                    template <typename COPY_FROM_ITERATOR_T>
                    Set_stdset (const EqualityComparerType& equalsComparer, COPY_FROM_ITERATOR_T start, COPY_FROM_ITERATOR_T end);

                public:
                    /**
                     */
                    nonvirtual Set_stdset& operator= (const Set_stdset& rhs) = default;

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
#include "Set_stdset.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Set_stdset_h_ */
