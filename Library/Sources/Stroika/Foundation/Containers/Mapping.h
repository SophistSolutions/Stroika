/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Mapping_h_
#define _Stroika_Foundation_Containers_Mapping_h_  1

#include    "../StroikaPreComp.h"

#include    "../Common/Compare.h"
#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"
#include    "../Memory/Optional.h"
#include    "../Traversal/Iterable.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Replace pair<Key,T> with KeyValuePair<Key,T>
 *              Documentent advantages (clearer naming .fKey versus .first, and .fValue versus .second)
 *              and easy mapping to/from pair<> stuff (can be methods of KeyValuePair<>).
 *              (SEE NEW Common/KeyValuePair)
 *
 *      @todo   Support more backends
 *              Especially HashTable, RedBlackTree, and stlhashmap
 *              And of course change default here
 *
 *      @todo   Not sure where this note goes - but eventually add "Database-Based" implementation of mapping
 *              and/or extenral file. Maybe also map to DynamoDB, MongoDB, etc... (but not here under Mapping,
 *              other db module would inherit from mapping).
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;


            template    <typename KEY_TYPE, typename VALUE_TYPE, typename KEY_EQUALS_COMPARER = Common::ComparerWithEquals<KEY_TYPE>, typename VALUE_EQUALS_COMPARER = Common::ComparerWithEqualsOptionally<VALUE_TYPE>>
            struct   Mapping_DefaultTraits {
                /**
                 */
                typedef KEY_EQUALS_COMPARER KeyEqualsCompareFunctionType;

                RequireConceptAppliesToTypeMemberOfClass(Concept_EqualsCompareFunctionType, KeyEqualsCompareFunctionType);

                /**
                 * only defined optionally...(see what can call this - gen list here @todo)
                 */
                typedef VALUE_EQUALS_COMPARER ValueEqualsCompareFunctionType;
            };


            /**
             *      Mapping which allows for the association of two elements, and key and
             *  a value. The key UNIQUELY specifies its associated value.
             *
             *  @see    SortedMapping<Key,T>
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS = Mapping_DefaultTraits<KEY_TYPE, VALUE_TYPE>>
            class   Mapping : public Iterable<pair<KEY_TYPE, VALUE_TYPE>> {
            private:
                typedef Iterable<pair<KEY_TYPE, VALUE_TYPE>>  inherited;

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
                typedef KEY_TYPE    KeyType;

            public:
                /**
                 */
                typedef VALUE_TYPE  ValueType;

            public:
                /**
                 *  Just a short-hand for the KeyEqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                typedef typename TraitsType::KeyEqualsCompareFunctionType  KeyEqualsCompareFunctionType;

            public:
                /**
                 *  Just a short-hand for the ValueEqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                typedef typename TraitsType::ValueEqualsCompareFunctionType  ValueEqualsCompareFunctionType;

            public:
                /**
                 */
                Mapping ();
                Mapping (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& m);
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                explicit Mapping (const CONTAINER_OF_PAIR_KEY_T& cp);
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                explicit Mapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

            protected:
                explicit Mapping (const _SharedPtrIRep& rep);

            public:
#if     qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers
                nonvirtual  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& src) =   default;
#else
                nonvirtual  Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& operator= (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& src);
#endif

            public:
                /**
                 *  Note this method may not return a collection which is sorted. Note also, the
                 *  returned value is a copy of the keys (by value) - at least logically (implementations
                 *  maybe smart enough to use lazy copying)
                 */
                nonvirtual  Iterable<KeyType>   Keys () const;

            public:
                /**
                 *  Note - as since Lookup/1 returns an Optional<T> - it can be used very easily to provide
                 *  a default value on Lookup (so for case where not present) - as in:
                 *      returm m.Lookup (key).Value (putDefaultValueHere);
                 */
                nonvirtual  Memory::Optional<ValueType> Lookup (KeyType key) const;
                nonvirtual  bool                        Lookup (KeyType key, ValueType* item) const;

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

            public:
                /**
                 */
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                nonvirtual  void    AddAll (const CONTAINER_OF_PAIR_KEY_T& items);
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                nonvirtual  void    AddAll (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

            public:
                /**
                 *  NOTE- calling Remove(Key) when the key is not found is perfectly legal.
                 *
                 *  @todo CONSIDER:::
                 *      TBD in the case of Remove() on in iterator???? Probably should have consistent
                 *      answers but review Remove()for other containers as well.
                 */
                nonvirtual  void    Remove (KeyType key);
                nonvirtual  void    Remove (const Iterator<pair<KEY_TYPE, VALUE_TYPE>>& i);

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
                 *  Only supported value(s) for CONTAINER_OF_Key_T are:
                 *      o   std::map<Key,T>
                 */
                template    <typename   CONTAINER_OF_Key_T>
                nonvirtual  CONTAINER_OF_Key_T   As () const;
                template    <typename   CONTAINER_OF_Key_T>
                nonvirtual  void   As (CONTAINER_OF_Key_T* into) const;

            public:
                /*
                 *  Two Mappings are considered equal if they contain the same elements (keys) and each key is associated
                 *  with the same value. There is no need for the items to appear in the same order for the two Mappings to
                 *  be equal. There is no need for the backends to be of the same underlying representation either (stlmap
                 *  vers linkedlist).
                 *
                 *  Equals is commutative().
                 *
                 *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
                 */
                nonvirtual  bool    Equals (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const;

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

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

            public:
                /**
                 *      Syntactic sugar on Equals()
                 */
                nonvirtual  bool    operator== (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const;

            public:
                /**
                 *      Syntactic sugar on not Equals()
                 */
                nonvirtual  bool    operator!= (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs) const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for Mapping<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Mapping<T> container API.
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            class   Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep : public Iterable<pair<KEY_TYPE, VALUE_TYPE>>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual bool                Equals (const _IRep& rhs) const                 =   0;
                virtual void                RemoveAll ()                                    =   0;
                virtual  Iterable<KeyType>  Keys () const                                   =   0;
                virtual  bool               Lookup (KeyType key, ValueType* item) const     =   0;
                virtual  void               Add (KeyType key, ValueType newElt)             =   0;
                virtual  void               Remove (KeyType key)                            =   0;
                virtual  void               Remove (Iterator<pair<KEY_TYPE, VALUE_TYPE>> i) =   0;

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
#include    "Mapping.inl"

#endif  /*_Stroika_Foundation_Containers_Mapping_h_ */
