/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedCollection_h_
#define _Stroika_Foundation_Containers_SortedCollection_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "Collection.h"

/**
 *  \file
 *
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *
 *  TODO:
 *      @todo   Support Iterable<>::Where overload?
 *
 *      @todo   Add Equals(), Contains, Remove(T) methods (we have the virtuals in rep already)
 *
 *      @todo   Improve test cases, and notice that sorting doesn't actually work for sorted-linked-list.
 *
 *      @todo   Implement using redblback trees.
 *
 *      @todo   See if there is a good STL backend to use to implement this? std::multimap<T,void> is the closest
 *              I can see.
 *
 */

namespace Stroika::Foundation::Containers {

    /**
     *  \brief  A SortedCollection is a Collection<T> which remains sorted (iteration produces items sorted) even as you add and remove entries.
     *
     *  A SortedCollection is a Collection<T> which remains sorted (iteration produces items sorted) even as you add and remove entries.
     *
     *  Note that even though you cannot remove elements by value, or check "Contains" etc on Collection - in general, you always
     *  can with a SortedCollection, because the well-ordering required to define a sorted collection also imputes
     *  a notion of equality which is used for Contains etc.
     *
     *  \note   \em Iterators
     *      Note that iterators always run in sorted order, from least
     *      to largest. Items inserted before the current iterator index will not
     *      be encountered, and items inserted after the current index will be encountered.
     *      Items inserted at the current index remain undefined if they will
     *      be encountered or not.
     *
     *  @see Collection<T>
     *  @see SortedMapping<Key,T>
     *  @see SortedSet<T>
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Automatically-LEGACY_Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
     *
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *      o   Even though the base class Collection<T> provides no intrinsic comparison operators, to be sorted,
     *          does imply a comparison operator, so a SortedCollection<T> fully supports the c++ standard operator<=> strong comparison
     *          feature.
     *      o   Compare sequentially using the associated GetInOrderComparer ()
     *      o   Only supported if C++20 comparison supported (easy for C++17, but no need since close to abandoing C++17 support and not regression)
     */
    template <typename T>
    class SortedCollection : public Collection<T> {
    private:
        using inherited = Collection<T>;

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
        using ArchetypeContainerType = SortedCollection<T>;

    public:
        /**
         *  This CAN be used as the argument to a SortedSet<> as InOrderComparerType, but
         *  we allow any template in the SortedSet<> CTOR for an inorderComparer that follows Common::IsStrictInOrderComparer () concept (need better name).
         */
        using InOrderComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool (T, T)>>;

    public:
        /**
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add (Enqueue)
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = inherited::template IsAddable_v<POTENTIALLY_ADDABLE_T>;

    public:
        /**
         *  All constructors either copy their source comparer (copy/move CTOR), or use the default INORDER_COMPARER for 'T'.
         *
         * \req IsStrictInOrderComparer<INORDER_COMPARER> () - for constructors with that type parameter
         * 
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        SortedCollection ();
        template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>* = nullptr>
        explicit SortedCollection (INORDER_COMPARER&& inorderComparer);
        SortedCollection (SortedCollection&& src) noexcept      = default;
        SortedCollection (const SortedCollection& src) noexcept = default;
        SortedCollection (const initializer_list<T>& src);
        template <typename INORDER_COMPARER, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>* = nullptr>
        SortedCollection (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<SortedCollection<T>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit SortedCollection (ITERABLE_OF_ADDABLE&& src);
        template <typename INORDER_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        SortedCollection (INORDER_COMPARER&& inOrderComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE>
        SortedCollection (ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);
        template <typename INORDER_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsStrictInOrderComparer<INORDER_COMPARER, T> ()>* = nullptr>
        SortedCollection (INORDER_COMPARER&& inOrderComparer, ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);

    protected:
        explicit SortedCollection (_IRepSharedPtr&& src) noexcept;
        explicit SortedCollection (const _IRepSharedPtr& src) noexcept;

    public:
        /**
         */
        nonvirtual SortedCollection& operator= (const SortedCollection& rhs) = default;

    public:
        /**
         *  Return the function used to compare if two elements are in-order (sorted properly)
         */
        nonvirtual InOrderComparerType GetInOrderComparer () const;

    public:
        /**
         * \brief Compares items with the already associated GetInOrderComparer(), and returns true if the item is found.
         */
        nonvirtual bool Contains (ArgByValueType<T> item) const;

    public:
        /**
         * \brief Removes the argument item with the already associated GetInOrderComparer(), and returns true if the item is found.
         *
         *  \note mutates container
         */
        using inherited::Remove;
        nonvirtual void Remove (ArgByValueType<T> item);

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         *  Compare sequentially using the associated GetInOrderComparer ()  
         */
        nonvirtual bool operator== (const SortedCollection& rhs) const;

    public:
        /**
         *  Compare sequentially using the associated GetInOrderComparer ()  
         */
        nonvirtual strong_ordering operator<=> (const SortedCollection& rhs) const;
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
     *  \brief  Implementation detail for SortedCollection<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the SortedCollection<T> container API.
     */
    template <typename T>
    class SortedCollection<T>::_IRep : public Collection<T>::_IRep {
    public:
        virtual InOrderComparerType GetInOrderComparer () const                             = 0;
        virtual bool                Equals (const typename Collection<T>::_IRep& rhs) const = 0;
        virtual bool                Contains (ArgByValueType<T> item) const                 = 0;
        using Collection<T>::_IRep::Remove;
        virtual void Remove (ArgByValueType<T> item) = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "SortedCollection.inl"

#endif /*_Stroika_Foundation_Containers_SortedCollection_h_ */
