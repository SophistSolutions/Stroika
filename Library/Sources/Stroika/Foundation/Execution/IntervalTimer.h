/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_IntervalTimer_h_
#define _Stroika_Foundation_Execution_IntervalTimer_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
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
     * 
     *  Manage interval timers - like the javascript setIntervalTimer API.
     * 
     *  Add and remove timers.
     * 
     *  Support one-shot timers.
     * 
     *  Timers run on arbitrary thread.
     * 
     *  Can shut down manager at any time.
     * 
     *  Can support multiple 'managers' - but then you have to add explicitly. Or use Adder object to add
     *  to default/global IdleTimer manager.
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
        Manager& operator= (const Manager&) = delete;
        Manager& operator= (Manager&&) = default;

    public:
        nonvirtual void AddOneShot (const TimerCallback& intervalTimer, const Time::Duration& when);

    public:
        nonvirtual void AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis = nullopt);

    public:
        /**
         *  Can remove a repeating task, but cannot remove a oneShot, since it might not be there by the time you go to remove it.
         *  \req argument internvalTimer is registered.
         */
        nonvirtual void RemoveRepeating (const TimerCallback& intervalTimer) noexcept;

    public:
        /**
         *  At most one such object may exist. When it does, the IntervalTimer::Manager::sThe is active and usable. Its illegal to call otherwise.
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
    };

    /**
     *  Probably don't use directly. But this is the default implementation of interval timers.
     */
    class IntervalTimer::Manager::DefaultRep : public IRep {
    public:
        DefaultRep ();
        DefaultRep (nullptr_t);

    public:
        virtual void AddOneShot (const TimerCallback& intervalTimer, const Time::Duration& when) override;

    public:
        virtual void AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis) override;

    public:
        virtual void RemoveRepeating (const TimerCallback& intervalTimer) noexcept override;

    private:
        // hidden implementation so details not in header files
        struct Rep_;
        shared_ptr<Rep_> fHiddenRep_;
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
        Adder () = delete;
        Adder (Adder&& src);
        Adder (const Function<void (void)>& f, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis = nullopt);
        Adder (IntervalTimer::Manager& manager, const Function<void (void)>& f, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis = nullopt);

    public:
        ~Adder ();

    public:
        nonvirtual Adder& operator= (const Adder&) = delete;
        nonvirtual Adder& operator                 = (Adder&& rhs);

    private:
        const Time::Duration&    fRepeatInterval_;
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
