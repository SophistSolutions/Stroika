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

#if qStroika_HasComponent_OpenSSL
#include <openssl/bio.h>
#include <openssl/ssl.h>
#endif

#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/Exception.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/OperationNotSupportedException.h"
#include "Stroika/Foundation/Streams/InternallySynchronizedInputOutputStream.h"

#include "SocketStream.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Providers;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using std::byte;

#if qStroika_HasComponent_OpenSSL
namespace {
    class Rep_ : public OpenSSL::SocketStream::IRep {
    public:
        bool           fOpenForRead_{true};
        bool           fOpenForWrite_{true};
        SeekOffsetType fReadSeekOffset_{};
        variant<OpenSSL::ClientContext::Ptr, OpenSSL::ServerContext::Ptr> fContext_; // dont access really, but keep cuz fSSLConnection_ object may retain pointer to context inside here
        OpenSSL::SocketStream::LibRepType fSSLConnection_;

        Rep_ (OpenSSL::SocketStream::LibRepType&& r)
            : fSSLConnection_{move (r)}
        {
        }
        Rep_ (const ConnectionOrientedStreamSocket::Ptr& sd, const OpenSSL::ClientContext::Options& o)
            : fContext_{OpenSSL::ClientContext::New (o)}
            , fSSLConnection_{::SSL_new (get<OpenSSL::ClientContext::Ptr> (fContext_)->Get_SSL_CTX ())}
        {
            OpenSSL::Exception::ThrowLastErrorIfFailed (::SSL_set_fd (fSSLConnection_.get (), static_cast<int> (sd.GetNativeSocket ())));
            switch (int ar = ::SSL_connect (fSSLConnection_.get ())) {
                case 1:
                    break; // all is well
                case 2:
                    OpenSSL::Exception::ThrowLastError ();
                default:
                    if (ar < 0) {
                        OpenSSL::Exception::ThrowLastError ();
                    }
                    DbgTrace ("ar={}"_f, ar);
                    break;
            }
        }
        Rep_ (const ConnectionOrientedStreamSocket::Ptr& sd, const OpenSSL::ServerContext::Options& o)
            : fContext_{OpenSSL::ServerContext::New (o)}
            , fSSLConnection_{::SSL_new (get<OpenSSL::ServerContext::Ptr> (fContext_)->Get_SSL_CTX ())}
        {
            OpenSSL::Exception::ThrowLastErrorIfFailed (::SSL_set_fd (fSSLConnection_.get (), static_cast<int> (sd.GetNativeSocket ())));
            switch (int ar = ::SSL_accept (fSSLConnection_.get ())) {
                case 1:
                    break; // all is well
                case 2:
                    OpenSSL::Exception::ThrowLastError ();
                default:
                    if (ar < 0) {
                        OpenSSL::Exception::ThrowLastError ();
                    }
                    DbgTrace ("ar={}"_f, ar);
                    break;
            }
        }
        ~Rep_ ()
        {
            // SSL_shutdown logic highly questionable.. - DRAFT
            if (fSSLConnection_ and SSL_get_shutdown (fSSLConnection_.get ()) == 0) {
                ::SSL_shutdown (fSSLConnection_.get ());
            }
        }

        // Streams::InputOutputStream::IRep overrides
        virtual bool IsSeekable () const override
        {
            return false;
        }
        virtual void CloseWrite () override
        {
            if (IsOpenWrite ()) {
                if (::SSL_get_shutdown (fSSLConnection_.get ()) == 0) {
                    ::SSL_shutdown (fSSLConnection_.get ());
                }
                fOpenForWrite_ = false;
                if (not fOpenForRead_) {
                    fSSLConnection_.reset ();
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
                if (::SSL_get_shutdown (fSSLConnection_.get ()) == 0) {
                    ::SSL_shutdown (fSSLConnection_.get ());
                }
                fOpenForRead_ = false;
                if (not fOpenForWrite_) {
                    fSSLConnection_.reset ();
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
            // DRAFT - NOT SURE THIS IS RIGHT --LGP 2025-01-01
            // see  ssl_ctx_set_mode(3)
            Require (IsOpenRead ());
            int pending = ::SSL_pending (fSSLConnection_.get ());
            if (pending == 0) {
                return nullopt;
            }
            return pending;
        }
        virtual optional<SeekOffsetType> RemainingLength () override
        {
            Require (IsOpenRead ());
            return nullopt; // maybe in some cases we can answer this like closed, but not generally
        }
        virtual optional<span<byte>> Read (span<byte> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            // DRAFT - NOT SURE THIS IS RIGHT --LGP 2025-01-01
            // see  ssl_ctx_set_mode(3)
            Require (IsOpenRead ());
            optional<span<byte>> result;
            switch (blockFlag) {
                case NoDataAvailableHandling::eBlockIfNoDataAvailable: {
                    auto r = ::SSL_read (fSSLConnection_.get (), intoBuffer.data (), static_cast<int> (intoBuffer.size ()));
                    if (r > 0) {
                        result = intoBuffer.subspan (0, r);
                    }
                    else if (r == 0) {
                        // DOCS not clear, but I think this is just EOF
                        result = span<byte>{};
                    }
                    else {
                        // I THINK error - and must throw, but maybe zero is OK - and just EOF
                        OpenSSL::Exception::ThrowLastError ();
                    }
                } break;
                case NoDataAvailableHandling::eDontBlock: {
                    int pending = ::SSL_pending (fSSLConnection_.get ());
                    if (pending == 0) {
                        Execution::Throw (EWouldBlock::kThe);
                    }
                    auto r = ::SSL_read (fSSLConnection_.get (), intoBuffer.data (), static_cast<int> (intoBuffer.size ()));
                    if (r > 0) {
                        result = intoBuffer.subspan (0, r);
                    }
                    else if (r == 0) {
                        // DOCS not clear, but I think this is just EOF
                        result = span<byte>{};
                    }
                    else {
                        // I THINK error - and must throw, but maybe zero is OK - and just EOF
                        OpenSSL::Exception::ThrowLastError ();
                    }
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
            //static bool first = true;
            //if (first) {
            //    first = false;
            //    ::SSL_accept (fSSLConnection_.get ());//hack - need another API somehow for this
            //}
            int r = ::SSL_write (fSSLConnection_.get (), elts.data (), static_cast<int> (elts.size ()));
            if (r != elts.size ()) {
                // https://linux.die.net/man/3/ssl_write appears to indicate anything other than full write success is an error
                // and probably unrecoverable (unlike socket writes which can write incompletely).
                OpenSSL::Exception::ThrowLastError ();
            }
        }
        virtual void Flush () override
        {
            Require (IsOpenWrite ());
            // OpenSSL has no flush API, so write must do the trick...
        }

        // OpenSSL::SocketStream::IRep overrides
        virtual ::SSL* Get_SSL () const override
        {
            return fSSLConnection_.get ();
        }
    };
}

/*
 ********************************************************************************
 ************** Cryptography::Providers::OpenSSL::SocketStream ******************
 ********************************************************************************
 */
auto Cryptography::Providers::OpenSSL::SocketStream::New (LibRepType&& r) -> Ptr
{
    return Ptr{make_shared<Rep_> (move (r))};
}
auto Cryptography::Providers::OpenSSL::SocketStream::New (const ConnectionOrientedStreamSocket::Ptr& sd, const ClientContext::Options& o) -> Ptr
{
    return Ptr{make_shared<Rep_> (sd, o)};
}
auto Cryptography::Providers::OpenSSL::SocketStream::New (const ConnectionOrientedStreamSocket::Ptr& sd, const ServerContext::Options& o) -> Ptr
{
    return Ptr{make_shared<Rep_> (sd, o)};
}
#endif