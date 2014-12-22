/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_BlockingQueue_h_
#define _Stroika_Foundation_Execution_BlockingQueue_h_  1

#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "../Configuration/Common.h"
#include    "../Containers/Queue.h"
#include    "../Memory/Optional.h"

#include    "WaitableEvent.h"
#include    "Exceptions.h"



/*
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 * TODO:
 *      @todo   Add method to "CopyQueue" which takes a SNAPSHOT of the current Queue and returns it.
 *
 *      @todo   Add clear example of usage...
 *
 *      @todo   Use condition variables (instead of waitable event?) Or explain why not. Waitable event
 *              uses them, so maybe thats enouf
 *
 *      @todo   Perhaps have PeekHead() take timeout=0 optional param?
 *
 *      @todo   Perhaps rename to Message Queue or EventQueue
 *
 *              Event Q bad name, cuz misleading (not just for events and could be confused with
 *              waitable events).
 *
 *              Just DOOCUMENT that these are common synonyms.
 *
 *      @todo   Consider if/how to integrate with Foundation::Containers::Queue
 *
 *              I'm pretty sure the right answer is to SUBCLASS from Queue, but for AddTail/RemoveHead - and
 *              overloads with the timeout logic.
 *
 *              maybe just templated or otther param? What about priority Queue?
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
             *                      Throws exception    Special value       Blocks              Times out
             *  Insert              add(e)              offer(e)            put(e)              offer(e, time, unit)
             *  Remove              remove()            poll()              take()              poll(time, unit)
             *  Examine             Front()             peek()              not applicable      not applicable
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *
             */
            template    <typename T>
            class   BlockingQueue {
            public:
                /*
                 *  Note - you may want to pass in a specific queue object, to require use of a particular concrete implementation
                 *  for the Queue (such as one that doesnt allocate memory).
                 */
                BlockingQueue ();
                BlockingQueue (const Containers::Queue<T>& useQueue);

            public:
                /**
                 *  Blocks until item added, and throws if timeout exceeded. About the only way the
                 *  throw can happen is if the Q is full (or timeout is very small).
                 *
                 *  Typically this will return almost instantly.
                 *
                 *  Analagous to the java BlockingQueue<T>::put(e) and similar to the java
                 *  BlockingQueue<T>::offer() or BlockingQueue<T>::add () method.
                 */
                nonvirtual  void    AddTail (const T& e, Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                /**
                 *  Blocks until item removed, and throws if timeout exceeded.
                 *
                 *  If there are currently no items in the Q, this may wait indefinitely (up to timeout provided).
                 *
                 *  Similar to the java BlockingQueue<T>::take() or BlockingQueue<T>::poll (time) method.
                 *
                 *  @see RemoveHeadIfPossible()
                 */
                nonvirtual  T       RemoveHead (Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                /**
                 *  If the Q is empty, return an empty Optional<T>, and otherwise the same as Remove(0). Not blocking,
                 *  and will not timeout.
                 *
                 *  Analagous to the java BlockingQueue<T>::poll() method.
                 */
                nonvirtual  Memory::Optional<T>     RemoveHeadIfPossible (Time::DurationSecondsType timeout = 0);

            public:
                /**
                 *  Returns the front element from the Q, if there is one, and an empty Optional<T> if
                 *  there is none (without blocking).
                 *
                 *  Analagous to the java BlockingQueue<T>::peek() method.
                 */
                nonvirtual  Memory::Optional<T> PeekHead () const;

            public:
                /**
                 *  Returns true if the Q contains no items. Equivalent to PeekHead ().IsEmpty ()
                 */
                nonvirtual  bool    empty () const;

            public:
                /**
                 *  Returns the number of elements in the blocking queue (zero if empty).
                 */
                nonvirtual  size_t      GetLength () const;

            public:
                /**
                 *  Alias for GetLength()
                 */
                nonvirtual  size_t      length () const;

            private:
                WaitableEvent           fDataAvailable_;
                Containers::Queue<T>    fQueue_;
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
