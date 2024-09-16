/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Moved #includes outside #include guard to avoid deadly embrace
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Containers/Mapping.h"

#ifndef _Stroika_Foundation_Containers_SortedMapping_h_
#define _Stroika_Foundation_Containers_SortedMapping_h_ 1

/**
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers {

    using Common::IInOrderComparer;

    /**
     *      A SortedMapping is a Mapping<Key,T> which remains sorted (iterator) by the Key.
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
     *  @see    Mapping<Key,T>
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \em Concrete Implementations:
     *      o   @see Concrete::SortedMapping_SkipList<>
     *      o   @see Concrete::SortedMapping_stdset<>
     * 
     *  \em Factory:
     *      @see SortedMapping_Factory<> to see default implementations.
     * 
     *  \note <a href="ReadMe.md#Container Element comparisons">Container Element comparisons</a>:
     *        See about ElementInOrderComparerType, ElementThreeWayComparerType and GetElementThreeWayComparer etc
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Mappings (base class) are already intrinsically equals-comparable.
     *
     *      o   Since SortedMapping implies an ordering on the elements of the mapping, we can use this to define a
     *          compare_three_way ordering for SortedMapping<>
     *
     *      o   Since this was never supported before (not a regression) - and close to end of C++17 specific development,
     *          only implementing three way compare for C++20 or later.
     *
     *      o   Note we don't need todo a comparison on value, since value uniquely defined by KEY, and we have a total ordering
     *          on the KEYS.
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class [[nodiscard]] SortedMapping : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

    protected:
        class _IRep;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        /**
         */
        using KeyInOrderComparerType =
            Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool (ArgByValueType<KEY_TYPE>, ArgByValueType<KEY_TYPE>)>>;

    public:
        /**
         */
        using KeyThreeWayComparerType =
            Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare,
                                                  function<strong_ordering (ArgByValueType<KEY_TYPE>, ArgByValueType<KEY_TYPE>)>>;

    public:
        /**
         *  This constructor creates a concrete sorted mapping object, either empty,
         *  or initialized with any argument values.
         *
         *  The underlying data structure of the Mapping is chosen by @see Factory::SortedMapping_Factory<>
         *
         *  \par Example Usage
         *      \code
         *          Mapping<int, int>       m{{1, 2}, {2, 4}};
         *          SortedMapping<int, int> sm{m};
         *      \endcode
         * 
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        SortedMapping ()
            requires (IInOrderComparer<less<KEY_TYPE>, KEY_TYPE>);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        explicit SortedMapping (KEY_INORDER_COMPARER&& inorderComparer);
        SortedMapping (SortedMapping&&) noexcept      = default;
        SortedMapping (const SortedMapping&) noexcept = default;
        SortedMapping (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
            requires (IInOrderComparer<less<KEY_TYPE>, KEY_TYPE>);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        SortedMapping (KEY_INORDER_COMPARER&& inorderComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        SortedMapping (KEY_INORDER_COMPARER&& inorderComparer, const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        explicit SortedMapping (ITERABLE_OF_ADDABLE&& src)
            requires (IInOrderComparer<less<KEY_TYPE>, KEY_TYPE> and
                      not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : SortedMapping{}
        {
            _AssertRepValidType ();
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        SortedMapping (KEY_INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        SortedMapping (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
            requires (IInOrderComparer<less<KEY_TYPE>, KEY_TYPE>);
        template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        SortedMapping (KEY_INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    protected:
        explicit SortedMapping (shared_ptr<_IRep>&& src) noexcept;
        explicit SortedMapping (const shared_ptr<_IRep>& src) noexcept;

    public:
        /**
         */
        nonvirtual SortedMapping& operator= (SortedMapping&&) noexcept = default;
        nonvirtual SortedMapping& operator= (const SortedMapping&)     = default;

    public:
        /**
         *  Return the function used to compare if two elements are in-order (sorted properly)
         */
        nonvirtual KeyInOrderComparerType GetKeyInOrderComparer () const;

    public:
        /**
         *  Return the function used to compare if two elements are (sorted properly)
         */
        nonvirtual KeyThreeWayComparerType GetKeyThreeWayComparer () const;

    public:
        /**
         * \brief 'override' Iterable<>::Map () function so RESULT_CONTAINER defaults to SortedMapping, and improve that case to clone properties from this rep (such is rep type, ordering, etc).
         */
        template <typename RESULT_CONTAINER = SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>, invocable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ELEMENT_MAPPER>
        nonvirtual RESULT_CONTAINER Map (ELEMENT_MAPPER&& elementMapper) const
            requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>, typename RESULT_CONTAINER::value_type> or
                      convertible_to<invoke_result_t<ELEMENT_MAPPER, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>, optional<typename RESULT_CONTAINER::value_type>>)
        ;

    public:
        /**
         *  \brief subset of this SortedMapping matching filter-function
         * 
         *  Identical to base class code, but for different RESULT_CONTAINER default.
         */
        template <derived_from<Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>> RESULT_CONTAINER = SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>, typename INCLUDE_PREDICATE>
        nonvirtual RESULT_CONTAINER Where (INCLUDE_PREDICATE&& includeIfTrue) const
            requires (predicate<INCLUDE_PREDICATE, KEY_TYPE> or predicate<INCLUDE_PREDICATE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>);

    public:
        /**
         *  Compare sequentially using the associated GetKeyThreeWayComparer ()  
         */
        nonvirtual strong_ordering operator<=> (const SortedMapping& rhs) const;

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
     *  \brief  Implementation detail for SortedMapping<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the SortedMapping<T> container API.
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {
    private:
        using inherited = typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep;

    public:
        virtual KeyThreeWayComparerType GetKeyThreeWayComparer () const = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "SortedMapping.inl"

#endif /*_Stroika_Foundation_Containers_SortedMapping_h_ */
