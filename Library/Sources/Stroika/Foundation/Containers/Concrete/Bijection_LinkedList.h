/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Common/Compare.h"

#include "../Bijection.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE> is an LinkedList-based concrete implementation of the Bijection<DOMAIN_TYPE, RANGE_TYPE> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
                 *
                 */
                template <typename DOMAIN_TYPE, typename RANGE_TYPE>
                class Bijection_LinkedList : public Bijection<DOMAIN_TYPE, RANGE_TYPE> {
                private:
                    using inherited = Bijection<DOMAIN_TYPE, RANGE_TYPE>;

                public:
                    using InjectivityViolationPolicy      = typename inherited::InjectivityViolationPolicy;
                    using DomainEqualsCompareFunctionType = typename inherited::DomainEqualsCompareFunctionType;
                    using RangeEqualsCompareFunctionType  = typename inherited::RangeEqualsCompareFunctionType;

                public:
                    /**
                     *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
                     */
                    Bijection_LinkedList ();
                    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, typename ENABLE_IF_IS_COMPARER = enable_if_t<Common::IsPotentiallyComparerRelation<DOMAIN_TYPE, DOMAIN_EQUALS_COMPARER> () and Common::IsPotentiallyComparerRelation<RANGE_TYPE, RANGE_EQUALS_COMPARER> ()>>
                    explicit Bijection_LinkedList (const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer, ENABLE_IF_IS_COMPARER* = nullptr);
                    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, typename ENABLE_IF_IS_COMPARER = enable_if_t<Configuration::is_callable<DOMAIN_EQUALS_COMPARER>::value and Configuration::is_callable<RANGE_EQUALS_COMPARER>::value>>
                    explicit Bijection_LinkedList (InjectivityViolationPolicy injectivityCheckPolicy, const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer, ENABLE_IF_IS_COMPARER* = nullptr);
                    Bijection_LinkedList (const Bijection_LinkedList& src) = default;
                    template <typename CONTAINER_OF_ADDABLE>
                    Bijection_LinkedList (const CONTAINER_OF_ADDABLE& cp);
                    template <typename COPY_FROM_ITERATOR_KVP_T, typename ENABLE_IF = enable_if_t<Configuration::is_iterator<COPY_FROM_ITERATOR_KVP_T>::value>>
                    Bijection_LinkedList (COPY_FROM_ITERATOR_KVP_T start, COPY_FROM_ITERATOR_KVP_T end);

                public:
                    nonvirtual Bijection_LinkedList& operator= (const Bijection_LinkedList& rhs) = default;

                private:
                    class IImplRepBase_;
                    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
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

#include "Bijection_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_h_ */
