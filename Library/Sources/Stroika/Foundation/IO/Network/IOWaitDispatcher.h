/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_IOWaitDispatcher_h_
#define _Stroika_Foundation_IO_Network_IOWaitDispatcher_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Containers/Set.h"
#include    "../../Execution/Thread.h"
#include    "../../Execution/WaitForIOReady.h"

#include    "Socket.h"



/*
 *  \version    <a href="code_status.html#Early-Alpha">Early-Alpha</a>
 *
 * TODO:
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                /**
                 *  This maintains thread inside. VERY ROUGH DRAFT API. See Todo above
                 *
                 *  Basic idea is - construct it with a handler fucntion, and add and remove sockets to it, and it will notify the
                 *  handler whenever there is data available to read/write.
                 *
                 *  One way to think of this - is a portable abstraction of a 'select' loop.
                 */
                class  IOWaitDispatcher {
                public:
                    using CallBackType = function<void(Socket)>;

                public:
                    IOWaitDispatcher () = delete;
                    IOWaitDispatcher (CallBackType callback);
                    IOWaitDispatcher (const IOWaitDispatcher&) = delete;

                public:
                    nonvirtual  void    Add (Socket s);

                public:
                    nonvirtual  void    Remove (Socket s);

                public:
                    nonvirtual  void    clear ();

                private:
                    CallBackType                fHandler_;
                    mutex                       fCallingHandlers_;
                    Containers::Set<Socket>     fElts2Send_;
                    Execution::Thread           fThread_;
                    Execution::WaitForIOReady   fWaiter_;

                private:
                    nonvirtual  void    restartOngoingWait_ ();
                    nonvirtual  void    startthread_ ();
                };


            }
        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "IOWaitDispatcher.inl"

#endif  /*_Stroika_Foundation_IO_Network_IOWaitDispatcher_h_*/
