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
             *************************** Execution::WaitableEvent ***************************
             ********************************************************************************
             */
            inline  WaitableEvent::WaitableEvent ()
                : fMutex_ ()
                , fConditionVariable_ ()
                , fTriggered_ (false)
                , fThreadAbortCheckFrequency_ (0.5)
            {
            }
            inline  void    WaitableEvent::Reset ()
            {
                //Debug::TraceContextBumper ctx (SDKSTR ("WaitableEvent::Reset"));
                {
                    std::lock_guard<mutex> lockGaurd (fMutex_);
                    fTriggered_ = false;
                }
            }
            inline  void    WaitableEvent::Set ()
            {
                //Debug::TraceContextBumper ctx (SDKSTR ("WaitableEvent::Set"));
                {
                    std::lock_guard<mutex> lockGaurd (fMutex_);
                    fTriggered_ = true;
                    fConditionVariable_.notify_all ();
                }
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_WaitableEvent_inl_*/
