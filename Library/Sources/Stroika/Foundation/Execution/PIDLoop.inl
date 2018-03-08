/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_PIDLoop_inl_
#define _Stroika_Foundation_Execution_PIDLoop_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Characters/StringBuilder.h"
#include "../Characters/ToString.h"
#include "Sleep.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module (note the extra long name since its in a header)
//#define Stroika_Foundation_Execution_PIDLoop_USE_NOISY_TRACE_IN_THIS_MODULE_ 1

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /*
             ********************************************************************************
             ********************* PIDLoop<CONTROL_VAR_TYPE>::ControlParams *****************
             ********************************************************************************
             */
            template <typename CONTROL_VAR_TYPE>
            inline PIDLoop<CONTROL_VAR_TYPE>::ControlParams::ControlParams (ValueType p, ValueType i, ValueType d)
                : P (p)
                , I (i)
                , D (d)
            {
            }
            template <typename CONTROL_VAR_TYPE>
            inline bool PIDLoop<CONTROL_VAR_TYPE>::ControlParams::operator== (const ControlParams& rhs) const
            {
                return P == rhs.P and I == rhs.I and D == rhs.D;
            }
            template <typename CONTROL_VAR_TYPE>
            inline bool PIDLoop<CONTROL_VAR_TYPE>::ControlParams::operator!= (const ControlParams& rhs) const
            {
                return not operator== (rhs);
            }
            template <typename CONTROL_VAR_TYPE>
            inline Characters::String PIDLoop<CONTROL_VAR_TYPE>::ControlParams::ToString () const
            {
                Characters::StringBuilder out;
                out += L"{";
                out += L"P: " + Characters::ToString (P) + L"',";
                out += L"I: " + Characters::ToString (I) + L"',";
                out += L"D: " + Characters::ToString (D) + L"',";
                out += L"}";
                return out.str ();
            }

            /*
             ********************************************************************************
             ************************** PIDLoop<CONTROL_VAR_TYPE> ***************************
             ********************************************************************************
             */
            template <typename CONTROL_VAR_TYPE>
            PIDLoop<CONTROL_VAR_TYPE>::PIDLoop (const ControlParams& pidParams, Time::DurationSecondsType updatePeriod, const function<ValueType ()>& measureFunction, const function<void(ValueType o)>& outputFunction, ValueType initialSetPoint)
                : fPIDParams_ (pidParams)
                , fUpdatePeriod_ (updatePeriod)
                , fMeasureFunction_ (measureFunction)
                , fOutputFunction_ (outputFunction)
            {
                Require (updatePeriod > 0);
                fUpdatableParams_.rwget ()->fSetPoint_ = (initialSetPoint);
            }
            template <typename CONTROL_VAR_TYPE>
            PIDLoop<CONTROL_VAR_TYPE>::PIDLoop (AutoStartFlag, const ControlParams& pidParams, Time::DurationSecondsType updatePeriod, const function<ValueType ()>& measureFunction, const function<void(ValueType o)>& outputFunction, ValueType initialSetPoint)
                : PIDLoop (pidParams, updatePeriod, measureFunction, outputFunction, initialSetPoint)
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
            inline auto PIDLoop<CONTROL_VAR_TYPE>::GetUpdatePeriod () const -> Time::DurationSecondsType
            {
                return fUpdatePeriod_;
            }
            template <typename CONTROL_VAR_TYPE>
            void PIDLoop<CONTROL_VAR_TYPE>::RunDirectly ()
            {
                Time::DurationSecondsType nextRunAt = Time::GetTickCount ();
                while (true) {
                    SleepUntil (nextRunAt);
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
                    DbgTrace (L"Completed PIDLoop update: set-point=%s, measuredValue=%s, error=%s, derivative=%s, integral=%s, outputFunctionArgument=%s, nextRunAt=%f", Characters::ToString (setPoint).c_str (), Characters::ToString (measuredValue).c_str (), Characters::ToString (error).c_str (), Characters::ToString (derivative).c_str (), Characters::ToString (updateUpdatableParams->fIntegral_).c_str (), Characters::ToString (outputFunctionArgument).c_str (), nextRunAt);
#endif
                }
            }
            template <typename CONTROL_VAR_TYPE>
            Thread::Ptr PIDLoop<CONTROL_VAR_TYPE>::RunInThread ()
            {
                Require (fThread_.IsMissing ());
                fThread_ = Thread::New ([this]() { RunDirectly (); }, Thread::eAutoStart);
                return *fThread_;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Execution_PIDLoop_inl_*/
