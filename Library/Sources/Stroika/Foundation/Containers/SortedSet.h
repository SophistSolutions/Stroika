/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedSet_h_
#define _Stroika_Foundation_Containers_SortedSet_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "Set.h"

/**
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 *  TODO:
 *      @todo   Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *              concepts and that it USES that for the appropriate overloaded constructors.
 *
 *      @todo   Support Iterable<>::Where overload?
 *
 *      @todo   CRITICAL - need version where you can pass in a lambda to compare two things for <, so
 *              easy to construct a sorted set with your own sorter function!!!
 *
 *              This applies equally to other Stroika sorted types
 *
 *
 */

namespace Stroika::Foundation::Containers {

    /**
     *      A SortedSet is a Set<T> which remains sorted (iterator).
     *
     *  \note   \em Iterators
     *      Note that iterators always run in sorted order, from least
     *      to largest. Items inserted before the current iterator index will not
     *      be encountered, and items inserted after the current index will be encountered.
     *      Items inserted at the current index remain undefined if they will
     *      be encountered or not.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note Move constructor/assignment
     *      This maps to copy due to COW - see description of Iterable<T> for details.
     *
     *  \note Note About Iterators
     *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
     *
     *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
     *          the iterators are automatically updated internally to behave sensibly.
     *
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *        o Set (base class) are already intrinsically equals-comparable.
     *
     *        o Since SortedSet implies an ordering on the elements of the Set, we can use this to define a
     *          three_way_compare ordering for SortedSet<>
     *
     *        o Since this was never supported before (not a regression) - and close to end of C++17 specific development,
     *          only implementing three way compare for C++20 or later.
     */
    template <typename T>
    class SortedSet : public Set<T> {
    private:
        using inherited = Set<T>;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = SortedSet<T>;

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
         *  Ordering reletion applied to sort a 'SortedSet'. Returned by GetInOrderComparer ();
         */
        using ElementInOrderComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool (T, T)>>;

    public:
        /**
         *  All constructors either copy their source comparer (copy/move CTOR), or use the default INORDER comparer for 'T'.
         *
         * \req IsStrictInOrderComparer<INORDER_COMPARER> () - for constructors with that type parameter
         * 
         *  \note Implementation note:
         *        Reason for the not is_base_of_v<> restriction on CTOR/1(CONTAINER_OF_ADDABLE&&) is to prevent compiler from
         *        instantiating that constructor template for argument subclasses of this container type, and having those take precedence over the
         *        default X(const X&) CTOR.
         * 
         *        And also careful not to apply to non-iterables.
         *
         *  \todo   @todo https://stroika.atlassian.net/browse/STK-744 - rethink details of Stroika Container constructors
         */
        SortedSet ();
        template <typename INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>* = nullptr>
        explicit SortedSet (INORDER_COMPARER&& inorderComparer);
        SortedSet (const SortedSet& src) noexcept = default;
        SortedSet (const initializer_list<T>& src);
        template <typename INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> ()>* = nullptr>
        SortedSet (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<SortedSet<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        explicit SortedSet (const CONTAINER_OF_ADDABLE& src);
        template <typename INORDER_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>* = nullptr>
        SortedSet (INORDER_COMPARER&& inOrderComparer, const CONTAINER_OF_ADDABLE& src);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>* = nullptr>
        SortedSet (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename INORDER_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, INORDER_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>* = nullptr>
        SortedSet (INORDER_COMPARER&& inOrderComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

    protected:
        explicit SortedSet (const _IRepSharedPtr& src) noexcept;
        explicit SortedSet (_IRepSharedPtr&& src) noexcept;

    public:
        /**
         */
        nonvirtual SortedSet& operator= (const SortedSet& rhs) = default;

    public:
        /**
         *  Return the function used to compare if two elements are in-order (sorted properly)
         */
        nonvirtual ElementInOrderComparerType GetInOrderComparer () const;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         *  Compare sequentially using the associated GetInOrderComparer ()  
         */
        nonvirtual strong_ordering operator<=> (const SortedSet& rhs) const;
#endif

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
     *  \brief  Implementation detail for SortedSet<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the SortedSet<T> container API.
     */
    template <typename T>
    class SortedSet<T>::_IRep : public Set<T>::_IRep {
    public:
        virtual ElementInOrderComparerType GetInOrderComparer () const = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedSet.inl"

#endif /*_Stroika_Foundation_Containers_SortedSet_h_ */
