/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_BlockingQueue_h_
#define _Stroika_Foundation_Execution_BlockingQueue_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "../Containers/Queue.h"

#include "ConditionVariable.h"

/*
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   Perhaps have PeekHead() take timeout=0 optional param?
 *
 *      @todo   Consider linking this to ThreadPools - so that instead of having a single
 *              thread running the Q, you have an entire threadpool. Maybe thats an attachable
 *              attribute of the Q?
 *
 */

namespace Stroika::Foundation::Execution {

    /**
     *
     *  SEE http://docs.oracle.com/javase/7/docs/api/java/util/concurrent/BlockingQueue.html
     *
     *                      Throws exception    Special value       Blocks              Times out
     *  Insert              add(e)              offer(e)            put(e)              offer(e, time, unit)
     *  Remove              remove()            poll()              take()              poll(time, unit)
     *  Examine             Front()             peek()              not applicable      not applicable
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     *
     *  \par Example Usage (RegressionTest10_BlockingQueue_)
     *      \code
     *          enum { START = 0, END = 100 };
     *          int expectedValue = (START + END) * (END - START + 1) / 2;
     *          int counter =   0;
     *          BlockingQueue<function<void()>> q;
     *
     *          Verify (q.size () == 0);
     *
     *          Thread::Ptr  producerThread = Thread::New (
     *              [&q, &counter] () {
     *                  for (int incBy = START; incBy <= END; ++incBy) {
     *                      q.AddTail ([&counter, incBy] () { counter += incBy; });
     *                  }
     *                  q.SignalEndOfInput ();
     *              },
     *              Thread::eAutoStart,
     *              "Producer"_k
     *          );
     *          Thread::Ptr  consumerThread = Thread::New (
     *              [&q] () {
     *                  while (true) {
     *                      function<void()>    f   =   q.RemoveHead ();
     *                      f();
     *                  }
     *              },
     *              Thread::eAutoStart,
     *              "Consumer"_k
     *          );
     *          Time::DurationSecondsType killAt = 10.0 + Time::GetTickCount ();
     *          while (counter != expectedValue and Time::GetTickCount () < killAt) {
     *              Execution::Sleep (500ms);
     *          }
     *          Verify (counter == expectedValue);
     *          producerThread.WaitForDone ();      // producer already set to run off the end...
     *          consumerThread.WaitForDone ();      // consumer will end due to exception reading from end
     *      \endcode
     * 
     *  \note Aliases
     *        This could easily be called EventQueue or MessageQueue, as its well suited to those sorts of uses.
     */
    template <typename T>
    class BlockingQueue {
    public:
        /*
         *  Note - you may want to pass in a specific queue object, to require use of a particular concrete implementation
         *  for the Queue (such as one that doesn't allocate memory). But when contructing a blocking Q (even with another Q)
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
         *  \note this is illegal to call (assertion error) if SignalEndOfInput () has been called on this BlockingQueue.
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
        nonvirtual void SignalEndOfInput ();

    public:
        /**
         *  This returns true iff SignalEndOfInput () has been called. It does NOT imply the BlockingQueue is empty.
         *
         *  This routine exist because there is no other non-blocking way to check (peek) at see if you are at end of input.
         */
        nonvirtual bool EndOfInputHasBeenQueued () const;

    public:
        /**
         *  This returns true iff SignalEndOfInput () has been called and the BlockingQueue is empty.
         *
         *  \note Equivalent to EndOfInputHasBeenQueued () and empty ()
         *
         *  \note   Once this is true, it will always remain true.
         *
         *  \note   This function is non-blocking.
         * 
         *  \see also empty ()
         */
        nonvirtual bool QAtEOF () const;

    public:
        /**
         *  Blocks until item removed, and throws if timeout exceeded.
         *
         *  If there are currently no items in the Q, this may wait indefinitely (up to timeout provided).
         *
         *  If there are no available entries, and SignalEndOfInput () has been called, this will throw a Streams::EOFException
         *  no matter what the timeout value given.
         *
         *  Similar to the java BlockingQueue<T>::take() or BlockingQueue<T>::poll (time) method.
         *
         *  @see RemoveHeadIfPossible()
         */
        nonvirtual T RemoveHead (Time::DurationSecondsType timeout = Time::kInfinite);

    public:
        /**
         *  Like RemoveHead() except that on timeout, returns empty optional<T> instead
         *  of throwing.
         *
         *  If there is an entry at the head of the Q, return it immediately. Wait up til
         *  'timeout' seconds for an entry to appear. Return 'missing' value if none appears.
         *
         *  If timeout == 0 (the default) this amounts to peeking (but with remove), and never waits.
         *
         *  Analagous to the java BlockingQueue<T>::poll () method.
         */
        nonvirtual optional<T> RemoveHeadIfPossible (Time::DurationSecondsType timeout = 0);

    public:
        /**
         *  Returns the front element from the Q, if there is one, and an empty optional<T> if
         *  there is none (without blocking).
         *
         *  Analagous to the java BlockingQueue<T>::peek() method.
         */
        nonvirtual optional<T> PeekHead () const;

    public:
        /**
         *  Returns true if the Q contains no items. Equivalent to PeekHead ().empty ()
         * 
         *  \see also QAtEOF ()
         */
        nonvirtual bool empty () const;

    public:
        /**
         *  Returns the number of elements in the blocking queue (zero if empty).
         */
        nonvirtual size_t size () const;

    public:
        /**
         *  Alias for size()
         */
        nonvirtual size_t length () const;

    public:
        /**
         *  Get a copy of the entire owned Queue. NOTE - modifications to the returned copy have no effect on Queue associated 
         *  Queue with the BlockingQueue
         */
        nonvirtual Containers::Queue<T> GetQueue () const;

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

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "BlockingQueue.inl"

#endif /*_Stroika_Foundation_Execution_BlockingQueue_h_*/
