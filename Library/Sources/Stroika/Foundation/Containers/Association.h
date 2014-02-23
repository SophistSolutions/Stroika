/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Association_h_
#define _Stroika_Foundation_Containers_Association_h_  1

#include    "../StroikaPreComp.h"

#include    <map>

#include    "../Common/Compare.h"
#include    "../Common/KeyValuePair.h"
#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"
#include    "../Memory/Optional.h"
#include    "../Traversal/Iterable.h"



/*
 *  \file
 *              ****VERY ROUGH UNUSABLE DRAFT
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
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
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = Common::ComparerWithEquals<KEY_TYPE>, typename VALUE_EQUALS_COMPARER = Common::ComparerWithEqualsOptionally<VALUE_TYPE>>
            struct   Association_DefaultTraits {
                /**
                 */
                using   KeyEqualsCompareFunctionType    =   KEY_EQUALS_COMPARER;

                RequireConceptAppliesToTypeMemberOfClass(Concept_EqualsCompareFunctionType, KeyEqualsCompareFunctionType);

                /**
                 * only defined optionally...(see what can call this - gen list here @todo)
                 */
                using   ValueEqualsCompareFunctionType  =   VALUE_EQUALS_COMPARER;

                /**
                 *  Define typedef for this Association traits object (so other traits can generically allow recovery of the
                 *  underlying Association's TRAITS objects.
                 */
                using   AssociationTraitsType               =    Association_DefaultTraits<KEY_TYPE, VALUE_TYPE, KEY_EQUALS_COMPARER, VALUE_EQUALS_COMPARER>;
            };


            /**
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = Association_DefaultTraits<KEY_TYPE, VALUE_TYPE>>
            class   Association : public Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>> {
            private:
                using   inherited       =   Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>;

            protected:
                class   _IRep;
                using   _SharedPtrIRep  =   typename inherited::template _USING_SHARED_IMPL_<_IRep>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType  =   Association<KEY_TYPE, VALUE_TYPE, TRAITS>;

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
                 *  Just a short-hand for the ValueEqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using   ValueEqualsCompareFunctionType  =   typename TraitsType::ValueEqualsCompareFunctionType;

            public:
                /**
                 *  This constructor creates a concrete Association object, either empty, or initialized with any argument
                 *  values.
                 *
                 *  The underlying data structure of the Association is defined by @see Concrete::Association_Factory<>
                 */
                Association ();
                Association (const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& m);
                Association (const std::initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& m);
                Association (const std::initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& m);
                Association (const std::map<KEY_TYPE, VALUE_TYPE>& m);
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                explicit Association (const CONTAINER_OF_PAIR_KEY_T& cp);
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                explicit Association (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

            protected:
                explicit Association (const _SharedPtrIRep& rep);

#if     qDebug
            public:
                ~Association ();
#endif
            public:
                nonvirtual  Association<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& src) =   default;

            public:
                /**
                 *  Keys () returns an Iterable object with just the key part of the Association.
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
                nonvirtual  Iterable<KeyType>   Keys () const;

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
                 *  Always safe to call. If result empty/missing, returns argument 'default' or 'sentinal' value.
                 */
                nonvirtual  ValueType   LookupValue (KeyType key, ValueType defaultValue = ValueType ()) const;

            public:
                /**
                 *  Synonym for not (Lookup (key).empty ())
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
                 *  Two Associations are considered equal if they contain the same elements (keys) and each key is associated
                 *  with the same value. There is no need for the items to appear in the same order for the two Associations to
                 *  be equal. There is no need for the backends to be of the same underlying representation either (stlmap
                 *  vers linkedlist).
                 *
                 *  Equals is commutative().
                 *
                 *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
                 */
                nonvirtual  bool    Equals (const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const;

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 */
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                nonvirtual  Association<KEY_TYPE, VALUE_TYPE, TRAITS>& operator+= (const CONTAINER_OF_PAIR_KEY_T& items);

            public:
                /**
                 */
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                nonvirtual  Association<KEY_TYPE, VALUE_TYPE, TRAITS>& operator-= (const CONTAINER_OF_PAIR_KEY_T& items);

            public:
                /**
                 *      Syntactic sugar on Equals()
                 */
                nonvirtual  bool    operator== (const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const;

            public:
                /**
                 *      Syntactic sugar on not Equals()
                 */
                nonvirtual  bool    operator!= (const Association<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            using   Traversal::IteratorOwnerID;


            /**
             *  \brief  Implementation detail for Association<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Association<T> container API.
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            class   Association<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep : public Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual bool                Equals (const _IRep& rhs) const                                 =   0;
                virtual void                RemoveAll ()                                                    =   0;
                virtual  Iterable<KeyType>  Keys () const                                                   =   0;
                // always clear/set item, and ensure return value == item->IsValidItem());
                // 'item' arg CAN be nullptr
                virtual  bool               Lookup (KeyType key, Memory::Optional<ValueType>* item) const   =   0;
                virtual  void               Add (KeyType key, ValueType newElt)                             =   0;
                virtual  void               Remove (KeyType key)                                            =   0;
                virtual  void               Remove (Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> i)         =   0;
#if     qDebug
                virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) =   0;
#endif

                /*
                 *  Reference Implementations (often not used except for ensure's, but can be used for
                 *  quickie backends).
                 *
                 *  Importantly, these are all non-virtual so not actually pulled in or even compiled unless
                 *  the sucblass refers to the method in a subclass virtual override.
                 */
            protected:
                nonvirtual bool    _Equals_Reference_Implementation (const _IRep& rhs) const;
            };


        }
    }
}


/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
//#include    "Association.inl"

#endif  /*_Stroika_Foundation_Containers_Association_h_ */
