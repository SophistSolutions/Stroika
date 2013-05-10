/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_BlockingQueue_h_
#define _Stroika_Foundation_Execution_BlockingQueue_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/Optional.h"

#include    "Exceptions.h"


/*
 * TODO:
 *      @todo   Perhaps rename to Message Queue or EventQueue
 *
 *      @todo   Consider if/how to integrate with Foundation::Containers::Queue
 *
 *      @todo   Add docs on why no WaitForMultipleObjects, and instead use
 *              http://docs.oracle.com/javase/7/docs/api/java/util/concurrent/BlockingQueue.html
 *              (which is already in README file for Foundation::Execution::ReadMe.md
 *
 *      @todo   This - I THINK - essentially replaces the need for WaitForMultipleObjects.
 *              Maybe add utility class that runs threads that wait on each individual object,
 *              and then POST an event to this Q when available. Then the caller can wait on events
 *              for that Q.
 *                  Maybe at least have a utility class that takes a socket set (fd_set) and
 *              posts (but what?) to the Q (maybe utility class templated and takes value to
 *              be posted?) - so does a posix select/poll (careful to be cancelable) - and
 *              posts to event Q as needed.
 *
 *      @todo   Consider linking this to ThreadPools - so that instead of having a single
 *              thread running the Q, you have an entire threadpool. Maybe thats an attachable
 *              attribute of the Q?
 *
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *
             *  SEE http://docs.oracle.com/javase/7/docs/api/java/util/concurrent/BlockingQueue.html
             *
             *  maybe templated like the java one?
             *
             *                      Throws exception    Special value       Blocks              Times out
             *  Insert              add(e)              offer(e)            put(e)              offer(e, time, unit)
             *  Remove              remove()            poll()              take()              poll(time, unit)
             *  Examine             Front()             peek()              not applicable      not applicable
             */
            template    <typename T>
            class   BlockingQueue {
            public:
                /**
                 *  Blocks until item added, and throws if timeout exceeded. About the only way the
                 *  throw can happen is if the Q is full (or timeout is very small).
                 *
                 *  Analagous to the java BlockingQueue<T>::offer() or BlockingQueue<T>::add () method.
                 */
                nonvirtual  void    Add (T e, Time::DurationSecondsType timeout = Time::kInfinite);


            public:
                /**
                 *  Blocks unti item removed, and throws if timeout exceeded.
                 *
                 *  Similar to the java BlockingQueue<T>::take() or BlockingQueue<T>::poll (time) method.
                 */
                nonvirtual  T       Remove (Time::DurationSecondsType timeout = Time::kInfinite);


            public:
                /**
                 *  If the Q is empty, return an empty Optional<T>, and otherwise the same as Remove(0). Not blocking,
                 *  and will not timeout.
                 *
                 *  Analagous to the java BlockingQueue<T>::poll() method.
                 */
                nonvirtual  Memory::Optional<T>     RemoveIfPossible ();


            public:
                /**
                 *  Returns the front element from the Q, if there is one, and an empty Optional<T> if
                 *  there is none.
                 *
                 *  Analagous to the java BlockingQueue<T>::peek() method.
                 */
                nonvirtual  Memory::Optional<T> PeekFront () const;
            };


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BlockingQueue.inl"

#endif  /*_Stroika_Foundation_Execution_BlockingQueue_h_*/
