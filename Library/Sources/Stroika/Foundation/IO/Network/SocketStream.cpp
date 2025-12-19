/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Foundation_Common_Platform_Windows
#include <io.h>
#elif qStroika_Foundation_Common_Platform_POSIX
#include <unistd.h>
#endif
#include <cstdlib>

#include "Stroika/Foundation/Execution/OperationNotSupportedException.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Streams/InternallySynchronizedInputOutputStream.h"

#include "SocketStream.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using std::byte;

namespace {
    class Rep_ : public InputOutputStream::IRep<byte> {
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
            if (IsOpenWrite ()) {
                fSD_.Shutdown (Socket::ShutdownTarget::eWrites);
                if (not fOpenForRead_) { // if Both closed
                    fSD_.Close ();
                    fSD_.reset ();
                }
                fOpenForWrite_ = false;
            }
            Ensure (not IsOpenWrite ());
        }
        virtual bool IsOpenWrite () const override
        {
            return fOpenForWrite_;
        }
        virtual void CloseRead () override
        {
            if (fOpenForRead_) {
                fSD_.Shutdown (Socket::ShutdownTarget::eReads);
                if (not fOpenForWrite_) { // if Both closed
                    fSD_.Close ();
                    fSD_.reset ();
                }
                fOpenForRead_ = false;
            }
            Ensure (not IsOpenRead ());
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
        virtual optional<size_t> AvailableToRead () override
        {
            Require (IsOpenRead ());
            return fSD_.AvailableToRead ();
        }
        virtual optional<SeekOffsetType> RemainingLength () override
        {
            Require (IsOpenRead ());
            return nullopt; // maybe in some cases we can answer this like closed, but not generally
        }
        virtual optional<span<byte>> Read (span<byte> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            Require (IsOpenRead ());
            optional<span<byte>> result;
            switch (blockFlag) {
                case NoDataAvailableHandling::eBlockIfNoDataAvailable:
                    result = fSD_.Read (intoBuffer);
                    break;
                case NoDataAvailableHandling::eDontBlock: {
                    auto o = fSD_.ReadNonBlocking (intoBuffer);
                    if (o == nullopt) {
                        Execution::Throw (EWouldBlock::kThe);
                    }
                    result = *o;
                } break;
                default:
                    RequireNotReached ();
            }
            Assert (result);
            fReadSeekOffset_ += result->size ();
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
}

/*
 ********************************************************************************
 **************************** IO::Network::SocketStream *************************
 ********************************************************************************
 */
auto SocketStream::New (const ConnectionOrientedStreamSocket::Ptr& sd) -> Ptr
{
    return Ptr{Memory::MakeSharedPtr<Rep_> (sd)};
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
