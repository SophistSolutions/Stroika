/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_IntervalTimer_h_
#define _Stroika_Foundation_Execution_IntervalTimer_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "../Containers/Collection.h"
#include "../Execution/Function.h"
#include "../Time/Duration.h"
#include "../Time/Realtime.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Execution {

    /**
     *  \brief Manage interval timers - like the javascript setIntervalTimer API.
     * 
     *  o   Add and remove timers.
     *  o   Support one-shot timers.
     *  o   Timers run on arbitrary thread.
     *  o   Can shut down manager at any time.
     *  o   Can support multiple 'managers' - but then you have to add explicitly. Or use Adder object to add
     *      to default/global IdleTimer manager.
     * 
     *  \note https://stackoverflow.com/questions/33234403/using-setinterval-in-c
     * 
     *  note TODO:
     *      \todo  sometimes want to use threadpool/ Sometime async; sometimes a single thread iff stuff
     */
    class IntervalTimer {
    public:
        /**
         *  Note: these timers CAN throw, and SHOULD throw if interrupted, but the Idle Manager will 'eat' those
         *  exceptions.
         *  
         *  \req TimerCallback must be cancelable!
         */
        using TimerCallback = Function<void ()>;

    public:
        class Manager;

    public:
        class Adder;

    public:
        struct RegisteredTask;
    };

    /**
     *  Very early draft implementation. Later allow plugin 'IManager' API to allow for different backend approaches,
     *  and allow adders to optionally target different managers.
     * 
     *  \note Timers can only be added after the start of main (), and must be removed before the end of main.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     *
     */
    class IntervalTimer::Manager {
    public:
        class IRep;

    public:
        class DefaultRep;

    public:
        /**
         *  Argument to Manager can be nullptr, but then not usable.
         */
        Manager (const Manager&) = delete;
        Manager (Manager&&)      = default;
        Manager (const shared_ptr<IRep>& rep);

    public:
        ~Manager () = default;

    public:
        nonvirtual Manager& operator= (const Manager&) = delete;
        nonvirtual Manager& operator= (Manager&&)      = default;

    public:
        /**
         *  \brief Add a timer to be called once after duration when
         * 
         *  \req intervalTimer valid funciton ptr (not null)
         *  \req when >= 0
         */
        nonvirtual void AddOneShot (const TimerCallback& intervalTimer, const Time::Duration& when);

    public:
        /**
         *  \brief Add a timer to be called repeatedly after duration repeatInterval
         * 
         *  \req intervalTimer valid funciton ptr (not null)
         *  \req repeatInterval >= 0
         *  \req hysteresis == nullopt or hysteresis >= 0
         */
        nonvirtual void AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis = nullopt);

    public:
        /**
         *  Can remove a repeating task, but cannot remove a oneShot, since it might not be there by the time you go to remove it.
         * 
         *  \req argument internvalTimer is registered.
         */
        nonvirtual void RemoveRepeating (const TimerCallback& intervalTimer) noexcept;

    public:
        nonvirtual Containers::Collection<RegisteredTask> GetAllRegisteredTasks () const;

    public:
        /**
         *  At most one such object may exist. When it does, the IntervalTimer::Manager::sThe is active and usable. 
         *  Its illegal to call otherwise.
         * 
         *  \par Example Usage
         *      \code
         *          main () {
         *              ...
         *              // near the beginning, before IntervalManager used
         *              Execution::IntervalTimer::Manager::Activator intervalTimerMgrActivator;
         *      \endcode
         *
         *  \req (Debug::AppearsDuringMainLifetime ()); during activator lifetime
         */
        struct Activator {
            Activator ();
            ~Activator ();
        };

    public:
        /**
         *  Default interval timer, but you can specify others.
         */
        static Manager sThe;

    private:
        shared_ptr<IRep> fRep_;
    };

    /**
     */
    class IntervalTimer::Manager::IRep {
    public:
        virtual ~IRep () = default;

    public:
        virtual void AddOneShot (const TimerCallback& intervalTimer, const Time::Duration& when) = 0;

    public:
        virtual void AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis) = 0;

    public:
        virtual void RemoveRepeating (const TimerCallback& intervalTimer) noexcept = 0;

    public:
        virtual Containers::Collection<RegisteredTask> GetAllRegisteredTasks () const = 0;
    };

    /**
     *  Probably don't use directly. But this is the default implementation of interval timers.
     */
    class IntervalTimer::Manager::DefaultRep : public IRep {
    public:
        DefaultRep ();

    public:
        virtual void AddOneShot (const TimerCallback& intervalTimer, const Time::Duration& when) override;

    public:
        virtual void AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis) override;

    public:
        virtual void RemoveRepeating (const TimerCallback& intervalTimer) noexcept override;

    public:
        virtual Containers::Collection<RegisteredTask> GetAllRegisteredTasks () const override;

    private:
        // hidden implementation so details not in header files
        struct Rep_;
        shared_ptr<Rep_> fHiddenRep_;
    };

    /**
     *  Just used for reporting from the ItervalTimer::Manager (e.g. for debugging, to dump the status).
     */
    struct IntervalTimer::RegisteredTask {
        Execution::Function<void (void)> fCallback;
        Time::DurationSecondsType        fCallNextAt;
        optional<Time::Duration>         fFrequency; // if missing, this is a one-shot event

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual Characters::String ToString () const;
    };

    /**
     *  \brief Adder adds the given function object to the (for now default; later optionally explicit) IntervalTimer manager, and
     *         when its destroyed, the timer is removed.
     * 
     *  While the timer is registered, it will be called peridocially from some arbitrary thread.
     * 
     *  Easiest way to add/remove idle manager. Construct one and its lifetime matches time when callback is potentially aftive.
     *  Destruction of Adder object removes from the Q. Be sure lifetime of these guys inside lifetime of main.
     */
    class IntervalTimer::Adder {
    public:
        /**
         */
        enum class RunImmediatelyFlag {
            eDontRunImmediately,
            /**
             *  If specified as argument to Adder, then the callback function 'f' will be invoked DIRECTLY from constructor once
             *  as well as being added as a repeated item.
             * 
             *  Pretty commonly, if we have a task you want done every x seconds, you will also want it done IMMEDIATELY
             *  as well.
             */
            eRunImmediately
        };

    public:
        static constexpr RunImmediatelyFlag eDontRunImmediately = RunImmediatelyFlag::eDontRunImmediately;
        static constexpr RunImmediatelyFlag eRunImmediately     = RunImmediatelyFlag::eRunImmediately;

    public:
        /**
         *  \req (but unenforced) - lifetime of manager must be > that of created Adder
         */
        Adder () = delete;
        Adder (Adder&& src) noexcept;
        Adder (const Function<void (void)>& f, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis = nullopt);
        Adder (const Function<void (void)>& f, const Time::Duration& repeatInterval, RunImmediatelyFlag runImmediately, const optional<Time::Duration>& hysteresis = nullopt);
        Adder (IntervalTimer::Manager& manager, const Function<void (void)>& f, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis = nullopt);
        Adder (IntervalTimer::Manager& manager, const Function<void (void)>& f, const Time::Duration& repeatInterval, RunImmediatelyFlag runImmediately, const optional<Time::Duration>& hysteresis = nullopt);

    public:
        ~Adder ();

    public:
        nonvirtual Adder& operator= (const Adder&) = delete;
        nonvirtual Adder& operator= (Adder&& rhs) noexcept;

    public:
        /**
         */
        nonvirtual Function<void (void)> GetCallback () const;

    private:
        Time::Duration           fRepeatInterval_;
        optional<Time::Duration> fHysteresis_;
        IntervalTimer::Manager*  fManager_;
        Function<void (void)>    fFunction_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IntervalTimer.inl"

#endif /*_Stroika_Foundation_Execution_IntervalTimer_h_*/
