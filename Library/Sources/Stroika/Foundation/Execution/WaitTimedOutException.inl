/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitTimedOutException_inl_
#define _Stroika_Foundation_Execution_WaitTimedOutException_inl_    1


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
             ********************** Execution::WaitTimedOutException ************************
             ********************************************************************************
             */
            inline  void    ThrowTimeoutExceptionAfter (Time::DurationSecondsType afterTickCount)
            {
                if (Time::GetTickCount () > afterTickCount) {
                    DoThrow (WaitTimedOutException ());
                }
                CheckForThreadAborting ();
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_WaitTimedOutException_inl_*/
