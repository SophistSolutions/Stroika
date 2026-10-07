/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_IntervalTimer_h_
#define _Stroika_Foundation_Execution_IntervalTimer_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <functional>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/UniqueID.h"
#include "Stroika/Foundation/Containers/KeyedCollection.h"
#include "Stroika/Foundation/Time/Duration.h"
#include "Stroika/Foundation/Time/Realtime.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
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
     *  \note Easiest way to use is IntervalTimer::Adder - see constructor example below
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
         *  \pre TimerCallback must be cancelable!
         */
        using TimerCallback = function<void ()>;

    public:
        class Manager;

    public:
        class Adder;

    public:
        /**
         *  \brief Names a timer Manager::AddOneShot or AddRepeating added - for RemoveRepeating. No two the same in a process, so
         *         one from another Manager names none of this one's; nor does TimerID{}.
         *
         *  \note Why an ID, and not the callback, to name a timer by: @see CallbackRegistry (the same callback can be added twice,
         *        too - two timers).
         */
        using TimerID = Common::UniqueID<IntervalTimer>;

    public:
        /**
         *  Used for reporting from the IntervalTimer::Manager (e.g. for debugging, to dump the status).
         */
        struct RegisteredTask {
            TimerID                  fID;
            TimerCallback            fCallback;
            Time::TimePointSeconds   fCallNextAt;
            optional<Time::Duration> fFrequency; // if missing, this is a one-shot event
            optional<Time::Duration> fHysteresis;

        public:
            /**
             *  @see Characters::ToString ()
             */
            nonvirtual Characters::String ToString () const;
        };

    private:
        struct Key_Extractor_ {
            TimerID operator() (const RegisteredTask& r) const
            {
                return r.fID;
            };
        };

    public:
        using RegisteredTaskCollection =
            Containers::KeyedCollection<RegisteredTask, TimerID, Containers::KeyedCollection_DefaultTraits<RegisteredTask, TimerID, Key_Extractor_>>;
    };

    /**
     *  Calls the timers added to it, when due. Its backend is an IRep - DefaultRep (one thread, from its first timer on) unless
     *  another is given; Manager::sThe (made by a Manager::Activator) is the one an Adder adds to, unless given another.
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
         *  \brief Add a timer to be called once after duration when - named by the TimerID returned
         *
         *  \pre intervalTimer valid function ptr (not null)
         *  \pre when >= 0
         */
        nonvirtual TimerID AddOneShot (const TimerCallback& intervalTimer, const Time::Duration& when);

    public:
        /**
         *  \brief Add a timer to be called repeatedly after duration repeatInterval - named by the TimerID returned, for
         *         RemoveRepeating
         *
         *  \pre intervalTimer valid function ptr (not null)
         *  \pre repeatInterval >= 0
         *  \pre hysteresis == nullopt or hysteresis >= 0
         */
        nonvirtual TimerID AddRepeating (const TimerCallback& intervalTimer, const Time::Duration& repeatInterval,
                                         const optional<Time::Duration>& hysteresis = nullopt);

    public:
        /**
         *  Can remove a repeating task, but cannot remove a oneShot, since it might not be there by the time you go to remove it.
         *
         *  Once this returns, the timer's callback is not running, and is not called again: if it is running now, this waits for
         *  that call to finish - unless called from that callback itself (on the timer's thread), when that call finishes after.
         *  So do not call this holding a lock that callback takes.
         *
         *  \pre timer is registered (here)
         */
        nonvirtual void RemoveRepeating (TimerID timer) noexcept;

    public:
        /**
         */
        nonvirtual RegisteredTaskCollection GetAllRegisteredTasks () const;

    public:
        /**
         *  \brief  Having explicit activator object allows for users to control the starting/stopping of facility in a managed fashion.
         * 
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
         *  \pre (Debug::AppearsDuringMainLifetime ()); during activator lifetime
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
     *  The Manager names each timer (its TimerID) - and a rep keeps it by that name.
     */
    class IntervalTimer::Manager::IRep {
    public:
        virtual ~IRep () = default;

    public:
        virtual void AddOneShot (TimerID timer, const TimerCallback& intervalTimer, const Time::Duration& when) = 0;

    public:
        virtual void AddRepeating (TimerID timer, const TimerCallback& intervalTimer, const Time::Duration& repeatInterval,
                                   const optional<Time::Duration>& hysteresis) = 0;

    public:
        /**
         *  \brief As Manager::RemoveRepeating says: once it returns, the timer's callback is not running, and is not called again
         */
        virtual void RemoveRepeating (TimerID timer) noexcept = 0;

    public:
        virtual RegisteredTaskCollection GetAllRegisteredTasks () const = 0;
    };

    /**
     *  Probably don't use directly. But this is the default implementation of interval timers.
     */
    class IntervalTimer::Manager::DefaultRep : public IRep {
    public:
        DefaultRep ();

    public:
        virtual void AddOneShot (TimerID timer, const TimerCallback& intervalTimer, const Time::Duration& when) override;

    public:
        virtual void AddRepeating (TimerID timer, const TimerCallback& intervalTimer, const Time::Duration& repeatInterval,
                                   const optional<Time::Duration>& hysteresis) override;

    public:
        virtual void RemoveRepeating (TimerID timer) noexcept override;

    public:
        virtual RegisteredTaskCollection GetAllRegisteredTasks () const override;

    private:
        // hidden implementation so details not in header files
        struct Rep_;
        shared_ptr<Rep_> fHiddenRep_;
    };

    /**
     *  \brief Adder adds the given function object to the (for now default; later optionally explicit) IntervalTimer manager, and
     *         when its destroyed, the timer is removed.
     * 
     *  While the timer is registered, it will be called periodically from some arbitrary thread.
     * 
     *  Easiest way to add/remove idle manager. Construct one and its lifetime matches time when callback is potentially aftive.
     *  Destroying the Adder removes the timer (@see Manager::RemoveRepeating): once the destructor returns, the callback is not
     *  running, and is not called again - so do not destroy one holding a lock its callback takes (the callback itself may
     *  destroy it). Be sure lifetime of these guys inside lifetime of main.
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
        using RunImmediatelyFlag::eDontRunImmediately;
        using RunImmediatelyFlag::eRunImmediately;

    public:
        /**
         *  \pre (but unenforced) - lifetime of manager must be > that of created Adder
         *  \note if no manager specified, IntervalTimer::Manager::sThe is used.
         * 
         *  \par Example Usage
         *      \code
         *          namespace {
         *              unique_ptr<IntervalTimer::Adder>    sIntervalTimerAdder_;
         *          }
         *          Activator::Activator ()
         *          {
         *              sIntervalTimerAdder_ = make_unique<IntervalTimer::Adder> (
         *                  [] () { sKeepCachedMonitorsUpToDate_.DoOnce (); }
         *                  , 1min
         *                  , IntervalTimer::Adder::eRunImmediately);
         *          }
         *          Activator::~Activator ()
         *          {
         *              sIntervalTimerAdder_.reset ();
         *          }
         *      \endcode
         */
        Adder () = delete;
        Adder (Adder&& src) noexcept;
        Adder (const TimerCallback& f, const Time::Duration& repeatInterval, const optional<Time::Duration>& hysteresis = nullopt);
        Adder (const TimerCallback& f, const Time::Duration& repeatInterval, RunImmediatelyFlag runImmediately,
               const optional<Time::Duration>& hysteresis = nullopt);
        Adder (IntervalTimer::Manager& manager, const TimerCallback& f, const Time::Duration& repeatInterval,
               const optional<Time::Duration>& hysteresis = nullopt);
        Adder (IntervalTimer::Manager& manager, const TimerCallback& f, const Time::Duration& repeatInterval,
               RunImmediatelyFlag runImmediately, const optional<Time::Duration>& hysteresis = nullopt);

    public:
        ~Adder ();

    public:
        /**
         *  \brief Removes this Adder's own timer, and takes over rhs's (not adding it again: rhs, moved from, removes nothing).
         */
        nonvirtual Adder& operator= (Adder&& rhs) noexcept;
        nonvirtual Adder& operator= (const Adder&) = delete;

    public:
        /**
         */
        nonvirtual TimerCallback GetCallback () const;

    private:
        IntervalTimer::Manager* fManager_; // null if moved from
        TimerCallback           fFunction_;
        TimerID                 fTimer_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "IntervalTimer.inl"

#endif /*_Stroika_Foundation_Execution_IntervalTimer_h_*/
