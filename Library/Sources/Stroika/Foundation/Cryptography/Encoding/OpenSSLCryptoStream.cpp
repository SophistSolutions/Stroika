/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qHasFeature_OpenSSL
#include <openssl/err.h>
#include <openssl/evp.h>
#endif

#include "../../Containers/Common.h"
#include "../../Debug/Assertions.h"
#include "../../Execution/Common.h"
#include "../../Memory/SmallStackBuffer.h"

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
            : fCTX_ (::EVP_CIPHER_CTX_new ())
            , fFinalCalled_ (false)
        {
            ::EVP_CIPHER_CTX_init (fCTX_);
            cryptoParams.fInitializer (fCTX_, d);
        }
        InOutStrmCommon_ (const InOutStrmCommon_&) = delete;
        InOutStrmCommon_& operator= (const InOutStrmCommon_&) = delete;
        ~InOutStrmCommon_ ()
        {
            Verify (::EVP_CIPHER_CTX_cleanup (fCTX_) == 1);
            ::EVP_CIPHER_CTX_free (fCTX_);
        }
        static constexpr size_t _GetMinOutBufSize (size_t n)
        {
            return n + EVP_MAX_BLOCK_LENGTH;
        }
        // return nBytes in outBuf, throws on error
        size_t _runOnce (const byte* data2ProcessStart, const byte* data2ProcessEnd, byte* outBufStart, [[maybe_unused]] byte* outBufEnd)
        {
            Require (outBufStart <= outBufEnd and static_cast<size_t> (outBufEnd - outBufStart) >= _GetMinOutBufSize (data2ProcessEnd - data2ProcessStart)); // always need out buf big enuf for inbuf
            int outLen = 0;
            Cryptography::OpenSSL::Exception::ThrowLastErrorIfFailed (::EVP_CipherUpdate (fCTX_, reinterpret_cast<unsigned char*> (outBufStart), &outLen, reinterpret_cast<const unsigned char*> (data2ProcessStart), static_cast<int> (data2ProcessEnd - data2ProcessStart)));
            Ensure (outLen >= 0);
            Ensure (outLen <= (outBufEnd - outBufStart));
            return size_t (outLen);
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
            Cryptography::OpenSSL::Exception::ThrowLastErrorIfFailed (::EVP_CipherFinal_ex (fCTX_, reinterpret_cast<unsigned char*> (outBufStart), &outLen));
            fFinalCalled_ = true;
            Ensure (outLen >= 0);
            Ensure (outLen <= (outBufEnd - outBufStart));
            return size_t (outLen);
        }
        EVP_CIPHER_CTX* fCTX_;
        bool            fFinalCalled_;
    };
}
#endif

#if qHasFeature_OpenSSL
class OpenSSLInputStream::Rep_ : public InputStream<byte>::_IRep, private InOutStrmCommon_ {
private:
    static constexpr size_t kInBufSize_ = 10 * 1024;

public:
    Rep_ (const OpenSSLCryptoParams& cryptoParams, Direction d, const InputStream<byte>::Ptr& realIn)
        : InputStream<byte>::_IRep ()
        , InOutStrmCommon_ (cryptoParams, d)
        , fCriticalSection_ ()
        , fOutBuf_ (_GetMinOutBufSize (kInBufSize_))
        , fOutBufStart_ (nullptr)
        , fOutBufEnd_ (nullptr)
        , fRealIn_ (realIn)
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
    virtual SeekOffsetType SeekRead (Whence /*whence*/, SignedSeekOffsetType /*offset*/) override
    {
        RequireNotReached ();
        Require (IsOpenRead ());
        return 0;
    }
    virtual size_t Read (byte* intoStart, byte* intoEnd) override
    {
        /*
         *  Keep track if unread bytes in fOutBuf_ - bounded by fOutBufStart_ and fOutBufEnd_.
         *  If none to read there - pull from fRealIn_ src, and push those through the cipher.
         *  and use that to re-populate fOutBuf_.
         */
        Require (intoStart < intoEnd);
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
        Require (IsOpenRead ());
        if (fOutBufStart_ == fOutBufEnd_) {
            /*
             *  Then pull from 'real in' stream until we have reach EOF there, or until we have some bytes to output
             *  on our own.
             */
            byte toDecryptBuf[kInBufSize_];
        Again:
            size_t n2Decrypt = fRealIn_.Read (begin (toDecryptBuf), end (toDecryptBuf));
            if (n2Decrypt == 0) {
                size_t nBytesInOutBuf = _cipherFinal (fOutBuf_.begin (), fOutBuf_.end ());
                Assert (nBytesInOutBuf <= fOutBuf_.GetSize ());
                fOutBufStart_ = fOutBuf_.begin ();
                fOutBufEnd_   = fOutBufStart_ + nBytesInOutBuf;
            }
            else {
                fOutBuf_.GrowToSize_uninitialized (_GetMinOutBufSize (NEltsOf (toDecryptBuf)));
                size_t nBytesInOutBuf = _runOnce (begin (toDecryptBuf), begin (toDecryptBuf) + n2Decrypt, fOutBuf_.begin (), fOutBuf_.end ());
                Assert (nBytesInOutBuf <= fOutBuf_.GetSize ());
                if (nBytesInOutBuf == 0) {
                    // This can happen with block ciphers - we put stuff in, and get nothing out. But we cannot return EOF
                    // yet, so try again...
                    goto Again;
                }
                else {
                    fOutBufStart_ = fOutBuf_.begin ();
                    fOutBufEnd_   = fOutBufStart_ + nBytesInOutBuf;
                }
            }
        }
        if (fOutBufStart_ < fOutBufEnd_) {
            size_t n2Copy = min (fOutBufEnd_ - fOutBufStart_, intoEnd - intoStart);
            (void)::memcpy (intoStart, fOutBufStart_, n2Copy);
            fOutBufStart_ += n2Copy;
            return n2Copy;
        }
        return 0; // EOF
    }
    virtual optional<size_t> ReadNonBlocking (ElementType* intoStart, ElementType* intoEnd) override
    {
        Require ((intoStart == nullptr and intoEnd == nullptr) or (intoEnd - intoStart) >= 1);
        [[maybe_unused]] auto&& critSec = lock_guard{fCriticalSection_};
        Require (IsOpenRead ());
        // advance fOutBufStart_ if possible, and then we know if there is upstream data, and can use _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream
        if (fOutBufStart_ == fOutBufEnd_) {
            byte toDecryptBuf[kInBufSize_];
        Again:
            optional<size_t> n2Decrypt = fRealIn_.ReadNonBlocking (begin (toDecryptBuf), end (toDecryptBuf));
            if (not n2Decrypt.has_value ()) {
                // if no known data upstream, we cannot say if this is EOF
                return {};
            }
            else if (n2Decrypt == 0u) {
                size_t nBytesInOutBuf = _cipherFinal (fOutBuf_.begin (), fOutBuf_.end ());
                Assert (nBytesInOutBuf <= fOutBuf_.GetSize ());
                fOutBufStart_ = fOutBuf_.begin ();
                fOutBufEnd_   = fOutBufStart_ + nBytesInOutBuf;
            }
            else {
                fOutBuf_.GrowToSize_uninitialized (_GetMinOutBufSize (NEltsOf (toDecryptBuf)));
                size_t nBytesInOutBuf = _runOnce (begin (toDecryptBuf), begin (toDecryptBuf) + *n2Decrypt, fOutBuf_.begin (), fOutBuf_.end ());
                Assert (nBytesInOutBuf <= fOutBuf_.GetSize ());
                if (nBytesInOutBuf == 0) {
                    // This can happen with block ciphers - we put stuff in, and get nothing out. But we cannot return EOF
                    // yet, so try again...
                    goto Again;
                }
                else {
                    fOutBufStart_ = fOutBuf_.begin ();
                    fOutBufEnd_   = fOutBufStart_ + nBytesInOutBuf;
                }
            }
        }
        return _ReadNonBlocking_ReferenceImplementation_ForNonblockingUpstream (intoStart, intoEnd, fOutBufEnd_ - fOutBufStart_);
    }

private:
    mutable recursive_mutex                                            fCriticalSection_;
    Memory::SmallStackBuffer<byte, kInBufSize_ + EVP_MAX_BLOCK_LENGTH> fOutBuf_;
    byte*                                                              fOutBufStart_;
    byte*                                                              fOutBufEnd_;
    InputStream<byte>::Ptr                                             fRealIn_;
};
#endif

#if qHasFeature_OpenSSL
class OpenSSLOutputStream::Rep_ : public OutputStream<byte>::_IRep, private InOutStrmCommon_ {
public:
    Rep_ (const OpenSSLCryptoParams& cryptoParams, Direction d, const OutputStream<byte>::Ptr& realOut)
        : OutputStream<byte>::_IRep ()
        , InOutStrmCommon_ (cryptoParams, d)
        , fCriticalSection_ ()
        , fRealOut_ (realOut)
    {
    }
    virtual ~Rep_ ()
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
    virtual void Write (const byte* start, const byte* end) override
    {
        Require (start < end); // for OutputStream<byte> - this funciton requires non-empty write
        Require (IsOpenWrite ());
        SmallStackBuffer<byte, 1000 + EVP_MAX_BLOCK_LENGTH> outBuf (SmallStackBufferCommon::eUninitialized, _GetMinOutBufSize (end - start));
        [[maybe_unused]] auto&&                             critSec        = lock_guard{fCriticalSection_};
        size_t                                              nBytesEncypted = _runOnce (start, end, outBuf.begin (), outBuf.end ());
        Assert (nBytesEncypted <= outBuf.GetSize ());
        fRealOut_.Write (outBuf.begin (), outBuf.begin () + nBytesEncypted);
    }

    virtual void Flush () override
    {
        Require (IsOpenWrite ());
        byte   outBuf[EVP_MAX_BLOCK_LENGTH];
        size_t nBytesInOutBuf = _cipherFinal (begin (outBuf), end (outBuf));
        Assert (nBytesInOutBuf < sizeof (outBuf));
        fRealOut_.Write (begin (outBuf), begin (outBuf) + nBytesInOutBuf);
    }

private:
    mutable recursive_mutex fCriticalSection_;
    OutputStream<byte>::Ptr fRealOut_;
};
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ******************** Cryptography::OpenSSLInputStream **************************
 ********************************************************************************
 */
namespace {
    void ApplySettings2CTX_ (EVP_CIPHER_CTX* ctx, const EVP_CIPHER* cipher, Direction d, bool nopad, bool useArgumentKeyLength, const Memory::BLOB& key, const Memory::BLOB& initialIV)
    {
        RequireNotNull (ctx);
        RequireNotNull (cipher);
        bool enc = (d == Direction::eEncrypt);
        if (nopad) {
            Verify (::EVP_CIPHER_CTX_set_padding (ctx, 0) == 1);
        }
        Cryptography::OpenSSL::Exception::ThrowLastErrorIfFailed (::EVP_CipherInit_ex (ctx, cipher, NULL, nullptr, nullptr, enc));
        size_t keyLen = EVP_CIPHER_CTX_key_length (ctx);
        size_t ivLen  = EVP_CIPHER_CTX_iv_length (ctx);

        if (useArgumentKeyLength) {
            keyLen = key.length ();
            Verify (::EVP_CIPHER_CTX_set_key_length (ctx, static_cast<int> (keyLen)) == 1);
        }

        SmallStackBuffer<byte> useKey{SmallStackBufferCommon::eUninitialized, keyLen};
        SmallStackBuffer<byte> useIV{SmallStackBufferCommon::eUninitialized, ivLen};

        (void)::memset (useKey.begin (), 0, keyLen);
        (void)::memset (useIV.begin (), 0, ivLen);

        (void)::memcpy (useKey.begin (), key.begin (), min (keyLen, key.size ()));
        if (not initialIV.empty ()) {
            (void)::memcpy (useIV.begin (), initialIV.begin (), min (ivLen, initialIV.size ()));
        }
        Cryptography::OpenSSL::Exception::ThrowLastErrorIfFailed (::EVP_CipherInit_ex (ctx, nullptr, NULL, reinterpret_cast<unsigned char*> (useKey.begin ()), reinterpret_cast<unsigned char*> (useIV.begin ()), enc));
    }
}

OpenSSLCryptoParams::OpenSSLCryptoParams (CipherAlgorithm alg, const BLOB& key, const BLOB& initialIV)
    : fInitializer ()
{
    bool nopad = false;
    switch (alg) {
        case CipherAlgorithm::eRC2_CBC: {
            fInitializer = [nopad, key, initialIV] (EVP_CIPHER_CTX* ctx, Direction d) {
                ApplySettings2CTX_ (ctx, ::EVP_rc2_cbc (), d, nopad, true, key, initialIV);
            };
        } break;
        case CipherAlgorithm::eRC2_ECB: {
            fInitializer = [nopad, key, initialIV] (EVP_CIPHER_CTX* ctx, Direction d) {
                ApplySettings2CTX_ (ctx, ::EVP_rc2_ecb (), d, nopad, true, key, initialIV);
            };
        } break;
        case CipherAlgorithm::eRC2_CFB: {
            fInitializer = [nopad, key, initialIV] (EVP_CIPHER_CTX* ctx, Direction d) {
                ApplySettings2CTX_ (ctx, ::EVP_rc2_cfb (), d, nopad, true, key, initialIV);
            };
        } break;
        case CipherAlgorithm::eRC2_OFB: {
            fInitializer = [nopad, key, initialIV] (EVP_CIPHER_CTX* ctx, Direction d) {
                ApplySettings2CTX_ (ctx, ::EVP_rc2_ofb (), d, nopad, true, key, initialIV);
            };
        } break;
        case CipherAlgorithm::eRC4: {
            fInitializer = [nopad, key, initialIV] (EVP_CIPHER_CTX* ctx, Direction d) {
                ApplySettings2CTX_ (ctx, ::EVP_rc4 (), d, nopad, true, key, initialIV);
            };
        } break;
        default: {
            fInitializer = [alg, nopad, key, initialIV] (EVP_CIPHER_CTX* ctx, Direction d) {
                ApplySettings2CTX_ (ctx, Convert2OpenSSL (alg), d, nopad, false, key, initialIV);
            };
            break;
        }
    }
}

OpenSSLCryptoParams::OpenSSLCryptoParams (CipherAlgorithm alg, const DerivedKey& derivedKey)
    : OpenSSLCryptoParams (alg, derivedKey.fKey, derivedKey.fIV)
{
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ******************** Cryptography::OpenSSLInputStream **************************
 ********************************************************************************
 */
auto OpenSSLInputStream::New (const OpenSSLCryptoParams& cryptoParams, Direction direction, const InputStream<byte>::Ptr& realIn) -> Ptr
{
    return _mkPtr (make_shared<Rep_> (cryptoParams, direction, realIn));
}

auto OpenSSLInputStream::New (Execution::InternallySynchronized internallySynchronized, const OpenSSLCryptoParams& cryptoParams, Direction direction, const InputStream<byte>::Ptr& realIn) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return InternalSyncRep_::New (cryptoParams, direction, realIn);
        case Execution::eNotKnownInternallySynchronized:
            return New (cryptoParams, direction, realIn);
        default:
            RequireNotReached ();
            return New (cryptoParams, direction, realIn);
    }
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ******************* Cryptography::OpenSSLOutputStream **************************
 ********************************************************************************
 */
auto OpenSSLOutputStream::New (const OpenSSLCryptoParams& cryptoParams, Direction direction, const OutputStream<byte>::Ptr& realOut) -> Ptr
{
    return _mkPtr (make_shared<Rep_> (cryptoParams, direction, realOut));
}

auto OpenSSLOutputStream::New (Execution::InternallySynchronized internallySynchronized, const OpenSSLCryptoParams& cryptoParams, Direction direction, const OutputStream<byte>::Ptr& realOut) -> Ptr
{
    switch (internallySynchronized) {
        case Execution::eInternallySynchronized:
            return InternalSyncRep_::New (cryptoParams, direction, realOut);
        case Execution::eNotKnownInternallySynchronized:
            return New (cryptoParams, direction, realOut);
        default:
            RequireNotReached ();
            return New (cryptoParams, direction, realOut);
    }
}
#endif
