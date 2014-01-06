/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Signals_h_
#define _Stroika_Foundation_Execution_Signals_h_    1

#include    "../StroikaPreComp.h"

#include    <csignal>
#include    <thread>

#include    "../Configuration/Common.h"



/**
 * Description:
 *
 *      This module defines support for POSIX (and std c++ defined) Signals (not to be confused
 *  with the 'Signals and slots' design pattern which is largely unrelated).
 *
 *
 * TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             */
            typedef int     SignalIDType;



            /**
             */
            wstring SignalToName (SignalIDType signal);


            /**
             * Send the given signal to a specific thread (within this process)
             *
             *  @todo EXPLAIN HOW THIS INTERACTS WITH SignalHandledr::Tyep::eSafe!!!! - use thius with eDIRECT!
             */
            void    SendSignal (thread::native_handle_type h, SignalIDType signal);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Signals.inl"

#endif  /*_Stroika_Foundation_Execution_Signals_h_*/
