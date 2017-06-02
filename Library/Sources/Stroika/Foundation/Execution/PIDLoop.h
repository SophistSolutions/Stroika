/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_PIDLoop_h_
#define _Stroika_Foundation_Execution_PIDLoop_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"

#include "../Characters/String.h"
#include "../Time/Realtime.h"
#include "Synchronized.h"
#include "Thread.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *
 *      @todo   rough, untested draft
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /**
             *      \note   Based on https://en.wikipedia.org/wiki/PID_controller (Discrete implementation)
             *
             *  This code is internally synchronized (except that the caller must handle any synchonizaiton required by the measureFunction and the outputFunction).
             *
             *  This code captures the basic logic/implementation of a discrete PID loop. It can be run from an owner-controlled
             *  thread, or internally maintain its own thread to run the loop in (the easiest to use is the later).
             *
             *  \par Example Usage
             *      \code
             *      PIDLoop<> pidLoop { PIDLoop<>::ControlParams { 3, -4.0, 4.0 }, 1.0, [] () -> double { return getTemperature (); }, [] (double voltage) { setDAC (voltage); } };
             *      pidLoop.SetSetPoint (23.3);
             *      pidLoop.RunInThread ();
             *      Sleep (1*60);   // run PID loop 1 minutes
             *      pidLoop.SetSetPoint (40.0);             // then raise temperature
             *      Sleep (10*60);  // then run PID loop 10 minutes, and when PIDLoop goes out of scope, auto-terminates
             *      \endcode
             *
             *  \note   To have code which preflights or cleanps up after the PIDLoop, create your own subclass of PIDLoop<>, and do your
             *          initialization in the CTOR, and cleanup in the DTOR, so leverage and respects the RAII.
             *
             */
            template <typename CONTROL_VAR_TYPE = double>
            class PIDLoop {
            public:
                using ValueType = CONTROL_VAR_TYPE;

            public:
                struct ControlParams {
                    ControlParams () = default;
                    ControlParams (ValueType p, ValueType i, ValueType d);
                    ValueType P{};
                    ValueType I{};
                    ValueType D{};

                    nonvirtual bool operator== (const ControlParams& rhs) const;
                    nonvirtual bool operator!= (const ControlParams& rhs) const;

                    /**
                      *  @see Characters::ToString ();
                      */
                    nonvirtual Characters::String ToString () const;
                };

            public:
                /**
                 *  Measure function produces PV = ProcessVariable, and SP - SetPoint is target value
                 */
                PIDLoop ()               = delete;
                PIDLoop (const PIDLoop&) = delete;
                PIDLoop (const ControlParams& pidParams, Time::DurationSecondsType timeDelta, const function<ValueType ()>& measureFunction, const function<void(ValueType o)>& outputFunction, ValueType initialSetPoint = {});
                nonvirtual PIDLoop& operator= (const PIDLoop&) = delete;

            public:
                /**
                 *  If any existing PIDLoop thread is running (due to RunInThead call), this will abort that thread
                 *  and wait for it to terminate before the PIDLoop destructor completes. If its calling
                 *  a measure or output function, this means those must respect the thread abort call or a hang can
                 *  result.
                 */
                ~PIDLoop ();

            public:
                /**
                 */
                nonvirtual ValueType GetSetPoint () const;

            public:
                /**
                 *  This can be called anytime, and the PIDLoop will automatically adjust.
                 */
                nonvirtual void SetSetPoint (ValueType sp);

            public:
                /**
                 *  Typically this is what you would do, and recieve the Thread object, to cancel (Abort)
                 *
                 *  \req only called once.
                 *
                 *  If ever run, PIDLoop DTOR automatically terminates Run loop and waits for thread to terminate.
                 */
                nonvirtual Thread RunInThread ();

            public:
                /**
                 *  Run this function - in the body of your thread. Interupt with a thread abort.
                 *
                 *  Use this if you want to control thread usage yourself. Otherwise, try @see RunInThread
                 */
                nonvirtual void RunDirectly ();

            private:
                ControlParams             fPIDParams_;
                Time::DurationSecondsType fTimeDelta_; // time between loop iterations
                function<ValueType ()>    fMeasureFunction_;
                function<void(ValueType o)> fOutputFunction_;
                struct UpdatableParams_ {
                    ValueType fSetPoint_  = {};
                    ValueType fPrevError_ = {};
                    ValueType fIntegral_  = {};
                };
                Synchronized<UpdatableParams_> fUpdatableParams_;
                Memory::Optional<Thread>       fThread_;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "PIDLoop.inl"

#endif /*_Stroika_Foundation_Execution_PIDLoop_h_*/
