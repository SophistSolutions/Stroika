/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_PIDLoop_h_
#define _Stroika_Foundation_Execution_PIDLoop_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"

#include    "../Time/Realtime.h"



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
			 *		\note	Based on https://en.wikipedia.org/wiki/PID_controller (Discrete implementation)
             *
             */
            template    <typename CONTROL_VAR_TYPE = double>
            class	PIDLoop {
			public:
                using   ValueType = CONTROL_VAR_TYPE;
			public:
                struct  ControlParams {
                    ValueType   P;
                    ValueType   I;
                    ValueType   D;
                };
			public:
				// Measure function produces PV = ProcessVariable, and SP - SetPoint is target value
                PIDLoop (const ControlParams& pidParams, const function<ValueType()>& measureFunction, const function<(ValueType o)>& outputFunction, ValueType initialSetPoint = {});

			public:
                void    SetSetPoint (sp)
                {
                    fSetPoint = sp;
                }

			public:
                void    Run ()
                {
                    while (true) {
                        ValueType	measuredValue = measureFunction ();
                        ValueType   error = fSetPoint - measuredValue;
                        ValueType	derivative = (error - fPrevError) / dt;
                        ValueType	output = fPIDParams_.P * error + fPIDParams_.I * integral + fPIDParams_.D * derivitive;
                        outputFunction (output);
                        fPrevErrror = error;
                        wait (dt);
                    }
                }

			private:
                ControlParams	fPIDParams_;
                Time::DurationSecondsType dt;
                ValueType fPrevError = {};
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
