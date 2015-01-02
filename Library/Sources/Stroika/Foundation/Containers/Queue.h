/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Queue_h_
#define _Stroika_Foundation_Containers_Queue_h_  1

#include    "../StroikaPreComp.h"

#include    "../Common/Compare.h"
#include    "../Execution/Synchronized.h"
#include    "../Memory/Optional.h"
#include    "../Traversal/Iterable.h"

#include    "UpdatableIterable.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 *
 *  TODO:
 *
 *
 *      @todo   Consider adding 'max-size' feature to Q. With this set, attempt to add item past
 *              max size would throw. This can be handy with stuff like blocking q? and probably
 *              other cases as well (when you dont want q to grow indefinitely).
 *
 *      @todo   REVIEW BlockingQueue<> code - and add variants returning Optional - so that BlockingQueue.
 *              can be done without a critical section.
 *
 *      @todo   Embelish docs about iteration order, and order of interpretation of Iteratorbased copy CTOR
 *              and then implement properly. Maybe add AddAll() method? Or EnqueAll??
 *
 *      @todo   Select carefully thoguht through principle descriptive documemtatnion (probably: HEAD/TAIL)
 *              and use that docuemntation PRINCIPLALLY THORUHGOUT THE API (and class docs).
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
             *  A Queue is a first-in-first-out (FIFO) data structure, where elements are arranged
             *  n well-ordered fashion, from HEAD to TAIL.
             *
             *  The HEAD if the Queue is where elements are removed. The TAIL of the queue is where
             *  elements enter the queue.
             *
             *  Queues always iterate from Head (aka front) to Tail: the same order as removals
             *  would encounter items, dequeing them.
             *
             *  @see Sedgewick, 30-31.(@todo CHECK REFERNECE)
             *
             *  Related classes include Deques, which allow addition and removal at
             *  either end, and PriorityQueues, which allow removal based on the priority
             *  assigned to an item.
             *
             *  @see Deque<T> - which allow addition and removal at either end
             *  @see PriorityQueues<T, TRAITS> - which allow removal based on the priority
             *          assigned to an item.
             *
             * Notes:
             *      <<< TODO - OBSOLETE - DONT DOCUMENT HERE WHAT IS DEFAULT. INSTEAD - Add
             *      @see link to FACTORY - which tells you what is the default!!!
             *
             *      @todo also - that doc about circular array is perhaps what we SHOUDL use by default
             *      but NYI
             *
             *      We currently default to the circular array implementation, as it is
             *  fastest under most circumstances. One drawback to it is that it has
             *  unpredictable costs for an Enqueue operation. DoubleLinkList is usually
             *  slower, but has very predictable costs.
             *
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T>
            class   Queue : public UpdatableIterable<T> {
            private:
                using   inherited   =   UpdatableIterable<T>;

            public:
                /**
                 *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
                 */
                using   ArchetypeContainerType      =   Queue<T>;

            protected:
                class   _IRep;

            protected:
                using   _QueueSharedPtrIRep  =   typename inherited::template SharedPtrImplementationTemplate<_IRep>;

            protected:
                using   _SharedPtrIRep  =   _QueueSharedPtrIRep;

            public:
                /**
                 *  @todo Document carefully Queue(start,end) iter order - so copy works well! -- SEE DESIGN IN TODO DOCS ABOVE
                 */
                Queue ();
                Queue (const Queue<T>& src);
                Queue (const initializer_list<T>& src);
                template <typename CONTAINER_OF_T>
                explicit Queue (const CONTAINER_OF_T& src);
                template <typename COPY_FROM_ITERATOR_OF_T>
                explicit Queue (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            protected:
                explicit Queue (const _SharedPtrIRep& rep);
                explicit Queue (_SharedPtrIRep&& rep);

#if     qDebug
            public:
                ~Queue ();
#endif

            public:
                /**
                 */
                nonvirtual  Queue<T>& operator= (const Queue<T>& rhs) = default;
#if     qCompilerAndStdLib_DefaultedAssignementOpOfRValueReference_Buggy
                nonvirtual  Queue<T>& operator= (Queue<T> && rhs)
                {
                    inherited::operator= (move (rhs));
                    return *this;
                }
#else
                nonvirtual  Queue<T>& operator= (Queue<T> && rhs) = default;
#endif

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
                nonvirtual  Memory::Optional<T>       HeadIf () const;

            public:
                /**
                 */
                nonvirtual  T       RemoveHead ();

            public:
                /**
                 */
                nonvirtual  Memory::Optional<T>       RemoveHeadIf ();

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
                 *
                 */
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    AddAllToTail (const CONTAINER_OF_T& s);
                template    <typename COPY_FROM_ITERATOR_OF_T>
                nonvirtual  void    AddAllToTail (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

            public:
                /**
                 */
                nonvirtual  void    RemoveAll ();

            public:
                /**
                 *  Two Queues are considered equal if they contain the same elements (by comparing them
                 *  with EQUALS_COMPARER (which defaults to operator== (T,T))
                 *  in exactly the same order (iteration).
                 *
                 *  If == is predefined, you can just call Equals() - but if its not, or if you wish
                 *  to compare with an alternative comparer, just pass it as a template parameter.
                 *
                 *  Equals is commutative().
                 *
                 *  Computational Complexity: O(N)
                 */
                template    <typename EQUALS_COMPARER = Common::ComparerWithEquals<T>>
                nonvirtual  bool    Equals (const Queue<T>& rhs) const;

            public:
                /**
                 *      Syntactic sugar for Equals()
                 *
                 *  \note   This function uses Common::ComparerWithEquals<T>, which in turn uses operator==(T,T). To
                 *          use a different comparer, call Equals() directly.
                 */
                nonvirtual  bool    operator== (const Queue<T>& rhs) const;

            public:
                /**
                 *      Syntactic sugar for not Equals()
                 *
                 *  \note   This function uses Common::ComparerWithEquals<T>, which in turn uses operator==(T,T). To
                 *          use a different comparer, call Equals() directly.
                 */
                nonvirtual  bool    operator!= (const Queue<T>& rhs) const;

            protected:
#if     qCompilerAndStdLib_SafeReadRepAccessor_mystery_Buggy
                template    <typename REP_SUB_TYPE>
                struct  _SafeReadRepAccessor  {
                    typename Iterable<T>::_ReadOnlyIterableIRepReference    fAccessor;
                    _SafeReadRepAccessor (const Iterable<T>* s)
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
                using   _SafeReadRepAccessor = typename Iterable<T>::template _SafeReadRepAccessor<T2>;
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
             *  \brief  Implementation detail for Queue<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Queue<T> container API.
             */
            template    <typename T>
            class   Queue<T>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep () = default;

            protected:
                using   _SharedPtrIRep = typename Queue<T>::_SharedPtrIRep;

            public:
                virtual ~_IRep () = default;

            public:
                virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const                  =   0;
                virtual void                AddTail (T item)                                                        =   0;
                virtual T                   RemoveHead ()                                                           =   0;
                virtual Memory::Optional<T> RemoveHeadIf ()                                                         =   0;
                virtual T                   Head () const                                                           =   0;
                virtual Memory::Optional<T> HeadIf () const                                                         =   0;
#if     qDebug
                virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const   =   0;
#endif
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
