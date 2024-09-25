/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_Array_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_Array_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/MultiSet.h"
#include "Stroika/Foundation/Containers/Private/ArraySupport.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *
     * \note Runtime performance/complexity:
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class MultiSet_Array : public Private::ArrayBasedContainer<MultiSet_Array<T, TRAITS>, MultiSet<T, TRAITS>, true> {
    private:
        using inherited = Private::ArrayBasedContainer<MultiSet_Array<T, TRAITS>, MultiSet<T, TRAITS>, true>;

    public:
        using CounterType                 = typename inherited::CounterType;
        using ElementEqualityComparerType = typename inherited::ElementEqualityComparerType;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \see docs on MultiSet<> constructor
         */
        MultiSet_Array ();
        template <IEqualsComparer<T> EQUALS_COMPARER>
        explicit MultiSet_Array (EQUALS_COMPARER&& equalsComparer);
        MultiSet_Array (MultiSet_Array&&) noexcept      = default;
        MultiSet_Array (const MultiSet_Array&) noexcept = default;
        MultiSet_Array (const initializer_list<T>& src);
        template <IEqualsComparer<T> EQUALS_COMPARER>
        MultiSet_Array (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src);
        MultiSet_Array (const initializer_list<value_type>& src);
        template <IEqualsComparer<T> EQUALS_COMPARER>
        MultiSet_Array (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src);
        template <IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, MultiSet_Array<T, TRAITS>>)
        explicit MultiSet_Array (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : MultiSet_Array{}
        {
            if constexpr (Common::IHasSizeMethod<ITERABLE_OF_ADDABLE>) {
                this->reserve (src.size ());
            }
            AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IEqualsComparer<T> EQUALS_COMPARER, IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
        MultiSet_Array (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
        MultiSet_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IEqualsComparer<T> EQUALS_COMPARER, IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
        MultiSet_Array (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual MultiSet_Array& operator= (MultiSet_Array&&) noexcept = default;
        nonvirtual MultiSet_Array& operator= (const MultiSet_Array&)     = default;

    private:
        using IImplRepBase_ = Containers::Private::ArrayBasedContainerIRep<typename MultiSet<T, TRAITS>::_IRep>;
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (IEqualsComparer<T>) EQUALS_COMPARER>
        class Rep_;

    private:
        nonvirtual void AssertRepValidType_ () const;

    private:
        friend inherited;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "MultiSet_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_MultiSet_Array_h_*/
