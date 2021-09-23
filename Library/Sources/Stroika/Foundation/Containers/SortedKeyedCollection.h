/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedKeyedCollection_h_
#define _Stroika_Foundation_Containers_SortedKeyedCollection_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "KeyedCollection.h"

/**
 *  \file
 *
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *                  (DRAFT/placeholder - no where near functional)
 *
 *
 *  TODO:
 *
 */

namespace Stroika::Foundation::Containers {

    /**
     *  Traits to define the well-ordering of elements of the SortedKeyedCollection.
     *
     *  Note - that a well-ordering also imputes a notion of equality (not (a<b or b < a)), so we define
     *  that as well.
     *
     */
    template <typename T, typename KEY_TYPE, typename DEFAULT_KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>, typename DEFAULT_KEY_WELL_ORDER_COMPARER = less<KEY_TYPE>, typename DEFAULT_KEY_EXTRACTOR = void>
    struct SortedKeyedCollection_DefaultTraits : KeyedCollection_DefaultTraits<T, KEY_TYPE, DEFAULT_KEY_EQUALS_COMPARER, DEFAULT_KEY_EXTRACTOR> 
    {
        /**
         */
        using KeyedCollectionTraits = KeyedCollection_DefaultTraits<T, KEY_TYPE, DEFAULT_KEY_EQUALS_COMPARER, DEFAULT_KEY_EXTRACTOR> ;

        /**
        * #if 0
        * /// WRONG UPDATE
         *  This is the type returned by GetElementEqualsComparer () and CAN be used as the argument to a KeyedCollection<> as EqualityComparer, but
         *  we allow any template in the KeyedCollection<> CTOR for an equalityComparer that follows the Common::IsEqualsComparer () concept.
         *
         *  \note   @see also EqualsComparer{} to compare whole KeyedCollection<>s
         * #endif
         */
        using KeyWellOrderCompareFunctionType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eStrictInOrder, function<bool (ArgByValueType<KEY_TYPE>, ArgByValueType<KEY_TYPE>)>>;
 
       /**
         *  Default comparer if not specified in constructor (e.g. default-constructor SortedKeyedCollection())
         */
        static const inline KeyWellOrderCompareFunctionType kDefaultKeyWellOrderComparer{DEFAULT_KEY_WELL_ORDER_COMPARER{}};
    };

    /**
     *  \brief  A SortedKeyedCollection is a KeyedCollection<T> which remains sorted (iteration produces items sorted) even as you add and remove entries.
     *
     *  A SortedKeyedCollection is a KeyedCollection<T> which remains sorted (iteration produces items sorted) even as you add and remove entries.
     *
     *  Note that even though you cannot remove elements by value, or check "Contains" etc on Collection - in general, you always
     *  can with a SortedKeyedCollection, because the well-ordering required to define a sorted collection also imputes
     *  a notion of equality which is used for Contains etc.
     *
     *  \note   \em Iterators
     *      Note that iterators always run in sorted order, from least
     *      to largest. Items inserted before the current iterator index will not
     *      be encountered, and items inserted after the current index will be encountered.
     *      Items inserted at the current index remain undefined if they will
     *      be encountered or not.
     *
     *  @see Collection<T, TRAITS>
     *  @see SortedMapping<Key,T>
     *  @see SortedSet<T, TRAITS>
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Automatically-LEGACY_Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
     *
     */
    template <typename KEY_TYPE, typename T, typename TRAITS = SortedKeyedCollection_DefaultTraits<KEY_TYPE, T>>
    class SortedKeyedCollection : public KeyedCollection<T, KEY_TYPE, TRAITS> {
    private:
        using inherited = KeyedCollection<KEY_TYPE, T>;

    protected:
        class _IRep;

    protected:
#if qCompilerAndStdLib_TemplateTemplateWithTypeAlias_Buggy
        using _SharedIRepPtr = conditional_t<Stroika::Foundation::Traversal::kIterableUsesStroikaSharedPtr, Stroika::Foundation::Memory::SharedPtr<_IRep>, shared_ptr<_IRep>>;
#else
        using _SharedIRepPtr = typename inherited::template PtrImplementationTemplate<_IRep>;
#endif

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = SortedKeyedCollection<T, KEY_TYPE, TRAITS>;

    public:
        /**
         *  Just a short-hand for the 'TRAITS' part of SortedKeyedCollection<T,TRAITS>. This is often handy to use in
         *  building other templates.
         */
        using TraitsType = TRAITS;

    public:
        /**
         *  Just a short-hand for the WellOrderCompareFunctionType specified through traits. This is often handy to use in
         *  building other templates.
         */
        using WellOrderCompareFunctionType = typename TraitsType::WellOrderCompareFunctionType;

    public:
        /**
        * 
        * &&& WRONG TODO UPDATE - CLONED FROM BASE CLASS &&& DOCS &&&
        * 
         *  For the CTOR overload with CONTAINER_OF_ADDABLE, its anything that supports c.begin(), c.end () to find
         *  all the elements.
         *
         *  All constructors either copy their source comparer (copy/move CTOR), or use the provided argument comparer
         *  (which in turn defaults to equal_to<T>).
         *
         *  If TRAITS (TraitsType) has a valid default extractor, enable certain constructors.
         *
         *  \note For efficiency sake, the base constructor takes a templated EQUALS_COMPARER (avoiding translation to function<bool(T,T)>>, but
         *        for simplicity sake, many of the other constructors force that conversion.
         *
         * \req IsEqualsComparer<EQUALS_COMPARER> () - for constructors with that type parameter
         *
         *  \par Example Usage
         *      \code
         *      \endcode
         *
         *  \note Implementation note:
         *        Reason for the not is_base_of_v<> restriction on CTOR/1(CONTAINER_OF_ADDABLE&&) is to prevent compiler from
         *        instantiating that constructor template for argument subclasses of this container type, and having those take precedence over the
         *        default X(const X&) CTOR.
         * 
         *        And also careful not to apply to non-iterables.
         */
        template <typename KE = typename TraitsType::DefaultKeyExtractor, enable_if_t<Configuration::is_callable_v<KE>>* = nullptr>
        SortedKeyedCollection (KeyEqualityComparerType keyComparer = TraitsType::kDefaultKeyEqualsComparer);
        SortedKeyedCollection (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer = TraitsType::kDefaultKeyEqualsComparer);
        SortedKeyedCollection (const KeyedCollection& src) noexcept = default;
        template <typename CONTAINER_OF_ADDABLE, typename KE = typename TraitsType::DefaultKeyExtractor, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<KeyedCollection<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Configuration::is_callable_v<KE>>* = nullptr>
        SortedKeyedCollection (CONTAINER_OF_ADDABLE&& src, KeyEqualityComparerType keyComparer = TraitsType::kDefaultKeyEqualsComparer);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<KeyedCollection<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        SortedKeyedCollection (CONTAINER_OF_ADDABLE&& src, KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer = TraitsType::kDefaultKeyEqualsComparer);

    protected:
        explicit SortedKeyedCollection (const _SharedIRepPtr& src);
        explicit SortedKeyedCollection (_SharedIRepPtr&& src);

    public:
        /**
         */
        nonvirtual SortedKeyedCollection<T, TRAITS>& operator= (const SortedKeyedCollection<T, KEY_TYPE, TRAITS>& rhs);
        nonvirtual SortedKeyedCollection<T, TRAITS>& operator= (SortedKeyedCollection<T, KEY_TYPE, TRAITS>&& rhs) = default;

    protected:
        nonvirtual void _AssertRepValidType () const;
    };

    /**
     *  \brief  Implementation detail for SortedKeyedCollection<T, TRAITS> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the SortedKeyedCollection<T, TRAITS> container API.
     */
    template <typename T, typename TRAITS>
    class SortedKeyedCollection<T, TRAITS>::_IRep : public KeyedCollection<T>::_IRep {
    public:
       // virtual bool Equals (const typename Collection<T>::_IRep& rhs) const = 0;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "SortedKeyedCollection.inl"

#endif /*_Stroika_Foundation_Containers_SortedKeyedCollection_h_ */
