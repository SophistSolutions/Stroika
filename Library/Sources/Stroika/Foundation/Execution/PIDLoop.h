/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_PIDLoop_h_
#define _Stroika_Foundation_Execution_PIDLoop_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"

#include    "../Time/Realtime.h"
#include    "Synchronized.h"
#include    "Thread.h"



/**
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *
 *      @todo   rough, untested draft
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *      \note   Based on https://en.wikipedia.org/wiki/PID_controller (Discrete implementation)
             *
             *  This code is internally synchronized (except that the caller must handle any synchonizaiton required by the measureFunction and the outputFunction).
             *
             *  \par Example Usage
             *      \code
             *      PIDLoop<> pidLoop { PIDLoop<>::ControlParams { 3, -4.0, 4.0 }, 1.0, [] () -> double { return getTemperature (); }, [] (double voltage) { setDAC (voltage); } };
             *      pidLoop.SetSetPoint (23.3);
             *      pidLoop.RunInThread ();
             *      Sleep (10*60);  // run PID loop 10 minutes, and when PIDLoop goes out of scope, auto-terminates
             *      \endcode
             *
             */
            template    <typename CONTROL_VAR_TYPE = double>
            class   PIDLoop {
            public:
                using   ValueType = CONTROL_VAR_TYPE;

            public:
                struct  ControlParams {
                    ValueType   P;
                    ValueType   I;
                    ValueType   D;
                };

            public:
                /**
                 *  Measure function produces PV = ProcessVariable, and SP - SetPoint is target value
                 */
                PIDLoop () = delete;
                PIDLoop (const PIDLoop&) = delete;
                PIDLoop (const ControlParams& pidParams, Time::DurationSecondsType timeDelta, const function<ValueType()>& measureFunction, const function<(ValueType o)>& outputFunction, ValueType initialSetPoint = {});
                PIDLoop& operator= (const PIDLoop&) = delete;

            public:
                ~PIDLoop ();

            public:
                /**
                 */
                nonvirtual  ValueType    GetSetPoint () const;

            public:
                /**
                 */
                nonvirtual  void    SetSetPoint (ValueType sp);

            public:
                /**
                 *  Typically this is what you would do, and recieve the Thread object, to cancel (Abort)
                 *
                 *  \req only called once.
                 *
                 *  If ever run, PIDLoop DTOR automatically terminates Run loop and waits for thread to terminate.
                 */
                nonvirtual  Thread    RunInThread ();

            public:
                /**
                 *  Run this function - in the body of your thread. Interupt with a thread abort.
				 *
				 *	Use this if you want to control thread usage yourself. Otherwise, try @see RunInThread
                 */
                nonvirtual  void    RunDirectly ();

            private:
                ControlParams               fPIDParams_;
                Time::DurationSecondsType   fTimeDelta_;
                function<ValueType()>       fMeasureFunction_;
                function<(ValueType o)>     fOutputFunction_;
                Synchronized<ValueType>     fSetPoint_ = {};
                ValueType                   fPrevError_ = {};
                ValueType                   fIntegral_ = {};
                Optional<Thread>            fThread_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "PIDLoop.inl"

#endif  /*_Stroika_Foundation_Execution_PIDLoop_h_*/
