/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Set_Array_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Private/ArraySupport.h"
#include "Stroika/Foundation/Containers/Set.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Set_Array<T> is an Array-based concrete implementation of the Set<T> container pattern.
     *
     * \note Runtime performance/complexity:
     *      Set_stdset<T> is a compact representation, but ONLY reasonable for very small sets. Performance is O(N) as set grows.
     *
     *      o   Additions and Removals are generally O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class Set_Array : public Private::ArrayBasedContainer<Set_Array<T>, Set<T>, true> {
    private:
        using inherited = Private::ArrayBasedContainer<Set_Array<T>, Set<T>, true>;

    public:
        using ElementEqualityComparerType = typename inherited::ElementEqualityComparerType;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \see docs on Set<> constructor
         */
        Set_Array ();
        template <IEqualsComparer<T> EQUALS_COMPARER>
        explicit Set_Array (EQUALS_COMPARER&& equalsComparer);
        Set_Array (Set_Array&&) noexcept      = default;
        Set_Array (const Set_Array&) noexcept = default;
        Set_Array (const initializer_list<value_type>& src);
        template <IEqualsComparer<T> EQUALS_COMPARER>
        Set_Array (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Set_Array<T>>)
        explicit Set_Array (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Set_Array{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IEqualsComparer<T> EQUALS_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
        Set_Array (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Set_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IEqualsComparer<T> EQUALS_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        Set_Array (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Set_Array& operator= (Set_Array&&) noexcept = default;
        nonvirtual Set_Array& operator= (const Set_Array&)     = default;

    private:
        using IImplRepBase_ = Containers::Private::ArrayBasedContainerIRep<typename Set<T>::_IRep>;
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (IEqualsComparer<T>) EQUALS_COMPARER>
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

#include "Set_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Set_Array_h_ */
