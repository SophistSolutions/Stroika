/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_PIDLoop_h_
#define _Stroika_Foundation_Execution_PIDLoop_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"

#include    "../Time/Realtime.h"
#include    "Sleep.h"
#include    "Synchronized.h"



/**
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *
 *      @todo   quick hack - not ready for prime time
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *      \note   Based on https://en.wikipedia.org/wiki/PID_controller (Discrete implementation)
             *
             *  This code is internally synchronized (except that the caller must handle any synchonizaiton required by the measureFunction and the outputFunction).
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
                /**
                 */
                nonvirtual  ValueType    GetSetPoint () const;

            public:
                /**
                 */
                nonvirtual  void    SetSetPoint (ValueType sp);

            public:
                /**
                 *  Run this function - in the body of your thread. Interupt with a thread abort.
                 */
                nonvirtual  void    Run ();

            private:
                ControlParams               fPIDParams_;
                Time::DurationSecondsType   fTimeDelta_;
                function<ValueType()>       fMeasureFunction_;
                function<(ValueType o)>     fOutputFunction_;
                Synchronized<ValueType>     fSetPoint_ = {};
                ValueType                   fPrevError_ = {};
                ValueType                   fIntegral_ = {};
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
