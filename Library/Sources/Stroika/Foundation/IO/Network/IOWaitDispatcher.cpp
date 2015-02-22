/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Debug/Assertions.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Execution/Thread.h"

#include    "IOWaitDispatcher.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::IO::Network;




/*
********************************************************************************
************************* IO::Network::IOWaitDispatcher ************************
********************************************************************************
*/
IOWaitDispatcher::IOWaitDispatcher (CallBackType callback)
    : fHandler_ (callback)
{
}

void    IOWaitDispatcher::Add (Socket s)
{
    fWaiter_.Add (s);
    restartOngoingWait_ ();
}

void    IOWaitDispatcher::Remove (Socket s)
{
    fWaiter_.Remove (s);
    // No need to wait here because we ignore any sockets reported that no longer apply
}

void    IOWaitDispatcher::clear ()
{
    fWaiter_.clear ();
}

void    IOWaitDispatcher::restartOngoingWait_ ()
{
    if (fCallingHandlers_.try_lock ()) {
        // then we need to kill the thread to interupt a wait...
        fThread_.AbortAndWaitForDone ();
        startthread_ ();
    }
    else {
        // we're done because if locked, we're NOT doing a wait
    }
}

void    IOWaitDispatcher::startthread_ ()
{
    fThread_ = Thread ([this] () {
        /// need some 'set' to tell if alreadye in list todo
        for (/*Socket*/auto si : fWaiter_.Wait ()) {

            // must 'upcast' - but tricky todo

            //fElts2Send_.Add (si);
        }
        {
            lock_guard<mutex>   l { fCallingHandlers_ };
            /// @todo MUST INTERSECT etls2send with set of items waiting on...
            for (Socket s : fElts2Send_) {
                fHandler_ (s);
            }
        }
    });
    fThread_.Start ();
}
