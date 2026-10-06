/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CallbackRegistry_h_
#define _Stroika_Foundation_Execution_CallbackRegistry_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <atomic>
#include <compare>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "Stroika/Foundation/Common/Common.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   https://github.com/SophistSolutions/Stroika/issues/1210 - a choice of what to do with a callback's exception
 */

namespace Stroika::Foundation::Characters {
    class String;
}

namespace Stroika::Foundation::Execution {

    namespace Private_ {
        inline atomic<uint64_t> sCallbackRegistryLastID_{0};
    }

    /**
     *  \brief The callbacks an object calls when something happens - each added, and removed by the ID Add gave it - with the
     *         guarantees that needs when they are called on a thread of the object's own, and added and removed on others.
     *
     *      o   Call holds no lock while a callback runs. So Add never waits for a running callback, and a callback may Add,
     *          Remove (itself too) or Call, or wait for a thread doing so.
     *      o   Once Remove returns, that callback is not running, and is never called again - except that, called from within
     *          that very callback, the call already under way finishes. So Remove waits for a call running on another thread:
     *          do not call it holding a lock that callback takes.
     *      o   An exception from a callback is logged (DbgTrace) and ignored, and the rest are still called - except
     *          Thread::AbortException, which ends the Call, so its thread can end.
     *      o   'Before' and 'after' are as the registry's lock orders them - each Add, Remove and RemoveAll, and the start of
     *          each Call, takes it in turn (from different threads at once, in no defined order). So a Call calls the callbacks
     *          added before it started, and not removed since: one added during a Call is first called by the next.
     *
     *  Its lock is a plain (non-recursive) mutex, held only to read or update the list of callbacks - never while one runs,
     *  so nothing can re-enter it - and so cheap: one uncontended lock to Add or Remove; to Call, one, and two more for each
     *  callback it runs. (A use that cannot afford even that need not use this.)
     *
     *  \note   Defined only for callbacks returning void: Call runs any number of them, so has no one result to give back - and
     *          choosing one (the first's, the last's, or all of them, as boost::signals2's 'combiners' do) is a different
     *          abstraction.
     *
     *  \note   It replaces Execution::Function's job of making callbacks removable - a footgun. A Function equals only copies of
     *          itself (each construction is given a new number), so removing a callback works only with the Function added, or
     *          a copy of it - and nothing enforces that, as a lambda converts to a Function implicitly:
     *          \code
     *              monitor.AddCallback ([this] (const Event& e) { OnEvent_ (e); });
     *              ...
     *              monitor.RemoveCallback ([this] (const Event& e) { OnEvent_ (e); }); // a new Function: removes nothing
     *          \endcode
     *          That compiles, runs, and silently leaves the callback in place - to be called after its 'this' is gone. And the
     *          equality is that wrapper's own, around a copyable function<>, where the standard's wrappers - function<>, C++23's
     *          move_only_function, C++26's copyable_function - compare only to nullptr, by design. Here the registration, not
     *          the callable, has the identity: Add returns an ID, the only thing Remove takes, so there is nothing to get wrong;
     *          and nothing is asked of a callback but to be called (so, once C++23 is the floor, it could be a
     *          move_only_function).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     *
     *  \par Example Usage
     *      \code
     *          CallbackRegistry<void (const Event&)> fOnEvent;     // in an object that calls them from a thread of its own
     *          ...
     *          auto id = fOnEvent.Add ([] (const Event& e) { DbgTrace ("{}"_f, e); });
     *          fOnEvent.Call (e);              // on that thread: each callback, in the order added
     *          fOnEvent.Remove (id);           // once this returns, that callback is not running, and is not called again
     *      \endcode
     */
    template <typename FUNCTION_SIGNATURE>
    class CallbackRegistry;

    template <typename... ARGS>
    class CallbackRegistry<void (ARGS...)> {
    public:
        using Callback = function<void (ARGS...)>;

    public:
        /**
         *  \brief Names a callback Add added, for Remove. No two the same in a process - so one from another CallbackRegistry
         *         removes nothing; nor does ID{}, which names none.
         */
        class ID {
        public:
            constexpr ID () = default;

        public:
            constexpr bool            operator== (const ID& rhs) const  = default;
            constexpr strong_ordering operator<=> (const ID& rhs) const = default;

        public:
            /**
             *  @see Characters::ToString ()
             */
            nonvirtual Characters::String ToString () const;

        private:
            constexpr explicit ID (uint64_t id);

        private:
            uint64_t fID_{0}; // 0: none

        private:
            friend CallbackRegistry;
        };

    public:
        CallbackRegistry ()                        = default;
        CallbackRegistry (const CallbackRegistry&) = delete;

    public:
        /**
         *  \pre no Call under way, on any thread - not just no callback running (@see RemoveAll)
         */
        ~CallbackRegistry ();

    public:
        nonvirtual CallbackRegistry& operator= (const CallbackRegistry&) = delete;

    public:
        /**
         *  \brief Add callback, to be called by each Call from now on (not one already under way). Never waits for a running
         *         callback.
         *
         *  \pre callback != nullptr
         */
        nonvirtual ID Add (const Callback& callback);

    public:
        /**
         *  \brief Once this returns, that callback is not running, and is never called again - except that, called from within
         *         it, the call already under way finishes. Removing one not added, or already removed, does nothing.
         */
        nonvirtual void Remove (ID id) noexcept;

    public:
        /**
         *  \brief Remove every callback: once this returns, none is running, and none is called again - except that, called from
         *         within one, the call already under way finishes (and the rest of that Call calls none).
         *
         *  Why: so an owner can go away while another thread may be calling its callbacks. Once this returns, nothing a callback
         *  uses - the owner, what the callbacks captured - is in use, or will be, so it can be destroyed. And called from within
         *  a callback (an owner its own callback destroys), it returns, rather than waiting forever for that callback to end.
         *
         *  It removes the callbacks added before it, in its lock's order (@see CallbackRegistry). One added after - even by
         *  another thread while this waits for a running callback - is not removed, and each Call from then on calls it, as any
         *  other. So an owner going away must first stop whatever might add one.
         *
         *  \note The registry itself must still outlive any Call under way, which - though it calls no more callbacks - still
         *        takes the registry's lock: destroy it only once the thread calling Call has stopped, or keep it in a shared_ptr
         *        that thread holds too.
         */
        nonvirtual void RemoveAll () noexcept;

    public:
        /**
         *  \brief Call each callback, in the order added, with args - holding no lock while one runs. An exception from a
         *         callback is logged and ignored, except Thread::AbortException.
         */
        nonvirtual void Call (ARGS... args);

    private:
        struct Entry_ {
            Entry_ (ID id, const Callback& callback);
            ID                 fID;
            Callback           fCallback;
            bool               fRemoved{false}; // guarded by fMutex_
            vector<thread::id> fRunningOn;      // guarded by fMutex_: the threads calling it now (the same one, twice, if Calls nest)
        };

    private:
        mutable mutex              fMutex_;
        condition_variable         fCallEnded_; // with fMutex_: for a Remove waiting for a running callback
        vector<shared_ptr<Entry_>> fEntries_;   // guarded by fMutex_
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CallbackRegistry.inl"

#endif /*_Stroika_Foundation_Execution_CallbackRegistry_h_*/
