/*
* Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
*/
#ifndef _Stroika_Foundation_Containers_Association_h_
#define _Stroika_Foundation_Containers_Association_h_ 1

#include "../StroikaPreComp.h"

#include "../Common/Compare.h"
#include "../Common/KeyValuePair.h"
#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Execution/Synchronized.h"
#include "../Traversal/Iterable.h"
#include "Common.h"

/*
 *  \file
 *
 **  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
 ***VERY ROUGH UNUSABLE DRAFT*
 *
 *
 *  TODO:
 *
 *
 */

namespace Stroika::Foundation::Containers {

    using Common::KeyValuePair;
    using Configuration::ArgByValueType;
    using Traversal::Iterable;
    using Traversal::Iterator;

    /**
     *      Association which allows for the association of two elements, and key and
     *  a value. The key UNIQUELY specifies its associated value.
     *
     *  @see    SortedAssociation<Key,T>
     *
     *  \note   Design Note:
     *      \note   We used Iterable<KeyValuePair<Key,T>> instead of Iterable<pair<Key,T>> because it makes for
     *              more readable usage (foo.fKey versus foo.first, and foo.fValue verus foo.second).
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
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
     *      Included <map> and have explicit CTOR for map<> so that Stroika Association can be used more interoperably
     *      with map<> - and used without an explicit CTOR. Use Explicit CTOR to avoid accidental converisons. But
     *      if you declare an API with Association<KEY_TYPE,MAPPED_VALUE_TYPE> arguments, its important STL sources passing in map<> work transparently.
     *
     *      Similarly for initalizer_list.
     *
     *  \note   Design Note:
     *      Defined operator[](KEY_TYPE) const - to return MAPPED_VALUE_TYPE, instead of optional<MAPPED_VALUE_TYPE> because
     *      this adds no value - you can always use Lookup or LookupValue. The reason to use operator[] is
     *      as convenient syntactic sugar. But if you have to check (the elt not necessarily present) - then you
     *      may as well use Lookup () - cuz the code's going to look ugly anyhow.
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
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association : public Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> {
    private:
        using inherited = Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>;

    protected:
        class _IRep;

    protected:
        using _AssociationRepSharedPtr = typename inherited::template SharedPtrImplementationTemplate<_IRep>;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = Association<KEY_TYPE, MAPPED_VALUE_TYPE>;

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
         *  This is the type returned by GetKeyEqualsComparer () and CAN be used as the argument to a Association<> as KeyEqualityComparer, but
         *  we allow any template in the Association<> CTOR for a keyEqualityComparer that follows the Common::IsEqualsComparer () concept (need better name).
         */
        using KeyEqualsCompareFunctionType = Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool(key_type, key_type)>>;

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
         *        Collection<pair<int,int>> c;
         *        std::map<int,int> m;
         *
         *        Association<int,int> m1  = {pair<int, int>{1, 1}, pair<int, int>{2, 2}, pair<int, int>{3, 2}};
         *        Association<int,int> m2  = m1;
         *        Association<int,int> m3  { m1 };
         *        Association<int,int> m4  { m1.begin (), m1.end () };
         *        Association<int,int> m5  { c };
         *        Association<int,int> m6  { m };
         *        Association<int,int> m7  { m.begin (), m.end () };
         *        Association<int,int> m8  { move (m1) };
         *      \endcode
         */
        Association ();
        Association (const Association& src) noexcept = default;
#if 1
        // I think this casuses crash in IO::Transfer regression test - not sure how - only on UNIX - retest...
        // reproduced (not carefully) 2018-04-04
        Association (Association&& src) noexcept = default; //  https://stroika.atlassian.net/browse/STK-541  (assume OK here)
#endif
        Association (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        Association (const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Association<KEY_TYPE, MAPPED_VALUE_TYPE>*>>* = nullptr>
        Association (const CONTAINER_OF_ADDABLE& src);
        template <typename COPY_FROM_ITERATOR_KEY_T>
        Association (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

    protected:
        explicit Association (const _AssociationRepSharedPtr& rep) noexcept;
        explicit Association (_AssociationRepSharedPtr&& rep) noexcept;

#if qDebug
    public:
        ~Association ();
#endif

    public:
        /**
         */
        nonvirtual Association& operator= (const Association& rhs) = default;
        nonvirtual Association& operator= (Association&& rhs) = default;

    public:
        nonvirtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const
        {
            // tmphack
            return KeyEqualsCompareFunctionType{equal_to<key_type>{}};
        }

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
        *  Always safe to call. If result of Lookup () '!has_value', returns argument 'default' or 'sentinal' value.
        *
        *  \note Alias LookupOrDefault
        */
        nonvirtual mapped_type LookupValue (ArgByValueType<key_type> key, ArgByValueType<mapped_type> defaultValue = mapped_type ()) const;

    public:
        /**
        *  \req ContainsKey (key);
        */
        nonvirtual mapped_type operator[] (ArgByValueType<key_type> key) const;

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
        *  Add the association between key and newElt. If key was already associated with something
        *  else, the association is silently updated, and the size of the iterable does not change.
        *  Also - we guarantee that even if the association is different, if the key has not changed,
        *  then the iteration order is not changed (helpful for AddAll() semantics, and perhaps elsewhere).
        *
        *  \note This behavior when the entry already exists differs from the behavior of std::map::insert (@see http://en.cppreference.com/w/cpp/container/map/insert)
        *        "Inserts element(s) into the container, if the container doesn't already contain an element with an equivalent key".
        *        This behavior is analagous to the new std-c++17 std::map::insert_or_assign () - @see http://en.cppreference.com/w/cpp/container/map/insert_or_assign
        */
        nonvirtual void Add (ArgByValueType<key_type> key, ArgByValueType<mapped_type> newElt);
        nonvirtual void Add (ArgByValueType<KeyValuePair<key_type, mapped_type>> p);

    public:
        /**
        *  \note   AddAll/2 is alias for .net AddRange ()
        */
        template <typename CONTAINER_OF_KEYVALUE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_KEYVALUE>>* = nullptr>
        nonvirtual void AddAll (const CONTAINER_OF_KEYVALUE& items);
        template <typename COPY_FROM_ITERATOR_KEYVALUE>
        nonvirtual void AddAll (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end);

    public:
        /**
        *  NOTE- calling Remove(Key) when the key is not found is perfectly legal.
        *
        *  @todo CONSIDER:::
        *      TBD in the case of Remove() on in iterator???? Probably should have consistent
        *      answers but review Remove()for other containers as well.
        */
        nonvirtual void Remove (ArgByValueType<key_type> key);
        nonvirtual void Remove (const Iterator<value_type>& i);

    public:
        /**
        */
        nonvirtual void RemoveAll ();
        template <typename CONTAINER_OF_ADDABLE>
        nonvirtual void RemoveAll (const CONTAINER_OF_ADDABLE& items);
        template <typename COPY_FROM_ITERATOR_KEY_T>
        nonvirtual void RemoveAll (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

    public:
        /**
        *  Remove all items from this container UNLESS they are in the argument set to RetainAll().
        *
        *  This restricts the 'Keys' list of Association to the argument data, but preserving
        *  any associations.
        *
        *  \note   Java comparison
        *          Association.keySet.retainAll (collection);
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
        */
        template <typename CONTAINER_OF_KEY_TYPE>
        nonvirtual void RetainAll (const CONTAINER_OF_KEY_TYPE& items);

    public:
        /**
        *  Apply the function funciton to each element, and return a subset Association including just the ones for which it was true.
        *
        *  \note   Alias - this could have been called 'Subset' - as it constructs a subset Association (filtering on key or key-value pairs)
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
        nonvirtual ArchetypeContainerType Where (const function<bool(ArgByValueType<key_type>)>& includeIfTrue) const;
        nonvirtual ArchetypeContainerType Where (const function<bool(ArgByValueType<KeyValuePair<key_type, mapped_type>>)>& includeIfTrue) const;

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

    private:
        template <typename CONTAINER_OF_Key_T>
        nonvirtual CONTAINER_OF_Key_T As_ (enable_if_t<is_convertible_v<typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>, int> usesInsertPair = 0) const;
        template <typename CONTAINER_OF_Key_T>
        nonvirtual CONTAINER_OF_Key_T As_ (enable_if_t<!is_convertible_v<typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>, int> usesDefaultIterableImpl = 0) const;

    public:
        /**
         *  Two Associations are considered equal if they contain the same elements (keys) and each key is associated
         *  with the same value. There is no need for the items to appear in the same order for the two Associations to
         *  be equal. There is no need for the backends to be of the same underlying representation either (stlmap
         *  vers linkedlist).
         *
         *  Equals is commutative().
         *
         *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
         */
        template <typename VALUE_EQUALS_COMPARER = equal_to<MAPPED_VALUE_TYPE>>
        nonvirtual bool Equals (const Association<KEY_TYPE, MAPPED_VALUE_TYPE>& rhs, const VALUE_EQUALS_COMPARER& valueEqualsComparer = {}) const;

    public:
        /**
         */
        nonvirtual void Accumulate (ArgByValueType<key_type> key, ArgByValueType<mapped_type> newValue, const function<mapped_type (ArgByValueType<mapped_type>, ArgByValueType<mapped_type>)>& f = [](ArgByValueType<mapped_type> l, ArgByValueType<mapped_type> r) -> mapped_type { return l + r; }, mapped_type initialValue = {});

    public:
        /**
         * \brief STL-ish alias for Remove ().
         */
        nonvirtual void erase (ArgByValueType<key_type> key);
        nonvirtual void erase (const Iterator<value_type>& i);

    public:
        /**
         * \brief STL-ish alias for RemoveAll ().
         */
        nonvirtual void clear ();

    public:
        /**
        */
        template <typename CONTAINER_OF_ADDABLE>
        nonvirtual Association<KEY_TYPE, MAPPED_VALUE_TYPE> operator+ (const CONTAINER_OF_ADDABLE& items) const;

    public:
        /**
        */
        template <typename CONTAINER_OF_ADDABLE>
        nonvirtual Association<KEY_TYPE, MAPPED_VALUE_TYPE>& operator+= (const CONTAINER_OF_ADDABLE& items);

    public:
        /**
        */
        template <typename CONTAINER_OF_ADDABLE>
        nonvirtual Association<KEY_TYPE, MAPPED_VALUE_TYPE>& operator-= (const CONTAINER_OF_ADDABLE& items);

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
        using inherited = _IRep;

#if qCompilerAndStdLib_TemplateTypenameReferenceToBaseOfBaseClassMemberNotFound_Buggy
    protected:
        using _APPLY_ARGTYPE      = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
#endif

    protected:
        _IRep () = default;

    public:
        virtual ~_IRep () = default;

    protected:
        using _AssociationRepSharedPtr = typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_AssociationRepSharedPtr;

#if qCompilerAndStdLib_TemplateTypenameReferenceToBaseOfBaseClassMemberNotFound_Buggy
    protected:
        using _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
#endif

    public:
        virtual _AssociationRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const = 0;
        virtual Iterable<key_type>       Keys () const                                          = 0;
        virtual Iterable<mapped_type>    MappedValues () const                                  = 0;
        // always clear/set item, and ensure return value == item->IsValidItem());
        // 'item' arg CAN be nullptr
        virtual bool Lookup (ArgByValueType<KEY_TYPE> key, optional<mapped_type>* item) const = 0;
        virtual void Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<mapped_type> newElt)   = 0;
        virtual void Remove (ArgByValueType<KEY_TYPE> key)                                    = 0;
        virtual void Remove (const Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& i)    = 0;
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const = 0;
#endif

    protected:
        nonvirtual Iterable<key_type> _Keys_Reference_Implementation () const;
        nonvirtual Iterable<mapped_type> _Values_Reference_Implementation () const;
    };

    /**
     *      Syntactic sugar on Equals()
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    bool operator== (const Association<KEY_TYPE, MAPPED_VALUE_TYPE>& lhs, const Association<KEY_TYPE, MAPPED_VALUE_TYPE>& rhs);

    /**
     *      Syntactic sugar on not Equals()
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    bool operator!= (const Association<KEY_TYPE, MAPPED_VALUE_TYPE>& lhs, const Association<KEY_TYPE, MAPPED_VALUE_TYPE>& rhs);

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Association.inl"

#endif /*_Stroika_Foundation_Containers_Association_h_ */
