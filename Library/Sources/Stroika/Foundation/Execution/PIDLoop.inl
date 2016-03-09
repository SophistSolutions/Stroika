/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_PIDLoop_inl_
#define _Stroika_Foundation_Execution_PIDLoop_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Sleep.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ************************** PIDLoop<CONTROL_VAR_TYPE> ***************************
             ********************************************************************************
             */
            template    <typename CONTROL_VAR_TYPE>
            PIDLoop<CONTROL_VAR_TYPE>::PIDLoop (const ControlParams& pidParams, Time::DurationSecondsType timeDelta, const function<ValueType()>& measureFunction, const function<(ValueType o)>& outputFunction, ValueType initialSetPoint)
                : fPIDParams_ (pidParams)
                , fTimeDelta_ (timeDelta)
                , fMeasureFunction_ (measureFunction)
                , fOutputFunction_ (outputFunction)
                , fSetPoint_ (initialSetPoint)
            {
            }
            template    <typename CONTROL_VAR_TYPE>
            PIDLoop<CONTROL_VAR_TYPE>::~PIDLoop ()
            {
                if (fThread_) {
                    fThread_->AbortAndWaitTilDone ();
                }
            }
            template    <typename CONTROL_VAR_TYPE>
            inline  auto    PIDLoop<CONTROL_VAR_TYPE>::GetSetPoint () const -> ValueType
            {
                return fSetPoint_;
            }
            template    <typename CONTROL_VAR_TYPE>
            inline  void    PIDLoop<CONTROL_VAR_TYPE>::SetSetPoint (ValueType sp)
            {
                if (sp != fUpdatableParams_->fSetPoint_) {
                    // we cannot predict how fPrevError_/fIntegral_ should change with a new setpoint, so clear them
                    fUpdatableParams_ = { sp, ValueType {}, ValueType {} };
                }
            }
            template    <typename CONTROL_VAR_TYPE>
            void    PIDLoop<CONTROL_VAR_TYPE>::RunDirectly ()
            {
                Time::DurationSecondsType   nextRunAt = Time::GetTickCount ();
                while (true) {
                    SleepUntil (nextRunAt);
                    ValueType   measuredValue = fMeasureFunction_ ();
                    ValueType   error = fUpdatableParams_->fSetPoint_ - measuredValue;
                    fUpdatableParams_->fIntegral_ += error * fTimeDelta_;
                    ValueType   derivative = (error - fPrevError) / fTimeDelta_;
                    fUpdatableParams_->fPrevError_ = error;
                    fOutputFunction_ (fPIDParams_.P * error + fPIDParams_.I * integral + fPIDParams_.D * derivative);
                    nextRunAt += fTimeDelta_;
                }
            }
            template    <typename CONTROL_VAR_TYPE>
            Thread    PIDLoop<CONTROL_VAR_TYPE>::RunInThread ()
            {
                Require (fThread_.IsMissing ());
                fThread_ = Thread ([this] () { RunDirectly (); }, Thread::eAutoStart);
                return *fThread_;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_PIDLoop_inl_*/
