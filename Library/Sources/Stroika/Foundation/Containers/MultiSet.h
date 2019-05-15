/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_MultiSet_h_
#define _Stroika_Foundation_Containers_MultiSet_h_ 1

#include "../StroikaPreComp.h"

#include <set>

#include "../Common/Compare.h"
#include "../Common/CountedValue.h"
#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Execution/Synchronized.h"
#include "../Memory/SharedByValue.h"
#include "../Traversal/Iterable.h"
#include "Common.h"
#include "DefaultTraits/MultiSet.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-428 - See how to map EqualsComparer to CountedValue we use
 *              probably added template param to CountedValue - TRAITS.
 *
 *              MAYBE add    using   MultisetEntryType                       =       CountedValue<T>; to
 *              TRAITS object. Then use throughout.... Not sure this is worth parameterizing, but
 *              COULD help address unifying the EQUALS support!
 *
 *      @todo   Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *              concepts and that it USES that for the appropriate overloaded constructors.
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
 *      @todo   Fix MultiSet<> CTOR overload taking pointers (sb iterators) - overload so both cases -
 *              Iterator<T> and Iterator<CountedValue<T>>. Ise enableOf(isconvertible....)
 *
 *      @todo   Consider rewriting all MultiSet<> concrete types using Mapping<T,counttype> concrete impl?
 *              Might not work easily but document why... (Add () semantics - but maybe).
 *
 *      @todo   AddAll() and CTOR for MultiSet (and SortedMultiSet and concrete types) is confused by having
 *              overload taking T* and CountedValue<T>*. Issue is that we cannot do templated iterator
 *              and templated objhect CTOR while these are iteratored (without mcuh better partial
 *              template specializaiton - I THINK????). Maybe use different method for one or the other
 *              to distinguish?
 *
 *              USE SFINAE stuff we used in Mapping<> etc. Simplify AddAll and do the magic in Add.
 *
 *      @todo   Current DOCS for MultiSet::Remove() say that for variant T, count, the value MUST be present.
 *              But I think this is not in the spirit we've used elsewhere, due to multithreading.
 *              Better to allow them to not be present, else hard to synchonize (check and remove)
 *
 */

namespace Stroika::Foundation::Containers {

    using Common::CountedValue;
    using Configuration::ArgByValueType;
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
     *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
     *
     *  \note Note About Iterators
     *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
     *
     *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
     *          the iterators are automatically updated internally to behave sensibly.
     *
     */
    template <typename T, typename TRAITS = DefaultTraits::MultiSet<T>>
    class MultiSet : public Iterable<CountedValue<T>> {
    private:
        using inherited = Iterable<CountedValue<T>>;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = MultiSet<T, TRAITS>;

    public:
        /**
         *  Just a short-hand for the 'TRAITS' part of MultiSet<T,TRAITS>. This is often handy to use in
         *  building other templates.
         */
        using TraitsType = TRAITS;

    public:
        /**
         *      \brief  MultiSetOfElementType is just a handly copy of the *T* template type which this
         *              MultiSet<T, TRAITS> parameterizes counting.
         */
        using MultiSetOfElementType = T;

    public:
        /**
         *      \brief
         */
        using CounterType = typename TraitsType::CounterType;

    protected:
        class _IRep;

    protected:
        using _MultiSetRepSharedPtr = typename inherited::template PtrImplementationTemplate<_IRep>;

    public:
        /**
         *  This is the type returned by GetEqualsComparer () and CAN be used as the argument to a MultiSet<> as EqualityComparer, but
         *  we allow any template in the Set<> CTOR for an equalityComparer that follows the Common::IsEqualsComparer () concept (need better name).
         */
        using EqualityComparerType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool (T, T)>>;

    public:
        /**
         *  All constructors either copy their source comparer (copy/move CTOR), or use the provided argument comparer
         *  (which in turn defaults to equal_to<T>).
         *
         *  \note For efficiency sake, the base constructor takes a templated EQUALS_COMPARER (avoiding translation to function<bool(T,T)>>, but
         *        for simplicity sake, many of the other constructors force that conversion.
         *
         * \req IsEqualsComparer<EQUALS_COMPARER> () - for constructors with that type parameter
         *
         *  \par Example Usage
         *      \code
         *          Collection<int> c;
         *          std::vector<int> v;
         *
         *          MultiSet<int> s1  = {1, 2, 3};
         *          MultiSet<int> s2  = s1;
         *          MultiSet<int> s3  { s1 };
         *          MultiSet<int> s4  { s1.begin (), s1.end () };
         *          MultiSet<int> s5  { c };
         *          MultiSet<int> s6  { v };
         *          MultiSet<int> s7  { v.begin (), v.end () };
         *          MultiSet<int> s8  { move (s1) };
         *          MultiSet<int> s9  { Common::mkEqualsComparer([](int l, int r) { return l == r; }), c};
         *      \endcode
         */
        MultiSet ();
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>* = nullptr>
        explicit MultiSet (EQUALS_COMPARER&& equalsComparer);
        MultiSet (const MultiSet& src) noexcept = default;
        MultiSet (MultiSet&& src) noexcept      = default;
        MultiSet (const initializer_list<T>& src);
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>* = nullptr>
        MultiSet (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src);
        MultiSet (const initializer_list<CountedValue<T>>& src);
        template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>* = nullptr>
        MultiSet (EQUALS_COMPARER&& equalsComparer, const initializer_list<CountedValue<T>>& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<MultiSet<T, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        explicit MultiSet (CONTAINER_OF_ADDABLE&& src);
        template <typename EQUALS_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<MultiSet<T, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        MultiSet (EQUALS_COMPARER&& equalsComparer, const CONTAINER_OF_ADDABLE& src);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>* = nullptr>
        MultiSet (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename EQUALS_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>* = nullptr>
        MultiSet (EQUALS_COMPARER&& equalsComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

    protected:
        explicit MultiSet (const _MultiSetRepSharedPtr& rep) noexcept;
        explicit MultiSet (_MultiSetRepSharedPtr&& rep) noexcept;

#if qDebug
    public:
        ~MultiSet ();
#endif

    public:
        nonvirtual MultiSet& operator= (const MultiSet& rhs) = default;
        nonvirtual MultiSet& operator= (MultiSet&& rhs) = default;

    public:
        /**
         *  Contains (item) is equivalent to OccurrencesOf (item) != 0, but maybe faster (since it doesn't need to compute
         *  the fully tally).
         */
        nonvirtual bool Contains (ArgByValueType<T> item) const;

    public:
        /**
         */
        nonvirtual void Add (ArgByValueType<T> item);
        nonvirtual void Add (ArgByValueType<T> item, CounterType count);
        nonvirtual void Add (const CountedValue<T>& item);

    public:
        /**
         *  \note   AddAll/2 is alias for .net AddRange ()
         *          and AddAll/2 - the iterator can be Iterator<T> or Iterator<CountedValue<T>>
         */
        template <typename COPY_FROM_ITERATOR>
        nonvirtual void AddAll (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE>>* = nullptr>
        nonvirtual void AddAll (CONTAINER_OF_ADDABLE&& src);

    public:
        /**
         * This function requires that the iterator 'i' came from this container.
         *
         * The value pointed to by 'i' is removed.
         *
         *  If using the item/count or just item overloads, then MultiSet<> requires that the removed items are present.
         */
        nonvirtual void Remove (ArgByValueType<T> item);
        nonvirtual void Remove (ArgByValueType<T> item, CounterType count);
        nonvirtual void Remove (const Iterator<CountedValue<T>>& i);

    public:
        /**
         */
        nonvirtual void RemoveAll ();
        nonvirtual void RemoveAll (ArgByValueType<T> item);

    public:
        /**
         * if newCount == 0, equivalent to Remove(i). Require not i.Done () - so it must point to a given item.
         */
        nonvirtual void UpdateCount (const Iterator<CountedValue<T>>& i, CounterType newCount);

    public:
        /**
         *  It's perfectly legal for i to be missing before or after.
         */
        nonvirtual void SetCount (ArgByValueType<T> i, CounterType newCount);

    public:
        /**
         *  OccurrencesOf() returns the number of occurences of 'item' in the tally. The items are compared with operator==.
         *
         *  If there are no copies of item in the MultiSet, 0 is returned.
         */
        nonvirtual CounterType OccurrencesOf (ArgByValueType<T> item) const;

    public:
        /**
         *  Returns the sum of all tallys of all contained elements. This is equivalent
         *  to Elements ().size ().
         */
        nonvirtual CounterType TotalOccurrences () const;

    public:
        /**
         * \brief STL-ish alias for RemoveAll ().
         */
        nonvirtual void clear ();

    public:
        /**
         * This is like the MultiSet was a Collection<T> (or plain Iterable<T>). If something is in there N times,
         *  it will show up in iteration N times. No guarnatee is made as to order of iteration.
         *
         *  \par Example Usage
         *      \code
         *          MultiSet<T> t;
         *          for (T i : t.Elements ()) {
         *          }
         *      \endcode
         *
         *  Elements () makes no guarantess about whether or not modifications to the underlying MultiSet<> will
         *  appear in the Elements() Iterable<T>.
         *
         *  @see UniqueElements
         */
        nonvirtual Iterable<T> Elements () const;

    public:
        /**
         *  \par Example Usage
         *      \code
         *      MultiSet<T> t;
         *      for (T i : t.UniqueElements ()) {
         *      }
         *      \endcode
         *
         *  UniqueElements () makes no guarantess about whether or not modifications to the underlying MultiSet<>
         *  will appear in the UniqueElements() Iterable<T> (so no guarantee if live copy or when copy made).
         */
        nonvirtual Iterable<T> UniqueElements () const;

    public:
        /**
         *  Return the function used to compare if two elements are equal
         */
        nonvirtual EqualityComparerType GetEqualsComparer () const;

    public:
        struct EqualsComparer;

    public:
        /*
         */
        [[deprecated ("in Stroika v2.1d24 - use EqualsComparer{} () or == instead")]] bool Equals (const MultiSet& rhs) const;

    public:
        /**
         *  Synonym for Add (), or AddAll() (depending on argument);
         */
        nonvirtual MultiSet& operator+= (ArgByValueType<T> item);
        nonvirtual MultiSet& operator+= (const MultiSet& t);

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

    using Traversal::IteratorOwnerID;

    /**
     */
    template <typename T, typename TRAITS>
    class MultiSet<T, TRAITS>::_IRep
        : public Iterable<CountedValue<T>>::_IRep
#if !qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        ,
          public Traversal::IterableBase::enable_shared_from_this_PtrImplementationTemplate<typename MultiSet<T, TRAITS>::_IRep>
#endif
    {
    private:
        using inherited = typename Iterable<CountedValue<T>>::_IRep;

#if qCompilerAndStdLib_TemplateTypenameReferenceToBaseOfBaseClassMemberNotFound_Buggy
    protected:
        using _APPLY_ARGTYPE      = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
#endif

    protected:
        using _MultiSetRepSharedPtr = typename MultiSet::_MultiSetRepSharedPtr;

    public:
        using CounterType = typename MultiSet::CounterType;

    protected:
        _IRep () = default;

    public:
        virtual EqualityComparerType  GetEqualsComparer () const                                             = 0;
        virtual _MultiSetRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const                 = 0;
        virtual bool                  Equals (const _IRep& rhs) const                                        = 0;
        virtual bool                  Contains (ArgByValueType<T> item) const                                = 0;
        virtual void                  Add (ArgByValueType<T> item, CounterType count)                        = 0;
        virtual void                  Remove (ArgByValueType<T> item, CounterType count)                     = 0;
        virtual void                  Remove (const Iterator<CountedValue<T>>& i)                            = 0;
        virtual void                  UpdateCount (const Iterator<CountedValue<T>>& i, CounterType newCount) = 0;
        virtual CounterType           OccurrencesOf (ArgByValueType<T> item) const                           = 0;
        // Subtle point - shared rep argument to Elements() allows shared ref counting
        // without the cost of a clone or enable_shared_from_this
        virtual Iterable<T> Elements (const _MultiSetRepSharedPtr& rep) const = 0;
        // Subtle point - shared rep argument to Elements() allows shared ref counting
        // without the cost of a clone or enable_shared_from_this
        virtual Iterable<T> UniqueElements (const _MultiSetRepSharedPtr& rep) const = 0;
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const = 0;
#endif

        /*
     *  Reference Implementations (often not used except for ensure's, but can be used for
     *  quickie backends).
     *
     *  Importantly, these are all non-virtual so not actually pulled in or even compiled unless
     *  the sucblass refers to the method in a subclass virtual override.
     */
    protected:
        nonvirtual bool _Equals_Reference_Implementation (const _IRep& rhs) const;
        nonvirtual Iterable<T> _Elements_Reference_Implementation (const _MultiSetRepSharedPtr& rep) const;
        nonvirtual Iterable<T> _UniqueElements_Reference_Implementation (const _MultiSetRepSharedPtr& rep) const;

    private:
        struct ElementsIteratorHelperContext_;
        struct ElementsIteratorHelper_;

    protected:
        struct _ElementsIterableHelper;

    private:
        struct UniqueElementsIteratorHelperContext_;
        struct UniqueElementsIteratorHelper_;

    protected:
        struct _UniqueElementsHelper;
    };

    /**
     *  \brief Compare MultiSet<>s for equality. 
     *
     *  Two MultiSet are considered equal if they contain the same elements (by comparing them with operator==)
     *  with the same count. In short, they are equal if OccurrencesOf() each item in the LHS equals the OccurrencesOf()
     *  the same item in the RHS.
     *
     *  Equals is commutative().
     *
     *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
     *
     *  @todo - document computational complexity
     *
     *  \note   Not to be confused with EqualityComparerType and GetEqualsComparer () which compares ELEMENTS of MultiSet<T> for equality.
     */
    template <typename T, typename TRAITS>
    struct MultiSet<T, TRAITS>::EqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
        nonvirtual bool operator() (const MultiSet& lhs, const MultiSet& rhs) const;
    };

    /**
     *  Basic comparison operator overloads with the obivous meaning, and simply indirect to @MultiSet<>::EqualsComparer
     */
    template <typename T, typename TRAITS>
    bool operator== (const MultiSet<T, TRAITS>& lhs, const MultiSet<T, TRAITS>& rhs);
    template <typename T, typename TRAITS>
    bool operator!= (const MultiSet<T, TRAITS>& lhs, const MultiSet<T, TRAITS>& rhs);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MultiSet.inl"

#endif /*_Stroika_Foundation_Containers_MultiSet_h_ */
