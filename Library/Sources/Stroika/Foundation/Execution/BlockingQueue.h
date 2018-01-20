/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_BlockingQueue_h_
#define _Stroika_Foundation_Execution_BlockingQueue_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "../Containers/Queue.h"
#include "../Memory/Optional.h"

#include "ConditionVariable.h"
#include "Exceptions.h"
#include "Synchronized.h"
#include "WaitableEvent.h"

/*
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   Consider if RemoveHead () should throw other exception at EOF?
 *
 *      @todo   Add method to "CopyQueue" which takes a SNAPSHOT of the current Queue and returns it.
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

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /**
             *
             *  SEE http://docs.oracle.com/javase/7/docs/api/java/util/concurrent/BlockingQueue.html
             *
             *                      Throws exception    Special value       Blocks              Times out
             *  Insert              add(e)              offer(e)            put(e)              offer(e, time, unit)
             *  Remove              remove()            poll()              take()              poll(time, unit)
             *  Examine             Front()             peek()              not applicable      not applicable
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
             *
             *  \par Example Usage (RegressionTest10_BlockingQueue_)
             *      \code
             *      enum { START = 0, END = 100 };
             *      int expectedValue = (START + END) * (END - START + 1) / 2;
             *      int counter =   0;
             *      BlockingQueue<function<void()>> q;
             *
             *      Verify (q.GetLength () == 0);
             *
             *      Thread::Ptr  producerThread = Thread::New (
             *          [&q, &counter] () {
             *              for (int incBy = START; incBy <= END; ++incBy) {
             *                  q.AddTail ([&counter, incBy] () { counter += incBy; });
             *              }
             *              q.EndOfInput ();
             *          },
             *          Thread::eAutoStart,
             *          String { L"Producer" }
             *      );
             *      Thread::Ptr  consumerThread = Thread::New (
             *          [&q] () {
             *              while (true) {
             *                  function<void()>    f   =   q.RemoveHead ();
             *                  f();
             *              }
             *          },
             *          Thread::eAutoStart,
             *          String { L"Consumer" }
             *      );
             *      Time::DurationSecondsType killAt = 10.0 + Time::GetTickCount ();
             *      while (counter != expectedValue and Time::GetTickCount () < killAt) {
             *          Execution::Sleep (500ms);
             *      }
             *      Verify (counter == expectedValue);
             *      producerThread.WaitForDone ();      // producer already set to run off the end...
             *      consumerThread.WaitForDone ();      // consumer will end due to exception reading from end
             *      \endcode
             */
            template <typename T>
            class BlockingQueue {
            public:
                /*
                 *  Note - you may want to pass in a specific queue object, to require use of a particular concrete implementation
                 *  for the Queue (such as one that doesnt allocate memory). But when contructing a blocking Q (even with another Q)
                 *  the 'useQueue' must be empty.
                 */
                BlockingQueue () = default;
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
                 *
                 *  \note this is illegal to call (assertion error) if EndOfInput () has been called on this BlockingQueue.
                 */
                nonvirtual void AddTail (const T& e, Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                /**
                 *  Cause any future calls to AddTail () to be illegal, and any pending (while BlockingQueue empty) calls to RemoveHead to not block but
                 *  throw a timeout error (no matter the timeout provided).
                 *
                 *  \note This doesn't delete the current entries in the blocking queue, so they will still get consumed. This just prevents
                 *        the Q from blocking while its being emptied out. This is like adding the special value 'EOF' to the end of Q.
                 */
                nonvirtual void EndOfInput ();

            public:
                /**
                 *  This returns true iff EndOfInput () has been called. It does NOT imply the BlockingQueue is empty.
                 *
                 *  This routine exist because there is no other non-blocking way to check (peek) at see if you are at end of input.
                 */
                nonvirtual bool EndOfInputHasBeenQueued () const;

            public:
                /**
                 *  This returns true iff EndOfInput () has been called and the BlockingQueue is empty.
                 *
                 *  \note Equivilent to EndOfInputHasBeenQueued () and empty ()
                 *
                 *  \note   Once this is true, it will always remain true.
                 *
                 *  \note   This function is non-blocking.
                 */
                nonvirtual bool QAtEOF () const;

            public:
                /**
                 *  Blocks until item removed, and throws if timeout exceeded.
                 *
                 *  If there are currently no items in the Q, this may wait indefinitely (up to timeout provided).
                 *
                 *  If there are no available entries, and EndOfInput () has been called, this will throw a timeout exception
                 *  no matter what the timeout value given.
                 *
                 *  Similar to the java BlockingQueue<T>::take() or BlockingQueue<T>::poll (time) method.
                 *
                 *  @see RemoveHeadIfPossible()
                 */
                nonvirtual T RemoveHead (Time::DurationSecondsType timeout = Time::kInfinite);

            public:
                /**
                 *  Like RemoveHead() except that on timeout, returns empty Optional<T> instead
                 *  of throwing.
                 *
                 *  If there is an entry at the head of the Q, return it immediately. Wait up til
                 *  'timeout' seconds for an entry to appear. Return 'missing' value if none appears.
                 *
                 *  If timeout == 0 (the default) this amounts to peeking (but with remove), and never waits.
                 *
                 *  Analagous to the java BlockingQueue<T>::poll () method.
                 */
                nonvirtual Memory::Optional<T> RemoveHeadIfPossible (Time::DurationSecondsType timeout = 0);

            public:
                /**
                 *  Returns the front element from the Q, if there is one, and an empty Optional<T> if
                 *  there is none (without blocking).
                 *
                 *  Analagous to the java BlockingQueue<T>::peek() method.
                 */
                nonvirtual Memory::Optional<T> PeekHead () const;

            public:
                /**
                 *  Returns true if the Q contains no items. Equivalent to PeekHead ().IsEmpty ()
                 */
                nonvirtual bool empty () const;

            public:
                /**
                 *  Returns the number of elements in the blocking queue (zero if empty).
                 */
                nonvirtual size_t GetLength () const;

            public:
                /**
                 *  Alias for GetLength()
                 */
                nonvirtual size_t length () const;

            public:
                /**
                 */
                nonvirtual void clear ();

            private:
                mutable ConditionVariable<> fCondtionVariable_;
                bool                        fEndOfInput_{false};
                Containers::Queue<T>        fQueue_;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BlockingQueue.inl"

#endif /*_Stroika_Foundation_Execution_BlockingQueue_h_*/
