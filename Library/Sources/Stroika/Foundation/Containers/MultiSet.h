/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_MultiSet_h_
#define _Stroika_Foundation_Containers_MultiSet_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <set>

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Common/CountedValue.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Memory/SharedByValue.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Containers/DefaultTraits/MultiSet.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   http://stroika-bugs.sophists.com/browse/STK-428 - See how to map EqualsComparer to CountedValue we use
 *              probably added template param to CountedValue - TRAITS.
 *
 *              MAYBE add    using   MultisetEntryType                       =       CountedValue<T>; to
 *              TRAITS object. Then use throughout.... Not sure this is worth parameterizing, but
 *              COULD help address unifying the EQUALS support!
 *
 *      @todo   IMPORTANT - FIX TRAITS support like I did for Mapping/Set<> - Sorted...
 *              see git commit # 3c5bf0ecd686af850ff77761cf94142a33f48588
 *
 *              Key is adding MultiSetTraitsType to the traits and making generic base class
 *              for MultiSet<T> - its traits - same as wtih SortedTraits.
 *
 *              Also likewise key for MultiSet_stdmap<> - cuz now you cannot assign MultiSet_stdmap<> to
 *              MultiSet<T>!!!!
 *
 *      @todo   Consider rewriting all MultiSet<> concrete types using Mapping<T,counttype> concrete impl?
 *              Might not work easily but document why... (Add () semantics - but maybe).
 *
 *
 */

namespace Stroika::Foundation::Containers {

    using Common::CountedValue;
    using Common::IEqualsComparer;
    using Configuration::ArgByValueType;
    using Traversal::IInputIterator;
    using Traversal::IIterableOf;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *  A MultiSet<T, TRAITS> A collection of elements where each time you add something, the MultiSet tallies
     *  the number of times that thing has been entered. This is not a commonly used class, but handy when you want to count things.
     *
     *  MultiSet<T, TRAITS> inherits from Iterable<CountedValue<T>> instead of Iterable<T> because if you are
     *  using a MultiSet, you probably want access to the counts as you iterate - not just the
     *  unique elements (though we make it easy to get that iterator too with Elements () or
     *  UniqueElements ()).
     *
     *  A MultiSet<T, TRAITS> makes no promises about ordering of elements in iteration.
     *
     *  @see    http://en.wikipedia.org/wiki/Multiset_(abstract_data_type)#Multiset
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   \em Aliases         Tally (Stroika 1.0), Bag (from SmallTalk-80)
     *
     *  \em Concrete Implementations:
     *      o   @see Concrete::MultiSet_Array<>
     *      o   @see Concrete::MultiSet_LinkedList<>
     *      o   @see Concrete::SortedMultiSet_stdmap<>
     *      o   @see Concrete::SortedMultiSet_SkipList<>
     *
     *  \em Factory:
     *      @see MultiSet_Factory<> to see default implementations.
     *
     *  \note   Though the name (and function) is quite similar to the std::multiset<>, this version is generally MUCH
     *          more convenient to use, in that when you iterate over it, you get the items with their counts, not
     *          each element repeated that many times (in some arbitrary order). To get the std::multiset behavior, use
     *          MultiSet<T>::Elements ().
     *
     *  \note <a href="ReadMe.md#Container Element comparisons">Container Element comparisons</a>:
     *        See about ElementInOrderComparerType, ElementThreeWayComparerType and GetElementThreeWayComparer etc
     *
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   static_assert (equality_comparable<MultiSet<T>>);
     *
     *      o   Multisets intrinsically know how to compare their elements (for equality) - even if equal_to<T> not defined
     *
     *          Two MultiSet are considered equal if they contain the same elements (by comparing them with GetElementEqualsComparer ())
     *          with the same count. In short, they are equal if OccurrencesOf() each item in the LHS equals the OccurrencesOf()
     *          the same item in the RHS.
     *
     *          Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
     *          @todo - document computational complexity
     *
     *          ThreeWayComparer support is NOT provided for Multisets, because there is no intrinsic ordering among the elements
     *          of the multiset (keys) - even if there was some way to compare the T elements.
     *
     */
    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class [[nodiscard]] MultiSet : public Iterable<typename TRAITS::CountedValueType> {
    private:
        using inherited = Iterable<typename TRAITS::CountedValueType>;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = MultiSet;

    public:
        /**
         *  Just a short-hand for the 'TRAITS' part of MultiSet<T,TRAITS>. This is often handy to use in
         *  building other templates.
         */
        using TraitsType = TRAITS;

    public:
        /**
         *      \brief  MultiSetOfElementType is just a handy copy of the *T* template type which this
         *              MultiSet<T, TRAITS> parameterizes counting.
         */
        using MultiSetOfElementType = T;

    public:
        /**
         *      \brief
         */
        using CounterType = typename TraitsType::CounterType;

    public:
        /**
         *  typename TRAITS::CountedValueType - not 'T' itself
         * 
         *  @see inherited::value_type
         */
        using value_type = typename inherited::value_type;
        static_assert (same_as<typename TRAITS::CountedValueType, value_type>);

    protected:
        class _IRep;

    public:
        /**
         *  This is the type returned by GetElementEqualsComparer () and CAN be used as the argument to a MultiSet<> as EqualityComparer, but
         *  we allow any template in the Set<> CTOR for an equalityComparer that follows the IEqualsComparer concept.
         */
        using ElementEqualityComparerType =
            Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool (ArgByValueType<T>, ArgByValueType<T>)>>;
        static_assert (IEqualsComparer<ElementEqualityComparerType, T>);

    public:
        /**
         *  All constructors either copy their source comparer (copy/move CTOR), or use the provided argument comparer
         *  (which in turn defaults to equal_to<T>).
         *
         *  \note For efficiency sake, the base constructor takes a templated EQUALS_COMPARER (avoiding translation to function<bool(T,T)>>
         *        so the REP can see the actual type, but the MultiSet API itself erases this specific type using std::function.
         *
         * \req IEqualsComparer<EQUALS_COMPARER> - for constructors with that type parameter
         * 
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         *
         *  \par Example Usage
         *      \code
         *          Collection<int> c;
         *          std::vector<int> v;
         *
         *          MultiSet<int> s1  = {1, 2, 3};
         *          MultiSet<int> s2  = s1;
         *          MultiSet<int> s3{ s1 };
         *          MultiSet<int> s4{ s1.begin (), s1.end () };
         *          MultiSet<int> s5{ c };
         *          MultiSet<int> s6{ v };
         *          MultiSet<int> s7{ v.begin (), v.end () };
         *          MultiSet<int> s8{ move (s1) };
         *          MultiSet<int> s9{ Common::DeclareEqualsComparer([](int l, int r) { return l == r; }), c};
         *      \endcode
         */
        MultiSet ()
            requires (IEqualsComparer<equal_to<T>, T>);
        template <IEqualsComparer<T> EQUALS_COMPARER>
        explicit MultiSet (EQUALS_COMPARER&& equalsComparer);
        MultiSet (MultiSet&&) noexcept      = default;
        MultiSet (const MultiSet&) noexcept = default;
        MultiSet (const initializer_list<T>& src)
            requires (IEqualsComparer<equal_to<T>, T>);
        template <IEqualsComparer<T> EQUALS_COMPARER>
        MultiSet (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src);
        MultiSet (const initializer_list<value_type>& src);
        template <IEqualsComparer<T> EQUALS_COMPARER>
        MultiSet (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src);
        template <IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
        explicit MultiSet (ITERABLE_OF_ADDABLE&& src)
            requires (IEqualsComparer<equal_to<T>, T> and not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, MultiSet<T, TRAITS>>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : MultiSet{}
        {
            AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;
        template <IEqualsComparer<T> EQUALS_COMPARER, IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
        MultiSet (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
        MultiSet (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end)
            requires (IEqualsComparer<equal_to<T>, T>);
        template <IEqualsComparer<T> EQUALS_COMPARER, IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
        MultiSet (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end);

    protected:
        explicit MultiSet (shared_ptr<_IRep>&& rep) noexcept;
        explicit MultiSet (const shared_ptr<_IRep>& rep) noexcept;

    public:
        nonvirtual MultiSet& operator= (MultiSet&&) noexcept = default;
        nonvirtual MultiSet& operator= (const MultiSet&)     = default;

    public:
        /**
         *  Contains (item) is equivalent to OccurrencesOf (item) != 0, but maybe faster (since it doesn't need to compute
         *  the fully tally).
         */
        nonvirtual bool Contains (ArgByValueType<T> item) const;

    public:
        /**
         *  \note mutates container
         */
        nonvirtual void Add (ArgByValueType<T> item);
        nonvirtual void Add (ArgByValueType<T> item, CounterType count);
        nonvirtual void Add (const value_type& item);

    public:
        /**
         *  \note   AddAll/2 is alias for .net AddRange ()
         *          and AddAll/2 - the iterator can be Iterator<T> or Iterator<typename TRAITS::CountedValueType>
         *
         *  \req IInputIterator<typename TRAITS::CountedValueType> or IIterableOf<typename TRAITS::CountedValueType>
         *
         *  \note mutates container
         */
        template <IInputIterator<typename TRAITS::CountedValueType> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
        nonvirtual void AddAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end);
        template <IIterableOf<typename TRAITS::CountedValueType> ITERABLE_OF_ADDABLE>
        nonvirtual void AddAll (ITERABLE_OF_ADDABLE&& items);

    public:
        /**
         *  \brief remove the argument data from the multiset. The data specified MUST exist (require) - else use RemoveIf ()
         * 
         *  \see RemoveIf
         * 
         *  \req count >= 1
         *
         * This function requires that the iterator 'i' came from this container.
         *
         * The value pointed to by 'i' is removed.
         *
         *  If using the item/count or just item overloads, then MultiSet<> requires that the removed items are present.
         *
         *  \param nextI - if provided (not null) - will be filled in with the next value after where iterator i is pointing - since i is invalidated by changing the container)
         *
         *  \note mutates container
         */
        nonvirtual void Remove (ArgByValueType<T> item, CounterType count = 1);
        nonvirtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI = nullptr);

    public:
        /**
         *  \brief remove the argument data from the multiset (can specify remove of more than are present) - returns number actually removed (multisets never have count < 0)
         * 
         *  \req count >= 1
         * 
         *  \note mutates container
         */
        nonvirtual size_t RemoveIf (ArgByValueType<T> item, CounterType count = 1);

    public:
        /**
         *  \brief RemoveAll removes all, or all matching (predicate, iterator range, equals comparer or whatever) items.
         * 
         *  The no-arg overload removes all (quickly).
         * 
         *  The overloads that remove some subset of the items returns the number of items so removed.
         * 
         *  \note mutates container
         */
        nonvirtual void   RemoveAll ();
        nonvirtual size_t RemoveAll (ArgByValueType<T> item);

    public:
        /**
         * if newCount == 0, equivalent to Remove(i). Require not i.Done () - so it must point to a given item.
         *
         *  On return, nextI, if non-null, will point to the next element after the argument 'i' (useful for iteration).
         * 
         *  \note mutates container
         */
        nonvirtual void UpdateCount (const Iterator<value_type>& i, CounterType newCount, Iterator<value_type>* nextI = nullptr);

    public:
        /**
         *  It's perfectly legal for i to be missing before or after.
         *
         *  \note mutates container
         */
        nonvirtual void SetCount (ArgByValueType<T> i, CounterType newCount);

    public:
        /**
         *  OccurrencesOf() returns the number of occurrences of 'item' in the tally. The items are compared with operator==.
         *
         *  If there are no copies of item in the MultiSet, 0 is returned.
         */
        nonvirtual CounterType OccurrencesOf (ArgByValueType<T> item) const;

    public:
        /**
         *  Returns the sum of all this MultiSets contained elements. This is equivalent
         *  to Elements ().size ().
         */
        nonvirtual CounterType TotalOccurrences () const;

    public:
        /**
         * \brief STL-ish alias for Remove ().
         */
        nonvirtual Iterator<value_type> erase (const Iterator<value_type>& i);

    public:
        /**
         * \brief STL-ish alias for RemoveAll ().
         *
         *  \note mutates container
         */
        nonvirtual void clear ();

    public:
        /**
         * This is like the MultiSet was a Collection<T> (or plain Iterable<T>). If something is in there N times,
         *  it will show up in iteration N times. No guarantee is made as to order of iteration.
         *
         *  \par Example Usage
         *      \code
         *          MultiSet<T> t;
         *          for (T i : t.Elements ()) {
         *              // Like a collection iteration - if item t.OccurrencesOf(i) == 3, then this will be encountered 3 times in the iteration
         *          }
         *      \endcode
         *
         *  Elements () makes no guarantees about whether or not modifications to the underlying MultiSet<> will
         *  appear in the Elements() Iterable<T>.
         *
         *  @see UniqueElements
         */
        nonvirtual Iterable<T> Elements () const;

    public:
        /**
         *  \par Example Usage
         *      \code
         *          MultiSet<T> t;
         *          for (T i : t.UniqueElements ()) {
         *              // Like a set iteration - if item t.OccurrencesOf(i) == 3, then this will be encountered 1 times in the iteration
         *          }
         *      \endcode
         *
         *  UniqueElements () operates as if it copies the data from the original container, and will not reflect any subsequent changes.
         */
        nonvirtual Iterable<T> UniqueElements () const;

    public:
        /**
         *  \brief Find the most commonly occurring elements of the multiset (list with count ordered most to last)
         * 
         *  \par Example Usage
         *      \code
         *          MultiSet<int> test{1, 1, 5, 1, 6, 5};
         *          EXPECT_TRUE (test.Top ().SequentialEquals ({{1, 3}, {5, 2}, {6, 1}}));
         *          EXPECT_TRUE (test.Top (1).SequentialEquals ({{1, 3}}));
         *      \endcode
         * 
         *  \note n is allowed to exceed the size of the MultiSet, and the result of Top (n) maybe fewer than n values
         *
         *  \See Iterable<T>::Top
         *  \See TopElements
         */
        nonvirtual Iterable<typename TRAITS::CountedValueType> Top () const;
        nonvirtual Iterable<typename TRAITS::CountedValueType> Top (size_t n) const;

    public:
        /**
         *  \brief Find the most commonly occurring elements of the multiset (list of elements - without count - ordered most to last)
         * 
         *  \par Example Usage
         *      \code
         *          MultiSet<int> test{1, 1, 5, 1, 6, 5};
         *          EXPECT_TRUE (test.TopElements ().SequentialEquals ({1, 5, 6}));
         *          EXPECT_TRUE (test.TopElements (1).SequentialEquals ({1}));
         *      \endcode
         * 
         *  \note n is allowed to exceed the size of the MultiSet, and the result of Top (n) maybe fewer than n values
         *
         *  \See Iterable<T>::Top
         *  \See Top
         */
        nonvirtual Iterable<T> TopElements () const;
        nonvirtual Iterable<T> TopElements (size_t n) const;

    public:
        /**
         * \brief 'override' Iterable<>::Map () function so RESULT_CONTAINER defaults to MultiSet, and improve that case to clone properties from this rep (such is rep type, ordering, etc).
         */
        template <typename RESULT_CONTAINER = MultiSet<T, TRAITS>, invocable<T> ELEMENT_MAPPER>
        nonvirtual RESULT_CONTAINER Map (ELEMENT_MAPPER&& elementMapper) const
            requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, typename TRAITS::CountedValueType>, typename RESULT_CONTAINER::value_type> or
                      convertible_to<invoke_result_t<ELEMENT_MAPPER, typename TRAITS::CountedValueType>, optional<typename RESULT_CONTAINER::value_type>>);

    public:
        /**
         *  See Iterable<T>::Where - except defaults to MultiSet, and handles cloning rep properties for that special case
         */
        template <derived_from<Iterable<typename TRAITS::CountedValueType>> RESULT_CONTAINER = MultiSet<T, TRAITS>, predicate<typename TRAITS::CountedValueType> INCLUDE_PREDICATE>
        nonvirtual RESULT_CONTAINER Where (INCLUDE_PREDICATE&& includeIfTrue) const;

    public:
        /**
         *  Return the function used to compare if two elements are equal (not to be confused with MultiSet<>::EqualsComparer)
         */
        nonvirtual ElementEqualityComparerType GetElementEqualsComparer () const;

    public:
        /**
         * @see comparisons section of @MultiSet documentation
         */
        nonvirtual bool operator== (const MultiSet& rhs) const;

    public:
        /**
         *  Synonym for Add (), or AddAll() (depending on argument);
         *
         *  \note mutates container
         */
        nonvirtual MultiSet& operator+= (ArgByValueType<T> item);
        nonvirtual MultiSet& operator+= (const MultiSet& items);

    protected:
        /**
         *  \brief Utility to get WRITABLE underlying shared_ptr (replacement for what we normally do - _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ())
         *         but where we also handle the cloning/patching of the associated iterator
         * 
         *  When you have a non-const operation (such as Remove) with an argument of an Iterator<>, then due to COW,
         *  you may end up cloning the container rep, and yet the Iterator<> contains a pointer to the earlier rep (and so maybe unusable).
         * 
         *  Prior to Stroika 2.1b14, this was handled elegantly, and automatically, by the iterator patching mechanism. But that was deprecated (due to cost, and
         *  rarity of use), in favor of this more restricted feature, where we just patch the iterators on an as-needed basis.
         * 
         *  \todo @todo - could be smarter about moves and avoid some copies here - I think, and this maybe performance sensitive enough to look into that... (esp for COMMON case where no COW needed)
         */
        nonvirtual tuple<_IRep*, Iterator<value_type>> _GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i);

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
     */
    template <typename T, typename TRAITS>
    class MultiSet<T, TRAITS>::_IRep : public Iterable<typename TRAITS::CountedValueType>::_IRep {
    private:
        using inherited = typename Iterable<typename TRAITS::CountedValueType>::_IRep;

    public:
        using CounterType = typename MultiSet::CounterType;

    protected:
        _IRep () = default;

    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const                                              = 0;
        virtual shared_ptr<_IRep>           CloneEmpty () const                                                            = 0;
        virtual shared_ptr<_IRep>           CloneAndPatchIterator (Iterator<value_type>* i) const                          = 0;
        virtual bool                        Equals (const _IRep& rhs) const                                                = 0;
        virtual bool                        Contains (ArgByValueType<T> item) const                                        = 0;
        virtual void                        Add (ArgByValueType<T> item, CounterType count)                                = 0;
        virtual size_t                      RemoveIf (ArgByValueType<T> item, CounterType count)                           = 0;
        virtual void                        Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)            = 0;
        virtual void        UpdateCount (const Iterator<value_type>& i, CounterType newCount, Iterator<value_type>* nextI) = 0;
        virtual CounterType OccurrencesOf (ArgByValueType<T> item) const                                                   = 0;

        /*
     *  Reference Implementations (often not used except for ensure's, but can be used for
     *  quickie backends).
     *
     *  Importantly, these are all non-virtual so not actually pulled in or even compiled unless
     *  the sucblass refers to the method in a subclass virtual override.
     */
    protected:
        nonvirtual bool _Equals_Reference_Implementation (const _IRep& rhs) const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MultiSet.inl"

#endif /*_Stroika_Foundation_Containers_MultiSet_h_ */
