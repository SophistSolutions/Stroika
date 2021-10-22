/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Mapping_h_
#define _Stroika_Foundation_Containers_Mapping_h_ 1

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
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Add an "Update (iterator<T> i, Value)" method, similar to other containers
 *              like Sequence. Note - intentionally don't allow updating the key???
 *
 *      @todo   Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *              concepts and that it USES that for the appropriate overloaded constructors.
 *
 *      @todo   Support more backends
 *              Especially HashTable, RedBlackTree, and stlhashmap
 *              And of course change default here
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
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *  @todo consider moving this elesewhere in containers code (Containers/Common.h) as this maybe useful elsewhere
     */
    enum class AddReplaceMode {
        eAddIfMissing,
        eAddReplaces
    };

    /**
     *      Mapping which allows for the association of two elements: a key and
     *  a value. The key UNIQUELY specifies its associated value.
     *
     *  @see    SortedMapping<Key,T>
     *
     *  \note   Design Note:
     *      \note   We used Iterable<KeyValuePair<Key,T>> instead of Iterable<pair<Key,T>> because it makes for
     *              more readable usage (foo.fKey versus foo.first, and foo.fValue verus foo.second).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \em Concrete Implementations:
     *      o   @see Concrete::Mapping_Array<>
     *      o   @see Concrete::Mapping_LinkedList<>
     *      o   @see Concrete::Mapping_stdmap<>
     *
     *  \em Factory:
     *      @see Concrete::Mapping_Factory<> to see default implementations.
     *
     *  \em Design Note:
     *      Included <map> and have explicit CTOR for map<> so that Stroika Mapping can be used more interoperably
     *      with map<> - and used without an explicit CTOR. Use Explicit CTOR to avoid accidental converisons. But
     *      if you declare an API with Mapping<KEY_TYPE,MAPPED_VALUE_TYPE> arguments, its important STL sources passing in map<> work transparently.
     *
     *      Similarly for std::initalizer_list.
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
     *      o   Standard Stroika Comparison equality (==, !=) support
     *
     *          Two Mappings are considered equal if they contain the same elements (keys) and each key is associated
     *          with the same value. There is no need for the items to appear in the same order for the two Mappings to
     *          be equal. There is no need for the backends to be of the same underlying representation either (stlmap
     *          vers linkedlist).
     *
     *          \req lhs and rhs arguments must have the same (or equivilent) EqualsComparers.
     *
     *          @todo - document computational complexity
     *
     *          ThreeWayComparer support is NOT provided for Mapping, because there is no intrinsic ordering among the elements
     *          of the mapping (keys) - even if there was some way to compare the values.
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Mapping : public Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> {
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
        using ArchetypeContainerType = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        /**
         *  like std::map<>::key_type
         */
        using key_type = KEY_TYPE;

    public:
        /**
         *  like std::map<>::mapped_type
         */
        using mapped_type = MAPPED_VALUE_TYPE;

    public:
        /**
         *  @see inherited::value_type
         */
        using value_type = typename inherited::value_type;

    public:
        /**
         *  This is the type returned by GetKeyEqualsComparer () and CAN be used as the argument to a Mapping<> as KeyEqualityComparer, but
         *  we allow any template in the Mapping<> CTOR for a keyEqualityComparer that follows the Common::IsEqualsComparer () concept (need better name).
         */
        using KeyEqualsCompareFunctionType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool (key_type, key_type)>>;

    public:
        /**
         *  This constructor creates a concrete mapping object, either empty, or initialized with any argument
         *  values.
         *
         *  The underlying data structure (and performance characteristcs) of the Mapping is
         *  defined by @see Factory::Mapping_Factory<>
         *
         *  \par Example Usage
         *      \code
         *          Collection<pair<int,int>> c;
         *          std::map<int,int> m;
         *
         *          Mapping<int,int> m1  = {pair<int, int>{1, 1}, pair<int, int>{2, 2}, pair<int, int>{3, 2}};
         *          Mapping<int,int> m2  = m1;
         *          Mapping<int,int> m3  { m1 };
         *          Mapping<int,int> m4  { m1.begin (), m1.end () };
         *          Mapping<int,int> m5  { c };
         *          Mapping<int,int> m6  { m };
         *          Mapping<int,int> m7  { m.begin (), m.end () };
         *          Mapping<int,int> m8  { move (m1) };
         *          Mapping<int,int> m9  { Common::DeclareEqualsComparer ([](int l, int r) { return l == r; }) };
         *      \endcode
         * 
         *  \note Implementation note:
         *        Reason for the not is_base_of_v<> restriction on CTOR/1(CONTAINER_OF_ADDABLE&&) is to prevent compiler from
         *        instantiating that constructor template for argument subclasses of this container type, and having those take precedence over the
         *        default X(const X&) CTOR.
         * 
         *        And also careful not to apply to non-iterables.
         */
        Mapping ();
        template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>* = nullptr>
        explicit Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer);
        Mapping (const Mapping& src) noexcept = default;
        Mapping (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>* = nullptr>
        Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        Mapping (const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>* = nullptr>
        Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> and not is_base_of_v<Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>* = nullptr>
        explicit Mapping (CONTAINER_OF_ADDABLE&& src);
        template <typename KEY_EQUALS_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>>* = nullptr>
        Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer, const CONTAINER_OF_ADDABLE& src);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>* = nullptr>
        Mapping (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);
        template <typename KEY_EQUALS_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>* = nullptr>
        Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

    protected:
        explicit Mapping (const _IRepSharedPtr& rep) noexcept;
        explicit Mapping (_IRepSharedPtr&& rep) noexcept;

#if qDebug
    public:
        ~Mapping ();
#endif

    public:
        /**
         */
        nonvirtual Mapping& operator= (const Mapping& rhs) = default;

    public:
        /**
         */
        nonvirtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const;

    public:
        /**
         *  Keys () returns an Iterable object with just the key part of the Mapping.
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
         *  MappedValues () returns an Iterable object with just the value part of the Mapping.
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
         *  Note - as since Lookup/1 returns an optional<T> - it can be used very easily to provide
         *  a default value on Lookup (so for case where not present) - as in:
         *      returm m.Lookup (key).Value (putDefaultValueHere);
         *
         *  Note - for both overloads taking an item pointer, the pointer may be nullptr (in which case not assigned to).
         *  But if present, will always be assigned to if Lookup returns true (found). And for the optional overload
         *      \req    Ensure (item == nullptr or returnValue == item->has_value());
         *
         *  \note   Alias - Lookup (key, mapped_type* value) - is equivilent to .Net TryGetValue ()
         */
        nonvirtual optional<mapped_type> Lookup (ArgByValueType<key_type> key) const;
        nonvirtual bool                  Lookup (ArgByValueType<key_type> key, optional<mapped_type>* item) const;
        nonvirtual bool                  Lookup (ArgByValueType<key_type> key, mapped_type* item) const;
        nonvirtual bool                  Lookup (ArgByValueType<key_type> key, nullptr_t) const;

    public:
        /**
         *  \note Alias LookupOrException
         */
        template <typename THROW_IF_MISSING>
        nonvirtual mapped_type LookupChecked (ArgByValueType<key_type> key, const THROW_IF_MISSING& throwIfMissing) const;

    public:
        /**
         *  Always safe to call. If result of Lookup () !has_value, returns argument 'default' or 'sentinal' value.
         *
         *  \note Alias LookupOrDefault
         */
        nonvirtual mapped_type LookupValue (ArgByValueType<key_type> key, ArgByValueType<mapped_type> defaultValue = mapped_type{}) const;

    public:
        /**
         *  \req ContainsKey (key);
         *
         *  \note   Design Note:
         *      Defined operator[](KEY_TYPE) const - to return const MAPPED_VALUE_TYPE, instead of optional<MAPPED_VALUE_TYPE> because
         *      this adds no value - you can always use Lookup or LookupValue. The reason to use operator[] is
         *      as convenient syntactic sugar. But if you have to check (the elt not necessarily present) - then you
         *      may as well use Lookup () - cuz the code's going to look ugly anyhow.
         *
         *      Defined operator[](KEY_TYPE) const - to return MAPPED_VALUE_TYPE instead of MAPPED_VALUE_TYPE& because we then couldn't control
         *      the lifetime of that reference, and it would be unsafe as the underlying object was changed.
         *
         *      And therefore we return CONST of that type so that code like m["xx"].a = 3 won't compile (and wont just assign to a temporary that disappears
         *      leading to confusion).
         *
         *  \note In the future, it may make sense to have operator[] return a PROXY OBJECT, so that it MIGHT be assignable. But that wouldn't work with
         *        cases like Mapping<String,OBJ> where you wanted to access OBJs fields as in m["xx"].a = 3
         *
         */
        nonvirtual add_const_t<mapped_type> operator[] (ArgByValueType<key_type> key) const;

    public:
        /**
         *  Synonym for Lookup (key).has_value ()
         */
        nonvirtual bool ContainsKey (ArgByValueType<key_type> key) const;

    public:
        /**
         *  Likely inefficeint for a map, but perhaps helpful. Walks entire list of entires
         *  and applies VALUE_EQUALS_COMPARER (defaults to operator==) on each value, and returns
         *  true if contained. Perhpas not very useful but symetric to ContainsKey().
         */
        template <typename VALUE_EQUALS_COMPARER = equal_to<MAPPED_VALUE_TYPE>>
        nonvirtual bool ContainsMappedValue (ArgByValueType<mapped_type> v, const VALUE_EQUALS_COMPARER& valueEqualsComparer = {}) const;

    public:
        /**
         *  Add the association between key and newElt. 
         *
         *  If key was already associated with something, consult argument addReplaceMode (defaults to AddReplaceMode::eAddReplaces).
         *  if 'replaces' then replace, and if 'addif' then do nothing on Add ()
         *
         *  \returns bool: The (generally ignored) return value boolean indicates if a new item was added (so size of iterable increased).
         *  This value returned is FALSE for the case of when the value remains unchanged or even if the value is updated (overwriting the previous association).
         *
         *  Also - we guarantee that even if the association is different, if the key has not changed,
         *  then the iteration order is not changed (helpful for AddAll() semantics, and perhaps elsewhere).
         *
         *  \note This behavior when the entry already exists differs from the behavior of std::map::insert (@see http://en.cppreference.com/w/cpp/container/map/insert)
         *        "Inserts element(s) into the container, if the container doesn't already contain an element with an equivalent key".
         *        This behavior is analagous to the new std-c++17 std::map::insert_or_assign () - @see http://en.cppreference.com/w/cpp/container/map/insert_or_assign
         *
         *  \note mutates container
         *
         *  \note - this returns true if a CLEAR change happened. But mappings dont have a VALUE_COMPARER by default. So no way
         *          to return if the MAPPING ITSELF changed. @todo - CONSIDER adding optional VALUE_COMPARER to AddIf, so it can return
         *          true if the mapping CHANGES (mapped to value changes). May need a different name (meaning maybe we've picked a bad name here)
         *
         *  \note Similar to Set<>::AddIf() - but here there is the ambiguity about whether to change what is mapped to (which we do differntly
         *        between Add and AddIf) and no such issue exists with Set<>::AddIf. But return true if they make a change.
         */
        nonvirtual bool Add (ArgByValueType<key_type> key, ArgByValueType<mapped_type> newElt, AddReplaceMode addReplaceMode = AddReplaceMode::eAddReplaces);
        nonvirtual bool Add (ArgByValueType<KeyValuePair<key_type, mapped_type>> p, AddReplaceMode addReplaceMode = AddReplaceMode::eAddReplaces);

    public:
        /**
         *  \summary Add all the argument (container or bound range of iterators) elements; if replaceExistingMapping true (default) force replace on each. Return count of added items (not count of updated items)
         *
         *  \note   AddAll/2 is alias for .net AddRange ()
         *
         *  \note mutates container
         */
        template <typename CONTAINER_OF_KEYVALUE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_KEYVALUE>>* = nullptr>
        nonvirtual unsigned int AddAll (CONTAINER_OF_KEYVALUE&& items, AddReplaceMode addReplaceMode = AddReplaceMode::eAddReplaces);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        nonvirtual unsigned int AddAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end, AddReplaceMode addReplaceMode = AddReplaceMode::eAddReplaces);

    public:
        /**
         *  NOTE- calling Remove(Key) when the key is not found is perfectly legal.
         *
         *  @todo CONSIDER:::
         *      TBD in the case of Remove() on in iterator???? Probably should have consistent
         *      answers but review Remove()for other containers as well.
         *
         *  Remove with iterator returns the adjusted iterator value, now pointing to the next value to use (as in save that iterator value, i++) and remove the
         *  i iterator value).
         *
         *  \note mutates container
         */
        nonvirtual void Remove (ArgByValueType<key_type> key);
        nonvirtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI = nullptr);

    public:
        /**
         *  \note mutates container
         */
        nonvirtual void RemoveAll ();
        template <typename CONTAINER_OF_ADDABLE>
        nonvirtual void RemoveAll (const CONTAINER_OF_ADDABLE& items);
        template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
        nonvirtual void RemoveAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end);

    public:
        /**
         *  Remove all items from this container UNLESS they are in the argument set to RetainAll().
         *
         *  This restricts the 'Keys' list of Mapping to the argument data, but preserving
         *  any associations.
         *
         *  \note   Java comparison
         *          mapping.keySet.retainAll (collection);
         *
         *  \par    Example Usage
         *      \code
         *          fStaticProcessStatsForThisSpill_.RetainAll (fDynamicProcessStatsForThisSpill_.Keys ());     // lose static data for processes no longer running
         *      \endcode
         *
         * \note    Something of an alias for 'Subset()' or 'Intersects', as this - in-place computes the subset
         *          of the Mapping<> that intersects with the argument keys.
         *
         * \todo    Consider having const function Intersects() - or Subset() - that produces a copy of the results of RetrainAll()
         *          without modifying this object.
         *
         *  \note mutates container
         */
        template <typename CONTAINER_OF_KEY_TYPE>
        nonvirtual void RetainAll (const CONTAINER_OF_KEY_TYPE& items);

    public:
        /**
         *  Apply the function function to each element, and return a subset Mapping including just the ones for which it was true.
         *
         *  \note   Alias - this could have been called 'Subset' - as it constructs a subset mapping (filtering on key or key-value pairs)
         *
         *  @see Iterable<T>::Where
         *
         *  \par Example Usage
         *      \code
         *           Mapping<int, int> m{KeyValuePair<int, int>{1, 3}, KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{4, 5}, KeyValuePair<int, int>{5, 7}};
         *           VerifyTestResult ((m.Where ([](const KeyValuePair<int, int>& value) { return Math::IsPrime (value.fKey); }) == Mapping<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{5, 7}}));
         *           VerifyTestResult ((m.Where ([](int key) { return Math::IsPrime (key); }) == Mapping<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{5, 7}}));
         *      \endcode
         */
        nonvirtual ArchetypeContainerType Where (const function<bool (ArgByValueType<key_type>)>& includeIfTrue) const;
        nonvirtual ArchetypeContainerType Where (const function<bool (ArgByValueType<KeyValuePair<key_type, mapped_type>>)>& includeIfTrue) const;

    public:
        /**
         *  Return a subset of this Mapping<> where the keys are included in the argument includeKeys set..
         *
         *  \note   Alias - this could have been called 'Subset' - as it constructs a subset mapping (where the given keys intersect)
         *
         *  @see Iterable<T>::Where
         *  @see Where
         *
         *  \note   CONCEPT - CONTAINER_OF_KEYS must support the 'Contains' API - not that set, and Iterable<> do this.
         *
         *  \par Example Usage
         *      \code
         *           Mapping<int, int> m{KeyValuePair<int, int>{1, 3}, KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{3, 5}, KeyValuePair<int, int>{4, 5}, KeyValuePair<int, int>{5, 7}};
         *           VerifyTestResult ((m.WithKeys (initializer_list<int> {2, 5}) == Mapping<int, int>{KeyValuePair<int, int>{2, 4}, KeyValuePair<int, int>{5, 7}}));
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
         * simply indirect to @Mapping<>::EqualsComparer;
         * only defined if there is a default equals comparer for mapped_type
         */
        nonvirtual bool operator== (const Mapping& rhs) const;
#endif

    public:
        /**
         */
        nonvirtual void Accumulate (
            ArgByValueType<key_type> key, ArgByValueType<mapped_type> newValue, const function<mapped_type (ArgByValueType<mapped_type>, ArgByValueType<mapped_type>)>& f = [] (ArgByValueType<mapped_type> l, ArgByValueType<mapped_type> r) -> mapped_type { return l + r; }, mapped_type initialValue = {});

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
        template <typename CONTAINER_OF_ADDABLE>
        nonvirtual Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> operator+ (const CONTAINER_OF_ADDABLE& items) const;

    public:
        /**
         */
        template <typename CONTAINER_OF_ADDABLE>
        nonvirtual Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& operator+= (const CONTAINER_OF_ADDABLE& items);

    public:
        /**
         */
        template <typename CONTAINER_OF_ADDABLE>
        nonvirtual Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& operator-= (const CONTAINER_OF_ADDABLE& items);

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
     *  \brief  Implementation detail for Mapping<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the Mapping<T> container API.
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep
        : public Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IRep
#if !qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        ,
          public Traversal::IterableBase::enable_shared_from_this_PtrImplementationTemplate<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep>
#endif
    {
    private:
        using inherited = typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IRep;

#if qCompilerAndStdLib_TemplateTypenameReferenceToBaseOfBaseClassMemberNotFound_Buggy
    protected:
        using _IterableRepSharedPtr = typename inherited::_IterableRepSharedPtr;
        using _APPLY_ARGTYPE        = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE   = typename inherited::_APPLYUNTIL_ARGTYPE;
#endif

    protected:
        _IRep () = default;

    public:
        virtual ~_IRep () = default;

    protected:
        using _IRepSharedPtr = typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRepSharedPtr;

    public:
        virtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const                                                                                                     = 0;
        virtual _IRepSharedPtr               CloneEmpty (IteratorOwnerID forIterableEnvelope) const                                                                            = 0;
        virtual _IRepSharedPtr               CloneAndPatchIterator (Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>* i, IteratorOwnerID obsoleteForIterableEnvelope) const = 0;
        virtual Iterable<key_type>           Keys () const                                                                                                                     = 0;
        virtual Iterable<mapped_type>        MappedValues () const                                                                                                             = 0;
        // always clear/set item, and ensure return value == item->IsValidItem());
        // 'item' arg CAN be nullptr
        virtual bool Lookup (ArgByValueType<KEY_TYPE> key, optional<mapped_type>* item) const = 0;
        // return true if NEW mapping added (container enlarged) - if replaceExistingMapping we unconditionally update but can still return false
        virtual bool Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<mapped_type> newElt, AddReplaceMode addReplaceMode) = 0;
        virtual void Remove (ArgByValueType<KEY_TYPE> key)                                                                 = 0;
        virtual void Remove (const Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& i)                                 = 0;
        //  call before remove - if adjustAt == nullopt, means removedAll
        virtual void PatchIteratorBeforeRemove (const optional<Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>>& adjustmentAt, Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>* i) const = 0;

#if qDebug
        virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const = 0;
#endif

    protected:
        nonvirtual Iterable<key_type> _Keys_Reference_Implementation () const;
        nonvirtual Iterable<mapped_type> _Values_Reference_Implementation () const;
    };

    /**
     *  \brief Compare Mappings<>s for equality. 
     *
     *  \note   Not to be confused with GetKeyEqualsComparer () which compares KEY ELEMENTS of Mapping for equality.
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename VALUE_EQUALS_COMPARER>
    struct Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::EqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
        constexpr EqualsComparer (const VALUE_EQUALS_COMPARER& valueEqualsComparer = {});
        nonvirtual bool       operator() (const Mapping& lhs, const Mapping& rhs) const;
        VALUE_EQUALS_COMPARER fValueEqualsComparer;
    };

#if __cpp_impl_three_way_comparison < 201907
    /**
     *  Basic comparison operator overloads with the obvious meaning, and simply indirect to @Mapping<>::EqualsComparer
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    bool operator== (const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& lhs, const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& rhs);
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    bool operator!= (const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& lhs, const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& rhs);
#endif

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Mapping.inl"

#endif /*_Stroika_Foundation_Containers_Mapping_h_ */
