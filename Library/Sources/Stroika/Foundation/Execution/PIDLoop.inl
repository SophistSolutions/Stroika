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
            PIDLoop<CONTROL_VAR_TYPE>::PIDLoop (const ControlParams& pidParams, Time::DurationSecondsType timeDelta, const function<ValueType ()>& measureFunction, const function<void(ValueType o)>& outputFunction, ValueType initialSetPoint)
                : fPIDParams_ (pidParams)
                , fTimeDelta_ (timeDelta)
                , fMeasureFunction_ (measureFunction)
                , fOutputFunction_ (outputFunction)
            {
                Require (timeDelta > 0);
                fUpdatableParams_.rwget ()->fSetPoint_ = (initialSetPoint);
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
            void PIDLoop<CONTROL_VAR_TYPE>::RunDirectly ()
            {
                Time::DurationSecondsType nextRunAt = Time::GetTickCount ();
                while (true) {
                    SleepUntil (nextRunAt);
                    ValueType measuredValue = fMeasureFunction_ ();
                    ValueType error         = fUpdatableParams_->fSetPoint_ - measuredValue;
                    fUpdatableParams_->fIntegral_ += error * fTimeDelta_;
                    ValueType derivative           = (error - fUpdatableParams_->fPrevError) / fTimeDelta_;
                    fUpdatableParams_->fPrevError_ = error;
                    fOutputFunction_ (fPIDParams_.P * error + fPIDParams_.I * fUpdatableParams_->fIntegral_ + fPIDParams_.D * derivative);
                    nextRunAt += fTimeDelta_;
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
