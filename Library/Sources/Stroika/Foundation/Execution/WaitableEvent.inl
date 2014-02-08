/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitableEvent_inl_
#define _Stroika_Foundation_Execution_WaitableEvent_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            //redeclare to avoid having to #include Thread.h
            void    CheckForThreadAborting ();


            /*
             ********************************************************************************
             ********************** Execution::WaitableEvent::WE_ ***************************
             ********************************************************************************
             */
            inline  void    WaitableEvent::WE_::Reset ()
            {
                std::lock_guard<mutex> lockGaurd (fMutex);
                fTriggered = false;
            }
            inline  void    WaitableEvent::WE_::Set ()
            {
                std::lock_guard<mutex> lockGaurd (fMutex);
                fTriggered = true;
                fConditionVariable.notify_all ();
            }


            /*
             ********************************************************************************
             *************************** Execution::WaitableEvent ***************************
             ********************************************************************************
             */
            inline  void    WaitableEvent::Reset ()
            {
                //Debug::TraceContextBumper ctx (SDKSTR ("WaitableEvent::Reset"));
                fWE_.Reset ();
            }
            inline  void    WaitableEvent::Set ()
            {
                //Debug::TraceContextBumper ctx (SDKSTR ("WaitableEvent::Set"));
                fWE_.Set ();
            }
            inline  void    WaitableEvent::Wait (Time::DurationSecondsType timeout)
            {
                fWE_.WaitUntil (timeout + Time::GetTickCount ());
            }
            inline  void    WaitableEvent::WaitUntil (Time::DurationSecondsType timeoutAt)
            {
                fWE_.WaitUntil (timeoutAt);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_WaitableEvent_inl_*/
