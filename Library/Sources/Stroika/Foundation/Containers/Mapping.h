/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Mapping_h_
#define _Stroika_Foundation_Containers_Mapping_h_  1

/*
 *  TODO:
 *      @todo   Support thread safety
 *
 *      @todo   Support more backends
 *              Especially HashTable, RedBlackTree, and stlmap, and stlhashmap
 *              And of course change default here
 *
 *      @todo
 *
 *
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/Optional.h"

#include    "Iterable.h"
#include    "Iterator.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      Mapping which allows for the association of two elements, and key and
             *  a value. The key UNIQUELY specifies its associated value.
             *
             *  @see    Dictionary<Key,T>
             */
            template    <typename Key, typename T>
            class   Mapping : public Iterable<pair<Key, T>> {
            private:
                typedef Iterable<pair<Key, T>>  inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

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
                 */
                nonvirtual  Memory::Optional<T> Lookup (Key key) const;
                nonvirtual  bool                Lookup (Key key, T* item) const;

            public:
                /**
                 *  Synonym for Lookup (key).get () != nullptr
                 */
                nonvirtual  bool ContainsKey (Key key) const;

            public:
                /**
                 *  Likely inefficeint for a map, but perhaps helpful. Walks entire list of entires
                 *  and applies operator== on each value, and returns true if contained. Perhpas not
                 *  very useful but symetric to ContainsKey().
                 *
                 *  NOTE: ONLY defined if T supports operator==
                 */
                nonvirtual  bool ContainsValue (T v) const;

            public:
                /**
                 *  Add the association between key and newElt. If key was already associated with something
                 *  else, the association is silently updated, and the size of the iterable does not change.
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
                 */
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                nonvirtual  Mapping<Key, T>& operator+= (const CONTAINER_OF_PAIR_KEY_T& items);


            public:
                /**
                 */
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                nonvirtual  Mapping<Key, T>& operator-= (const CONTAINER_OF_PAIR_KEY_T& items);


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
                virtual void            RemoveAll ()                        =   0;
                virtual  Iterable<Key>  Keys () const                       =   0;
                virtual  bool           Lookup (Key key, T* item) const     =   0;
                virtual  void           Add (Key key, T newElt)             =   0;
                virtual  void           Remove (Key key)                    =   0;
                virtual  void           Remove (Iterator<pair<Key, T>> i)    =   0;
            };


        }
    }
}
#endif  /*_Stroika_Foundation_Containers_Mapping_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Mapping.inl"
