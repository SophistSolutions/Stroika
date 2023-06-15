/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Association.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Association_LinkedList_h_
#define _Stroika_Foundation_Containers_Concrete_Association_LinkedList_h_

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an LinkedList-based concrete implementation of the Association<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::AssociationTraitsType> container pattern.
     *
     * \note Performance Notes:
     *      o   size () is O(N)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association_LinkedList : public Association<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Association<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v  = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using value_type                   = typename inherited::value_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        /**
         *  \see docs on Association<> constructor
         */
        Association_LinkedList ();
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
        explicit Association_LinkedList (KEY_EQUALS_COMPARER&& keyEqualsComparer);
        Association_LinkedList (Association_LinkedList&& src) noexcept      = default;
        Association_LinkedList (const Association_LinkedList& src) noexcept = default;
        Association_LinkedList (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
        Association_LinkedList (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        template <IIterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE,
                  enable_if_t<not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>>>* = nullptr>
        explicit Association_LinkedList (ITERABLE_OF_ADDABLE&& src);
#else
        template <IIterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        explicit Association_LinkedList (ITERABLE_OF_ADDABLE&& src)
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>>);
#endif
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IIterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        Association_LinkedList (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Association_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Association_LinkedList (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Association_LinkedList& operator= (Association_LinkedList&& rhs) noexcept = default;
        nonvirtual Association_LinkedList& operator= (const Association_LinkedList& rhs)     = default;

    private:
        class IImplRepBase_;
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
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

#include "Association_LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Association_LinkedList_h_ */
