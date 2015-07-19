/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bijection_h_
#define _Stroika_Foundation_Containers_Bijection_h_  1

#include    "../StroikaPreComp.h"

#include    <utility>

#include    "../Common/Compare.h"
#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"
#include    "../Execution/Synchronized.h"
#include    "../Memory/Optional.h"
#include    "../Traversal/Iterable.h"

#include    "UpdatableIterable.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *  TODO:
 *
 *      @todo   Started using concepts on CTORs, but make sure THIS supports the appropriate new Container
 *              concepts and that it USES that for the appropriate overloaded constructors.
 *
 *      @todo   Support EachWith?
 *
 *      @todo   Bijection_InjectivityViolationPolicy not respected.
 *
 *      @todo   Preimage() and Image () NTI
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



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;


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
                using   DomainEqualsCompareFunctionType     =   DOMAIN_EQUALS_COMPARER;

                RequireConceptAppliesToTypeMemberOfClass(Concept_EqualsCompareFunctionType, DomainEqualsCompareFunctionType);

                /**
                 * only defined optionally...(see what can call this - gen list here @todo)
                 */
                using   RangeEqualsCompareFunctionType      =   RANGE_EQUALS_COMPARER;

                /**
                 *  Define typedef for this Bijection traits object (so other traits can generically allow recovery of the
                 *  underlying Bijection's TRAITS objects.
                 */
                using   BijectionTraitsType                 =   Bijection_DefaultTraits<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>;

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
             *  See also http://en.wikipedia.org/wiki/Bijection
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
             *
             *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
             */
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS = Bijection_DefaultTraits<DOMAIN_TYPE, RANGE_TYPE>>
            class   Bijection : public UpdatableIterable<pair<DOMAIN_TYPE, RANGE_TYPE>> {
            private:
                using   inherited   =   UpdatableIterable<pair<DOMAIN_TYPE, RANGE_TYPE>>;

            protected:
                class   _IRep;

            protected:
                using   _BijectionSharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            protected:
                using   _SharedPtrIRep  =   _BijectionSharedPtrIRep;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType  =   Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>;

            public:
                /**
                 */
                using   TraitsType      =   TRAITS;

            public:
                /**
                 */
                using   DomainType      =   DOMAIN_TYPE;

            public:
                /**
                 */
                using   RangeType       =   RANGE_TYPE;

            public:
                /**
                 *  Just a short-hand for the DomainEqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using   DomainEqualsCompareFunctionType     =   typename TraitsType::DomainEqualsCompareFunctionType;

            public:
                /**
                 *  Just a short-hand for the RangeEqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                using   RangeEqualsCompareFunctionType      =   typename TraitsType::RangeEqualsCompareFunctionType;

            public:
                /**
                 *  This constructor creates a concrete Bijection object, either empty, or initialized with any argument
                 *  values.
                 *
                 *  The underlying data structure of the Bijection is defined by @see Concrete::Bijection_Factory<>
                 */
                Bijection ();
                Bijection (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& src);
                Bijection (Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>&& src);
                Bijection (const std::initializer_list<pair<DOMAIN_TYPE, RANGE_TYPE>>& src);
                template    < typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF = typename enable_if < Configuration::has_beginend<CONTAINER_OF_PAIR_KEY_T>::value && !std::is_convertible<const CONTAINER_OF_PAIR_KEY_T*, const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>*>::value >::type >
                explicit Bijection (const CONTAINER_OF_PAIR_KEY_T& src);
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                explicit Bijection (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

            protected:
                explicit Bijection (const _SharedPtrIRep& src);
                explicit Bijection (_SharedPtrIRep&& src);

#if     qDebug
            public:
                ~Bijection ();
#endif

            public:
                /**
                 */
                nonvirtual  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& operator= (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs) =   default;
#if     qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy
                nonvirtual  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& operator= (Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS> && rhs)
                {
                    inherited::operator= (move (rhs));
                    return *this;
                }
#else
                nonvirtual  Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& operator= (Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS> && rhs) = default;
#endif

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
                nonvirtual  Iterable<DomainType>   Preimage () const;

            public:
                /**
                 *  Image () returns an Iterable object with just the key part of the Bijection.
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
                nonvirtual  Iterable<RangeType>   Image () const;

            public:
                /**
                 *  Note - as since Lookup/1 returns an Optional<T> - it can be used very easily to provide
                 *  a default value on Lookup (so for case where not present) - as in:
                 *      returm m.Lookup (key).Value (putDefaultValueHere);
                 *
                 *  Note - for both overloads taking an item pointer, the pointer may be nullptr (in which case not assigned to).
                 *  But if present, will always be assigned to if Lookup returns true (found). And for the optional overload
                 *      \req    Ensure (item == nullptr or returnValue == item->IsPresent());
                 *
                 *  @see   LookupValue ()
                 *  @see   InverseLookup ()
                 *  @see   InverseLookupValue ()
                 */
                nonvirtual  Memory::Optional<RangeType> Lookup (DomainType key) const;
                nonvirtual  bool                        Lookup (DomainType key, Memory::Optional<RangeType>* item) const;
                nonvirtual  bool                        Lookup (DomainType key, RangeType* item) const;
                nonvirtual  bool                        Lookup (DomainType key, nullptr_t) const;

            public:
                /**
                 *  Always safe to call. If result empty/missing, returns argument 'default' or 'sentinal' value.
                 *
                 *  @see   Lookup ()
                 *  @see   InverseLookup ()
                 *  @see   InverseLookupValue ()
                 */
                nonvirtual  RangeType   LookupValue (DomainType key, RangeType defaultValue = RangeType ()) const;

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
                 *
                 *  @see   Lookup ()
                 *  @see   LookupValue ()
                 *  @see   InverseLookupValue ()
                 */
                nonvirtual  Memory::Optional<DomainType>    InverseLookup (RangeType key) const;
                nonvirtual  bool                            InverseLookup (RangeType key, Memory::Optional<DomainType>* item) const;
                nonvirtual  bool                            InverseLookup (RangeType key, DomainType* item) const;
                nonvirtual  bool                            InverseLookup (RangeType key, nullptr_t) const;

            public:
                /**
                 *  Always safe to call. If result empty/missing, returns argument 'default' or 'sentinal' value.
                 *
                 *  @see   Lookup ()
                 *  @see   LookupValue ()
                 *  @see   InverseLookup ()
                 */
                nonvirtual  DomainType   InverseLookupValue (RangeType key, DomainType defaultValue = DomainType ()) const;

            public:
                /**
                 *  Synonym for (Lookup (v).IsPresent ()) or Preimage ().Contains (v)
                 */
                nonvirtual  bool    ContainsDomainElement (DomainType key) const;

            public:
                /**
                 *  Synonym for (InverseLookup (v).IsPresent ()) or Image ().Contains (v)
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
                nonvirtual  void    Add (pair<DomainType, RangeType> p);
                template    < typename KEYVALUEPAIR, typename ENABLE_IF_TEST = typename enable_if < !is_convertible<const KEYVALUEPAIR&, pair<DomainType, RangeType>>::value, void >::type >
                nonvirtual  void    Add (KEYVALUEPAIR p);

            public:
                /**
                 */
                template    <typename CONTAINER_OF_KEYVALUE, typename ENABLE_IF = typename enable_if < Configuration::has_beginend<CONTAINER_OF_KEYVALUE>::value>::type >
                nonvirtual  void    AddAll (const CONTAINER_OF_KEYVALUE& items);
                template    <typename COPY_FROM_ITERATOR_KEYVALUE>
                nonvirtual  void    AddAll (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end);

            public:
                /**
                 */
                nonvirtual  void    Remove (const Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>& i);

            public:
                /**
                 *  This removes any mapping from 'd' to anything. It is not an error if 'd' isn not already in the domain.
                 */
                nonvirtual  void    RemoveDomainElement (DomainType d);

            public:
                /**
                 *  This removes any mapping from anything to 'r'. It is not an error if 'r' isn not already in the range.
                 */
                nonvirtual  void    RemoveRangeElement (RangeType r);

            public:
                /**
                 */
                nonvirtual  void    RemoveAll ();

            public:
                /*
                 *  Return a mapping - in the reverse direction (a Bijection is (restricted) form of mapping. The type of
                 *  TARGET_CONTAINER can be either a Bijection (in the opposite direction as the source), or
                 *  A Mapping<> (also in the reverse direction).
                 *
                 *  Note - this returns a copy (by value) of this Bijections data.
                 */
                template    <typename   TARGET_CONTAINER = Bijection<RANGE_TYPE, DOMAIN_TYPE>>
                nonvirtual  TARGET_CONTAINER    Inverse () const;

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
                template    <typename CONTAINER_PAIR_RANGE_DOMAIN>
                nonvirtual  CONTAINER_PAIR_RANGE_DOMAIN As () const;

            public:
                /**
                 *  Two Bijections are considered equal if they contain the same elements (Preimage) and each key is associated
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

            protected:
#if     qCompilerAndStdLib_SafeReadRepAccessor_mystery_Buggy
                template    <typename REP_SUB_TYPE>
                struct  _SafeReadRepAccessor  {
                    typename Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::_ReadOnlyIterableIRepReference    fAccessor;
                    _SafeReadRepAccessor (const Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>* s)
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
                using   _SafeReadRepAccessor = typename Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::template _SafeReadRepAccessor<T2>;
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
             *  \brief  Implementation detail for Bijection<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Bijection<T> container API.
             */
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            class   Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_IRep : public UpdatableIterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::_IRep {
            private:
                using   inherited   =   typename UpdatableIterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::_IRep;

            protected:
                using   _SharedPtrIRep = typename Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>::_SharedPtrIRep;

            protected:
                _IRep () = default;

            public:
                virtual ~_IRep () = default;

            public:
                virtual _SharedPtrIRep          CloneEmpty (IteratorOwnerID forIterableEnvelope) const                      =   0;
                virtual bool                    Equals (const _IRep& rhs) const                                             =   0;
                virtual  Iterable<DomainType>   Preimage () const                                                           =   0;
                virtual  Iterable<RangeType>    Image () const                                                              =   0;
                // always clear/set item, and ensure return value == item->IsValidItem());
                // 'item' arg CAN be nullptr
                virtual  bool                   Lookup (DomainType key, Memory::Optional<RangeType>* item) const            =   0;
                virtual  bool                   InverseLookup (RangeType key, Memory::Optional<DomainType>* item) const     =   0;
                virtual  void                   Add (DomainType key, RangeType newElt)                                      =   0;
                virtual  void                   RemoveDomainElement (DomainType d)                                          =   0;
                virtual  void                   RemoveRangeElement (RangeType r)                                            =   0;
                virtual  void                   Remove (const Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>& i)                   =   0;

#if     qDebug
                virtual void    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const                       =   0;
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


            /**
             *      Syntactic sugar on Equals()
             */
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            nonvirtual  bool    operator== (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& lhs, const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs);

            /**
             *      Syntactic sugar on not Equals()
             */
            template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
            bool    operator!= (const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& lhs, const Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>& rhs);


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
