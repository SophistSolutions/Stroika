/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bijection_h_
#define _Stroika_Foundation_Containers_Bijection_h_  1

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
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *      @todo   VERY PRELIMAINRY - NOT REALLY BIJECTION YET!!!
 *
 *      @todo   Best backend I can think of would be two opposing maps (or hash tables). Discuss with
 *              Sterl to see if he can think of any way todo this that doesn't double the storage
 *              of a regular Mapping (without exhaustive search on lookup from range).
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;
            using   Common::KeyValuePair;


            /**
             */
            enum Bijection_InjectivityViolationPolicy {
                eAssertionError,
                eThrowException,
            };


            /**
             */
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename DOMAIN_EQUALS_COMPARER = Common::ComparerWithEquals<DOMAIN_TYPE>, typename RANGE_EQUALS_COMPARER = Common::ComparerWithEqualsOptionally<RANGE_TYPE>>
            struct   Bijection_DefaultTraits {
                /**
                 */
                typedef DOMAIN_EQUALS_COMPARER DomainEqualsCompareFunctionType;

                RequireConceptAppliesToTypeMemberOfClass(Concept_EqualsCompareFunctionType, DomainEqualsCompareFunctionType);

                /**
                 * only defined optionally...(see what can call this - gen list here @todo)
                 */
                typedef RANGE_EQUALS_COMPARER RangeEqualsCompareFunctionType;

                /**
                 *  Define typedef for this Bijection traits object (so other traits can generically allow recovery of the
                 *  underlying Bijection's TRAITS objects.
                 */
                typedef Bijection_DefaultTraits<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>  BijectionTraitsType;

                /**
                 */
                DEFINE_CONSTEXPR_CONSTANT(Bijection_InjectivityViolationPolicy, InjectivityViolationPolicy, Bijection_InjectivityViolationPolicy::eAssertionError);
            };


            /**
             * \brief   Bijection which allows for the bijective (1-1) association of two elements.
             *
             *  Bijection which allows for the bijective (1-1) association of two elements. This means that one element
             *  of the domain maps to exactly one element of the range, and that one element of the range maps uniquely to
             *  one element of the range, and these mappings happen in a way that the mapping is fully invertable.
             *
             *  Design Notes:
             *      \note   We used Iterable<pair<DOMAIN_TYPE,RANGE_TYPE>> instead of
             *              Iterable<KeyValuePairType<DOMAIN_TYPE,RANGE_TYPE>> because its completly symetric - both
             *              directions the values are keys.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             *  \em Concrete Implementations:
             *      o   @see Concrete::Bijection_LinkedList<>
             *
             *  \em Factory:
             *      @see Concrete::Bijection_Factory<> to see default implementations.
             */
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS = Bijection_DefaultTraits<DOMAIN_TYPE, RANGE_TYPE>>
            class   Bijection : public Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>> {
            private:
                typedef Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>  inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 */
                typedef TRAITS      TraitsType;

            public:
                /**
                 */
                typedef DOMAIN_TYPE    DomainType;

            public:
                /**
                 */
                typedef RANGE_TYPE  RangeType;

            public:
                /**
                 *  Just a short-hand for the DomainEqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                typedef typename TraitsType::DomainEqualsCompareFunctionType  DomainEqualsCompareFunctionType;

            public:
                /**
                 *  Just a short-hand for the RangeEqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                typedef typename TraitsType::RangeEqualsCompareFunctionType  RangeEqualsCompareFunctionType;

            public:
                /**
                 *  This constructor creates a concrete Bijection object, either empty, or initialized with any argument
                 *  values.
                 *
                 *  The underlying data structure of the Bijection is defined by @see Concrete::Bijection_Factory<>
                 */
                Bijection ();
                Bijection (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& src);
                Bijection (const std::initializer_list<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>>& src);
                Bijection (const std::initializer_list<pair<DOMAIN_TYPE, RANGE_TYPE>>& src);
                Bijection (const std::map<DOMAIN_TYPE, RANGE_TYPE>& src);
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                explicit Bijection (const CONTAINER_OF_PAIR_KEY_T& src);
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                explicit Bijection (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

            protected:
                explicit Bijection (const _SharedPtrIRep& rep);

            public:
                nonvirtual  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& operator= (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs) =   default;

            public:
                /**
                & @todo - REPLACE THIS WITH DOMAINELEMENTS() and RangeElements()
                *
                 *  DomainElements () returns an Iterable object with just the key part of the Bijection.
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
                nonvirtual  Iterable<DomainType>   DomainElements () const;

            public:
                /**
                & @todo - REPLACE THIS WITH DOMAINELEMENTS() and RangeElements()
                *
                 *  DomainElements () returns an Iterable object with just the key part of the Bijection.
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
                nonvirtual  Iterable<RangeType>   RangeElements () const;

            public:
                /**
                *   @todo Need InverseLookup () too!
                *
                 *  Note - as since Lookup/1 returns an Optional<T> - it can be used very easily to provide
                 *  a default value on Lookup (so for case where not present) - as in:
                 *      returm m.Lookup (key).Value (putDefaultValueHere);
                 *
                 *  Note - for both overloads taking an item pointer, the pointer may be nullptr (in which case not assigned to).
                 *  But if present, will always be assigned to if Lookup returns true (found). And for the optional overload
                 *      \req    Ensure (item == nullptr or returnValue == item->IsPresent());
                 */
                nonvirtual  Memory::Optional<RangeType> Lookup (DomainType key) const;
                nonvirtual  bool                        Lookup (DomainType key, Memory::Optional<RangeType>* item) const;
                nonvirtual  bool                        Lookup (DomainType key, RangeType* item) const;
                nonvirtual  bool                        Lookup (DomainType key, nullptr_t) const;

            public:
                /**
                 *  Always safe to call. If result empty/missing, returns argument 'default' or 'sentinal' value.
                 */
                nonvirtual  RangeType   LookupValue (DomainType key, RangeType defaultValue = RangeType ()) const;

            public:
                /**
                * @todo rename/use ContainsDomainElement () / ContainsRangeEleemnt()
                 *  Synonym for (Lookup (v).IsPresent ()) or DomainElements ().Contains (v)
                 */
                nonvirtual  bool    ContainsDomainElement (DomainType key) const;

            public:
                /**
                * @todo rename/use ContainsDomainElement () / ContainsRangeEleemnt()
                 *  Likely inefficeint for a map, but perhaps helpful. Walks entire list of entires
                 *  and applies operator== on each value, and returns true if contained. Perhpas not
                 *  very useful but symetric to ContainsKey().
                 *
                 *  \req RequireConceptAppliesToTypeInFunction(RequireOperatorEquals, T);
                 */
                nonvirtual  bool    ContainsRangeElement (RangeType v) const;

            public:
                /**
                 *  Add the association between key and newElt. If key was already associated with something
                 *  else, the association is silently updated, and the size of the iterable does not change.
                 *  Also - we guarantee that even if the association is different, if the key has not changed,
                 *  then the iteration order is not changed (helpful for AddAll() semantics, and perhaps elsewhere).
                 */
                nonvirtual  void    Add (DomainType key, RangeType newElt);
                nonvirtual  void    Add (KeyValuePair<DomainType, RangeType> p);

            public:
                /**
                 */
                template    <typename CONTAINER_OF_KEYVALUE>
                nonvirtual  void    AddAll (const CONTAINER_OF_KEYVALUE& items);
                template    <typename COPY_FROM_ITERATOR_KEYVALUE>
                nonvirtual  void    AddAll (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end);

            public:
                /**
                *   @todo REMOVE() of KEY bad idea for bijection!
                 *  NOTE- calling Remove(Key) when the key is not found is perfectly legal.
                 *
                 *  @todo CONSIDER:::
                 *      TBD in the case of Remove() on in iterator???? Probably should have consistent
                 *      answers but review Remove()for other containers as well.
                 */
                nonvirtual  void    Remove (DomainType key);
                nonvirtual  void    Remove (const Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>& i);

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
                 *  (ITERATOR_OF<pair<Key,Value>>,ITERATOR_OF<pair<Key,Value>>) OR
                 *  (ITERATOR_OF<pair<Key,Value>>,ITERATOR_OF<pair<Key,Value>>).
                 *
                 *  These As<> overloads also may require the presence of an insert(ITERATOR, Value) method
                 *  of CONTAINER_OF_Key_T.
                 *
                 *  So - for example, Sequence<pair<DomainType,RangeType>>, map<DomainType,RangeType>,
                 *  vector<pair<DomainType,RangeType>>, etc...
                 */
                template    <typename CONTAINER_OF_Key_T>
                nonvirtual  CONTAINER_OF_Key_T As () const;

            private:
                template    <typename CONTAINER_OF_Key_T>
                nonvirtual  CONTAINER_OF_Key_T  As_ (typename enable_if <is_convertible <typename CONTAINER_OF_Key_T::value_type, pair<DOMAIN_TYPE, RANGE_TYPE>>::value, int>::type usesInsertPair = 0) const;
                template    <typename   CONTAINER_OF_Key_T>
                nonvirtual  CONTAINER_OF_Key_T  As_ (typename enable_if < !is_convertible <typename CONTAINER_OF_Key_T::value_type, pair<DOMAIN_TYPE, RANGE_TYPE>>::value, int >::type usesDefaultIterableImpl = 0) const;

            public:
                /**
                 *  Two Bijections are considered equal if they contain the same elements (DomainElements) and each key is associated
                 *  with the same value. There is no need for the items to appear in the same order for the two Bijections to
                 *  be equal. There is no need for the backends to be of the same underlying representation either (stlmap
                 *  vers linkedlist).
                 *
                 *  Equals is commutative().
                 *
                 *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
                 */
                nonvirtual  bool    Equals (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs) const;

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 */
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                nonvirtual  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& operator+= (const CONTAINER_OF_PAIR_KEY_T& items);

            public:
                /**
                 */
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                nonvirtual  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& operator-= (const CONTAINER_OF_PAIR_KEY_T& items);

            public:
                /**
                 *      Syntactic sugar on Equals()
                 */
                nonvirtual  bool    operator== (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs) const;

            public:
                /**
                 *      Syntactic sugar on not Equals()
                 */
                nonvirtual  bool    operator!= (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs) const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for Bijection<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Bijection<T> container API.
             */
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            class   Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep : public Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual bool                    Equals (const _IRep& rhs) const                                     =   0;
                virtual void                    RemoveAll ()                                                        =   0;
                virtual  Iterable<DomainType>   DomainElements () const                                             =   0;
                virtual  Iterable<RangeType>    RangeElements () const                                              =   0;
                // always clear/set item, and ensure return value == item->IsValidItem());
                // 'item' arg CAN be nullptr
                virtual  bool                   Lookup (DomainType key, Memory::Optional<RangeType>* item) const    =   0;
                virtual  void                   Add (DomainType key, RangeType newElt)                              =   0;
                virtual  void                   Remove (DomainType key)                                             =   0;
                virtual  void                   Remove (Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>> i)                  =   0;

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
#include    "Bijection.inl"

#endif  /*_Stroika_Foundation_Containers_Bijection_h_ */
