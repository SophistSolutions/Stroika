/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Association_h_
#define _Stroika_Foundation_Containers_Association_h_ 1

#include "../StroikaPreComp.h"

#include "../Common/Compare.h"
#include "../Common/KeyValuePair.h"
#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Traversal/Iterable.h"
#include "Common.h"

/*
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Support more backends
 *              Especially HashTable, RedBlackTree, and stlhashmap
 *
 *      @todo   Not sure where this note goes - but eventually add "Database-Based" implementation of mapping
 *              and/or extenral file. Maybe also map to DynamoDB, MongoDB, etc... (but not here under Mapping,
 *              other db module would inherit from mapping).
 *
 *      @todo   Keys() method should probably return Set<key_type> - instead of Iterable<key_type>, but concerned about
 *              creating container type interdependencies
 *
 */

namespace Stroika::Foundation::Containers {

    using Common::KeyValuePair;
    using Configuration::ArgByValueType;
    using Configuration::ExtractValueType_t;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *  \brief An Assocation pairs key values with (possibly multiple or none) mapped_type values. Like Mapping<>, but allowing multiple items associated with 'key'
     * 
     *      Association which allows for the association of two elements: a key and
     *  a value. Unlike a Mapping<>, this assocation may not be unique..
     *
     *  @see    SortedAssociation<Key,T>
     *
     *  \note   The term 'KEY' usually implies a UNIQUE mapping to the associated value, but DOES NOT do so in this container ArcheType.
     *          Though databases generally use key to imply unique, https://en.cppreference.com/w/cpp/container/multimap, for example, does not.
     * 
     *  \note   Design Note:
     *      \note   We used Iterable<KeyValuePair<Key,T>> instead of Iterable<pair<Key,T>> because it makes for
     *              more readable usage (foo.fKey versus foo.first, and foo.fValue verus foo.second).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \em Concrete Implementations:
     *      o   @see Concrete::Association_Array<>
     *      o   @see Concrete::Association_LinkedList<>
     *      o   @see Concrete::Association_stdmap<>
     *
     *  \em Factory:
     *      @see Concrete::Association_Factory<> to see default implementations.
     *
     *  \em Design Note:
     *      Included <map> and have explicit CTOR for multimap<> so that Stroika Association can be used more interoperably
     *      with multimap<> - and used without an explicit CTOR. Use Explicit CTOR to avoid accidental converisons. But
     *      if you declare an API with Association<KEY_TYPE,MAPPED_VALUE_TYPE> arguments, its important STL sources passing in multimap<> work transparently.
     *
     *      Similarly for std::initalizer_list.
     *
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison equality (==, !=) support
     *
     *          Two Associations are considered equal if they contain the same elements (keys) and each key is associated
     *          with the same value. There is no need for the items to appear in the same order for the two Associations to
     *          be equal. There is no need for the backends to be of the same underlying representation either (stlmap
     *          vers linkedlist).
     *
     *          \req lhs and rhs arguments must have the same (or equivilent) EqualsComparers.
     *
     *          @todo - document computational complexity
     *
     *          ThreeWayComparer support is NOT provided for Association, because there is no intrinsic ordering among the elements
     *          of the Association (keys) - even if there was some way to compare the values.
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association : public Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> {
    private:
        using inherited = Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>;

    protected:
        class _IRep;

    protected:
        using _IRepSharedPtr = typename inherited::template PtrImplementationTemplate<_IRep>;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = Association<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        /**
         *  @see inherited::value_type
         */
        using value_type = typename inherited::value_type;

    public:
        /**
         *  like std::multimap<>::key_type
         */
        using key_type = KEY_TYPE;

    public:
        /**
         *  like std::multimap<>::mapped_type
         */
        using mapped_type = MAPPED_VALUE_TYPE;

    public:
        /**
         *  This is the type returned by GetKeyEqualsComparer () and CAN be used as the argument to a Association<> as KeyEqualityComparer, but
         *  we allow any template in the Association<> CTOR for a keyEqualityComparer that follows the Common::IsEqualsComparer () concept (need better name).
         */
        using KeyEqualsCompareFunctionType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool (key_type, key_type)>>;

    public:
        /**
         *  \brief check if the argument type can be passed as argument to the arity/1 overload of Add ()
         */
        template <typename POTENTIALLY_ADDABLE_T>
        static constexpr bool IsAddable_v = is_convertible_v<POTENTIALLY_ADDABLE_T, value_type>;

    public:
        /**
         *  This constructor creates a concrete Association object, either empty, or initialized with any argument
         *  values.
         *
         *  The underlying data structure (and performance characteristcs) of the Association is
         *  defined by @see Factory::Association_Factory<>
         *
         *  \par Example Usage
         *      \code
         *          Collection<pair<int,int>> c;
         *          std::map<int,int> m;
         *
         *          Association<int,int> m1  = {pair<int, int>{1, 1}, pair<int, int>{2, 2}, pair<int, int>{3, 2}};
         *          Association<int,int> m2  = m1;
         *          Association<int,int> m3  { m1 };
         *          Association<int,int> m4  { m1.begin (), m1.end () };
         *          Association<int,int> m5  { c };
         *          Association<int,int> m6  { m };
         *          Association<int,int> m7  { m.begin (), m.end () };
         *          Association<int,int> m8  { move (m1) };
         *          Association<int,int> m9  { Common::DeclareEqualsComparer ([](int l, int r) { return l == r; }) };
         *      \endcode
         * 
         *  \note   <a href="ReadMe.md#Container Constructors">See general information about container constructors that applies here</a>
         */
        Association ();
        template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ()>* = nullptr>
        explicit Association (KEY_EQUALS_COMPARER&& keyEqualsComparer);
        Association (Association&& src) noexcept      = default;
        Association (const Association& src) noexcept = default;
        Association (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ()>* = nullptr>
        Association (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        Association (const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ()>* = nullptr>
        Association (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Association<KEY_TYPE, MAPPED_VALUE_TYPE>, decay_t<ITERABLE_OF_ADDABLE>>>* = nullptr>
        explicit Association (ITERABLE_OF_ADDABLE&& src);
        template <typename KEY_EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        Association (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERABLE_OF_ADDABLE&& src);
        template <typename ITERATOR_OF_ADDABLE, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        Association (ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);
        template <typename KEY_EQUALS_COMPARER, typename ITERATOR_OF_ADDABLE, enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>* = nullptr>
        Association (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);

    protected:
        explicit Association (_IRepSharedPtr&& rep) noexcept;
        explicit Association (const _IRepSharedPtr& rep) noexcept;

    public:
        /**
         */
        nonvirtual Association& operator= (Association&& rhs) = default;
        nonvirtual Association& operator= (const Association& rhs) = default;

    public:
        /**
         */
        nonvirtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const;

    public:
        /**
         *  Keys () returns an Iterable object with just the key part of the Association.
         *
         *  \note   Keys () will return a an Iterable producing (iterating) elements in
         *          the same order as the collection it is created from.
         *
         *          It is equivilent to copying the underlying collection and 'projecting' the
         *          key fields.
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
         *
         *  See:
         *      @see MappedValues ()
         */
        nonvirtual Iterable<key_type> Keys () const;

    public:
        /**
         *  MappedValues () returns an Iterable object with just the value part of the Association.
         *
         *  \note   MappedValues () will return a an Iterable producing (iterating) elements in
         *          the same order as the collection it is created from.
         *
         *          It is equivilent to copying the underlying collection and 'projecting' the
         *          value fields.
         *
         *  \em Design Note:
         *      The analagous method in C#.net - Dictionary<TKey, TValue>.ValueCollection
         *      (https://msdn.microsoft.com/en-us/library/x8bctb9c%28v=vs.110%29.aspx).aspx) returns a live reference
         *      to the underlying keys. We could have (fairly easily) done that, but I didn't see the point.
         *
         *      In .net, the typical model is that you have a pointer to an object, and pass around that
         *      pointer (so by reference semantics) - so this returning a live reference makes more sense there.
         *
         *      Since Stroika containers are logically copy-by-value (even though lazy-copied), it made more
         *      sense to apply that lazy-copy (copy-on-write) paradigm here, and make the returned set of
         *      keys a logical copy at the point 'keys' is called.
         *
         *  \note   Alias - this could also have been called Image ()
         *
         *  See:
         *      @see Keys ()
         */
        nonvirtual Iterable<mapped_type> MappedValues () const;

    public:
        /**
         *  \brief Return an Iterable<mapped_type> of all the associated items (can be empty if none). This iterable is a snapshot at the time of call (but maybe lazy COW copied snapshot so still cheap)
         */
        nonvirtual Traversal::Iterable<mapped_type> Lookup (ArgByValueType<key_type> key) const;

    public:
        /**
         *  \brief Lookup and return the first (maybe arbitrarily chosen which is first) value with this key, and throw if there are none.
         * 
         *  \note Alias LookupOrException
         */
        template <typename THROW_IF_MISSING>
        nonvirtual mapped_type LookupOneChecked (ArgByValueType<key_type> key, const THROW_IF_MISSING& throwIfMissing) const;

    public:
        /**
         *  \brief Lookup and return the first (maybe arbitrarily chosen which is first) value with this key, and otherwise return argument value as default.
         *
         *  \note Alias LookupOneOrDefault
         */
        nonvirtual mapped_type LookupOneValue (ArgByValueType<key_type> key, ArgByValueType<mapped_type> defaultValue = mapped_type{}) const;

    public:
        /**
         *  Synonym for Lookup (key).has_value ()
         * 
         *  \note same as OccurrencesOf (key) != 0
         */
        nonvirtual bool ContainsKey (ArgByValueType<key_type> key) const;

    public:
        /**
         *  OccurrencesOf() returns the number of occurences of 'item' in the association.
         */
        nonvirtual size_t OccurrencesOf (ArgByValueType<key_type> item) const;

    public:
        /**
         *  Likely inefficient, but perhaps helpful. Walks entire list of entires
         *  and applies VALUE_EQUALS_COMPARER (defaults to operator==) on each value, and returns
         *  true if contained. Perhpas not very useful but symetric to ContainsKey().
         */
        template <typename VALUE_EQUALS_COMPARER = equal_to<MAPPED_VALUE_TYPE>>
        nonvirtual bool ContainsMappedValue (ArgByValueType<mapped_type> v, const VALUE_EQUALS_COMPARER& valueEqualsComparer = {}) const;

    public:
        /**
         *  Add the association between key and newElt. Note, this increases teh size of the container by one, even if key was already present in the association.
         *
         *  \note mutates container
         */
        nonvirtual void Add (ArgByValueType<key_type> key, ArgByValueType<mapped_type> newElt);
        nonvirtual void Add (ArgByValueType<value_type> p);

    public:
        /**
         *  \summary Add all the argument (container or bound range of iterators) elements.
         *
         *  \note   AddAll/2 is alias for .net AddRange ()
         *
         *  \req IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>
         *  \req IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>
         *
         *  \note AddAll () does not return the number of items added because all items are added (so the count can be made on the iterators/diff or items.size()
         * 
         *  \note mutates container
         */
        template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>* = nullptr>
        nonvirtual void AddAll (ITERABLE_OF_ADDABLE&& items);
        template <typename ITERATOR_OF_ADDABLE>
        nonvirtual void AddAll (ITERATOR_OF_ADDABLE start, ITERATOR_OF_ADDABLE end);

    public:
        /**
         * \brief Remove the given item (which must exist).
         * 
         * \note - for the argument 'key' overload, this is a change in Stroika 2.1b14: before it was legal and silently ignored if you removed an item that didn't exist.
         * 
         *  Remove with iterator returns the adjusted iterator value, now pointing to the next value to use (as in save that iterator value, ++i) and remove the
         *  i iterator value).
         *
         *  \note mutates container
         */
        nonvirtual void Remove (ArgByValueType<key_type> key);
        nonvirtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI = nullptr);

    public:
        /**
         * \brief Remove the given item, if it exists. Return true if found and removed.
         * 
         *  \note mutates container
         */
        nonvirtual bool RemoveIf (ArgByValueType<key_type> key);

    public:
        /**
         *  \brief RemoveAll removes all, or all matching (predicate, iterator range, equals comparer or whatever) items.
         * 
         *  The no-arg overload removes all (quickly).
         * 
         *  The overloads that remove some subset of the items returns the number of items so removed, and use RemoveIf() so that the
         *  argument items designated to be removed MAY not be present.
         * 
         *  \note mutates container
         */
        nonvirtual void RemoveAll ();
        template <typename ITERABLE_OF_KEY_OR_ADDABLE>
        nonvirtual size_t RemoveAll (const ITERABLE_OF_KEY_OR_ADDABLE& items);
        template <typename ITERATOR_OF_KEY_OR_ADDABLE>
        nonvirtual size_t RemoveAll (ITERATOR_OF_KEY_OR_ADDABLE start, ITERATOR_OF_KEY_OR_ADDABLE end);
        template <typename PREDICATE, enable_if_t<Configuration::IsTPredicate<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>, PREDICATE> ()>* = nullptr>
        nonvirtual size_t RemoveAll (const PREDICATE& p);

    public:
        /**
         *  Update the value associated with the iterator 'i', without changing iteration order in any way (cuz the key not changed).
         *  Note - if iterating, because this modifies the underlying container, the caller should pass 'i' in as a reference parameter to 'nextI'
         *  to have it updated to safely continue iterating.
         *
         *  \note mutates container
         *  \note As with ALL methods that modify the Association, this invalidates the iterator 'i', but if you pass nextI (can be same variable as i) - it will be updated with a valid iterator pointing to the same location.
         */
        nonvirtual void Update (const Iterator<value_type>& i, ArgByValueType<mapped_type> newValue, Iterator<value_type>* nextI = nullptr);

    public:
        /**
         *  Remove all items from this container UNLESS they are in the argument set to RetainAll().
         *
         *  This restricts the 'Keys' list of Association to the argument data, but preserving
         *  any associations.
         *
         *  \note   Java comparison
         *          association.keySet.retainAll (collection);
         *
         *  \par    Example Usage
         *      \code
         *          fStaticProcessStatsForThisSpill_.RetainAll (fDynamicProcessStatsForThisSpill_.Keys ());     // lose static data for processes no longer running
         *      \endcode
         *
         * \note    Something of an alias for 'Subset()' or 'Intersects', as this - in-place computes the subset
         *          of the Association<> that intersects with the argument keys.
         *
         * \todo    Consider having const function Intersects() - or Subset() - that produces a copy of the results of RetrainAll()
         *          without modifying this object.
         *
         *  \note mutates container
         */
        template <typename ITERABLE_OF_KEY_TYPE>
        nonvirtual void RetainAll (const ITERABLE_OF_KEY_TYPE& items);

    public:
        /**
         *  Apply the function function to each element, and return a subset Association including just the ones for which it was true.
         *
         *  \note   Alias - this could have been called 'Subset' - as it constructs a subset association (filtering on key or key-value pairs)
         *
         *  @see Iterable<T>::Where
         *
         *  \par Example Usage
         *      \code
         *           Association<int, int> m{KeyValuePair<int, int>{1, 3}, KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{4, 5}, KeyValuePair<int, int>{5, 7}};
         *           VerifyTestResult ((m.Where ([](const KeyValuePair<int, int>& value) { return Math::IsPrime (value.fKey); }) == Association<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{5, 7}}));
         *           VerifyTestResult ((m.Where ([](int key) { return Math::IsPrime (key); }) == Association<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{5, 7}}));
         *      \endcode
         */
        nonvirtual ArchetypeContainerType Where (const function<bool (ArgByValueType<key_type>)>& includeIfTrue) const;
        nonvirtual ArchetypeContainerType Where (const function<bool (ArgByValueType<value_type>)>& includeIfTrue) const;

    public:
        /**
         *  Return a subset of this Association<> where the keys are included in the argument includeKeys set..
         *
         *  \note   Alias - this could have been called 'Subset' - as it constructs a subset Association (where the given keys intersect)
         *
         *  @see Iterable<T>::Where
         *  @see Where
         *
         *  \note   CONCEPT - CONTAINER_OF_KEYS must support the 'Contains' API - not that set, and Iterable<> do this.
         *
         *  \par Example Usage
         *      \code
         *           Association<int, int> m{KeyValuePair<int, int>{1, 3}, KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{4, 5}, KeyValuePair<int, int>{5, 7}};
         *           VerifyTestResult ((m.WithKeys (initializer_list<int> {2, 5}) == Association<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{5, 7}}));
         *      \endcode
         */
        template <typename CONTAINER_OF_KEYS>
        nonvirtual ArchetypeContainerType WithKeys (const CONTAINER_OF_KEYS& includeKeys) const;
        nonvirtual ArchetypeContainerType WithKeys (const initializer_list<key_type>& includeKeys) const;

    public:
        /**
         *  This function should work for any container which accepts
         *  (ITERATOR_OF<KeyValuePair<Key,Value>>,ITERATOR_OF<KeyValuePair<Key,Value>>) OR
         *  (ITERATOR_OF<pair<Key,Value>>,ITERATOR_OF<pair<Key,Value>>).
         *
         *  These As<> overloads also may require the presence of an insert(ITERATOR, Value) method
         *  of CONTAINER_OF_Key_T.
         *
         *  So - for example, Sequence<KeyValuePair<key_type,ValueType>>, map<key_type,ValueType>,
         *  vector<pair<key_type,ValueType>>, etc...
         */
        template <typename CONTAINER_OF_Key_T>
        nonvirtual CONTAINER_OF_Key_T As () const;

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

    private:
        template <typename CONTAINER_OF_Key_T>
        nonvirtual CONTAINER_OF_Key_T As_ (enable_if_t<is_convertible_v<typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>, int> usesInsertPair = 0) const;
        template <typename CONTAINER_OF_Key_T>
        nonvirtual CONTAINER_OF_Key_T As_ (enable_if_t<!is_convertible_v<typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>, int> usesDefaultIterableImpl = 0) const;

    public:
        template <typename VALUE_EQUALS_COMPARER = equal_to<MAPPED_VALUE_TYPE>>
        struct EqualsComparer;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         * simply indirect to @Association<>::EqualsComparer;
         * only defined if there is a default equals comparer for mapped_type
         */
        nonvirtual bool operator== (const Association& rhs) const;
#endif

    public:
        /**
         * \brief like Add (key, newValue) - BUT newValue is COMBINED with the 'f' argument.
         * 
         *  The accumulator function combines the previous value associated with the new value given (using initialValue if key was not already present in the map).
         */
        nonvirtual void Accumulate (
            ArgByValueType<key_type>                                                                key,
            ArgByValueType<mapped_type>                                                             newValue,
            const function<mapped_type (ArgByValueType<mapped_type>, ArgByValueType<mapped_type>)>& f            = [] (ArgByValueType<mapped_type> l, ArgByValueType<mapped_type> r) -> mapped_type { return l + r; },
            mapped_type                                                                             initialValue = {});

    public:
        /**
         * \brief STL-ish alias for Remove ().
         */
        nonvirtual void erase (ArgByValueType<key_type> key);
        nonvirtual Iterator<value_type> erase (const Iterator<value_type>& i);

    public:
        /**
         * \brief STL-ish alias for RemoveAll ().
         */
        nonvirtual void clear ();

    public:
        /**
         */
        template <typename ITERABLE_OF_ADDABLE>
        nonvirtual Association operator+ (const ITERABLE_OF_ADDABLE& items) const;

    public:
        /**
         */
        template <typename ITERABLE_OF_ADDABLE>
        nonvirtual Association& operator+= (const ITERABLE_OF_ADDABLE& items);

    public:
        /**
         */
        template <typename ITERABLE_OF_KEY_OR_ADDABLE>
        nonvirtual Association& operator-= (const ITERABLE_OF_KEY_OR_ADDABLE& items);

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
     *  \brief  Implementation detail for Association<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the Association<T> container API.
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep
        : public Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IRep
#if !qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        ,
          public Traversal::IterableBase::enable_shared_from_this_PtrImplementationTemplate<typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep>
#endif
    {
    private:
        using inherited = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IRep;

    protected:
        _IRep () = default;

    public:
        virtual ~_IRep () = default;

    protected:
        using _IRepSharedPtr = typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRepSharedPtr;

    public:
        virtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const                         = 0;
        virtual _IRepSharedPtr               CloneEmpty () const                                   = 0;
        virtual _IRepSharedPtr               CloneAndPatchIterator (Iterator<value_type>* i) const = 0;
        virtual Iterable<key_type>           Keys () const                                         = 0;
        virtual Iterable<mapped_type>        MappedValues () const                                 = 0;
        // always clear/set item, and ensure return value == item->IsValidItem());
        // 'item' arg CAN be nullptr
        virtual Iterable<mapped_type> Lookup (ArgByValueType<KEY_TYPE> key) const                            = 0;
        virtual void                  Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<mapped_type> newElt) = 0;
        virtual bool                  RemoveIf (ArgByValueType<KEY_TYPE> key)                                = 0;
        // if nextI is non-null, its filled in with the next item in iteration order after i (has been removed)
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)                                       = 0;
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<mapped_type> newValue, Iterator<value_type>* nextI) = 0;

    protected:
        nonvirtual Iterable<key_type> _Keys_Reference_Implementation () const;
        nonvirtual Iterable<mapped_type> _Values_Reference_Implementation () const;
    };

    /**
     *  \brief Compare Associations<>s for equality. 
     *
     *  Two associations are equal, if they have the same domain, the same range, and each element in the domain
     *  has the same elements in its range (though there is NO NEED for the domain elements or range elements or associated elements
     *  to be in the same order).
     * 
     *  \note   This maybe expensive to compute, since all these different orderings are allowed when items still compare equal. However,
     *          the code makes some effort to make sure the common cases where things are all in the same order are detected as equal
     *          quickly.
     *
     *  \note   Not to be confused with GetKeyEqualsComparer () which compares KEY ELEMENTS of Association for equality.
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename VALUE_EQUALS_COMPARER>
    struct Association<KEY_TYPE, MAPPED_VALUE_TYPE>::EqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
        constexpr EqualsComparer (const VALUE_EQUALS_COMPARER& valueEqualsComparer = {});
        nonvirtual bool                                  operator() (const Association& lhs, const Association& rhs) const;
        [[NO_UNIQUE_ADDRESS_ATTR]] VALUE_EQUALS_COMPARER fValueEqualsComparer;
    };

#if __cpp_impl_three_way_comparison < 201907
    /**
     *  Basic comparison operator overloads with the obvious meaning, and simply indirect to @Association<>::EqualsComparer
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    bool operator== (const Association<KEY_TYPE, MAPPED_VALUE_TYPE>& lhs, const Association<KEY_TYPE, MAPPED_VALUE_TYPE>& rhs);
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    bool operator!= (const Association<KEY_TYPE, MAPPED_VALUE_TYPE>& lhs, const Association<KEY_TYPE, MAPPED_VALUE_TYPE>& rhs);
#endif

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Association.inl"

#endif /*_Stroika_Foundation_Containers_Association_h_ */
