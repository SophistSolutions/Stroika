/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Set_h_
#define _Stroika_Foundation_Containers_Set_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"
#include    "../Memory/SharedByValue.h"

#include    "Iterable.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 *
 *  TODO:
 *
 *      @todo   Use TRAITS mechanism - like with Bag<>
 *
 *      @todo   Implement first draft of code based on
 *              http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Set.hh
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
 *      @todo   Thread safety (/locking)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


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
            template    <typename   T>
            class   Set : public Iterable<T> {
            public:
                RequireElementTraitsInClass(RequireOperatorEquals, T);

            private:
                typedef Iterable<T> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 */
                Set ();
                Set (const Set<T>& s);
                template <typename CONTAINER_OF_T>
                explicit Set (const CONTAINER_OF_T& s);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Set (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit Set (const _SharedPtrIRep& rep);

            public:
                nonvirtual  Set<T>& operator= (const Set<T>& src);

            public:
                /**
                 */
                nonvirtual  bool Contains (T item) const;

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
                 *  \req RequireElementTraitsInMethod(RequireOperatorEquals, T);
                 *
                 *  @todo - document computational complexity
                 */
                nonvirtual  bool    Equals (const Set<T>& rhs) const;

            public:
                /**
                 *      +=/-= are equivilent Add(), AddAll(), Remove() and RemoveAll(). They are just syntactic sugar.
                 */
                nonvirtual  Set<T>& operator+= (T item);
                template    <typename CONTAINER_OF_T>
                nonvirtual  Set<T>& operator+= (const CONTAINER_OF_T& items);
                nonvirtual  Set<T>& operator-= (T item);
                template    <typename CONTAINER_OF_T>
                nonvirtual  Set<T>& operator-= (const CONTAINER_OF_T& items);

            public:
                /**
                 *      Syntactic sugar on Equals()
                 */
                nonvirtual  bool    operator== (const Set<T>& rhs) const;

            public:
                /**
                 *      Syntactic sugar on not Equals()
                 */
                nonvirtual  bool    operator!= (const Set<T>& rhs) const;

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            public:
                /*
                 *  Convert Set<T> losslessly into a standard supported C++ type.
                 *  Supported types include:
                 *      o   set<T>
                 *      o   vector<T>
                 *      o   list<T>
                 *      (maybe any container that takes CTOR (IT BEGIN, IT END) - but dont count on that yet...
                 */
                template    <typename   CONTAINER_OF_T>
                nonvirtual  CONTAINER_OF_T   As () const;
                template    <typename   CONTAINER_OF_T>
                nonvirtual  void    As (CONTAINER_OF_T* into) const;


            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for Set<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Set<T> container API.
             */
            template    <typename T>
            class   Set<T>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual bool    Equals (const _IRep& rhs) const    =   0;
                virtual bool    Contains (T item) const            =   0;
                virtual void    RemoveAll ()                       =   0;
                virtual void    Add (T item)                       =   0;
                virtual void    Remove (T item)                    =   0;
                virtual void    Remove (const Iterator<T>& i)      =   0;

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
