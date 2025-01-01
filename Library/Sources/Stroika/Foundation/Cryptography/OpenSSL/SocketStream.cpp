/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Foundation_Common_Platform_Windows
#include <io.h>
#elif qStroika_Foundation_Common_Platform_POSIX
#include <unistd.h>
#endif
#include <cstdlib>

#if qStroika_HasComponent_OpenSSL
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#endif

#include "Stroika/Foundation/Execution/OperationNotSupportedException.h"
#include "Stroika/Foundation/Streams/InternallySynchronizedInputOutputStream.h"

#include "SocketStream.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using std::byte;

#if qStroika_HasComponent_OpenSSL
namespace {
    // @todo wrong use OPENSSL API
    class Rep_ : public InputOutputStream::IRep<byte> {
    public:
        bool           fOpenForRead_{true};
        bool           fOpenForWrite_{true};
        SeekOffsetType fReadSeekOffset_{};
        SSL_CTX*       ctx = SSL_CTX_new (TLS_server_method ());

        Rep_ (const ConnectionOrientedStreamSocket::Ptr& sd, const OpenSSL::ClientContext::Options& o)
            : fSD_{sd}
        {
            //if (SSL_CTX_use_certificate (ctx, o.fClientCertificate->AsXXX()->Get_X509()) <= 0) {
            //    // Handle error
            //}
            //if (SSL_CTX_use_certificate_file (ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
            //    std::cerr << "Error loading certificate\n";
            //    // Handle error
            //}
        }
        Rep_ (const ConnectionOrientedStreamSocket::Ptr& sd, const OpenSSL::ServerContext::Options& o)
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
                if (not fOpenForRead_) {
                    fSD_.Close ();
                    fSD_.reset ();
                }
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
                if (not fOpenForWrite_) {
                    fSD_.Close ();
                    fSD_.reset ();
                }
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
            return fSD_.ReadNonBlocking (nullptr, nullptr);
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
                    result = intoBuffer.subspan (0, fSD_.Read (intoBuffer.data (), intoBuffer.data () + intoBuffer.size ()));
                    break;
                case NoDataAvailableHandling::eDontBlock: {
                    auto o = fSD_.ReadNonBlocking (intoBuffer.data (), intoBuffer.data () + intoBuffer.size ());
                    if (o == nullopt) {
                        Execution::Throw (EWouldBlock::kThe);
                    }
                    result = intoBuffer.subspan (0, *o);
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
 ****************** Cryptography::OpenSSL::SocketStream *************************
 ********************************************************************************
 */
auto Cryptography::OpenSSL::SocketStream::New (const ConnectionOrientedStreamSocket::Ptr& sd, const ClientContext::Options& o) -> Ptr
{
    return Ptr{make_shared<Rep_> (sd, o)};
}
auto Cryptography::OpenSSL::SocketStream::New (const ConnectionOrientedStreamSocket::Ptr& sd, const ServerContext::Options& o) -> Ptr
{
    return Ptr{make_shared<Rep_> (sd, o)};
}
#endif