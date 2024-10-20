/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qHasFeature_OpenSSL
#include <openssl/err.h>
#include <openssl/evp.h>
#endif

#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Streams/InternallySynchronizedInputStream.h"
#include "Stroika/Foundation/Streams/InternallySynchronizedOutputStream.h"

#include "OpenSSLCryptoStream.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Encoding;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

using Memory::BLOB;

// @todo examine/test https://github.com/saju/misc/blob/master/misc/openssl_aes.c

#if qHasFeature_OpenSSL && defined(_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#if OPENSSL_VERSION_NUMBER < 0x1010000fL
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#else
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "crypt32.lib")
#endif
#endif

//// VERY ROUGH DRAFT - NOT VERY CLOSE TO CORRECT FOR ALL ALGORITHSM
//// SEE http://www.openssl.org/docs/crypto/EVP_EncryptInit.html
/// SEE https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
//// for details on what todo

#if qHasFeature_OpenSSL

namespace {
    struct InOutStrmCommon_ {
        InOutStrmCommon_ (const OpenSSLCryptoParams& cryptoParams, Direction d)
            : fCTX_{::EVP_CIPHER_CTX_new ()}
        {
            try {
                cryptoParams.fInitializer (fCTX_, d);
            }
            catch (...) {
                ::EVP_CIPHER_CTX_free (fCTX_);
                Execution::ReThrow ();
            }
        }
        InOutStrmCommon_ (const InOutStrmCommon_&)            = delete;
        InOutStrmCommon_& operator= (const InOutStrmCommon_&) = delete;
        virtual ~InOutStrmCommon_ ()
        {
            ::EVP_CIPHER_CTX_free (fCTX_);
        }
        static constexpr size_t _GetMinOutBufSize (size_t n)
        {
            return n + EVP_MAX_BLOCK_LENGTH;
        }
        // return nBytes in outBuf, throws on error
        span<byte> _runOnce (span<const byte> data2Process, span<byte> outBuf)
        {
            Require (outBuf.size () >= _GetMinOutBufSize (data2Process.size ())); // always need out buf big enuf for inbuf
            int outLen = 0;
            Cryptography::OpenSSL::Exception::ThrowLastErrorIfFailed (
                ::EVP_CipherUpdate (fCTX_, reinterpret_cast<unsigned char*> (outBuf.data ()), &outLen,
                                    reinterpret_cast<const unsigned char*> (data2Process.data ()), static_cast<int> (data2Process.size ())));
            Ensure (outLen >= 0);
            Ensure (static_cast<size_t> (outLen) <= outBuf.size ());
            return outBuf.subspan (0, outLen);
        }
        // return nBytes in outBuf, throws on error
        // Can call multiple times - it keeps track itself if finalized.
        size_t _cipherFinal (byte* outBufStart, [[maybe_unused]] byte* outBufEnd)
        {
            Require (outBufStart <= outBufEnd and static_cast<size_t> (outBufEnd - outBufStart) >= _GetMinOutBufSize (0));
            if (fFinalCalled_) {
                return 0; // not an error - just zero more bytes
            }
            int outLen = 0;
            Cryptography::OpenSSL::Exception::ThrowLastErrorIfFailed (
                ::EVP_CipherFinal_ex (fCTX_, reinterpret_cast<unsigned char*> (outBufStart), &outLen));
            fFinalCalled_ = true;
            Ensure (outLen >= 0);
            Ensure (outLen <= (outBufEnd - outBufStart));
            return size_t (outLen);
        }
        EVP_CIPHER_CTX* fCTX_{nullptr};
        bool            fFinalCalled_{false};
    };
}

namespace {
    class OpenSSLInputStreamRep_ : public InputStream::IRep<byte>, private InOutStrmCommon_ {
    private:
        static constexpr size_t kInBufSize_ = 10 * 1024;

    public:
        OpenSSLInputStreamRep_ (const OpenSSLCryptoParams& cryptoParams, Direction d, const InputStream::Ptr<byte>& realIn)
            : InOutStrmCommon_{cryptoParams, d}
            , fRealIn_{realIn}
        {
        }
        virtual bool IsSeekable () const override
        {
            return false;
        }
        virtual void CloseRead () override
        {
            Require (IsOpenRead ());
            fRealIn_.Close ();
            Assert (fRealIn_ == nullptr);
            Ensure (not IsOpenRead ());
        }
        virtual bool IsOpenRead () const override
        {
            return fRealIn_ != nullptr;
        }
        virtual SeekOffsetType GetReadOffset () const override
        {
            RequireNotReached ();
            Require (IsOpenRead ());
            return 0;
        }
        virtual optional<size_t> AvailableToRead () override
        {
            // must override since not seekable
            if (not PreRead_ (NoDataAvailableHandling::eDontBlock)) {
                return nullopt;
            }
            return static_cast<size_t> (fOutBufEnd_ > fOutBufStart_);
        }
        virtual optional<SeekOffsetType> RemainingLength () override
        {
            return nullopt; // generally cannot tell without side-effects on input stream
        }
        virtual optional<span<ElementType>> Read (span<ElementType> intoBuffer, NoDataAvailableHandling blockFlag) override
        {
            /*
             *  Keep track if unread bytes in fOutBuf_ - bounded by fOutBufStart_ and fOutBufEnd_.
             *  If none to read there - pull from fRealIn_ src, and push those through the cipher.
             *  and use that to re-populate fOutBuf_.
             */
            Require (not intoBuffer.empty ());
            [[maybe_unused]] lock_guard critSec{fCriticalSection_};
            Require (IsOpenRead ());
            if (not PreRead_ (blockFlag)) {
                return nullopt;
            }
            if (fOutBufStart_ < fOutBufEnd_) {
                size_t n2Copy = min<size_t> (fOutBufEnd_ - fOutBufStart_, intoBuffer.size ());
                (void)::memcpy (intoBuffer.data (), fOutBufStart_, n2Copy);
                fOutBufStart_ += n2Copy;
                return intoBuffer.subspan (0, n2Copy);
            }
            return span<ElementType>{}; // EOF
        }
        // false means EWouldBlock
        bool PreRead_ (NoDataAvailableHandling blockFlag)
        {
            if (fOutBufStart_ == fOutBufEnd_) {
                /*
                 *  Then pull from 'real in' stream until we have reach EOF there, or until we have some bytes to output
                 *  on our own.
                 */
                byte toDecryptBuf[kInBufSize_];
            Again:
                size_t n2Decrypt = 0;
                switch (blockFlag) {
                    case NoDataAvailableHandling::eBlockIfNoDataAvailable: {
                        n2Decrypt = fRealIn_.Read (span{toDecryptBuf}, blockFlag).size ();
                    } break;
                    case NoDataAvailableHandling::eDontBlock: {
                        if (auto oRes = fRealIn_.ReadNonBlocking (span{toDecryptBuf})) {
                            n2Decrypt = oRes->size ();
                        }
                        else {
                            return false;
                        }
                    } break;
                    default:
                        RequireNotReached ();
                }

                if (n2Decrypt == 0) {
                    size_t nBytesInOutBuf = _cipherFinal (fOutBuf_.begin (), fOutBuf_.end ());
                    Assert (nBytesInOutBuf <= fOutBuf_.GetSize ());
                    fOutBufStart_ = fOutBuf_.begin ();
                    fOutBufEnd_   = fOutBufStart_ + nBytesInOutBuf;
                }
                else {
                    fOutBuf_.GrowToSize_uninitialized (_GetMinOutBufSize (NEltsOf (toDecryptBuf)));
                    span<byte> outBufUsed = _runOnce (span{toDecryptBuf, n2Decrypt}, span{fOutBuf_});
                    Assert (outBufUsed.size () <= fOutBuf_.GetSize ());
                    if (outBufUsed.size () == 0) {
                        // This can happen with block ciphers - we put stuff in, and get nothing out. But we cannot return EOF
                        // yet, so try again...
                        goto Again;
                    }
                    else {
                        fOutBufStart_ = fOutBuf_.begin ();
                        fOutBufEnd_   = fOutBufStart_ + outBufUsed.size ();
                    }
                }
            }
            return true;
        }

    private:
        mutable recursive_mutex                                        fCriticalSection_;
        Memory::InlineBuffer<byte, kInBufSize_ + EVP_MAX_BLOCK_LENGTH> fOutBuf_{_GetMinOutBufSize (kInBufSize_)};
        byte*                                                          fOutBufStart_{nullptr};
        byte*                                                          fOutBufEnd_{nullptr};
        InputStream::Ptr<byte>                                         fRealIn_;
    };

    class OpenSSLOutputStreamRep_ : public OutputStream::IRep<byte>, private InOutStrmCommon_ {
    public:
        OpenSSLOutputStreamRep_ (const OpenSSLCryptoParams& cryptoParams, Direction d, const OutputStream::Ptr<byte>& realOut)
            : InOutStrmCommon_{cryptoParams, d}
            , fRealOut_{realOut}
        {
        }
        virtual ~OpenSSLOutputStreamRep_ ()
        {
            // no need for critical section because at most one thread can be running DTOR at a time, and no other methods can be running
            try {
                Flush ();
            }
            catch (...) {
                // not great to do in DTOR, because we must drop exceptions on the floor!
            }
        }
        virtual bool IsSeekable () const override
        {
            return false;
        }
        virtual void CloseWrite () override
        {
            Require (IsOpenWrite ());
            fRealOut_.Close ();
            Assert (fRealOut_ == nullptr);
            Ensure (not IsOpenWrite ());
        }
        virtual bool IsOpenWrite () const override
        {
            return fRealOut_ != nullptr;
        }
        virtual SeekOffsetType GetWriteOffset () const override
        {
            RequireNotReached ();
            Require (IsOpenWrite ());
            return 0;
        }
        virtual SeekOffsetType SeekWrite (Whence /*whence*/, SignedSeekOffsetType /*offset*/) override
        {
            RequireNotReached ();
            Require (IsOpenWrite ());
            return 0;
        }
        // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
        // Writes always succeed fully or throw.
        virtual void Write (span<const byte> elts) override
        {
            Require (not elts.empty ());
            Require (IsOpenWrite ());
            StackBuffer<byte, 1000 + EVP_MAX_BLOCK_LENGTH> outBuf{Memory::eUninitialized, _GetMinOutBufSize (elts.size ())};
            [[maybe_unused]] auto&&                        critSec    = lock_guard{fCriticalSection_};
            span<byte>                                     outBufUsed = _runOnce (elts, span{outBuf});
            Assert (outBufUsed.size () <= outBuf.GetSize ());
            fRealOut_.Write (outBufUsed);
        }
        virtual void Flush () override
        {
            Require (IsOpenWrite ());
            byte   outBuf[EVP_MAX_BLOCK_LENGTH]; // intentionally uninitialized
            size_t nBytesInOutBuf = _cipherFinal (begin (outBuf), end (outBuf));
            Assert (nBytesInOutBuf < sizeof (outBuf));
            fRealOut_.Write (span{outBuf, nBytesInOutBuf});
        }

    private:
        mutable recursive_mutex fCriticalSection_;
        OutputStream::Ptr<byte> fRealOut_;
    };
}

/*
 ********************************************************************************
 ******************** Cryptography::OpenSSLInputStream **************************
 ********************************************************************************
 */
namespace {
    void ApplySettings2CTX_ (EVP_CIPHER_CTX* ctx, const EVP_CIPHER* cipher, Direction d, bool nopad, bool useArgumentKeyLength,
                             const Memory::BLOB& key, const Memory::BLOB& initialIV)
    {
        RequireNotNull (ctx);
        RequireNotNull (cipher);
        bool enc = (d == Direction::eEncrypt);
        if (nopad) {
            Verify (::EVP_CIPHER_CTX_set_padding (ctx, 0) == 1);
        }
        Cryptography::OpenSSL::Exception::ThrowLastErrorIfFailed (::EVP_CipherInit_ex (ctx, cipher, NULL, nullptr, nullptr, enc));
        size_t keyLen = ::EVP_CIPHER_CTX_key_length (ctx);
        size_t ivLen  = ::EVP_CIPHER_CTX_iv_length (ctx);

        if (useArgumentKeyLength) {
            keyLen = key.length ();
            Verify (::EVP_CIPHER_CTX_set_key_length (ctx, static_cast<int> (keyLen)) == 1);
        }

        StackBuffer<byte> useKey{Memory::eUninitialized, keyLen};
        StackBuffer<byte> useIV{Memory::eUninitialized, ivLen};

        (void)::memset (useKey.begin (), 0, keyLen);
        (void)::memset (useIV.begin (), 0, ivLen);

        (void)::memcpy (useKey.begin (), key.begin (), min (keyLen, key.size ()));
        if (not initialIV.empty ()) {
            (void)::memcpy (useIV.begin (), initialIV.begin (), min (ivLen, initialIV.size ()));
        }
        Cryptography::OpenSSL::Exception::ThrowLastErrorIfFailed (::EVP_CipherInit_ex (
            ctx, nullptr, NULL, reinterpret_cast<unsigned char*> (useKey.begin ()), reinterpret_cast<unsigned char*> (useIV.begin ()), enc));
    }
}

OpenSSLCryptoParams::OpenSSLCryptoParams (CipherAlgorithm alg, const BLOB& key, const BLOB& initialIV)
{
    using namespace OpenSSL;
    bool nopad                = false;
    bool useArgumentKeyLength = false;
    if (alg == CipherAlgorithms::kRC2_CBC () or alg == CipherAlgorithms::kRC2_ECB () or alg == CipherAlgorithms::kRC2_CFB () or
        alg == CipherAlgorithms::kRC2_OFB () or alg == CipherAlgorithms::kRC4 ()) {
        useArgumentKeyLength = true;
    }
    fInitializer = [=] (::EVP_CIPHER_CTX* ctx, Direction d) {
        ApplySettings2CTX_ (ctx, alg, d, nopad, useArgumentKeyLength, key, initialIV);
    };
}

OpenSSLCryptoParams::OpenSSLCryptoParams (CipherAlgorithm alg, const DerivedKey& derivedKey)
    : OpenSSLCryptoParams{alg, derivedKey.fKey, derivedKey.fIV}
{
}

/*
 ********************************************************************************
 ******************** Cryptography::OpenSSLInputStream **************************
 ********************************************************************************
 */
auto OpenSSLInputStream::New (const OpenSSLCryptoParams& cryptoParams, Direction direction, const InputStream::Ptr<byte>& realIn)
    -> Streams::InputStream::Ptr<byte>
{
    return Streams::InputStream::Ptr<byte>{make_shared<OpenSSLInputStreamRep_> (cryptoParams, direction, realIn)};
}

auto OpenSSLInputStream::New (Execution::InternallySynchronized internallySynchronized, const OpenSSLCryptoParams& cryptoParams,
                              Direction direction, const InputStream::Ptr<byte>& realIn) -> Streams::InputStream::Ptr<byte>
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return Streams::InternallySynchronizedInputStream::New<OpenSSLInputStreamRep_> ({}, cryptoParams, direction, realIn);
        case Execution::eNotKnownInternallySynchronized:
            return New (cryptoParams, direction, realIn);
        default:
            RequireNotReached ();
            return New (cryptoParams, direction, realIn);
    }
}

/*
 ********************************************************************************
 ******************* Cryptography::OpenSSLOutputStream **************************
 ********************************************************************************
 */
auto OpenSSLOutputStream::New (const OpenSSLCryptoParams& cryptoParams, Direction direction, const OutputStream::Ptr<byte>& realOut)
    -> Streams::OutputStream::Ptr<byte>
{
    return Streams::OutputStream::Ptr<byte>{make_shared<OpenSSLOutputStreamRep_> (cryptoParams, direction, realOut)};
}

auto OpenSSLOutputStream::New (Execution::InternallySynchronized internallySynchronized, const OpenSSLCryptoParams& cryptoParams,
                               Direction direction, const OutputStream::Ptr<byte>& realOut) -> Streams::OutputStream::Ptr<byte>
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return Streams::InternallySynchronizedOutputStream::New<OpenSSLOutputStreamRep_> ({}, cryptoParams, direction, realOut);
        case Execution::eNotKnownInternallySynchronized:
            return New (cryptoParams, direction, realOut);
        default:
            RequireNotReached ();
            return New (cryptoParams, direction, realOut);
    }
}
#endif
