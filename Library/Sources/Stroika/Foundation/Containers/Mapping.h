/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Mapping_h_
#define _Stroika_Foundation_Containers_Mapping_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"
#include    "../Memory/Optional.h"

#include    "Iterable.h"
#include    "Iterator.h"



/*
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *
 *      @todo   Use TRAITS mechanism - like with Bag<>
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


            /**
             *      Mapping which allows for the association of two elements, and key and
             *  a value. The key UNIQUELY specifies its associated value.
             *
             *  @see    SortedMapping<Key,T>
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             */
            template    <typename Key, typename T>
            class   Mapping : public Iterable<pair<Key, T>> {
            public:
                RequireElementTraitsInClass(RequireOperatorEquals, Key);

            private:
                typedef Iterable<pair<Key, T>>  inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                typedef Key     KeyType;
            public:
                typedef T       ValueType;

            public:
                /**
                 */
                Mapping ();
                Mapping (const Mapping<Key, T>& m);
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                explicit Mapping (const CONTAINER_OF_PAIR_KEY_T& cp);
                template    <typename COPY_FROM_ITERATOR_KEY_T>
                explicit Mapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

            protected:
                explicit Mapping (const _SharedPtrIRep& rep);

            public:
#if     qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers
                nonvirtual  Mapping<Key, T>& operator= (const Mapping<Key, T>& src) =   default;
#else
                nonvirtual  Mapping<Key, T>& operator= (const Mapping<Key, T>& src);
#endif

            public:
                /**
                 *  Note this method may not return a collection which is sorted. Note also, the
                 *  returned value is a copy of the keys (by value) - at least logically (implementations
                 *  maybe smart enough to use lazy copying)
                 */
                nonvirtual  Iterable<Key>   Keys () const;

            public:
                /**
                 *  Note - as since Lookup/1 returns an Optional<T> - it can be used very easily to provide
                 *  a default value on Lookup (so for case where not present) - as in:
                 *      returm m.Lookup (key).Value (putDefaultValueHere);
                 */
                nonvirtual  Memory::Optional<T> Lookup (Key key) const;
                nonvirtual  bool                Lookup (Key key, T* item) const;

            public:
                /**
                 *  Synonym for not (Lookup (key).empty ())
                 */
                nonvirtual  bool    ContainsKey (Key key) const;

            public:
                /**
                 *  Likely inefficeint for a map, but perhaps helpful. Walks entire list of entires
                 *  and applies operator== on each value, and returns true if contained. Perhpas not
                 *  very useful but symetric to ContainsKey().
                 *
                 *  \req RequireElementTraitsInMethod(RequireOperatorEquals, T);
                 */
                nonvirtual  bool    ContainsValue (T v) const;

            public:
                /**
                 *  Add the association between key and newElt. If key was already associated with something
                 *  else, the association is silently updated, and the size of the iterable does not change.
                 *  Also - we guarantee that even if the association is different, if the key has not changed,
                 *  then the iteration order is not changed (helpful for AddAll() semantics, and perhaps elsewhere).
                 */
                nonvirtual  void    Add (Key key, T newElt);

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
                nonvirtual  void    Remove (Key key);
                nonvirtual  void    Remove (const Iterator<pair<Key, T>>& i);

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
                 *  be equal.
                 *
                 *  Equals is commutative().
                 *
                 *  Note - this computation MAYBE very expensive, and not optimized (maybe do better in a future release - see TODO).
                 */
                nonvirtual  bool    Equals (const Mapping<Key, T>& rhs) const;

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            public:
                /**
                 */
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                nonvirtual  Mapping<Key, T>& operator+= (const CONTAINER_OF_PAIR_KEY_T& items);

            public:
                /**
                 */
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                nonvirtual  Mapping<Key, T>& operator-= (const CONTAINER_OF_PAIR_KEY_T& items);

            public:
                /**
                 *      Syntactic sugar on Equals()
                 */
                nonvirtual  bool    operator== (const Mapping<Key, T>& rhs) const;

            public:
                /**
                 *      Syntactic sugar on not Equals()
                 */
                nonvirtual  bool    operator!= (const Mapping<Key, T>& rhs) const;

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
            template    <typename Key, typename T>
            class   Mapping<Key, T>::_IRep : public Iterable<pair<Key, T>>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual bool            Equals (const _IRep& rhs) const     =   0;
                virtual void            RemoveAll ()                        =   0;
                virtual  Iterable<Key>  Keys () const                       =   0;
                virtual  bool           Lookup (Key key, T* item) const     =   0;
                virtual  void           Add (Key key, T newElt)             =   0;
                virtual  void           Remove (Key key)                    =   0;
                virtual  void           Remove (Iterator<pair<Key, T>> i)   =   0;

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
