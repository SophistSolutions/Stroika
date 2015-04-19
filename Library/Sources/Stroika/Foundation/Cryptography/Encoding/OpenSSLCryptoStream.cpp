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
            OpenSSLException::DoThrowLastErrorIfFailed (::EVP_CipherUpdate (&fCTX_, outBufStart, &outLen, data2ProcessStart, static_cast<int> (data2ProcessEnd - data2ProcessStart)));
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
            OpenSSLException::DoThrowLastErrorIfFailed (::EVP_CipherFinal_ex (&fCTX_, outBufStart, &outLen));
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






#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ********************** Cryptography::OpenSSLException **************************
 ********************************************************************************
 */
OpenSSLException::OpenSSLException (InternalErrorCodeType errorCode)
    : StringException (GetMessage (errorCode))
    , fErrorCode_ (errorCode)
{
}

OpenSSLException::InternalErrorCodeType OpenSSLException::GetErrorCode () const
{
    return fErrorCode_;
}

Characters::String  OpenSSLException::GetMessage (InternalErrorCodeType errorCode)
{
    char    buf[10 * 1024];
    buf[0] = '\0';
    ERR_error_string_n (errorCode, buf, NEltsOf (buf));
    return Characters::String::FromNarrowSDKString (buf);
}

void    OpenSSLException::DoThrowLastErrorIfFailed (int status)
{
    if (status != 1) {
        DoThrowLastError ();
    }
}

void    OpenSSLException::DoThrowLastError ()
{
    Execution::DoThrow (OpenSSLException (ERR_get_error ()));
}
#endif



#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ******************** Cryptography::OpenSSLInputStream **************************
 ********************************************************************************
 */
namespace {
    using   CipherAlgorithm = OpenSSLCryptoParams::CipherAlgorithm;
    const EVP_CIPHER* cvt2Cipher_ (CipherAlgorithm alg)
    {
        switch (alg) {
            case CipherAlgorithm::eAES_128_CBC:
                return ::EVP_aes_128_cbc ();
            case CipherAlgorithm::eAES_128_ECB:
                return ::EVP_aes_128_ecb ();
            case CipherAlgorithm::eAES_128_OFB:
                return ::EVP_aes_128_ofb ();
            case CipherAlgorithm::eAES_128_CFB1:
                return ::EVP_aes_128_cfb1 ();
            case CipherAlgorithm::eAES_128_CFB8:
                return ::EVP_aes_128_cfb8 ();
            case CipherAlgorithm::eAES_128_CFB128:
                return ::EVP_aes_128_cfb128 ();
            case CipherAlgorithm::eAES_192_CBC:
                return ::EVP_aes_192_cbc ();
            case CipherAlgorithm::eAES_192_ECB:
                return ::EVP_aes_192_ecb ();
            case CipherAlgorithm::eAES_192_OFB:
                return ::EVP_aes_192_ofb ();
            case CipherAlgorithm::eAES_192_CFB1:
                return ::EVP_aes_192_cfb1 ();
            case CipherAlgorithm::eAES_192_CFB8:
                return ::EVP_aes_192_cfb8 ();
            case CipherAlgorithm::eAES_192_CFB128:
                return ::EVP_aes_192_cfb128 ();
            case CipherAlgorithm::eAES_256_CBC:
                return ::EVP_aes_256_cbc ();
            case CipherAlgorithm::eAES_256_ECB:
                return ::EVP_aes_256_ecb ();
            case CipherAlgorithm::eAES_256_OFB:
                return ::EVP_aes_256_ofb ();
            case CipherAlgorithm::eAES_256_CFB1:
                return ::EVP_aes_256_cfb1 ();
            case CipherAlgorithm::eAES_256_CFB8:
                return ::EVP_aes_256_cfb8 ();
            case CipherAlgorithm::eAES_256_CFB128:
                return ::EVP_aes_256_cfb128 ();
            case CipherAlgorithm::eBlowfish_CBC:
                return ::EVP_bf_cbc ();
            case CipherAlgorithm::eBlowfish_ECB:
                return ::EVP_bf_ecb ();
            case CipherAlgorithm::eBlowfish_CFB:
                return ::EVP_bf_cfb ();
            case CipherAlgorithm::eBlowfish_OFB:
                return ::EVP_bf_ofb ();
            case CipherAlgorithm::eRC2_CBC:
                return ::EVP_rc2_cbc ();
            case CipherAlgorithm::eRC2_ECB:
                return ::EVP_rc2_ecb ();
            case CipherAlgorithm::eRC2_CFB:
                return ::EVP_rc2_cfb ();
            case CipherAlgorithm::eRC2_OFB:
                return ::EVP_rc2_ofb ();
            case CipherAlgorithm::eRC4:
                return ::EVP_rc4 ();
            default:
                RequireNotReached();
                return nullptr;
        }
    }
    const EVP_MD*   cvt2HashAlg_ (OpenSSLCryptoParams::HashAlg hashAlg)
    {
        using   HashAlg =   OpenSSLCryptoParams::HashAlg;
        switch (hashAlg) {
            case HashAlg::eMD5:
                return EVP_md5 ();
            case HashAlg::eSHA1:
                return EVP_sha1 ();
            default:
                RequireNotReached ();
                return nullptr;
        }
    }
}
pair<BLOB, BLOB> OpenSSLCryptoParams::DoDerviveKey (HashAlg hashAlg, CipherAlgorithm alg, pair<const Byte*, const Byte*> passwd, unsigned int keyLen)
{
    int i;
    int nrounds = 5;
//    unsigned char key[32];
//   unsigned char iv[32];
    const unsigned char* salt = nullptr;   // null or 8byte value

    /*
     * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
     * nrounds is the number of times the we hash the material. More rounds are more secure but
     * slower.
     */
    i = ::EVP_BytesToKey (cvt2Cipher_ (alg), cvt2HashAlg_ (hashAlg), salt, nullptr, 0, nrounds, nullptr, nullptr);
    if (i == 0) {
        OpenSSLException::DoThrowLastError ();
    }
    Verify (i >= 1);

    keyLen = i;
    size_t ivLen = i;

    Memory::SmallStackBuffer<Byte> useKey { keyLen };
    Memory::SmallStackBuffer<Byte> useIV { ivLen };

    i = ::EVP_BytesToKey (cvt2Cipher_ (alg), cvt2HashAlg_ (hashAlg), salt, passwd.first, passwd.second - passwd.first, nrounds, useKey.begin (), useIV.begin ());
    if (i == 0) {
        OpenSSLException::DoThrowLastError ();
    }
    return pair<BLOB, BLOB> (BLOB (useKey.begin (), useKey.end ()), BLOB (useIV.begin (), useIV.end ()));
}

pair<BLOB, BLOB> OpenSSLCryptoParams::DoDerviveKey (HashAlg hashAlg, CipherAlgorithm alg, const string& passwd, unsigned int keyLen)
{
    auto pwAsBytes  = pair<const Byte*, const Byte*> (reinterpret_cast<const Byte*> (passwd.c_str ()), reinterpret_cast<const Byte*> (passwd.c_str ()) + passwd.length ());
    return DoDerviveKey (hashAlg, alg, pwAsBytes, keyLen);
}

namespace {
    void    ApplySettings2CTX_ (EVP_CIPHER_CTX* ctx, const EVP_CIPHER* cipher, Direction d, bool nopad, bool useArgumentKeyLength, const Memory::BLOB& key, const Memory::BLOB& initialIV)
    {
        RequireNotNull (ctx);
        RequireNotNull (cipher);
        bool    enc = (d == Direction::eEncrypt);
        if (nopad) {
            Verify (::EVP_CIPHER_CTX_set_padding (ctx, 0) == 1);
        }
        OpenSSLException::DoThrowLastErrorIfFailed (::EVP_CipherInit_ex (ctx, cipher, NULL, nullptr, nullptr, enc));
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

        OpenSSLException::DoThrowLastErrorIfFailed (::EVP_CipherInit_ex (ctx, nullptr, NULL, useKey.begin (), useIV.begin (), enc));
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
                    ApplySettings2CTX_ (ctx, cvt2Cipher_ (alg), d, nopad, false, key, initialIV);
                };
                break;

            }
    }
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
