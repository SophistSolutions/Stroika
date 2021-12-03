/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bijection_h_
#define _Stroika_Foundation_Containers_Bijection_h_ 1

#include "../StroikaPreComp.h"

#include <utility>

#include "../Common/Compare.h"
#include "../Common/KeyValuePair.h"
#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Execution/Exceptions.h"
#include "../Traversal/Iterable.h"
#include "Common.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *
 *      @todo   Support Where (hide iterable one)
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

    using Configuration::ArgByValueType;
    using Traversal::Iterable;
    using Traversal::Iterator;

    class Bijection_Base {
    public:
        /**
         */
        enum class InjectivityViolationPolicy {
            eAssertionError,
            eThrowException,

            eDEFAULT = eAssertionError,

            Stroika_Define_Enum_Bounds (eAssertionError, eThrowException)
        };

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
     *  one element of the domain, and these mappings happen in a way that the mapping is fully invertable.
     *
     *  @see    http://en.wikipedia.org/wiki/Bijection
     *
     *  Design Notes:
     *      \note   We used Iterable<pair<DOMAIN_TYPE,RANGE_TYPE>> instead of
     *              Iterable<KeyValuePairType<DOMAIN_TYPE,RANGE_TYPE>> because its completly symetric - both
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
     *      @see Concrete::Bijection_Factory<> to see default implementations.
     *
     *  \note Note About Iterators
     *      o   Stroika container iterators must have shorter lifetime than the container they are iterating over.
     *
     *      o   Stroika container iterators are all automatically patched, so that if you change the underlying container
     *          the iterators are automatically updated internally to behave sensibly.
     *
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *      o   operator== and operator!= are supported
     *      o   All Bijections MUST have = comparable DOMAIN_TYPE and RANGE_TYPE, so operator== for the bijection is well-defined.
     *
     *          Two Bijections are considered equal if they contain the same elements (Preimage) and each key is associated
     *          with the same value. There is no need for the items to appear in the same order for the two Bijections to
     *          be equal. There is no need for the backends to be of the same underlying representation either (stlmap
     *          vers linkedlist).
     *
     *          Since a Bijection is not necessarily sorted, or in any particular order, < and > are not well defined.
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    class Bijection : public Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>, public Bijection_Base {
    private:
        using inherited = Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>;

    protected:
        class _IRep;

    protected:
        using _IRepSharedPtr = typename inherited::template PtrImplementationTemplate<_IRep>;

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
         *  we allow any template in the Set<> CTOR for an equalityComparer that follows the Common::IsEqualsComparer () concept (need better name).
         */
        using DomainEqualsCompareFunctionType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool (DomainType, DomainType)>>;

    public:
        /**
         *  This is the type returned by GetRangeEqualsComparer () and CAN be used as the argument to a Bijection<> as EqualityComparer, but
         *  we allow any template in the Set<> CTOR for an equalityComparer that follows the Common::IsEqualsComparer () concept (need better name).
         */
        using RangeEqualsCompareFunctionType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool (RangeType, RangeType)>>;

    public:
        /**
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add ()
         *
         *  \todo https://stroika.atlassian.net/browse/STK-651 - Experimental feature which might be used as a concept check on various templates
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable = is_convertible_v<POTENTIALLY_ADDABLE_T, value_type>;

    public:
        /**
         *  This constructor creates a concrete Bijection object, either empty, or initialized with any argument
         *  values.
         *
         *  The underlying data structure of the Bijection is defined by @see Factory::Bijection_Factory<>
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
        Bijection ();
        template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<DOMAIN_EQUALS_COMPARER, DOMAIN_TYPE> () and Common::IsPotentiallyComparerRelation<RANGE_EQUALS_COMPARER, RANGE_TYPE> ()>* = nullptr>
        explicit Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer);
        template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<DOMAIN_EQUALS_COMPARER, DOMAIN_TYPE> () and Common::IsPotentiallyComparerRelation<RANGE_EQUALS_COMPARER, RANGE_TYPE> ()>* = nullptr>
        explicit Bijection (InjectivityViolationPolicy injectivityCheckPolicy, DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer);
        Bijection (Bijection&& src) noexcept      = default;
        Bijection (const Bijection& src) noexcept = default;
        Bijection (const initializer_list<value_type>& src);
        template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<DOMAIN_EQUALS_COMPARER, DOMAIN_TYPE> () and Common::IsPotentiallyComparerRelation<RANGE_EQUALS_COMPARER, RANGE_TYPE> ()>* = nullptr>
        Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer, const initializer_list<value_type>& src);
        template <typename CONTAINER_OF_SINGLEVALUE_ADD_ARGS, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_SINGLEVALUE_ADD_ARGS, Common::KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> and not is_base_of_v<Bijection<DOMAIN_TYPE, RANGE_TYPE>, Configuration::remove_cvref_t<CONTAINER_OF_SINGLEVALUE_ADD_ARGS>>>* = nullptr>
        explicit Bijection (const CONTAINER_OF_SINGLEVALUE_ADD_ARGS& src);
        template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, typename CONTAINER_OF_SINGLEVALUE_ADD_ARGS, enable_if_t<Common::IsPotentiallyComparerRelation<DOMAIN_EQUALS_COMPARER, DOMAIN_TYPE> () and Common::IsPotentiallyComparerRelation<RANGE_EQUALS_COMPARER, RANGE_TYPE> () and Configuration::IsIterableOfT_v<CONTAINER_OF_SINGLEVALUE_ADD_ARGS, Common::KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>>>* = nullptr>
        Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer, const CONTAINER_OF_SINGLEVALUE_ADD_ARGS& src);
        template <typename COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG>>* = nullptr>
        Bijection (COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG start, COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG end);
        template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, typename COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG, enable_if_t<Common::IsPotentiallyComparerRelation<DOMAIN_EQUALS_COMPARER, DOMAIN_TYPE> () and Common::IsPotentiallyComparerRelation<RANGE_EQUALS_COMPARER, RANGE_TYPE> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG>>* = nullptr>
        Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer, COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG start, COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG end);

    protected:
        explicit Bijection (_IRepSharedPtr&& src) noexcept;
        explicit Bijection (const _IRepSharedPtr& src) noexcept;

    public:
        /**
         */
        nonvirtual Bijection& operator= (Bijection&& rhs) = default;
        nonvirtual Bijection& operator= (const Bijection& rhs) = default;

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
         *  Always safe to call. If result empty/missing, returns argument 'default' or 'sentinal' value.
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
         *  Always safe to call. If result empty/missing, returns argument 'default' or 'sentinal' value.
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
        nonvirtual Iterable<RangeType> Map (const Iterable<DomainType>& values) const;

    public:
        /**
         * For each value in the source set, map it back using the bijection to the target set.
         *  \req that each element be present in the bijection
         */
        nonvirtual Iterable<DomainType> InverseMap (const Iterable<RangeType>& values) const;

    public:
        nonvirtual Bijection Where (const function<bool (pair<DomainType, RangeType>)>& includeIfTrue) const;

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
        nonvirtual void Add (const pair<DomainType, RangeType>& p);
        template <typename KEYVALUEPAIR, enable_if_t<not is_convertible_v<KEYVALUEPAIR, pair<DOMAIN_TYPE, RANGE_TYPE>>>* = nullptr>
        nonvirtual void Add (KEYVALUEPAIR p);

    public:
        /**
         *  \note   AddAll/2 is alias for .net AddRange ()
         *
         *  \note mutates container
         */
        template <typename CONTAINER_OF_KEYVALUE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_KEYVALUE>>* = nullptr>
        nonvirtual void AddAll (const CONTAINER_OF_KEYVALUE& items);
        template <typename COPY_FROM_ITERATOR_KEYVALUE>
        nonvirtual void AddAll (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end);

    public:
        /**
         *  \note mutates container
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
        template <typename PREDICATE, enable_if_t<Configuration::IsTPredicate<pair<DOMAIN_TYPE, RANGE_TYPE>, PREDICATE> ()>* = nullptr>
        nonvirtual size_t RemoveAll (const PREDICATE& p);

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

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
         *
         *  @todo - document computational complexity
         *
         *  \note   Not to be confused with EqualityComparerType and GetEqualsComparer () which compares ELEMENTS of Bijection<> for equality.
         */
        nonvirtual bool operator== (const Bijection& rhs) const;
#endif

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
        template <typename CONTAINER_OF_SINGLEVALUE_ADD_ARGS>
        nonvirtual Bijection& operator+= (const CONTAINER_OF_SINGLEVALUE_ADD_ARGS& items);

    public:
        /**
         *
         *  \note mutates container
         */
        template <typename CONTAINER_OF_SINGLEVALUE_ADD_ARGS>
        nonvirtual Bijection& operator-= (const CONTAINER_OF_SINGLEVALUE_ADD_ARGS& items);

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

#if __cpp_impl_three_way_comparison < 201907
    private:
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wnon-template-friend\"");
        template <typename D, typename R>
        friend bool operator== (const Bijection<D, R>& lhs, const Bijection<D, R>& rhs);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wnon-template-friend\"");
#endif
    };

    /**
     *  \brief  Implementation detail for Bijection<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the Bijection<T> container API.
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    class Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep
        : public Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::_IRep
#if !qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        ,
          public Traversal::IterableBase::enable_shared_from_this_PtrImplementationTemplate<typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep>
#endif
    {
    private:
        using inherited = typename Iterable<value_type>::_IRep;

    protected:
        using _IRepSharedPtr = typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRepSharedPtr;

    protected:
        _IRep () = default;

    public:
        virtual ~_IRep () = default;

    public:
        virtual _IRepSharedPtr                  CloneEmpty () const                                   = 0;
        virtual _IRepSharedPtr                  CloneAndPatchIterator (Iterator<value_type>* i) const = 0;
        virtual bool                            Equals (const _IRep& rhs) const                       = 0;
        virtual DomainEqualsCompareFunctionType GetDomainEqualsComparer () const                      = 0;
        virtual RangeEqualsCompareFunctionType  GetRangeEqualsComparer () const                       = 0;
        virtual Iterable<DomainType>            Preimage () const                                     = 0;
        virtual Iterable<RangeType>             Image () const                                        = 0;
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

    protected:
        nonvirtual Iterable<DOMAIN_TYPE> _PreImage_Reference_Implementation () const;
        nonvirtual Iterable<RANGE_TYPE> _Image_Reference_Implementation () const;
    };

#if __cpp_impl_three_way_comparison < 201907
    /**
     *  Basic comparison operator overloads with the obvious meaning, and simply indirect to @Bijection<>::EqualsComparer
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    nonvirtual bool operator== (const Bijection<DOMAIN_TYPE, RANGE_TYPE>& lhs, const Bijection<DOMAIN_TYPE, RANGE_TYPE>& rhs);
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    bool operator!= (const Bijection<DOMAIN_TYPE, RANGE_TYPE>& lhs, const Bijection<DOMAIN_TYPE, RANGE_TYPE>& rhs);
#endif

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Bijection.inl"

#endif /*_Stroika_Foundation_Containers_Bijection_h_ */
