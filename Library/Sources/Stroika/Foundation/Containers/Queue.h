/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Queue_h_
#define _Stroika_Foundation_Containers_Queue_h_  1

#include    "../StroikaPreComp.h"

#include    "../Common/Compare.h"
#include    "../Traversal/Iterable.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *
 *  TODO:
 *
 *      @todo   Embelish docs about iteration order, and order of interpretation of Iteratorbased copy CTOR
 *              and then implement properly. Maybe add AddAll() method? Or EnqueAll??
 *
 *      @todo   trial balloon - EnqueueAll(CONTAINER), and Enqueue (STARTIT, ENDIT), enques
 *              items in order from start to end, so that when iterating over teh resulting queue, you
 *              encounter the items in teh order you added them. That makes EnqueueAll(CONTINER) as simple
 *              as EnqueueAll(C.begin(),C.end()).
 *
 *      @todo   Embellish test cases (regression tests), and fix/make sure copying works.
 *
 *      @todo   Review
 *              http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Queue.hh
 *              for any important lackings
 *
 *      @todo   Draw a picture of q Q of people waiting in line (in docs below)
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            using   Traversal::Iterable;
            using   Traversal::Iterator;


            /**
             *  NOTE - Traits for Queue<T, TRAITS> don't NEED an EQUALS_COMPARER, and the default one should
             *  should be fine (never called if never used).
             *
             *  It will only be invoked if you call
             *      o   Queue<T,TRAITS>::Equals ()
             *
             *  This means that
             *      Queue<SOME_TYPE_WITH_NO_OPERATOR_EQUALS> x;
             *      // works FINE, UNTIL you try to call Equals - and at that point you must adjust
             *      // the traits to specify the Equals() compare function.
             *
             */
            template    <typename T, typename EQUALS_COMPARER = Common::ComparerWithEqualsOptionally<T>>
            struct   Queue_DefaultTraits {
                /**
                 */
                typedef EQUALS_COMPARER EqualsCompareFunctionType;
            };


            /**
             *      Standard FIFO (first in - first out) queue. See Sedgewick, 30-31.(CHECK REFERNECE)
             *
             *      Queues always iterate from Head (aka front) to Tail: the same order as removals
             *  would encounter items, dequeing them.
             *
             *      Related classes include Deques, which allow addition and removal at
             *  either end, and PriorityQueues, which allow removal based on the priority
             *  assigned to an item.
             *
             *  @see Deque<T> - which allow addition and removal at either end
             *  @see PriorityQueues<T, TRAITS> - which allow removal based on the priority
             *          assigned to an item.
             *
             * Notes:
             *      We currently default to the circular array implementation, as it is
             *  fastest under most circumstances. One drawback to it is that it has
             *  unpredictable costs for an Enqueue operation. DoubleLinkList is usually
             *  slower, but has very predictable costs.
             *
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T, typename TRAITS = Queue_DefaultTraits<T>>
            class   Queue : public Iterable<T> {
            private:
                typedef Iterable<T> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 *  Just a short-hand for the 'TRAITS' part of Queue<T,TRAITS>. This is often handy to use in
                 *  building other templates.
                 */
                typedef TRAITS  TraitsType;

            public:
                /**
                 *  Just a short-hand for the EqualsCompareFunctionType specified through traits. This is often handy to use in
                 *  building other templates.
                 *
                 *  Note - though the type must exist, the implied 'Equals' function may never be compiled (so can be invalid)
                 *  if you avoid the documented methods (see EqualsCompareFunctionType above).
                 */
                typedef typename TraitsType::EqualsCompareFunctionType  EqualsCompareFunctionType;

            public:
                /**
                 *  @todo Document carefully Queue(start,end) iter order - so copy works well! -- SEE DESIGN IN TODO DOCS ABOVE
                 */
                Queue ();
                Queue (const Queue<T, TRAITS>& q);
                template <typename CONTAINER_OF_T>
                explicit Queue (const CONTAINER_OF_T& q);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Queue (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit Queue (const _SharedPtrIRep& rep);

            public:
                nonvirtual  Queue<T, TRAITS>& operator= (const Queue<T, TRAITS>& rhs);

            public:
                /**
                old DOCS.
                 * Head lets you peek at what would be the result of the next Dequeue. It is
                 * an error to call Head () with an empty Q.
                 */
                nonvirtual  void    AddTail (T item);

            public:
                /**
                 */
                nonvirtual  T       Head () const;

            public:
                /**
                 */
                nonvirtual  T       RemoveHead ();

            public:
                /**
                old DOCS.
                 * Stick the given item at the end of the Q. Since a Q is a LIFO structure,
                 * this item will be the removed (by a DeQueue) operation only after all other
                 * elements of the Q have been removed (DeQueued).
                 */
                nonvirtual  void    Enqueue (T item);       // AddTail

            public:
                /**
                old DOCS.
                 * Remove the first item from the Q. This is an error (assertion) if the Q is
                 * empty. This returns that last most distant (historical/time) item from the Q -
                 * IE the one who has been waiting the longest.
                 *
                 *  @todo maybe add DequeIf() - return Optional<T>?
                 */
                nonvirtual  T       Dequeue ();             //RemoveHead

            public:
                /**
                 */
                nonvirtual  void    RemoveAll ();

            public:
                /*
                 *  Two Queues are considered equal if they contain the same elements (by comparing them
                 *  with EqualsCompareFunctionType from the TRAITS object, which defaults to operator== (T,T))
                 *  in exactly the same order (iteration).
                 *
                 *  Equals is commutative().
                 *
                 *  \req RequireConceptAppliesToTypeInFunction(Concept_EqualsCompareFunctionType, EqualsCompareFunctionType);
                 *
                 *  Computational Complexity: O(N)
                 */
                nonvirtual  bool    Equals (const Queue<T, TRAITS>& rhs) const;

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();

            public:
                /**
                 *      Syntactic sugar for Equals()
                 */
                nonvirtual  bool    operator== (const Queue<T, TRAITS>& rhs) const;

            public:
                /**
                 *      Syntactic sugar for not Equals()
                 */
                nonvirtual  bool    operator!= (const Queue<T, TRAITS>& rhs) const;
            };


            /**
             *  \brief  Implementation detail for Queue<T, TRAITS> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Queue<T, TRAITS> container API.
             */
            template    <typename T, typename TRAITS>
            class   Queue<T, TRAITS>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual void        AddTail (T item)                        =   0;
                virtual T           RemoveHead ()                           =   0;
                virtual T           Head () const                           =   0;
                virtual void        RemoveAll ()                            =   0;
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Queue.inl"

#endif  /*_Stroika_Foundation_Containers_Queue_h_ */
