/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qPlatform_Windows
#include <io.h>
#elif qPlatform_POSIX
#include <unistd.h>
#endif
#include <cstdlib>

#include "../../Execution/OperationNotSupportedException.h"
#include "../../Streams/InternallySynchronizedInputOutputStream.h"

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
class SocketStream::Rep_ : public InputOutputStream::IRep<byte> {
public:
    bool           fOpenForRead_{true};
    bool           fOpenForWrite_{true};
    SeekOffsetType fReadSeekOffset_{};
    Rep_ (const ConnectionOrientedStreamSocket::Ptr& sd)
        : fSD_{sd}
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
        Require (IsOpenRead ());
        return fReadSeekOffset_;
    }
    virtual SeekOffsetType SeekRead (Whence /*whence*/, SignedSeekOffsetType /*offset*/) override
    {
        RequireNotReached (); // not seekable
        Require (IsOpenRead ());
        return 0;
    }
    virtual span<byte> Read (span<byte> intoBuffer) override
    {
        Require (IsOpenRead ());
        size_t n = fSD_.Read (intoBuffer.data (), intoBuffer.data () + intoBuffer.size ());
        fReadSeekOffset_ += n;
        return intoBuffer.subspan (0, n);
    }
    virtual optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) override
    {
        Require (IsOpenRead ());
        Require (IsOpenWrite ());
        optional<size_t> result = fSD_.ReadNonBlocking (intoStart, intoEnd);
        if (result and intoStart != nullptr) {
            fReadSeekOffset_ += *result;
        }
        return result;
    }
    virtual SeekOffsetType GetWriteOffset () const override
    {
        RequireNotReached (); // not seekable
        Require (IsOpenWrite ());
        return 0;
    }
    virtual SeekOffsetType SeekWrite (Whence /*whence*/, SignedSeekOffsetType /*offset*/) override
    {
        RequireNotReached (); // not seekable
        Require (IsOpenWrite ());
        return 0;
    }
    virtual void Write (span<const byte> elts) override
    {
        Require (IsOpenWrite ());
        fSD_.Write (elts);
    }
    virtual void Flush () override
    {
        Require (IsOpenWrite ());
        // socket has no flush API, so write must do the trick...
    }

private:
    ConnectionOrientedStreamSocket::Ptr fSD_;
};

/*
 ********************************************************************************
 **************************** IO::Network::SocketStream *************************
 ********************************************************************************
 */
auto SocketStream::New (const ConnectionOrientedStreamSocket::Ptr& sd) -> Ptr
{
    return Ptr{make_shared<Rep_> (sd)};
}

auto SocketStream::New (Execution::InternallySynchronized internallySynchronized, const ConnectionOrientedStreamSocket::Ptr& sd) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return InternallySynchronizedInputOutputStream::New<Rep_> ({}, sd);
        case Execution::eNotKnownInternallySynchronized:
            return New (sd);
        default:
            RequireNotReached ();
            return nullptr;
    }
}
