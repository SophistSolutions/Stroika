/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qHasFeature_OpenSSL
#include    <openssl/evp.h>
#include    <openssl/err.h>
#endif

#include    "../../Containers/Common.h"
#include    "../../Debug/Assertions.h"
#include    "../../Execution/Common.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "OpenSSLCryptoStream.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Cryptography;
using   namespace   Stroika::Foundation::Cryptography::Encoding;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Streams;

using   Memory::BLOB;


// @todo examine/test https://github.com/saju/misc/blob/master/misc/openssl_aes.c



#if     qHasFeature_OpenSSL && defined (_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment (lib, "libeay32.lib")
#pragma comment (lib, "ssleay32.lib")
#endif




//// VERY ROUGH DRAFT - NOT VERY CLOSE TO CORRECT FOR ALL ALGORITHSM
//// SEE http://www.openssl.org/docs/crypto/EVP_EncryptInit.html
//// for details on what todo


#if     qHasFeature_OpenSSL
namespace {
    struct  InOutStrmCommon_ {
        InOutStrmCommon_ (const OpenSSLCryptoParams& cryptoParams, Direction d)
            : fCTX_ ()
            , fFinalCalled_ (false)
        {
            ::EVP_CIPHER_CTX_init (&fCTX_);
            cryptoParams.fInitializer (&fCTX_, d);
        }
        InOutStrmCommon_ (const InOutStrmCommon_&) = delete;
        InOutStrmCommon_& operator= (const InOutStrmCommon_&) = delete;
        ~InOutStrmCommon_ ()
        {
            Verify (::EVP_CIPHER_CTX_cleanup (&fCTX_) == 1);
        }
        static  constexpr   size_t _GetMinOutBufSize (size_t n)
        {
            return n + EVP_MAX_BLOCK_LENGTH;
        }
        // return nBytes in outBuf, throws on error
        size_t _runOnce (const Byte* data2ProcessStart, const Byte* data2ProcessEnd, Byte* outBufStart, Byte* outBufEnd)
        {
            Require (outBufStart <= outBufEnd and static_cast<size_t> (outBufEnd - outBufStart) >= _GetMinOutBufSize (data2ProcessEnd - data2ProcessStart));  // always need out buf big enuf for inbuf
            int outLen = 0;
            Cryptography::OpenSSL::Exception::DoThrowLastErrorIfFailed (::EVP_CipherUpdate (&fCTX_, outBufStart, &outLen, data2ProcessStart, static_cast<int> (data2ProcessEnd - data2ProcessStart)));
            Ensure (outLen >= 0);
            Ensure (outLen <= (outBufEnd - outBufStart));
            return size_t (outLen);
        }
        // return nBytes in outBuf, throws on error
        // Can call multiple times - it keeps track itself if finalized.
        size_t _cipherFinal (Byte* outBufStart, Byte* outBufEnd)
        {
            Require (outBufStart <= outBufEnd and static_cast<size_t> (outBufEnd - outBufStart) >= _GetMinOutBufSize (0));
            if (fFinalCalled_) {
                return 0;   // not an error - just zero more bytes
            }
            int outLen = 0;
            Cryptography::OpenSSL::Exception::DoThrowLastErrorIfFailed (::EVP_CipherFinal_ex (&fCTX_, outBufStart, &outLen));
            fFinalCalled_ = true;
            Ensure (outLen >= 0);
            Ensure (outLen <= (outBufEnd - outBufStart));
            return size_t (outLen);
        }
        EVP_CIPHER_CTX  fCTX_;
        bool            fFinalCalled_;
    };
}
#endif



#if     qHasFeature_OpenSSL
class   OpenSSLInputStream::IRep_ : public BinaryInputStream::_IRep, private InOutStrmCommon_ {
private:
    DEFINE_CONSTEXPR_CONSTANT(size_t, kInBufSize_, 10 * 1024);

public:
    IRep_ (const OpenSSLCryptoParams& cryptoParams, Direction d, const BinaryInputStream& realIn)
        : BinaryInputStream::_IRep ()
        , InOutStrmCommon_ (cryptoParams, d)
        , fCriticalSection_ ()
        , fOutBuf_ (_GetMinOutBufSize (kInBufSize_))
        , fOutBufStart_ (nullptr)
        , fOutBufEnd_ (nullptr)
        , fRealIn_ (realIn)
    {
    }

    virtual size_t  Read (Byte* intoStart, Byte* intoEnd) override
    {
        {
            /*
             *  Keep track if unread bytes in fOutBuf_ - bounded by fOutBufStart_ and fOutBufEnd_.
             *  If none to read there - pull from fRealIn_ src, and push those through the cipher.
             *  and use that to re-populate fOutBuf_.
             */
            Require (intoStart < intoEnd);
            DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wfuture-compat\"");
            auto    critSec { Execution::make_unique_lock (fCriticalSection_) };
            DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wfuture-compat\"");
            if (fOutBufStart_ == fOutBufEnd_) {
                Byte toDecryptBuf[kInBufSize_];
                size_t n2Decrypt = fRealIn_.Read (begin (toDecryptBuf), end (toDecryptBuf));
                if (n2Decrypt == 0) {
                    size_t nBytesInOutBuf = _cipherFinal (fOutBuf_.begin (), fOutBuf_.end ());
                    fOutBufStart_ = fOutBuf_.begin ();
                    fOutBufEnd_ = fOutBufStart_ + nBytesInOutBuf;
                }
                else {
                    fOutBuf_.GrowToSize (_GetMinOutBufSize (NEltsOf (toDecryptBuf)));
                    size_t nBytesInOutBuf = _runOnce (begin (toDecryptBuf), begin (toDecryptBuf) + n2Decrypt, fOutBuf_.begin (), fOutBuf_.end ());
                    fOutBufStart_ = fOutBuf_.begin ();
                    fOutBufEnd_ = fOutBufStart_ + nBytesInOutBuf;
                }
            }
            if (fOutBufStart_ < fOutBufEnd_) {
                size_t  n2Copy = min (fOutBufEnd_ - fOutBufStart_, intoEnd - intoStart);
                (void)::memcpy (intoStart, fOutBufStart_, n2Copy);
                fOutBufStart_ += n2Copy;
                return n2Copy;
            }
        }
        return 0;   // EOF
    }

private:
    mutable mutex                                                           fCriticalSection_;
    Memory::SmallStackBuffer < Byte, kInBufSize_ + EVP_MAX_BLOCK_LENGTH >   fOutBuf_;
    Byte*                                                                   fOutBufStart_;
    Byte*                                                                   fOutBufEnd_;
    BinaryInputStream                                                       fRealIn_;
};
#endif






#if     qHasFeature_OpenSSL
class   OpenSSLOutputStream::IRep_ : public BinaryOutputStream::_IRep, private InOutStrmCommon_ {
public:
    IRep_ (const OpenSSLCryptoParams& cryptoParams, Direction d, const BinaryOutputStream& realOut)
        : BinaryOutputStream::_IRep ()
        , InOutStrmCommon_ (cryptoParams, d)
        , fCriticalSection_ ()
        , fRealOut_ (realOut)
    {
    }

    virtual ~IRep_ ()
    {
        // no need for critical section because at most one thread can be running DTOR at a time, and no other methods can be running
        try {
            Flush ();
        }
        catch (...) {
            // not great to do in DTOR, because we must drop exceptions on the floor!
        }
    }

    // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
    // Writes always succeed fully or throw.
    virtual void    Write (const Byte* start, const Byte* end) override
    {
        Require (start < end);  // for BinaryOutputStream - this funciton requires non-empty write
        Memory::SmallStackBuffer < Byte, 1000 + EVP_MAX_BLOCK_LENGTH >  outBuf (_GetMinOutBufSize (end - start));
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Wfuture-compat\"");
        auto    critSec { Execution::make_unique_lock (fCriticalSection_) };
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Wfuture-compat\"");
        size_t nBytesEncypted = _runOnce (start, end, outBuf.begin (), outBuf.end ());
        Assert (nBytesEncypted <= outBuf.GetSize ());
        fRealOut_.Write (outBuf.begin (), outBuf.begin () + nBytesEncypted);
    }

    virtual void    Flush () override
    {
        Byte    outBuf[EVP_MAX_BLOCK_LENGTH];
        size_t nBytesInOutBuf = _cipherFinal (begin (outBuf), end (outBuf));
        Assert (nBytesInOutBuf < sizeof (outBuf));
        fRealOut_.Write (begin (outBuf), begin (outBuf) + nBytesInOutBuf);
    }

private:
    mutable recursive_mutex     fCriticalSection_;
    BinaryOutputStream          fRealOut_;
};
#endif









#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ******************** Cryptography::OpenSSLInputStream **************************
 ********************************************************************************
 */
namespace {
    void    ApplySettings2CTX_ (EVP_CIPHER_CTX* ctx, const EVP_CIPHER* cipher, Direction d, bool nopad, bool useArgumentKeyLength, const Memory::BLOB& key, const Memory::BLOB& initialIV)
    {
        RequireNotNull (ctx);
        RequireNotNull (cipher);
        bool    enc = (d == Direction::eEncrypt);
        if (nopad) {
            Verify (::EVP_CIPHER_CTX_set_padding (ctx, 0) == 1);
        }
        Cryptography::OpenSSL::Exception::DoThrowLastErrorIfFailed (::EVP_CipherInit_ex (ctx, cipher, NULL, nullptr, nullptr, enc));
        size_t keyLen = EVP_CIPHER_CTX_key_length (ctx);
        size_t ivLen = EVP_CIPHER_CTX_iv_length (ctx);

        if (useArgumentKeyLength) {
            keyLen = key.length ();
            Verify (::EVP_CIPHER_CTX_set_key_length (ctx, static_cast<int> (keyLen)) == 1);
        }

        Memory::SmallStackBuffer<Byte> useKey { keyLen };
        Memory::SmallStackBuffer<Byte> useIV { ivLen };

        memset (useKey.begin (), 0, keyLen);
        memset (useIV.begin (), 0, ivLen);

        memcpy (useKey.begin (), key.begin (), min(keyLen, key.size ()));
        memcpy (useIV.begin (), initialIV.begin (), min(ivLen, initialIV.size ()));

        Cryptography::OpenSSL::Exception::DoThrowLastErrorIfFailed (::EVP_CipherInit_ex (ctx, nullptr, NULL, useKey.begin (), useIV.begin (), enc));
    }
}

OpenSSLCryptoParams::OpenSSLCryptoParams (CipherAlgorithm alg, Memory::BLOB key, Memory::BLOB initialIV)
    : fInitializer ()
{
    bool    nopad = false;
    switch (alg) {
        case CipherAlgorithm::eRC2_CBC: {
                fInitializer = [nopad, key, initialIV] (EVP_CIPHER_CTX * ctx, Direction d) {
                    ApplySettings2CTX_ (ctx, ::EVP_rc2_cbc (), d, nopad, true, key, initialIV);
                };
            }
            break;
        case CipherAlgorithm::eRC2_ECB: {
                fInitializer = [nopad, key, initialIV] (EVP_CIPHER_CTX * ctx, Direction d) {
                    ApplySettings2CTX_ (ctx, ::EVP_rc2_ecb (), d, nopad, true, key, initialIV);
                };
            }
            break;
        case CipherAlgorithm::eRC2_CFB: {
                fInitializer = [nopad, key, initialIV] (EVP_CIPHER_CTX * ctx, Direction d) {
                    ApplySettings2CTX_ (ctx, ::EVP_rc2_cfb (), d, nopad, true, key, initialIV);
                };
            }
            break;
        case CipherAlgorithm::eRC2_OFB: {
                fInitializer = [nopad, key, initialIV] (EVP_CIPHER_CTX * ctx, Direction d) {
                    ApplySettings2CTX_ (ctx, ::EVP_rc2_ofb (), d, nopad, true, key, initialIV);
                };
            }
            break;
        case CipherAlgorithm::eRC4: {
                fInitializer = [nopad, key, initialIV] (EVP_CIPHER_CTX * ctx, Direction d) {
                    ApplySettings2CTX_ (ctx, ::EVP_rc4 (), d, nopad, true, key, initialIV);
                };
            }
            break;
        default: {
                fInitializer = [alg, nopad, key, initialIV] (EVP_CIPHER_CTX * ctx, Direction d) {
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





#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ******************** Cryptography::OpenSSLInputStream **************************
 ********************************************************************************
 */
OpenSSLInputStream::OpenSSLInputStream (const OpenSSLCryptoParams& cryptoParams, Direction direction, const BinaryInputStream& realIn)
    : BinaryInputStream (shared_ptr<_IRep> (new IRep_ (cryptoParams, direction, realIn)))
{
}
#endif





#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ******************* Cryptography::OpenSSLOutputStream **************************
 ********************************************************************************
 */
OpenSSLOutputStream::OpenSSLOutputStream (const OpenSSLCryptoParams& cryptoParams, Direction direction, const BinaryOutputStream& realOut)
    : BinaryOutputStream (_SharedIRep (new IRep_ (cryptoParams, direction, realOut)))
{
}
#endif
