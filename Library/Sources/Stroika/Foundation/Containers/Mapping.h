/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Mapping_h_
#define _Stroika_Foundation_Containers_Mapping_h_  1

#include    "../StroikaPreComp.h"

#include    <map>

#include    "../Common/Compare.h"
#include    "../Common/KeyValuePair.h"
#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"
#include    "../Execution/Synchronized.h"
#include    "../Memory/Optional.h"
#include    "../Traversal/Iterable.h"

#include    "UpdatableIterable.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   ContainsValue() needs to be redone as template method  template    <typename VALUE_EQUALS_COMPARER = Common::ComparerWithEquals<VALUE_TYPE>>
 *              like Equals()
 *
 *      @todo   Support more backends
 *              Especially HashTable, RedBlackTree, and stlhashmap
 *              And of course change default here
 *
 *      @todo   Not sure where this note goes - but eventually add "Database-Based" implementation of mapping
 *              and/or extenral file. Maybe also map to DynamoDB, MongoDB, etc... (but not here under Mapping,
 *              other db module would inherit from mapping).
 *
 *      @todo    Keys() method should probably retunr Set<KeyType> - instead of Iterable<KeyType>.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;

            using   Common::KeyValuePair;


            /**
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = Common::ComparerWithEquals<KEY_TYPE>>
            struct   Mapping_DefaultTraits {
                /**
                 */
                using   KeyEqualsCompareFunctionType    =   KEY_EQUALS_COMPARER;

                RequireConceptAppliesToTypeMemberOfClass(Concept_EqualsCompareFunctionType, KeyEqualsCompareFunctionType);

                /**
                 *  Define typedef for this Mapping traits object (so other traits can generically allow recovery of the
                 *  underlying Mapping's TRAITS objects.
                 */
                using   MappingTraitsType               =    Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER>;
            };


            /**
             *      Mapping which allows for the association of two elements, and key and
             *  a value. The key UNIQUELY specifies its associated value.
             *
             *  @see    SortedMapping<Key,T>
             *
             *  Design Notes:
             *      \note   We used Iterable<KeyValuePair<Key,T>> instead of Iterable<pair<Key,T>> because it makes for
             *              more readable usage (foo.fKey versus foo.first, and foo.fValue verus foo.second).
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
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
             *      if you declare an API with Mapping<KEY_TYPE,VALUE_TYPE> arguments, its important STL sources passing in map<> work transparently.
             *
             *      Similarly for std::initalizer_list.
             *
             *  \note   Design Note:
             *      Defined operator[](KEY_TYPE) const - to return VALUE_TYPE, instead of Optional<VALUE_TYPE> because
             *      this adds no value - you can always use Lookup or LookupValue. The reason to use operator[] is
             *      as convenient syntactic sugar. But if you have to check (the elt not necessarily present) - then you
             *      may as well use Lookup () - cuz the code's going to look ugly anyhow.
             *
             *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE>>
            class   Mapping : public UpdatableIterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>> {
            private:
                using   inherited       =   UpdatableIterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>;

            protected:
                class   _IRep;
                using   _MappingSharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            protected:
                using   _SharedPtrIRep  =   _MappingSharedPtrIRep;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType  =   Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>;

            public:
                /**
                 */
                using   TraitsType  =   TRAITS;

            public:
                /**
                 */
                using   KeyType     =   KEY_TYPE;

            public:
                /**
                 */
                using   ValueType   =   VALUE_TYPE;

            public:
                /**
                 *  Just a short-hand for the KeyEqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using   KeyEqualsCompareFunctionType    =   typename TraitsType::KeyEqualsCompareFunctionType;

            public:
                /**
                 *  This constructor creates a concrete mapping object, either empty, or initialized with any argument
                 *  values.
                 *
                 *  The underlying data structure of the Mapping is defined by @see Concrete::Mapping_Factory<>
                 */
                Mapping ();
                Mapping (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& src);
                Mapping (const initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src);
                Mapping (const initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src);
                Mapping (const map<KEY_TYPE, VALUE_TYPE>& src);
                template    <typename TRAITS2>
                Mapping (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS2>& src);
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                explicit Mapping (const CONTAINER_OF_PAIR_KEY_T& src);
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                explicit Mapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

            protected:
                explicit Mapping (const _SharedPtrIRep& rep);
                explicit Mapping (_SharedPtrIRep&& rep);

#if     qDebug
            public:
                ~Mapping ();
#endif

            public:
                /**
                 */
                nonvirtual  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) =   default;
#if     qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy
                nonvirtual  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> && rhs)
                {
                    inherited::operator= (move (rhs));
                    return *this;
                }
#else
                nonvirtual  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (Mapping<KEY_TYPE, VALUE_TYPE, TRAITS> && rhs) = default;
#endif

            public:
                /**
                 *  Keys () returns an Iterable object with just the key part of the Mapping.
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
                 *
                 *  See:
                 *      @see Values ()
                 */
                nonvirtual  Iterable<KeyType>   Keys () const;

            public:
                /**
                 *  Values () returns an Iterable object with just the value part of the Mapping.
                 *
                 *  Note this method may not return a collection which is sorted. Note also, the
                 *  returned value is a copy of the keys (by value) - at least logically (implementations
                 *  maybe smart enough to use lazy copying).
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
                nonvirtual  Iterable<ValueType>   Values () const;

            public:
                /**
                 *  Note - as since Lookup/1 returns an Optional<T> - it can be used very easily to provide
                 *  a default value on Lookup (so for case where not present) - as in:
                 *      returm m.Lookup (key).Value (putDefaultValueHere);
                 *
                 *  Note - for both overloads taking an item pointer, the pointer may be nullptr (in which case not assigned to).
                 *  But if present, will always be assigned to if Lookup returns true (found). And for the optional overload
                 *      \req    Ensure (item == nullptr or returnValue == item->IsPresent());
                 */
                nonvirtual  Memory::Optional<ValueType> Lookup (KeyType key) const;
                nonvirtual  bool                        Lookup (KeyType key, Memory::Optional<ValueType>* item) const;
                nonvirtual  bool                        Lookup (KeyType key, ValueType* item) const;
                nonvirtual  bool                        Lookup (KeyType key, nullptr_t) const;

            public:
                /**
                 *  Always safe to call. If result of Lookup () 'IsMissing', returns argument 'default' or 'sentinal' value.
                 */
                nonvirtual  ValueType   LookupValue (KeyType key, ValueType defaultValue = ValueType ()) const;

            public:
                /**
                 *  \req ContainsKey (key);
                 */
                nonvirtual  ValueType   operator[] (KeyType key) const;

            public:
                /**
                 *  Synonym for Lookup (key).IsPresent ()
                 */
                nonvirtual  bool    ContainsKey (KeyType key) const;

            public:
                /**
                 *  Likely inefficeint for a map, but perhaps helpful. Walks entire list of entires
                 *  and applies operator== on each value, and returns true if contained. Perhpas not
                 *  very useful but symetric to ContainsKey().
                 *
                 *  \req RequireConceptAppliesToTypeInFunction(RequireOperatorEquals, T);
                 */
                nonvirtual  bool    ContainsValue (ValueType v) const;

            public:
                /**
                 *  Add the association between key and newElt. If key was already associated with something
                 *  else, the association is silently updated, and the size of the iterable does not change.
                 *  Also - we guarantee that even if the association is different, if the key has not changed,
                 *  then the iteration order is not changed (helpful for AddAll() semantics, and perhaps elsewhere).
                 */
                nonvirtual  void    Add (KeyType key, ValueType newElt);
                nonvirtual  void    Add (KeyValuePair<KeyType, ValueType> p);

            public:
                /**
                 */
                template    <typename CONTAINER_OF_KEYVALUE>
                nonvirtual  void    AddAll (const CONTAINER_OF_KEYVALUE& items);
                template    <typename COPY_FROM_ITERATOR_KEYVALUE>
                nonvirtual  void    AddAll (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end);

            public:
                /**
                 *  NOTE- calling Remove(Key) when the key is not found is perfectly legal.
                 *
                 *  @todo CONSIDER:::
                 *      TBD in the case of Remove() on in iterator???? Probably should have consistent
                 *      answers but review Remove()for other containers as well.
                 */
                nonvirtual  void    Remove (KeyType key);
                nonvirtual  void    Remove (const Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& i);

            public:
                /**
                 */
                nonvirtual  void    RemoveAll ();
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                nonvirtual  void    RemoveAll (const CONTAINER_OF_PAIR_KEY_T& items);
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                nonvirtual  void    RemoveAll (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

            public:
                /**
                 *  This function should work for any container which accepts
                 *  (ITERATOR_OF<KeyValuePair<Key,Value>>,ITERATOR_OF<KeyValuePair<Key,Value>>) OR
                 *  (ITERATOR_OF<pair<Key,Value>>,ITERATOR_OF<pair<Key,Value>>).
                 *
                 *  These As<> overloads also may require the presence of an insert(ITERATOR, Value) method
                 *  of CONTAINER_OF_Key_T.
                 *
                 *  So - for example, Sequence<KeyValuePair<KeyType,ValueType>>, map<KeyType,ValueType>,
                 *  vector<pair<KeyType,ValueType>>, etc...
                 */
                template    <typename CONTAINER_OF_Key_T>
                nonvirtual  CONTAINER_OF_Key_T As () const;

            private:
                template    <typename CONTAINER_OF_Key_T>
                nonvirtual  CONTAINER_OF_Key_T  As_ (typename enable_if <is_convertible <typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, VALUE_TYPE>>::value, int>::type usesInsertPair = 0) const;
                template    <typename   CONTAINER_OF_Key_T>
                nonvirtual  CONTAINER_OF_Key_T  As_ (typename enable_if < !is_convertible <typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, VALUE_TYPE>>::value, int >::type usesDefaultIterableImpl = 0) const;

            public:
                /**
                 *  Two Mappings are considered equal if they contain the same elements (keys) and each key is associated
                 *  with the same value. There is no need for the items to appear in the same order for the two Mappings to
                 *  be equal. There is no need for the backends to be of the same underlying representation either (stlmap
                 *  vers linkedlist).
                 *
                 *  Equals is commutative().
                 *
                 *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
                 */
                template    <typename VALUE_EQUALS_COMPARER = Common::ComparerWithEquals<VALUE_TYPE>>
                nonvirtual  bool    Equals (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const;

            public:
#if     qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy
                inline  static  bool    DefaultAccumulateArg_ (ValueType l, ValueType r)               {        return l + r;       }
#endif

            public:
                /**
                 *  EXPERIMENTAL API/UTILITY -- added 2015-01-16 to test
                 */
#if     qCompilerAndStdLib_DefaultParamerOfStaticFunctionWithValueLambdaOfWithEmptyClosure_Buggy
                nonvirtual  void    Accumulate (KeyType key, ValueType newValue, const function<ValueType(ValueType, ValueType)>& f = DefaultAccumulateArg_, ValueType initialValue = {})
#else
                nonvirtual  void    Accumulate (KeyType key, ValueType newValue, const function<ValueType(ValueType, ValueType)>& f = [] (ValueType l, ValueType r) -> ValueType { return l + r; }, ValueType initialValue = {})
#endif
                {
                    Add (key, f (LookupValue (key, initialValue), newValue));
                }

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 */
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                nonvirtual  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>   operator+ (const CONTAINER_OF_PAIR_KEY_T& items) const;

            public:
                /**
                 */
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                nonvirtual  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& operator+= (const CONTAINER_OF_PAIR_KEY_T& items);

            public:
                /**
                 */
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                nonvirtual  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& operator-= (const CONTAINER_OF_PAIR_KEY_T& items);

            protected:
#if     qCompilerAndStdLib_SafeReadRepAccessor_mystery_Buggy
                template    <typename REP_SUB_TYPE>
                struct  _SafeReadRepAccessor  {
                    typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_ReadOnlyIterableIRepReference    fAccessor;
                    _SafeReadRepAccessor (const Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>* s)
                        : fAccessor (s->_GetReadOnlyIterableIRepReference ())
                    {
                    }
                    nonvirtual  const REP_SUB_TYPE&    _ConstGetRep () const
                    {
                        EnsureMember (fAccessor.cget (), REP_SUB_TYPE);
                        return static_cast<const REP_SUB_TYPE&> (*fAccessor.cget ());   // static cast for performance sake - dynamic cast in Ensure
                    }
                };
#else
                /**
                 */
                template    <typename T2>
                using   _SafeReadRepAccessor = typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template _SafeReadRepAccessor<T2>;
#endif

            protected:
                /**
                 */
                template    <typename T2>
                using   _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<T2>;

            protected:
                nonvirtual  const _IRep&    _ConstGetRep () const;

            protected:
                nonvirtual  void    _AssertRepValidType () const;
            };


            using   Traversal::IteratorOwnerID;


            /**
             *  \brief  Implementation detail for Mapping<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Mapping<T> container API.
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            class   Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep
                : public UpdatableIterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_IRep
#if     !qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
                        , public Traversal::IterableBase::enable_shared_from_this_SharedPtrImplementationTemplate<typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep>
#endif
            {
            protected:
                _IRep () = default;

            public:
                virtual ~_IRep ()  = default;

            protected:
                using   _SharedPtrIRep = typename Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_SharedPtrIRep;

            public:
                virtual _SharedPtrIRep          CloneEmpty (IteratorOwnerID forIterableEnvelope) const                  =   0;
                virtual  Iterable<KeyType>      Keys () const                                                           =   0;
                virtual  Iterable<ValueType>    Values () const                                                         =   0;
                // always clear/set item, and ensure return value == item->IsValidItem());
                // 'item' arg CAN be nullptr
                virtual  bool                   Lookup (KeyType key, Memory::Optional<ValueType>* item) const           =   0;
                virtual  void                   Add (KeyType key, ValueType newElt)                                     =   0;
                virtual  void                   Remove (KeyType key)                                                    =   0;
                virtual  void                   Remove (const Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& i)          =   0;
#if     qDebug
                virtual void                    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const   =   0;
#endif

            protected:
                nonvirtual Iterable<KeyType>    _Keys_Reference_Implementation () const;
                nonvirtual Iterable<ValueType>  _Values_Reference_Implementation () const;
            };


            /**
             *      Syntactic sugar on Equals()
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            bool    operator== (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& lhs, const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs);

            /**
             *      Syntactic sugar on not Equals()
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            bool    operator!= (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& lhs, const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs);


        }
    }
}


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "Mapping.inl"

#endif  /*_Stroika_Foundation_Containers_Mapping_h_ */
