/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Compare.h"

#include "Stroika/Foundation/Containers/Bijection.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE> is an LinkedList-based concrete implementation of the Bijection<DOMAIN_TYPE, RANGE_TYPE> container pattern.
     *
     * \note Performance Notes:
     *      o   size () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    class Bijection_LinkedList : public Bijection<DOMAIN_TYPE, RANGE_TYPE> {
    private:
        using inherited = Bijection<DOMAIN_TYPE, RANGE_TYPE>;

    public:
        using DomainEqualsCompareFunctionType = typename inherited::DomainEqualsCompareFunctionType;
        using RangeEqualsCompareFunctionType  = typename inherited::RangeEqualsCompareFunctionType;
        using value_type                      = typename inherited::value_type;

    public:
        /**
         *  \see docs on Bijection constructor
         */
        Bijection_LinkedList ();
        template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
        explicit Bijection_LinkedList (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer);
        template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
        explicit Bijection_LinkedList (DataExchange::ValidationStrategy injectivityCheckPolicy,
                                       DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer);
        Bijection_LinkedList (Bijection_LinkedList&& src) noexcept      = default;
        Bijection_LinkedList (const Bijection_LinkedList& src) noexcept = default;
        Bijection_LinkedList (const initializer_list<value_type>& src);
        template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
        Bijection_LinkedList (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer,
                              const initializer_list<value_type>& src);
        template <IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>>)
        explicit Bijection_LinkedList (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Bijection_LinkedList{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER, IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERABLE_OF_ADDABLE>
        Bijection_LinkedList (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERATOR_OF_ADDABLE>
        Bijection_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER, IInputIterator<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERATOR_OF_ADDABLE>
        Bijection_LinkedList (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer,
                              ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Bijection_LinkedList& operator= (Bijection_LinkedList&& rhs) noexcept = default;
        nonvirtual Bijection_LinkedList& operator= (const Bijection_LinkedList& rhs)     = default;

    private:
        class IImplRepBase_;
        template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IEqualsComparer<DOMAIN_TYPE>) DOMAIN_EQUALS_COMPARER,
                  BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IEqualsComparer<RANGE_TYPE>) RANGE_EQUALS_COMPARER>
        class Rep_;

    private:
        nonvirtual void AssertRepValidType_ () const;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "Bijection_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_h_ */
