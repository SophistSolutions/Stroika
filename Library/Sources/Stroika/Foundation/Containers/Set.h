/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Set_h_
#define _Stroika_Foundation_Containers_Set_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Common/Compare.h"
#include    "../Configuration/Concepts.h"
#include    "../Memory/Optional.h"
#include    "../Memory/SharedByValue.h"
#include    "../Traversal/Iterable.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *
 *  TODO:
 *
 *      @todo   Implement more backends
 *              >   Set_BitString
 *              >   Set_Array
 *              >   Set_LinkedList
 *              >   Set_HashTable
 *              >   Set_RedBlackTree
 *              >   Set_stlset
 *              >   Set_stlunordered_set (really is hashset)
 *              >   Set_Treap
 *
 *      @todo   Add Union/Interesection/Difference methods (and maybe global functions?)
 *              Then see if we can lose the STL/SetUtils code? Gradually ...
 *
 *              maybe redo impls using Stroika impl, and use that to encourage
 *              redo of calling code - or at least for each function in that code - document
 *              how todo equiv in Stroika!
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;


            template    <typename T, typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
            struct   Set_DefaultTraits {
                /**
                 */
                typedef EQUALS_COMPARER EqualsCompareFunctionType;

                RequireConceptAppliesToTypeMemberOfClass(Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);
            };


            /**
             *      The Set class is based on SmallTalk-80, The Language & Its Implementation,
             *      page 148.
             *
             *      The basic idea here is that you cannot have multiple copies of the same
             *      thing into the set (like a mathemactical set).
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T, typename TRAITS = Set_DefaultTraits<T>>
            class   Set : public Iterable<T> {
            private:
                typedef Iterable<T> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of Set<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                typedef TRAITS  TraitsType;

            public:
                /**
                 *  Just a short-hand for the EqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 */
                typedef typename TraitsType::EqualsCompareFunctionType  EqualsCompareFunctionType;

            public:
                /**
                 */
                Set ();
                Set (const Set<T, TRAITS>& s);
                template <typename CONTAINER_OF_T>
                explicit Set (const CONTAINER_OF_T& s);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Set (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit Set (const _SharedPtrIRep& rep);

            public:
                nonvirtual  Set<T, TRAITS>& operator= (const Set<T, TRAITS>& rhs);

            public:
                /**
                 */
                nonvirtual  bool Contains (T item) const;

            public:
                /**
                 *  Like Contains - but a Set<> can use a comparison that only examines a part of T, making it useful to be able to return
                 *  the rest of T.
                 */
                nonvirtual  Memory::Optional<T> Lookup (T item) const;

            public:
                /**
                 */
                nonvirtual  void    Add (T item);

            public:
                /**
                 */
                template    <typename COPY_FROM_ITERATOR_OF_T>
                nonvirtual  void    AddAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    AddAll (const CONTAINER_OF_T& s);

            public:
                /**
                 */
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (const Iterator<T>& i);

            public:
                template    <typename COPY_FROM_ITERATOR_OF_T>
                nonvirtual  void    RemoveAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    RemoveAll (const CONTAINER_OF_T& s);
                nonvirtual  void    RemoveAll ();

            public:
                /*
                 *  Two Sets are considered equal if they contain the same elements (by comparing them with operator==).
                 *
                 *  Equals is commutative().
                 *
                 *  @todo - document computational complexity
                 */
                nonvirtual  bool    Equals (const Set<T, TRAITS>& rhs) const;

            public:
                /**
                 *      +=/-= are equivilent Add(), AddAll(), Remove() and RemoveAll(). They are just syntactic sugar.
                 */
                nonvirtual  Set<T, TRAITS>& operator+= (T item);
                template    <typename CONTAINER_OF_T>
                nonvirtual  Set<T, TRAITS>& operator+= (const CONTAINER_OF_T& items);
                nonvirtual  Set<T, TRAITS>& operator-= (T item);
                template    <typename CONTAINER_OF_T>
                nonvirtual  Set<T, TRAITS>& operator-= (const CONTAINER_OF_T& items);

            public:
                /**
                 *      Syntactic sugar on Equals()
                 */
                nonvirtual  bool    operator== (const Set<T, TRAITS>& rhs) const;

            public:
                /**
                 *      Syntactic sugar on not Equals()
                 */
                nonvirtual  bool    operator!= (const Set<T, TRAITS>& rhs) const;

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for Set<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Set<T, TRAITS> container API.
             */
            template    <typename T, typename TRAITS>
            class   Set<T, TRAITS>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual bool                Equals (const _IRep& rhs) const    =   0;
                virtual bool                Contains (T item) const            =   0;
                virtual Memory::Optional<T> Lookup (T item) const              =   0;
                virtual void                RemoveAll ()                       =   0;
                virtual void                Add (T item)                       =   0;
                virtual void                Remove (T item)                    =   0;
                virtual void                Remove (const Iterator<T>& i)      =   0;

                /*
                 *  Reference Implementations (often not used except for ensure's, but can be used for
                 *  quickie backends).
                 *
                 *  Importantly, these are all non-virtual so not actually pulled in or even compiled unless
                 *  the sucblass refers to the method in a subclass virtual override.
                 */
            public:
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
#include    "Set.inl"

#endif  /*_Stroika_Foundation_Containers_Set_h_ */
