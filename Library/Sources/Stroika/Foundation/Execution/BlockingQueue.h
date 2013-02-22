/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_BlockingQueue_h_
#define _Stroika_Foundation_Execution_BlockingQueue_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"

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
            template    <typename E>
            class   BlockingQueue {
                /**
                 *  Blocks unti item added, and throws if timeout exceeded.
                 *
                 *  This one function corresponds to the java BlockingQueue add/put/offer (with overloading), and right
                 *  now I see no value in offer(e).
                 */
                void Add (E e, Time::DurationSecondsType timeout = Time::kInfinite);

                /**
                 *  Blocks unti item added, and throws if timeout exceeded.
                 *
                 *  This one function corresponds to the java BlockingQueue remove/take/poll (with overloading), and right
                 *  now I see no value in offer(e).
                 */
                void Remove (E e, Time::DurationSecondsType timeout = Time::kInfinite);

                /**
                 * java element()
                 */
                E Front ();
                /// ... just starting to think out - the 'special value' stuff only works if we use Optional...
            };


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_BlockingQueue_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "BlockingQueue.inl"
