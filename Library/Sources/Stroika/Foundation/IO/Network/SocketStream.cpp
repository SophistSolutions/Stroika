/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <io.h>
#elif qPlatform_POSIX
#include <unistd.h>
#endif
#include <cstdlib>

#include "../../Execution/Exceptions.h"
#include "../../Execution/OperationNotSupportedException.h"

#include "SocketStream.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

/*
 ********************************************************************************
 ********************* IO::Network::SocketStream::Rep_ **************************
 ********************************************************************************
 */
class SocketStream::Rep_ : public InputOutputStream<Byte>::_IRep {
public:
    bool fOpenForRead_{true};
    bool fOpenForWrite_{true};
    Rep_ (const ConnectionOrientedSocket::Ptr& sd)
        : InputOutputStream<Byte>::_IRep ()
        , fSD_ (sd)
    {
    }
    virtual bool IsSeekable () const override
    {
        return false;
    }
    virtual void CloseWrite () override
    {
        Require (IsOpenWrite ());
        fSD_.Shutdown (Socket::ShutdownTarget::eWrites);
        if (not fOpenForRead_) {
            fSD_.Close ();
            fSD_.reset ();
        }
        Ensure (not IsOpenWrite ());
    }
    virtual bool IsOpenWrite () const override
    {
        return fOpenForWrite_;
    }
    virtual void CloseRead () override
    {
        Require (IsOpenRead ());
        fSD_.Shutdown (Socket::ShutdownTarget::eReads);
        if (not fOpenForWrite_) {
            fSD_.Close ();
            fSD_.reset ();
        }
    }
    virtual bool IsOpenRead () const override
    {
        return fOpenForRead_;
    }
    virtual SeekOffsetType GetReadOffset () const override
    {
        RequireNotReached (); // not seekable
        Require (IsOpenRead ());
        return 0;
    }
    virtual SeekOffsetType SeekRead (Whence whence, SignedSeekOffsetType offset) override
    {
        RequireNotReached (); // not seekable
        Require (IsOpenRead ());
        return 0;
    }
    virtual size_t Read (Byte* intoStart, Byte* intoEnd) override
    {
        Require (IsOpenRead ());
        return fSD_.Read (intoStart, intoEnd);
    }
    virtual Memory::Optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) override
    {
        Require (IsOpenRead ());
        Require (IsOpenWrite ());
        return fSD_.ReadNonBlocking (intoStart, intoEnd);
    }
    virtual SeekOffsetType GetWriteOffset () const override
    {
        RequireNotReached (); // not seekable
        Require (IsOpenWrite ());
        return 0;
    }
    virtual SeekOffsetType SeekWrite (Whence whence, SignedSeekOffsetType offset) override
    {
        RequireNotReached (); // not seekable
        Require (IsOpenWrite ());
        return 0;
    }
    virtual void Write (const Byte* start, const Byte* end) override
    {
        Require (IsOpenWrite ());
        fSD_.Write (start, end);
    }
    virtual void Flush () override
    {
        Require (IsOpenWrite ());
        // socket has no flush API, so write must do the trick...
    }

private:
    ConnectionOrientedSocket::Ptr fSD_;
};

/*
 ********************************************************************************
 **************************** IO::Network::SocketStream *************************
 ********************************************************************************
 */
auto SocketStream::New (const ConnectionOrientedSocket::Ptr& sd) -> Ptr
{
    return make_shared<Rep_> (sd);
}

auto SocketStream::New (Execution::InternallySyncrhonized internallySyncrhonized, const ConnectionOrientedSocket::Ptr& sd) -> Ptr
{
    switch (internallySyncrhonized) {
        case Execution::eInternallySynchronized:
            return InternalSyncRep_::New (sd);
        case Execution::eNotKnwonInternallySynchronized:
            return New (sd);
        default:
            RequireNotReached ();
            return nullptr;
    }
}

/*
 ********************************************************************************
 ******************** IO::Network::SocketStream::Ptr ****************************
 ********************************************************************************
 */
SocketStream::Ptr::Ptr (const shared_ptr<Rep_>& from)
    : inherited (from)
{
}
