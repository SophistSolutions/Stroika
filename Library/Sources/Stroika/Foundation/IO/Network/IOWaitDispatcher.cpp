/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/String_Constant.h"
#include    "../../Debug/Assertions.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Execution/Thread.h"
#include    "../../Memory/Optional.h"

#include    "IOWaitDispatcher.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::IO::Network;



using   FileDescriptorType  =   WaitForIOReady::FileDescriptorType;
using   Memory::Optional;



namespace {
    inline  WaitForIOReady::FileDescriptorType  cvt_ (Socket::PlatformNativeHandle pnh)
    {
        return reinterpret_cast<FileDescriptorType> (pnh);
    }
}


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
    AddAll (Set<Socket> { s });
}

void    IOWaitDispatcher::AddAll (const Set<Socket>& s)
{
    {
        auto rwLock = fSocketFDBijection_.rwget ();      // assure these keep fWaiter_ synconized which is why in same lock
        s.Apply ([&rwLock] (Socket si) { rwLock->Add (si, cvt_ (si.GetNativeSocket ())); });
        fWaiter_.AddAll (s);
    }
    RestartOngoingWait_ ();
}

void    IOWaitDispatcher::Remove (Socket s)
{
    RemoveAll (Set<Socket> { s });
}

void    IOWaitDispatcher::RemoveAll (const Set<Socket>& s)
{
    {
        auto rwLock = fSocketFDBijection_.rwget ();      // assure these keep fWaiter_ synconized which is why in same lock
        s.Apply ([&rwLock] (Socket si) { rwLock->RemoveDomainElement (si); });
        fWaiter_.RemoveAll (s);
    }
    // No need to RestartOngoingWait_ () here because we ignore any sockets reported that no longer apply
}

void    IOWaitDispatcher::clear ()
{
    auto rwLock = fSocketFDBijection_.rwget ();      // assure these keep fWaiter_ synconized which is why in same lock
    rwLock->clear ();
    fWaiter_.clear ();
}

Set<Socket> IOWaitDispatcher::GetSockets () const
{
    return Set<Socket> (fSocketFDBijection_.cget ()->Preimage ());
}

void       IOWaitDispatcher::SetSockets (const Set<Socket>& s)
{
    {
        auto rwLock = fSocketFDBijection_.rwget ();      // assure these keep fWaiter_ synconized which is why in same lock
        rwLock->clear ();
        s.Apply ([&rwLock] (Socket si) { rwLock->Add (si, cvt_ (si.GetNativeSocket ())); });
        fWaiter_.SetDescriptors (s);
    }
    RestartOngoingWait_ ();
}

void    IOWaitDispatcher::RestartOngoingWait_ ()
{
    //@todo REVIEW FOR RACES
    if (fCallingHandlers_.try_lock ()) {
        // then we need to kill the thread to interupt a wait...
        fThread_.AbortAndWaitForDone ();
        Startthread_ ();
    }
    else {
        // we're done because if locked, we're NOT doing a wait
    }
}

void    IOWaitDispatcher::Startthread_ ()
{
    static  const   String  kThreadName_ { String_Constant { L"IOWaitDispatcher::{}..."} };
    fThread_ = Thread (
    [this] () {
        /// need some 'set' to tell if alreadye in list todo
        for (FileDescriptorType fdi : fWaiter_.Wait ()) {
            fElts2Send_.Add (fdi);
        }
        {
            lock_guard<mutex>   l { fCallingHandlers_ };
            /// @todo MUST INTERSECT etls2send with set of items waiting on...
            for (FileDescriptorType fdi : fElts2Send_) {
                Optional<Socket>    si   =  fSocketFDBijection_.cget ()->InverseLookup (fdi);
                if (si) {
                    fHandler_ (*si);
                }
                else {
                    DbgTrace ("Socket fd %d missing, so presumably removed...", fdi);
                }
            }
        }
    }
    , Thread::eAutoStart
    , kThreadName_
               );
}
