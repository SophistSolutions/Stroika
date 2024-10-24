/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bijection_h_
#define _Stroika_Foundation_Containers_Bijection_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <utility>

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Common/KeyValuePair.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/DataExchange/ValidationStrategy.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

/*
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *
 *      @todo   Best backend I can think of would be two opposing maps (or hash tables). Discuss with
 *              Sterl to see if he can think of any way todo this that doesn't double the storage
 *              of a regular Mapping (without exhaustive search on lookup from range).
 *
 *              But currently just a functional, proof-of-concept linked list implementation.
 *
 *      @todo   Consider adding templated for RemoveAllFromPreimage/RemoveAllFromImage
 *              taking containers or iterator ranges.
 */

namespace Stroika::Foundation::Containers {

    using Common::IEqualsComparer;
    using Common::KeyValuePair;
    using Configuration::ArgByValueType;
    using Traversal::IInputIterator;
    using Traversal::IIterableOf;
    using Traversal::Iterable;
    using Traversal::Iterator;

    class Bijection_Base {
    public:
        class InjectivityViolation : public Execution::RuntimeErrorException<> {
        private:
            using inherited = Execution::RuntimeErrorException<>;

        public:
            InjectivityViolation ();
        };
    };

    /**
     * \brief   Bijection allows for the bijective (1-1) association of two elements.
     *
     *  Bijection allows for the bijective (1-1) association of two elements. This means that one element
     *  of the domain maps to exactly one element of the range, and that one element of the range maps uniquely to
     *  one element of the domain, and these mappings happen in a way that the mapping is fully invertible.
     *
     *  @see    http://en.wikipedia.org/wiki/Bijection
     *
     *  Design Notes:
     *      \note   We used Iterable<pair<DOMAIN_TYPE,RANGE_TYPE>> instead of
     *              Iterable<KeyValuePairType<DOMAIN_TYPE,RANGE_TYPE>> because its completely symmetric - both
     *              directions the values are keys.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   Bijection could not inherit from Mapping<> because the semantics of Removing a value are incompatible,
     *          and because (depending on the policy we adopt about error checking for invalid value in add) - we can have legit
     *          behavior on a Mapping BE illegal on the Bijection<>
     *
     *  \em Concrete Implementations:
     *      o   @see Concrete::Bijection_LinkedList<>
     *
     *  \em Factory:
     *      @see Bijection_Factory<> to see default implementations.
     *
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note <a href="ReadMe.md#Container Element comparisons">Container Element comparisons</a>:
     *        See about ElementInOrderComparerType, ElementThreeWayComparerType and GetElementThreeWayComparer etc
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   static_assert (equality_comparable<Bijection<...>>);
     * 
     *      o   All Bijections MUST have = comparable DOMAIN_TYPE and RANGE_TYPE, so operator== for the bijection is well-defined.
     *
     *          Two Bijections are considered equal if they contain the same elements (Preimage) and each key is associated
     *          with the same value. There is no need for the items to appear in the same order for the two Bijections to
     *          be equal. There is no need for the backends to be of the same underlying representation either (STL map
     *          vers linked-list).
     *
     *          Since a Bijection is not necessarily sorted, or in any particular order, < and > are not well defined.
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    class [[nodiscard]] Bijection : public Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>, public Bijection_Base {
    private:
        using inherited = Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>;

    protected:
        class _IRep;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = Bijection;

    public:
        /**
         *  @see inherited::value_type
         */
        using value_type = typename inherited::value_type;

    public:
        /**
         */
        using DomainType = DOMAIN_TYPE;

    public:
        /**
         */
        using RangeType = RANGE_TYPE;

    public:
        /**
         *  This is the type returned by GetDomainEqualsComparer () and CAN be used as the argument to a Bijection<> as EqualityComparer, but
         *  we allow any template in the Bijection<> CTOR for an equalityComparer that follows the Common::IEqualsComparer concept.
         */
        using DomainEqualsCompareFunctionType =
            Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool (ArgByValueType<DomainType>, ArgByValueType<DomainType>)>>;
        static_assert (IEqualsComparer<DomainEqualsCompareFunctionType, DomainType>);

    public:
        /**
         *  This is the type returned by GetRangeEqualsComparer () and CAN be used as the argument to a Bijection<> as EqualityComparer, but
         *  we allow any template in the Bijection<> CTOR for an equalityComparer that follows the IEqualsComparer concept
         */
        using RangeEqualsCompareFunctionType =
            Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool (RangeType, RangeType)>>;
        static_assert (IEqualsComparer<RangeEqualsCompareFunctionType, RangeType>);

    public:
        /**
         *  This constructor creates a concrete Bijection object, either empty, or initialized with any argument
         *  values.
         *
         *  The underlying data structure of the Bijection is defined by @see Factory::Bijection_Factory<>
         * 
         *  \note The constructor arguments DOMAIN_EQUALS_COMPARER or RANGE_EQUALS_COMPARER must be declared to be equals_comparers, to avoid
         *        ambiguity/accidental mix-ups between inorder and equals (or three way) comparers. Consider wrapping lambdas with Common::DeclareEqualsComparer
         *
         *  \note <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        Bijection ()
            requires (IEqualsComparer<equal_to<DOMAIN_TYPE>, DOMAIN_TYPE> and IEqualsComparer<equal_to<RANGE_TYPE>, RANGE_TYPE>);
        template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
        explicit Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer);
        template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
        explicit Bijection (DataExchange::ValidationStrategy injectivityCheckPolicy, DOMAIN_EQUALS_COMPARER&& domainEqualsComparer,
                            RANGE_EQUALS_COMPARER&& rangeEqualsComparer);
        Bijection (Bijection&&) noexcept      = default;
        Bijection (const Bijection&) noexcept = default;
        Bijection (const initializer_list<value_type>& src)
            requires (IEqualsComparer<equal_to<DOMAIN_TYPE>, DOMAIN_TYPE> and IEqualsComparer<equal_to<RANGE_TYPE>, RANGE_TYPE>);
        template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
        Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer, const initializer_list<value_type>& src);
        template <IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERABLE_OF_ADDABLE>
        explicit Bijection (ITERABLE_OF_ADDABLE&& src)
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Bijection<DOMAIN_TYPE, RANGE_TYPE>>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Bijection{}
        {
            AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            _AssertRepValidType ();
        }
#endif
        ;
        template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER, IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERABLE_OF_ADDABLE>
        Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
        Bijection (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE2&& end)
            requires (IEqualsComparer<equal_to<DOMAIN_TYPE>, DOMAIN_TYPE> and IEqualsComparer<equal_to<RANGE_TYPE>, RANGE_TYPE>);
        template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER,
                  IInputIterator<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERATOR_OF_ADDABLE, sentinel_for<remove_cvref_t<ITERATOR_OF_ADDABLE>> ITERATOR_OF_ADDABLE2>
        Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer, ITERATOR_OF_ADDABLE&& start,
                   ITERATOR_OF_ADDABLE2&& end);

    protected:
        explicit Bijection (shared_ptr<_IRep>&& src) noexcept;
        explicit Bijection (const shared_ptr<_IRep>& src) noexcept;

    public:
        /**
         */
        nonvirtual Bijection& operator= (Bijection&&) noexcept = default;
        nonvirtual Bijection& operator= (const Bijection&)     = default;

    public:
        /**
         */
        nonvirtual DomainEqualsCompareFunctionType GetDomainEqualsComparer () const;

    public:
        /**
         */
        nonvirtual RangeEqualsCompareFunctionType GetRangeEqualsComparer () const;

    public:
        /**
         *  Preimage () returns an Iterable object with just the domain (first) part of the Bijection.
         *
         *  Note this method may not return a collection which is sorted. Note also, the
         *  returned value is a copy of the keys (by value) - at least logically (implementations
         *  maybe smart enough to use lazy copying).
         *
         *  Note the returned Iterable is detached from the original, and doesn't see any changes
         *  to it, and its lifetime is like a copy of a shared_ptr - lasts as long as the
         *  reference.
         * 
         *  \em Design Note:
         *      The analagous method in C#.net - Dictionary<TKey, TValue>.KeyCollection
         *      (http://msdn.microsoft.com/en-us/library/yt2fy5zk(v=vs.110).aspx) returns a live reference
         *      to the underlying keys. We could have (fairly easily) done that, but I didn't see the point.
         *
         *      In .net, the typical model is that you have a pointer to an object, and pass around that
         *      pointer (so by reference semantics) - so this returning a live reference makes more sense there.
         *
         *      Since Stroika containers are logically copy-by-value (even though lazy-copied), it made more
         *      sense to apply that lazy-copy (copy-on-write) paradigm here, and make the returned set of
         *      keys a logical copy at the point 'keys' is called.
         */
        nonvirtual Iterable<DomainType> Preimage () const;

    public:
        /**
         *  Image () returns an Iterable object with just the range part of the Bijection.
         *
         *  Note this method may not return a collection which is sorted. Note also, the
         *  returned value is a copy of the rangle (by value) - at least logically (implementations
         *  maybe smart enough to use lazy copying).
         *
         *  Note the returned Iterable is detached from the original, and doesn't see any changes
         *  to it, and its lifetime is like a copy of a shared_ptr - lasts as long as the
         *  reference.
         *
         *  \em Design Note:
         *      The analagous method in C#.net - Dictionary<TKey, TValue>.KeyCollection
         *      (https://msdn.microsoft.com/en-us/library/ekcfxy3x(v=vs.110).aspx) returns a live reference
         *      to the underlying values. We could have (fairly easily) done that, but I didn't see the point.
         *
         *      In .net, the typical model is that you have a pointer to an object, and pass around that
         *      pointer (so by reference semantics) - so this returning a live reference makes more sense there.
         *
         *      Since Stroika containers are logically copy-by-value (even though lazy-copied), it made more
         *      sense to apply that lazy-copy (copy-on-write) paradigm here, and make the returned set of
         *      keys a logical copy at the point 'keys' is called.
         */
        nonvirtual Iterable<RangeType> Image () const;

    public:
        /**
         *  Note - as since Lookup/1 returns an optional<T> - it can be used very easily to provide
         *  a default value on Lookup (so for case where not present) - as in:
         *      returm m.Lookup (key).Value (putDefaultValueHere);
         *
         *  Note - for both overloads taking an item pointer, the pointer may be nullptr (in which case not assigned to).
         *  But if present, will always be assigned to if Lookup returns true (found). And for the optional overload
         *      \req    Ensure (item == nullptr or returnValue == item->has_value());
         *
         *  \note   Alias - Lookup (key, RangeType* value) - is equivilent to .Net TryGetValue ()
         *
         *  @see   LookupValue ()
         *  @see   InverseLookup ()
         *  @see   InverseLookupValue ()
         */
        nonvirtual optional<RangeType> Lookup (ArgByValueType<DomainType> key) const;
        nonvirtual bool                Lookup (ArgByValueType<DomainType> key, optional<RangeType>* item) const;
        nonvirtual bool                Lookup (ArgByValueType<DomainType> key, RangeType* item) const;
        nonvirtual bool                Lookup (ArgByValueType<DomainType> key, nullptr_t) const;

    public:
        /**
         *  Always safe to call. If result empty/missing, returns argument 'default' or 'sentinel' value.
         *
         *  @see   Lookup ()
         *  @see   InverseLookup ()
         *  @see   InverseLookupValue ()
         */
        nonvirtual RangeType LookupValue (ArgByValueType<DomainType> key, ArgByValueType<RangeType> defaultValue = RangeType ()) const;

    public:
        /**
         *  Note - as since InverseLookup/1 returns an optional<T> - it can be used very easily to provide
         *  a default value on Lookup (so for case where not present) - as in:
         *      returm m.InverseLookup (key).Value (putDefaultValueHere);
         *
         *  Note - for both overloads taking an item pointer, the pointer may be nullptr (in which case not assigned to).
         *  But if present, will always be assigned to if Lookup returns true (found). And for the optional overload
         *      \req    Ensure (item == nullptr or returnValue == item->has_value());
         *
         *  @see   Lookup ()
         *  @see   LookupValue ()
         *  @see   InverseLookupValue ()
         */
        nonvirtual optional<DomainType> InverseLookup (ArgByValueType<RangeType> key) const;
        nonvirtual bool                 InverseLookup (ArgByValueType<RangeType> key, optional<DomainType>* item) const;
        nonvirtual bool                 InverseLookup (ArgByValueType<RangeType> key, DomainType* item) const;
        nonvirtual bool                 InverseLookup (ArgByValueType<RangeType> key, nullptr_t) const;

    public:
        /**
         *  Always safe to call. If result empty/missing, returns argument 'default' or 'sentinel' value.
         *
         *  @see   Lookup ()
         *  @see   LookupValue ()
         *  @see   InverseLookup ()
         */
        nonvirtual DomainType InverseLookupValue (ArgByValueType<RangeType> key, ArgByValueType<DomainType> defaultValue = DomainType ()) const;

    public:
        /**
         * For each value in the source set, map it back using the bijection to the target set.
         *  \req that each element be present in the bijection
         */
        nonvirtual Iterable<RangeType> MapToRange (const Iterable<DomainType>& values) const;

    public:
        /**
         * For each value in the source set, map it back using the bijection to the target set.
         *  \req that each element be present in the bijection
         */
        nonvirtual Iterable<DomainType> MapToDomain (const Iterable<RangeType>& values) const;

    public:
        /**
         * \brief 'override' Iterable<>::Map () function so RESULT_CONTAINER defaults to Bijection, and improve that case to clone properties from this rep (such is rep type, etc).
         */
        template <typename RESULT_CONTAINER = Bijection<DOMAIN_TYPE, RANGE_TYPE>, invocable<pair<DOMAIN_TYPE, RANGE_TYPE>> ELEMENT_MAPPER>
        nonvirtual RESULT_CONTAINER Map (ELEMENT_MAPPER&& elementMapper) const
            requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, pair<DOMAIN_TYPE, RANGE_TYPE>>, typename RESULT_CONTAINER::value_type> or
                      convertible_to<invoke_result_t<ELEMENT_MAPPER, pair<DOMAIN_TYPE, RANGE_TYPE>>, optional<typename RESULT_CONTAINER::value_type>>);

    public:
        /**
         * \brief Like Iterable<T>::Where, but returning a bijection - subset of this bijection where includeIfTrue is true
         */
        template <derived_from<Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>> RESULT_CONTAINER = Bijection<DOMAIN_TYPE, RANGE_TYPE>, predicate<pair<DOMAIN_TYPE, RANGE_TYPE>> INCLUDE_PREDICATE>
        nonvirtual RESULT_CONTAINER Where (INCLUDE_PREDICATE&& includeIfTrue) const;

    public:
        /**
         *  essentially 'intersect but just comparing domain'
         */
        nonvirtual Bijection WhereDomainIntersects (const Iterable<DomainType>& domainValues) const;

    public:
        /**
         *  essentially 'intersect but just comparing range'
         */
        nonvirtual Bijection WhereRangeIntersects (const Iterable<RangeType>& rangeValues) const;

    public:
        /**
         *  Synonym for (Lookup (v).has_value ()) or Preimage ().Contains (v)
         */
        nonvirtual bool ContainsDomainElement (ArgByValueType<DomainType> key) const;

    public:
        /**
         *  Synonym for (InverseLookup (v).has_value ()) or Image ().Contains (v)
         */
        nonvirtual bool ContainsRangeElement (ArgByValueType<RangeType> v) const;

    public:
        /**
         *  Add the association between key and newElt. If key was already associated with something
         *  else, the association is silently updated, and the size of the iterable does not change.
         *  Also - we guarantee that even if the association is different, if the key has not changed,
         *  then the iteration order is not changed (helpful for AddAll() semantics, and perhaps elsewhere).
         *
         *  \note mutates container
         */
        nonvirtual void Add (ArgByValueType<DomainType> key, ArgByValueType<RangeType> newElt);
        template <typename ADDABLE_T>
        nonvirtual void Add (ADDABLE_T&& p)
            requires (convertible_to<ADDABLE_T, pair<DOMAIN_TYPE, RANGE_TYPE>> or convertible_to<ADDABLE_T, KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>>);

    public:
        /**
         *  \note   AddAll/2 is alias for .net AddRange ()
         *
         *  \note mutates container
         */
        template <IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> CONTAINER_OF_KEYVALUE>
        nonvirtual void AddAll (const CONTAINER_OF_KEYVALUE& items);
        template <IInputIterator<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> COPY_FROM_ITERATOR_KEYVALUE, sentinel_for<remove_cvref_t<COPY_FROM_ITERATOR_KEYVALUE>> COPY_FROM_ITERATOR_KEYVALUE2>
        nonvirtual void AddAll (COPY_FROM_ITERATOR_KEYVALUE&& start, COPY_FROM_ITERATOR_KEYVALUE2&& end);

    public:
        /**
         *  \note mutates container
         * 
         *  \param nextI - if provided (not null) - will be filled in with the next value after where iterator i is pointing - since i is invalidated by changing the container)
         */
        nonvirtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI = nullptr);

    public:
        /**
         *  This removes any mapping from 'd' to anything. It is not an error if 'd' isn not already in the domain.
         *
         *  \note mutates container
         */
        nonvirtual void RemoveDomainElement (ArgByValueType<DomainType> d);

    public:
        /**
         *  This removes any mapping from anything to 'r'. It is not an error if 'r' isn not already in the range.
         *
         *  \note mutates container
         */
        nonvirtual void RemoveRangeElement (ArgByValueType<RangeType> r);

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
        nonvirtual void RemoveAll ();
        template <predicate<pair<DOMAIN_TYPE, RANGE_TYPE>> PREDICATE>
        nonvirtual size_t RemoveAll (PREDICATE&& p);

    public:
        /*
         *  Return a mapping - in the reverse direction (a Bijection is (restricted) form of mapping. The type of
         *  TARGET_CONTAINER can be either a Bijection (in the opposite direction as the source), or
         *  A Mapping<> (also in the reverse direction).
         *
         *  Note - this returns a copy (by value) of this Bijections data.
         */
        template <typename TARGET_CONTAINER = Bijection<RangeType, DomainType>>
        nonvirtual TARGET_CONTAINER Inverse () const;

    public:
        /**
         *  This function should work for any container which accepts
         *  (ITERATOR_OF<pair<Key,Value>>,ITERATOR_OF<pair<Key,Value>>).
         *
         *  These As<> overloads also may require the presence of an insert(ITERATOR, Value) method
         *  of CONTAINER_OF_Key_T.
         *
         *  So - for example, Sequence<pair<DomainType,RangeType>>, map<DomainType,RangeType>,
         *  vector<pair<DomainType,RangeType>>, etc...
         *
         *  This works for:
         *      o   Mapping<DOMAIN_TYPE, RANGE_TYPE>
         *      o   map<DOMAIN_TYPE, RANGE_TYPE>
         *      o   vector<pair<DOMAIN_TYPE, RANGE_TYPE>>
         *      o   Sequence<pair<DOMAIN_TYPE, RANGE_TYPE>>
         */
        template <typename CONTAINER_PAIR_RANGE_DOMAIN>
        nonvirtual CONTAINER_PAIR_RANGE_DOMAIN As () const;

    public:
        /**
         *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
         *
         *  @todo - document computational complexity
         *
         *  \note   Not to be confused with EqualityComparerType and GetEqualsComparer () which compares ELEMENTS of Bijection<> for equality.
         */
        nonvirtual bool operator== (const Bijection& rhs) const;

    public:
        /**
         * \brief STL-ish alias for RemoveAll ().
         *
         *  \note mutates container
         */
        nonvirtual void clear ();

    public:
        /**
         * \brief STL-ish alias for Remove ().
         *
         *  \note mutates container
         */
        nonvirtual Iterator<value_type> erase (const Iterator<value_type>& i);

    public:
        /**
         *
         *  \note mutates container
         */
        template <IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERABLE_OF_ADDABLE>
        nonvirtual Bijection& operator+= (const ITERABLE_OF_ADDABLE& items);

    public:
        /**
         *
         *  \note mutates container
         */
        template <IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERABLE_OF_ADDABLE>
        nonvirtual Bijection& operator-= (const ITERABLE_OF_ADDABLE& items);

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
        using _SafeReadRepAccessor = typename Iterable<value_type>::template _SafeReadRepAccessor<T2>;

    protected:
        /**
         */
        template <typename T2>
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<T2>;

    protected:
        nonvirtual void _AssertRepValidType () const;

    public:
        nonvirtual [[deprecated ("String Stroika v3.0d1, Name deprecated - use MapToRange")]] Iterable<RangeType> Map (const Iterable<DomainType>& values) const
        {
            return MapToRange (values);
        }
        nonvirtual [[deprecated ("String Stroika v3.0d1, Name deprecated - use MapToRange")]] Iterable<DomainType>
        InverseMap (const Iterable<RangeType>& values) const
        {
            return MapToDomain (values);
        }
    };

    /**
     *  \brief  Implementation detail for Bijection<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the Bijection<T> container API.
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    class Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep : public Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::_IRep {
    private:
        using inherited = typename Iterable<value_type>::_IRep;

    protected:
        _IRep () = default;

    public:
        virtual ~_IRep () = default;

    public:
        virtual shared_ptr<_IRep>               CloneEmpty () const                                   = 0;
        virtual shared_ptr<_IRep>               CloneAndPatchIterator (Iterator<value_type>* i) const = 0;
        virtual bool                            Equals (const _IRep& rhs) const                       = 0;
        virtual DomainEqualsCompareFunctionType GetDomainEqualsComparer () const                      = 0;
        virtual RangeEqualsCompareFunctionType  GetRangeEqualsComparer () const                       = 0;
        // always clear/set item, and ensure return value == item->IsValidItem());
        // 'item' arg CAN be nullptr
        virtual bool Lookup (ArgByValueType<DOMAIN_TYPE> key, optional<RangeType>* item) const        = 0;
        virtual bool InverseLookup (ArgByValueType<RANGE_TYPE> key, optional<DomainType>* item) const = 0;
        virtual void Add (ArgByValueType<DOMAIN_TYPE> key, ArgByValueType<RANGE_TYPE> newElt)         = 0;
        virtual void RemoveDomainElement (ArgByValueType<DOMAIN_TYPE> d)                              = 0;
        virtual void RemoveRangeElement (ArgByValueType<RANGE_TYPE> r)                                = 0;
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)              = 0;

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
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Bijection.inl"

#endif /*_Stroika_Foundation_Containers_Bijection_h_ */
