/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_POSIX_SignalBlock_h_
#define _Stroika_Foundation_Execution_Platform_POSIX_SignalBlock_h_    1

#include    "../../../StroikaPreComp.h"

#include    <csignal>
#include    <mutex>

#if     !qPlatform_POSIX
#error  "ONLY INCLUDE FOR POSIX"
#endif

#include    "../../Signals.h"



/**
 *
 * TODO:
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Platform {
                namespace   POSIX {


                    /**
                     * For the lifetime of this object - save the initial signal block state for the given signal, and then block the given signal.
                     */
                    class   ScopedBlockCurrentThreadSignal {
                    public:
                        ScopedBlockCurrentThreadSignal (SignalID signal);
                        ScopedBlockCurrentThreadSignal () = delete;
                        ScopedBlockCurrentThreadSignal (const ScopedBlockCurrentThreadSignal&) = delete;
                        ~ScopedBlockCurrentThreadSignal ();

                    public:
                        nonvirtual  ScopedBlockCurrentThreadSignal& operator= (const ScopedBlockCurrentThreadSignal&) = delete;

                    private:
                        sigset_t    fRestoreMask_;
                    };


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SignalBlock.inl"

#endif  /*_Stroika_Foundation_Execution_Platform_POSIX_SignalBlock_h_*/
