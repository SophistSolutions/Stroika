/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/StringUtils.h"
#include    "../Containers/Common.h"
#include    "../Debug/Assertions.h"
#include    "../Memory/SmallStackBuffer.h"

#include    "OpenSSLCryptoStream.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Cryptography;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Streams;


//// VERY ROUGH DRAFT - NOT VERY CLOSE TO CORRECT FOR ALL ALGORITHSM
//// SEE http://www.openssl.org/docs/crypto/EVP_EncryptInit.html
//// for details on what todo


#if     qHas_OpenSSL
namespace {
    struct InOutStrmCommon_ {
        /* Allow enough space in output buffer for additional block */
        char inbuf[1024];
        char outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
        int inlen, outlen;

        InOutStrmCommon_ (const OpenSSLCryptoParams& cryptoParams, bool encrypt)
            : fCTX_ () {
            ::EVP_CIPHER_CTX_init (&fCTX_);
            cryptoParams.fInitializer (&fCTX_, encrypt);
        }

        ~InOutStrmCommon_ () {
            EVP_CIPHER_CTX_cleanup (&fCTX_);
        }

        int _runOnce () {
            inlen = fread(inbuf, 1, 1024, in);
            if(inlen <= 0) break;
            if(!EVP_CipherUpdate(&ctx, outbuf, &outlen, inbuf, inlen)) {
                /* Error */
                return 0;
            }
            fwrite(outbuf, 1, outlen, out);
	        return 1;
        }
		int _cipherFinal ()
		{
			if(!EVP_CipherFinal_ex(&ctx, outbuf, &outlen)) {
				/* Error */
				return 0;
			}
			fwrite(outbuf, 1, outlen, out);
			return 1;
		}
    };
}
#endif



#if     qHas_OpenSSL
class   OpenSSLInputStream::IRep_ : public BinaryInputStream::_IRep, public InOutStrmCommon_ {
public:
    IRep_ (const OpenSSLCryptoParams& cryptoParams, const BinaryInputStream& realIn)
        : BinaryInputStream::_IRep ()
        , InOutStrmCommon_ (cryptoParams, false)
        , fCriticalSection_ ()
        , fRealIn_ (realIn) {
    }

    virtual size_t  Read (Byte* intoStart, Byte* intoEnd) override {
        lock_guard<mutex>  critSec (fCriticalSection_);
        return fRealIn_.Read (intoStart, intoEnd);
    }

private:
    mutable mutex           fCriticalSection_;
    EVP_CIPHER_CTX          fCTX_;
    BinaryInputStream       fRealIn_;
};
#endif






#if     qHas_OpenSSL
class   OpenSSLOutputStream::IRep_ : public BinaryOutputStream::_IRep, public InOutStrmCommon_ {
public:
    IRep_ (const OpenSSLCryptoParams& cryptoParams, const BinaryOutputStream& realOut)
        : BinaryOutputStream::_IRep ()
        , InOutStrmCommon_ (cryptoParams, true)
        , fCriticalSection_ ()
        , fRealOut_ (realOut) {
    }

    // pointer must refer to valid memory at least bufSize long, and cannot be nullptr. BufSize must always be >= 1.
    // Writes always succeed fully or throw.
    virtual void            Write (const Byte* start, const Byte* end) override {
        Require (start < end);  // for BinaryOutputStream - this funciton requires non-empty write
        Require (not fAborted_);
        lock_guard<recursive_mutex>  critSec (fCriticalSection_);
        fRealOut_.Write (start, end);
    }

private:
    mutable recursive_mutex             fCriticalSection_;
    BinaryOutputStream                  fRealOut_;
};
#endif







#if     qHas_OpenSSL
/*
 ********************************************************************************
 ******************** Cryptography::OpenSSLInputStream **************************
 ********************************************************************************
 */
OpenSSLCryptoParams::OpenSSLCryptoParams (Algorithm alg, Memory::BLOB key, Memory::BLOB initialIV)
    : fInitializer ()
{
    switch (alg) {
        case Algorithm::eBlowfish_CBC: {
                fInitializer = [&key, &initialIV] (EVP_CIPHER_CTX * ctx, bool enc) {
                    EVP_CipherInit_ex (ctx, EVP_bf_cbc (), NULL, key, iv);
                };
            }
            break;
        case Algorithm::eRC2: {
                fInitializer = [&key, &initialIV] (EVP_CIPHER_CTX * ctx, bool enc) {
                    EVP_CipherInit_ex (ctx, EVP_rc2 (), NULL, NULL, NULL);
                    EVP_CIPHER_CTX_set_key_length (ctx, key.length ());
                    EVP_CipherInit_ex (ctx, NULL, NULL, key, iv);
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
