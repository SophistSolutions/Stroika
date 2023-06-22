/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include <set>

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

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Set_stdset<KEY_TYPE, VALUE_TYPE, TRAITS> is an std::set-based concrete implementation of the Set<T> container pattern.
     *
     * \note Performance Notes:
     *      Set_stdset<T> is a compact, and reasonable implementation of Set<>, and scales well.
     *
     *      o   size () is constant complexity
     *      o   Uses Memory::BlockAllocatorOrStdAllocatorAsAppropriate
     *      o   Additions and Removals are generally O(ln(N))
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T>
    class Set_stdset : public Set<T> {
    private:
        using inherited = Set<T>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using ElementEqualityComparerType = typename inherited::ElementEqualityComparerType;
        using value_type                  = typename inherited::value_type;

    public:
        /**
         */
        template <Common::IInOrderComparer<T> INORDER_COMPARER>
        using STDSET =
            set<value_type, INORDER_COMPARER, Memory::BlockAllocatorOrStdAllocatorAsAppropriate<value_type, sizeof (value_type) <= 1024>>;

    public:
        /**
         *  \see docs on Set<> constructor, but repalce EQUALS_COMPARER with INORDER_COMPARER
         */
        Set_stdset ();
        template <Common::IInOrderComparer<T> INORDER_COMPARER>
        explicit Set_stdset (INORDER_COMPARER&& inorderComparer);
        Set_stdset (Set_stdset&& src) noexcept      = default;
        Set_stdset (const Set_stdset& src) noexcept = default;
        Set_stdset (const initializer_list<value_type>& src);
        template <Common::IInOrderComparer<T> INORDER_COMPARER>
        Set_stdset (INORDER_COMPARER&& inorderComparer, const initializer_list<value_type>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Set_stdset<T>>)
        explicit Set_stdset (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Set_stdset{}
        {
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <Common::IInOrderComparer<T> INORDER_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
        Set_stdset (INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Set_stdset (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <Common::IInOrderComparer<T> INORDER_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
        Set_stdset (INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Set_stdset& operator= (Set_stdset&& rhs) noexcept = default;
        nonvirtual Set_stdset& operator= (const Set_stdset& rhs)     = default;

    private:
        class IImplRepBase_;
        template <Common::IInOrderComparer<T> INORDER_COMPARER>
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
#include "Set_stdset.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Set_stdset_h_ */
