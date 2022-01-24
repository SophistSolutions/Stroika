/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedAssociation_h_
#define _Stroika_Foundation_Containers_SortedAssociation_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "Association.h"

/**
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers {

    /**
     *      A SortedAssociation is a Association<Key,T> which remains sorted (iterator) by the Key.
     *
     *  \note Alias this class might have been called "Dictionary".
     *
     *  \note   \em Iterators
     *      Note that iterators always run in sorted order, from least
     *      to largest. Items inserted before the current iterator index will not
     *      be encountered, and items inserted after the current index will be encountered.
     *      Items inserted at the current index remain undefined if they will
     *      be encountered or not.
     *
     *  @see    Association<Key,T>
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *      o   Associations (base class) are already intrinsically equals-comparable.
     *
     *      o   Since SortedAssociation implies an ordering on the elements of the Association, we can use this to define a
     *          three_way_compare ordering for SortedAssociation<>
     *
     *      o   Since this was never supported before (not a regression) - and close to end of C++17 specific development,
     *          only implementing three way compare for C++20 or later.
     *
     *      o   Note we don't need todo a comparison on value, since value uniquely defined by KEY, and we have a total ordering
     *          on the KEYS.
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedAssociation : public Association<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Association<KEY_TYPE, MAPPED_VALUE_TYPE>;

    protected:
        class _IRep;

    protected:
#if qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy
        using _IRepSharedPtr = conditional_t<Stroika::Foundation::Traversal::kIterableUsesStroikaSharedPtr, Stroika::Foundation::Memory::SharedPtr<_IRep>, shared_ptr<_IRep>>;
#else
        using _IRepSharedPtr = typename inherited::template PtrImplementationTemplate<_IRep>;
#endif

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        /**
         *  This CAN be used as the argument to a SortedAssociation<> as InOrderComparerType, but
         *  we allow any template in the SortedSet<> CTOR for an inorderComparer that follows Common::IsStrictInOrderComparer () concept
         */
        using KeyInOrderKeyComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool (KEY_TYPE, KEY_TYPE)>>;

    public:
        /**
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add (Enqueue)
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;

    public:
        /**
         *  This constructor creates a concrete sorted Association object, either empty,
         *  or initialized with any argument values.
         *
         *  The underlying data structure of the Association is defined by @see Factory::SortedAssociation_Factory<>
         *
         *  \par Example Usage
         *      \code
         *          Association<int, int>       m{{1, 2}, {2, 4}};
         *          SortedAssociation<int, int> sm{m};
         *      \endcode
         * 
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        SortedAssociation ();
        template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>* = nullptr>
        explicit SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer);
        SortedAssociation (SortedAssociation&& src) noexcept      = default;
        SortedAssociation (const SortedAssociation& src) noexcept = default;
        SortedAssociation (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> ()>* = nullptr>
        SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit SortedAssociation (ITERABLE_OF_ADDABLE&& src);
        template <typename KEY_INORDER_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        SortedAssociation (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <typename KEY_INORDER_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<KEY_INORDER_COMPARER, KEY_TYPE> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        SortedAssociation (KEY_INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit SortedAssociation (_IRepSharedPtr&& src) noexcept;
        explicit SortedAssociation (const _IRepSharedPtr& src) noexcept;

    public:
        /**
         */
        nonvirtual SortedAssociation& operator= (SortedAssociation&& rhs) noexcept = default;
        nonvirtual SortedAssociation& operator= (const SortedAssociation& rhs) = default;

    public:
        /**
         *  Return the function used to compare if two elements are in-order (sorted properly)
         */
        nonvirtual KeyInOrderKeyComparerType GetInOrderKeyComparer () const;

    public:
        /**
         *  Compare sequentially using the associated GetInOrderKeyComparer ()  
         */
        nonvirtual strong_ordering operator<=> (const SortedAssociation& rhs) const;

    protected:
        /**
         */
        template <typename T2>
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<T2>;

    protected:
        /**
         */
        template <typename T2>
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<T2>;

    protected:
        nonvirtual void _AssertRepValidType () const;
    };

    /**
     *  \brief  Implementation detail for SortedAssociation<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the SortedAssociation<T> container API.
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep : public Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {
    private:
        using inherited = typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep;

    public:
        virtual KeyInOrderKeyComparerType GetInOrderKeyComparer () const = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedAssociation.inl"

#endif /*_Stroika_Foundation_Containers_SortedAssociation_h_ */
