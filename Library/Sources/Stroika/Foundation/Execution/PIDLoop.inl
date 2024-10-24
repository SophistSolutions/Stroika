/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Sleep.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module (note the extra long name since its in a header)
//#define Stroika_Foundation_Execution_PIDLoop_USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ********************* PIDLoop<CONTROL_VAR_TYPE>::ControlParams *****************
     ********************************************************************************
     */
    template <typename CONTROL_VAR_TYPE>
    inline PIDLoop<CONTROL_VAR_TYPE>::ControlParams::ControlParams (ValueType p, ValueType i, ValueType d)
        : P{p}
        , I{i}
        , D{d}
    {
    }
    template <typename CONTROL_VAR_TYPE>
    inline Characters::String PIDLoop<CONTROL_VAR_TYPE>::ControlParams::ToString () const
    {
        Characters::StringBuilder out;
        out << "{"sv;
        out << "P: "sv << P << "',"sv;
        out << "I: "sv << I << "',"sv;
        out << "D: "sv << D;
        out << "}"sv;
        return out.str ();
    }

    /*
     ********************************************************************************
     ************************** PIDLoop<CONTROL_VAR_TYPE> ***************************
     ********************************************************************************
     */
    template <typename CONTROL_VAR_TYPE>
    PIDLoop<CONTROL_VAR_TYPE>::PIDLoop (const ControlParams& pidParams, Time::DurationSeconds updatePeriod, const function<ValueType ()>& measureFunction,
                                        const function<void (ValueType o)>& outputFunction, ValueType initialSetPoint)
        : fPIDParams_{pidParams}
        , fUpdatePeriod_{updatePeriod}
        , fMeasureFunction_{measureFunction}
        , fOutputFunction_{outputFunction}
    {
        Require (updatePeriod > 0);
        fUpdatableParams_.rwget ()->fSetPoint_ = (initialSetPoint);
    }
    template <typename CONTROL_VAR_TYPE>
    PIDLoop<CONTROL_VAR_TYPE>::PIDLoop (AutoStartFlag, const ControlParams& pidParams, Time::DurationSeconds updatePeriod,
                                        const function<ValueType ()>& measureFunction, const function<void (ValueType o)>& outputFunction,
                                        ValueType initialSetPoint)
        : PIDLoop{pidParams, updatePeriod, measureFunction, outputFunction, initialSetPoint}
    {
        (void)RunInThread ();
    }
    template <typename CONTROL_VAR_TYPE>
    PIDLoop<CONTROL_VAR_TYPE>::~PIDLoop ()
    {
        if (fThread_) {
            fThread_->AbortAndWaitForDone ();
        }
    }
    template <typename CONTROL_VAR_TYPE>
    inline auto PIDLoop<CONTROL_VAR_TYPE>::GetSetPoint () const -> ValueType
    {
        return fUpdatableParams_->fSetPoint_;
    }
    template <typename CONTROL_VAR_TYPE>
    inline void PIDLoop<CONTROL_VAR_TYPE>::SetSetPoint (ValueType sp)
    {
        if (sp != fUpdatableParams_->fSetPoint_) {
            // we cannot predict how fPrevError_/fIntegral_ should change with a new setpoint, so clear them
            fUpdatableParams_ = {sp, ValueType{}, ValueType{}};
        }
    }
    template <typename CONTROL_VAR_TYPE>
    inline auto PIDLoop<CONTROL_VAR_TYPE>::GetControlParams () const -> ControlParams
    {
        return fPIDParams_;
    }
    template <typename CONTROL_VAR_TYPE>
    inline auto PIDLoop<CONTROL_VAR_TYPE>::GetUpdatePeriod () const -> Time::DurationSeconds
    {
        return fUpdatePeriod_;
    }
    template <typename CONTROL_VAR_TYPE>
    void PIDLoop<CONTROL_VAR_TYPE>::RunDirectly ()
    {
        Time::DurationSeconds nextRunAt = Time::GetTickCount ();
        while (true) {
            SleepUntil (nextRunAt);
            try {
                ValueType measuredValue = fMeasureFunction_ ();
                ValueType setPoint      = fUpdatableParams_->fSetPoint_;
                ValueType error         = setPoint - measuredValue;
                ValueType derivative    = (error - fUpdatableParams_->fPrevError_) / fUpdatePeriod_;
                {
                    auto updateUpdatableParams = fUpdatableParams_.rwget ();
                    updateUpdatableParams->fIntegral_ += error * fUpdatePeriod_;
                    updateUpdatableParams->fPrevError_ = error;
                }
                ValueType outputFunctionArgument = fPIDParams_.P * error + fPIDParams_.I * fUpdatableParams_->fIntegral_ + fPIDParams_.D * derivative;
                fOutputFunction_ (outputFunctionArgument);
                nextRunAt += fUpdatePeriod_;
#if Stroika_Foundation_Execution_PIDLoop_USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("Completed PIDLoop update: set-point={}, measuredValue{}, error={}, derivative={}, integral={}, outputFunctionArgument={}, nextRunAt={}"_f,
                          setPoint, measuredValue, error, derivative, fUpdatableParams_->fIntegral_, outputFunctionArgument, nextRunAt);
#endif
            }
            catch (const Thread::AbortException&) {
                Execution::ReThrow ();
            }
            catch (...) {
                DbgTrace (L"Suppressing exception in PIDLoop: {}"_f, current_exception ());
            }
        }
    }
    template <typename CONTROL_VAR_TYPE>
    Thread::Ptr PIDLoop<CONTROL_VAR_TYPE>::RunInThread ()
    {
        Require (not fThread_.has_value ());
        fThread_ = Thread::New ([this] () { RunDirectly (); }, Thread::eAutoStart);
        return *fThread_;
    }

}
