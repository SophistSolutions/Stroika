/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_PriorityQueue_h_
#define _Stroika_Foundation_Containers_PriorityQueue_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"



/*
 *
 *
 *  STATUS:
 *      CODE NO WHERE NEAR COMPILING - just rough draft of API based on 1992 Stroika
 *
 *
 *
 *  TODO:
 *
 *      (o)         Implement first draft of code based on
 *                  http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/PriorityQueue.hh
 *
 *      (o)         FIX so can remove top prioty itme to return btoih at a time.
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            typedef uint16_t    Priority;
            const   Priority    kMinPriority    =   kMinUInt16;
            const   Priority    kMaxPriority    =   kMaxUInt16;


            // Someday this should be renamed ...
            template    <typename T>
            class   PQEntry {
            public:
                PQEntry (T item, Priority p);

                T           fItem;
                Priority    fPriority;
            };
            template    <class T>
            Boolean   operator== (const PQEntry<T>& lhs, const PQEntry<T>& rhs);


            /*
             *  PriorityQueues are a like a Queue that allows retrieval based the priority assigned an item.
             *  This priority is given either at the time when the item is Enqueueed to the PriorityQueue, or
             *  by a function. The default function always assigns the lowest possible priority to an item.
             *  Priority start at zero and work upwards, so a zero priority item will be the last item
             *  removed from the PriorityQueue.
             *
             *  PriorityQueues support two kinds of iteration: over type T, or over ProirityQueueEntrys of
             *  type T. A PriorityQueueEntry is a simple structure that couples together the item and its
             *  priority.
             *
             *
             *  PriorityQueues always iterate from highest to lowest priority.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T>
            class   PriorityQueue : public Iterable<pair<T, Priority>> {
            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /*
                 */
                PriorityQueue ();
                PriorityQueue (const Queue<T>& s);

            protected:
                explicit PriorityQueue (const _SharedPtrIRep& rep);

            public:
                nonvirtual  PriorityQueue<T>& operator= (const PriorityQueue<T>& src);

            public:
                nonvirtual  void    RemoveAll ();

            public:
                nonvirtual  Iterable<T> Elements () const;

            public:
                nonvirtual  void        Enqueue (T item, Priority priority);

            public:
                nonvirtual  T           Dequeue ();

            public:
                nonvirtual  T           Head () const;

            public:
                nonvirtual  Priority    TopPriority () const;

            public:
                nonvirtual  void        RemoveHead ();

            public:
                nonvirtual  PriorityQueue<T>&   operator+= (T item);
                nonvirtual  PriorityQueue<T>&   operator+= (IteratorRep<PQEntry<T> >* it);
                nonvirtual  PriorityQueue<T>&   operator-- ();

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for PriorityQueue<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the PriorityQueue<T> container API.
             */
            template    <typename T>
            class   PriorityQueue<T>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual void        Enqueue (T item, Priority priority)     =   0;
                virtual T           Dequeue ()                              =   0;
                virtual T           Head () const                           =   0;
                virtual void        RemoveAll ()                            =   0;
                virtual Iterable<T> Elements () const                       =   0;
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "PriorityQueue.inl"

#endif  /*_Stroika_Foundation_Containers_PriorityQueue_h_ */
