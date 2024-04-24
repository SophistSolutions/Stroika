/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_h_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include <map>

#include "Stroika/Foundation/Containers/MultiSet.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    using Common::IInOrderComparer;

    /**
     * \note Performance Notes:
     *      o   size () is constant complexity
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class MultiSet_stdmap : public MultiSet<T, TRAITS> {
    private:
        using inherited = MultiSet<T, TRAITS>;

    public:
        /**
         */
        using TraitsType                  = typename inherited::TraitsType;
        using CounterType                 = typename inherited::CounterType;
        using ElementEqualityComparerType = typename inherited::ElementEqualityComparerType;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         *  \brief STDMAP is std::map<> that can be used inside MultiSet_stdmap
         */
        template <IInOrderComparer<T> INORDER_COMPARER = less<T>>
        using STDMAP =
            map<T, CounterType, INORDER_COMPARER, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<pair<const T, CounterType>, sizeof (value_type) <= 1024>>;

    public:
        /**
         *  \see docs on MultiSet<> constructor; except replace EQUALS_COMPARER with INORDER_COMPARER
         */
        MultiSet_stdmap ();
        template <IInOrderComparer<T> INORDER_COMPARER>
        explicit MultiSet_stdmap (INORDER_COMPARER&& comparer);
        MultiSet_stdmap (MultiSet_stdmap&& src) noexcept      = default;
        MultiSet_stdmap (const MultiSet_stdmap& src) noexcept = default;
        MultiSet_stdmap (const initializer_list<T>& src);
        template <IInOrderComparer<T> INORDER_COMPARER>
        MultiSet_stdmap (INORDER_COMPARER&& comparer, const initializer_list<T>& src);
        MultiSet_stdmap (const initializer_list<value_type>& src);
        template <IInOrderComparer<T> INORDER_COMPARER>
        MultiSet_stdmap (INORDER_COMPARER&& comparer, const initializer_list<value_type>& src);
        template <IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, MultiSet_stdmap<T, TRAITS>>)
        explicit MultiSet_stdmap (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : MultiSet_stdmap{}
        {
            AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInOrderComparer<T> INORDER_COMPARER, IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
        MultiSet_stdmap (INORDER_COMPARER&& comparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
        MultiSet_stdmap (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IInOrderComparer<T> INORDER_COMPARER, IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE>
        MultiSet_stdmap (INORDER_COMPARER&& comparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual MultiSet_stdmap& operator= (MultiSet_stdmap&& rhs) noexcept = default;
        nonvirtual MultiSet_stdmap& operator= (const MultiSet_stdmap& rhs)     = default;

    private:
        class IImplRepBase_;
        template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IInOrderComparer<T>) INORDER_COMPARER>
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
#include "MultiSet_stdmap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_h_ */
