/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

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



//// VERY ROUGH DRAFT - NOT VERY CLOSE TO CORRECT FOR ALL ALGORITHSM
//// SEE http://www.openssl.org/docs/crypto/EVP_EncryptInit.html
//// for details on what todo


#if     qHas_OpenSSL
namespace {
    struct InOutStrmCommon_ {
        InOutStrmCommon_ (const OpenSSLCryptoParams& cryptoParams)
            : fCTX_ ()
            , fFinalCalled_ (false)
        {
            ::EVP_CIPHER_CTX_init (&fCTX_);
            cryptoParams.fInitializer (&fCTX_);
        }
        ~InOutStrmCommon_ ()
        {
            ::EVP_CIPHER_CTX_cleanup (&fCTX_);
        }
        static  constexpr   size_t _GetMinOutBufSize (size_t n)
        {
            return n + EVP_MAX_BLOCK_LENGTH;
        }
        // return nBytes in outBuf, throws on error
        size_t _runOnce (const Byte* data2ProcessStart, const Byte* data2ProcessEnd, Byte* outBufStart, Byte* outBufEnd)
        {
            Require ((outBufEnd - outBufStart) >= _GetMinOutBufSize (data2ProcessEnd - data2ProcessStart));  // always need out buf big enuf for inbuf
            int outLen = 0;
            if(not ::EVP_CipherUpdate (&fCTX_, outBufStart, &outLen, data2ProcessStart, data2ProcessEnd - data2ProcessStart)) {
                /* Error */
                // THROW
                return 0;
            }
            Ensure (outLen >= 0);
            Ensure (outLen <= (outBufEnd - outBufStart));
            return size_t (outLen);
        }
        // return nBytes in outBuf, throws on error
        // Can call multiple times - it keeps track itself if finalized.
        size_t _cipherFinal (Byte* outBufStart, Byte* outBufEnd)
        {
            Require ((outBufEnd - outBufStart) >= _GetMinOutBufSize (0));
            if (fFinalCalled_) {
                return 0;   // not an error - just zero more bytes
            }
            int outLen = 0;
            if(not ::EVP_CipherFinal_ex (&fCTX_, outBufStart, &outLen)) {
                /* Error */
                // THROW
                return 0;
            }
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



#if     qHas_OpenSSL
class   OpenSSLInputStream::IRep_ : public BinaryInputStream::_IRep, private InOutStrmCommon_ {
private:
    DEFINE_CONSTEXPR_CONSTANT(size_t, kInBufSize_, 10 * 1024);

public:
    IRep_ (const OpenSSLCryptoParams& cryptoParams, const BinaryInputStream& realIn)
        : BinaryInputStream::_IRep ()
        , InOutStrmCommon_ (cryptoParams)
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
            auto    critSec { Execution::make_unique_lock (fCriticalSection_) };
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






#if     qHas_OpenSSL
class   OpenSSLOutputStream::IRep_ : public BinaryOutputStream::_IRep, private InOutStrmCommon_ {
public:
    IRep_ (const OpenSSLCryptoParams& cryptoParams, const BinaryOutputStream& realOut)
        : BinaryOutputStream::_IRep ()
        , InOutStrmCommon_ (cryptoParams)
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
        auto    critSec { Execution::make_unique_lock (fCriticalSection_) };
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







#if     qHas_OpenSSL
/*
 ********************************************************************************
 ******************** Cryptography::OpenSSLInputStream **************************
 ********************************************************************************
 */
OpenSSLCryptoParams::OpenSSLCryptoParams (Algorithm alg, Memory::BLOB key, Direction direction, Memory::BLOB initialIV)
    : fInitializer ()
{
    bool    enc = (direction == Direction::eEncrypt);
    switch (alg) {
        case Algorithm::eAES_128_CBC: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_128_cbc (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_128_ECB: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_128_ecb (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_128_OFB: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_128_ofb (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_128_CFB1: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_128_cfb1 (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_128_CFB8: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_128_cfb8 (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_128_CFB128: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_128_cfb128 (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_192_CBC: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_192_cbc (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_192_ECB: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_192_ecb (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_192_OFB: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_192_ofb (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_192_CFB1: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_192_cfb1 (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_192_CFB8: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_192_cfb8 (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_192_CFB128: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_192_cfb128 (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_256_CBC: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_256_cbc (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_256_ECB: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_256_ecb (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_256_OFB: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_256_ofb (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_256_CFB1: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_256_cfb1 (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_256_CFB8: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_256_cfb8 (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eAES_256_CFB128: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_aes_256_cfb128 (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eBlowfish_CBC: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_bf_cbc (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eBlowfish_ECB: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_bf_ecb (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eBlowfish_CFB: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_bf_cfb (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eBlowfish_OFB: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_bf_ofb (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eRC2_CBC: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_rc2_cbc (), NULL, NULL, NULL, enc);
                    ::EVP_CIPHER_CTX_set_key_length (ctx, key.length ());
                    ::EVP_CipherInit_ex (ctx, NULL, NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eRC2_ECB: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_rc2_ecb (), NULL, NULL, NULL, enc);
                    ::EVP_CIPHER_CTX_set_key_length (ctx, key.length ());
                    ::EVP_CipherInit_ex (ctx, NULL, NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eRC2_CFB: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_rc2_cfb (), NULL, NULL, NULL, enc);
                    ::EVP_CIPHER_CTX_set_key_length (ctx, key.length ());
                    ::EVP_CipherInit_ex (ctx, NULL, NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eRC2_OFB: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_rc2_ofb (), NULL, NULL, NULL, enc);
                    ::EVP_CIPHER_CTX_set_key_length (ctx, key.length ());
                    ::EVP_CipherInit_ex (ctx, NULL, NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        case Algorithm::eRC4: {
                fInitializer = [&key, &initialIV, &enc] (EVP_CIPHER_CTX * ctx) {
                    ::EVP_CipherInit_ex (ctx, ::EVP_rc4 (), NULL, key.begin (), initialIV.begin (), enc);
                };
            }
            break;
        default: {
                RequireNotReached();
            }
            break;
    }
}
#endif





#if     qHas_OpenSSL
/*
 ********************************************************************************
 ******************** Cryptography::OpenSSLInputStream **************************
 ********************************************************************************
 */
OpenSSLInputStream::OpenSSLInputStream (const OpenSSLCryptoParams& cryptoParams, const BinaryInputStream& realIn)
    : BinaryInputStream (shared_ptr<_IRep> (new IRep_ (cryptoParams, realIn)))
{
}
#endif





#if     qHas_OpenSSL
/*
 ********************************************************************************
 ******************* Cryptography::OpenSSLOutputStream **************************
 ********************************************************************************
 */
OpenSSLOutputStream::OpenSSLOutputStream (const OpenSSLCryptoParams& cryptoParams, const BinaryOutputStream& realOut)
    : BinaryOutputStream (_SharedIRep (new IRep_ (cryptoParams, realOut)))
{
}
#endif
