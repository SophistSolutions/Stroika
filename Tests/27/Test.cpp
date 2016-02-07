/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
//  TEST    Foundation::Cryptography
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>

#if     qPlatform_Windows
#include    <windows.h>
#include    <atlenc.h>
#endif

#include    "Stroika/Foundation/Characters/ToString.h"
#include    "Stroika/Foundation/Containers/Common.h"
#include    "Stroika/Foundation/Configuration/Endian.h"
#include    "Stroika/Foundation/Cryptography/Encoding/Algorithm/AES.h"
#include    "Stroika/Foundation/Cryptography/Encoding/Algorithm/Base64.h"
#include    "Stroika/Foundation/Cryptography/Encoding/Algorithm/RC4.h"
#include    "Stroika/Foundation/Cryptography/Digest/Algorithm/CRC32.h"
#include    "Stroika/Foundation/Cryptography/Digest/Algorithm/Jenkins.h"
#include    "Stroika/Foundation/Cryptography/Digest/Algorithm/MD5.h"
#include    "Stroika/Foundation/Cryptography/Digest/Algorithm/SuperFastHash.h"
#include    "Stroika/Foundation/Cryptography/Digest/DigestDataToString.h"
#include    "Stroika/Foundation/Cryptography/Encoding/OpenSSLCryptoStream.h"
#include    "Stroika/Foundation/Cryptography/Hash.h"
#include    "Stroika/Foundation/Cryptography/Format.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Memory/BLOB.h"
#include    "Stroika/Foundation/Memory/SmallStackBuffer.h"
#include    "Stroika/Foundation/Streams/ExternallyOwnedMemoryInputStream.h"
#include    "Stroika/Foundation/Streams/iostream/SerializeItemToBLOB.h"

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Cryptography;
using   namespace   Stroika::Foundation::Streams;


using   Memory::Byte;



namespace {
    uint32_t    ToLE_  (uint32_t n)
    {
        using Configuration::Endian;
        using Configuration::EndianConverter;
        return EndianConverter<uint32_t> (n, Configuration::GetEndianness (), Endian::eLittle);
    }
}







namespace {
    namespace AESTest_ {
        using   namespace   Cryptography::Encoding;
        using   namespace   Cryptography::Encoding::Algorithm;

        void    DoRegressionTests_ ()
        {
            /// @todo fix this test after I switch to using DerviedKey - this is useless as is - not doig anywhere near the rihgt htin
            /// which is why not working. I HTINK it will work if I use derviedKey!!!
            ///
            {
                // super quick hack - must validate results
                const   char    kKey[] = "Mr Key";
                const   char    kSrc[] = "This is a very good test of a very good test";
                /*
                 *  echo -n "This is a very good test of a very good test" | openssl enc -e -aes-256-cbc -a -nosalt -pass 'pass:Mr Key'
                 */
                const   char    kBase64EncodedResultAESCBC_[] = "MfNuP5LTVHfbeOAT8MAnfltNj05ZcRhEI2ySQoUhMCXUI8pYFKIPJ0PtX6eD0/W80IGy3Wg0U5cY3bXxWBltTQ==";
                const   Memory::BLOB key ((const Byte*)kKey, (const Byte*)kKey + ::strlen(kKey));
                const   Memory::BLOB src ((const Byte*)kSrc, (const Byte*)kSrc + ::strlen(kSrc));
                const   Memory::BLOB encodedVal = Encoding::Algorithm::DecodeBase64 (kBase64EncodedResultAESCBC_);
#if     qHasFeature_OpenSSL
                VerifyTestResult (DecodeAES (key, EncodeAES (key, src, AESOptions::e256_CBC), AESOptions::e256_CBC)  == src);
                if (false) {
                    // @todo Not yet working - fully - gets differnt results than commandline tool
                    VerifyTestResult (EncodeAES (key, src, AESOptions::e256_CBC) == encodedVal);
                    VerifyTestResult (DecodeAES (key, encodedVal, AESOptions::e256_CBC)  == src);
                }
#endif

            }
        }

    }
}



namespace  {
    namespace Base64Test {
        namespace PRIVATE_ {

#if     qPlatform_Windows
            namespace   {
                using   Encoding::Algorithm::LineBreak;
                vector<Byte>    DecodeBase64_ATL_ (const string& s)
                {
                    int                     dataSize1   =   ATL::Base64DecodeGetRequiredLength (static_cast<int> (s.length ()));
                    Memory::SmallStackBuffer<Byte>  buf1 (dataSize1);
                    if (ATL::Base64Decode (s.c_str (), static_cast<int> (s.length ()), buf1, &dataSize1)) {
                        return vector<Byte> (buf1.begin (), buf1.begin () + dataSize1);
                    }
                    return vector<Byte> ();
                }
                string  EncodeBase64_ATL_ (const vector<Byte>& b, LineBreak lb)
                {
                    size_t  totalSize       =   b.size ();
                    if (totalSize != 0) {
                        Memory::SmallStackBuffer<char>  relBuf (0);
                        int                     relEncodedSize  =   ATL::Base64EncodeGetRequiredLength (static_cast<int> (totalSize));
                        relBuf.GrowToSize (relEncodedSize);
                        VerifyTestResult (ATL::Base64Encode (Containers::Start (b), static_cast<int> (totalSize), relBuf, &relEncodedSize));
                        relBuf[relEncodedSize] = '\0';
                        if (lb == LineBreak::eCRLF_LB) {
                            return (static_cast<const char*> (relBuf));
                        }
                        else {
                            VerifyTestResult (lb == LineBreak::eLF_LB);
                            string  result;
                            result.reserve (relEncodedSize);
                            for (int i = 0; i < relEncodedSize; ++i) {
                                if (relBuf[i] == '\r') {
                                    //
                                    result.push_back ('\n');
                                    ++i;    // skip LF
                                }
                                else {
                                    result.push_back (relBuf[i]);
                                }
                            }
                            return result;
                        }
                    }
                    return string ();
                }
            }
#endif

            namespace   {
                inline  void    VERIFY_ATL_ENCODEBASE64_ (const vector<Byte>& bytes)
                {
#if     qPlatform_Windows
                    VerifyTestResult (Encoding::Algorithm::EncodeBase64 (ExternallyOwnedMemoryInputStream<Byte> (begin (bytes), end (bytes)), LineBreak::eCRLF_LB) == EncodeBase64_ATL_ (bytes, LineBreak::eCRLF_LB));
                    VerifyTestResult (Encoding::Algorithm::EncodeBase64 (ExternallyOwnedMemoryInputStream<Byte> (begin (bytes), end (bytes)), LineBreak::eLF_LB) == EncodeBase64_ATL_ (bytes, LineBreak::eLF_LB));
#endif
                }
                inline  void    VERIFY_ATL_DECODE_ ()
                {
#if     qPlatform_Windows
#else
#endif
                }
            }

            namespace   {
                void    VERIFY_ENCODE_DECODE_BASE64_IDEMPOTENT_ (const vector<Byte>& bytes)
                {
                    VerifyTestResult (Encoding::Algorithm::DecodeBase64 (Encoding::Algorithm::EncodeBase64 (ExternallyOwnedMemoryInputStream<Byte> (begin (bytes), end (bytes)))) == bytes);
                }
            }

            namespace   {
                void    DO_ONE_REGTEST_BASE64_ (const string& base64EncodedString, const vector<Byte>& originalUnEncodedBytes)
                {
                    Verify (Encoding::Algorithm::EncodeBase64 (ExternallyOwnedMemoryInputStream<Byte> (begin (originalUnEncodedBytes), end (originalUnEncodedBytes))) == base64EncodedString);
                    Verify (Encoding::Algorithm::DecodeBase64 (base64EncodedString) == originalUnEncodedBytes);
                    VERIFY_ATL_ENCODEBASE64_ (originalUnEncodedBytes);
                    VERIFY_ENCODE_DECODE_BASE64_IDEMPOTENT_ (originalUnEncodedBytes);
                }
            }
        }
        void    DoRegressionTests_ ()
        {

            {
                const   char    kSrc[] =
                    "This is a good test\r\n"
                    "We eat wiggly worms.\r\n"
                    "\r\n"
                    "That is a very good thing.****^^^#$#AS\r\n"
                    ;
                const   char    kEncodedVal[] = "VGhpcyBpcyBhIGdvb2QgdGVzdA0KV2UgZWF0IHdpZ2dseSB3b3Jtcy4NCg0KVGhhdCBpcyBhIHZl\r\ncnkgZ29vZCB0aGluZy4qKioqXl5eIyQjQVMNCg==";
                PRIVATE_::DO_ONE_REGTEST_BASE64_ (kEncodedVal, vector<Byte> ((const Byte*)kSrc, (const Byte*)kSrc + ::strlen(kSrc)));
            }

            {
                const   char    kSrc[]  =
                    "{\\rtf1 \\ansi {\\fonttbl {\\f0 \\fnil \\fcharset163 Times New Roman;}}{\\colortbl \\red0\\green0\\blue0;}\r\n"
                    "{\\*\\listtable{\\list \\listtemplateid12382 {\\listlevel \\levelnfc23 \\leveljc0 \\levelfollow0 \\levelstartat1 \\levelindent0 {\\leveltext \\levelnfc23 \\leveltemplateid17421 \\'01\\u8226  ?;}\\f0 \\fi-360 \\li720 \\jclisttab \\tx720 }\\listid292 }}\r\n"
                    "{\\*\\listoverridetable{\\listoverride \\listid292 \\listoverridecount0 \\ls1 }}\r\n"
                    "{\\*\\generator Sophist Solutions, Inc. Led RTF IO Engine - 3.1b2x;}\\pard \\plain \\f0 \\fs24 \\cf0 Had hay fever today. Not terrible, but several times. And I think a bit yesterda\r\n"
                    "y.}"
                    ;
                const   char    kEncodedVal[] =
                    "e1xydGYxIFxhbnNpIHtcZm9udHRibCB7XGYwIFxmbmlsIFxmY2hhcnNldDE2MyBUaW1lcyBOZXcg\r\n"
                    "Um9tYW47fX17XGNvbG9ydGJsIFxyZWQwXGdyZWVuMFxibHVlMDt9DQp7XCpcbGlzdHRhYmxle1xs\r\n"
                    "aXN0IFxsaXN0dGVtcGxhdGVpZDEyMzgyIHtcbGlzdGxldmVsIFxsZXZlbG5mYzIzIFxsZXZlbGpj\r\n"
                    "MCBcbGV2ZWxmb2xsb3cwIFxsZXZlbHN0YXJ0YXQxIFxsZXZlbGluZGVudDAge1xsZXZlbHRleHQg\r\n"
                    "XGxldmVsbmZjMjMgXGxldmVsdGVtcGxhdGVpZDE3NDIxIFwnMDFcdTgyMjYgID87fVxmMCBcZmkt\r\n"
                    "MzYwIFxsaTcyMCBcamNsaXN0dGFiIFx0eDcyMCB9XGxpc3RpZDI5MiB9fQ0Ke1wqXGxpc3RvdmVy\r\n"
                    "cmlkZXRhYmxle1xsaXN0b3ZlcnJpZGUgXGxpc3RpZDI5MiBcbGlzdG92ZXJyaWRlY291bnQwIFxs\r\n"
                    "czEgfX0NCntcKlxnZW5lcmF0b3IgU29waGlzdCBTb2x1dGlvbnMsIEluYy4gTGVkIFJURiBJTyBF\r\n"
                    "bmdpbmUgLSAzLjFiMng7fVxwYXJkIFxwbGFpbiBcZjAgXGZzMjQgXGNmMCBIYWQgaGF5IGZldmVy\r\n"
                    "IHRvZGF5LiBOb3QgdGVycmlibGUsIGJ1dCBzZXZlcmFsIHRpbWVzLiBBbmQgSSB0aGluayBhIGJp\r\n"
                    "dCB5ZXN0ZXJkYQ0KeS59"
                    ;
                PRIVATE_::DO_ONE_REGTEST_BASE64_ (kEncodedVal, vector<Byte> ((const Byte*)kSrc, (const Byte*)kSrc + ::strlen(kSrc)));
            }

            {
                const   char    kSrc[]          =   "()'asdf***Adasdf a";
                const   char    kEncodedVal[]   =   "KCknYXNkZioqKkFkYXNkZiBh";
                PRIVATE_::DO_ONE_REGTEST_BASE64_ (kEncodedVal, vector<Byte> ((const Byte*)kSrc, (const Byte*)kSrc + ::strlen(kSrc)));
            }
        }
    }
}



namespace  {
    namespace MD5Test {
        void    DoRegressionTests_ ()
        {
            // really this is a test of high level tools used in orig Cryptography::MD5 module, but these are really
            // generic utilities...
            using   DIGESTER_ = Digest::Digester<Digest::Algorithm::MD5>;
            {
                const   char    kSrc[] = "This is a very good test of a very good test";
                const   char    kEncodedVal[] = "08c8888b86d6300ade93a10095a9083a";
                VerifyTestResult (Format (DIGESTER_::ComputeDigest ((const Byte*)kSrc, (const Byte*)kSrc + ::strlen(kSrc))) == kEncodedVal);
            }
            {
                int tmp = 3;
                string  digestStr = Format (DIGESTER_::ComputeDigest (Streams::iostream::SerializeItemToBLOB (tmp)));
                VerifyTestResult (digestStr ==  "eccbc87e4b5ce2fe28308fd9f2a7baf3");
            }
            {
                int tmp = 3;
                string  digestStr = Digest::DigestDataToString<DIGESTER_> (tmp);
                VerifyTestResult (digestStr ==  "eccbc87e4b5ce2fe28308fd9f2a7baf3");
            }
        }
    }
}




namespace {
    template    <typename HASHER>
    void    DoCommonHasherTest_ (const Byte* dataStart, const Byte* dataEnd, uint32_t answer)
    {
        VerifyTestResult (HASHER::ComputeDigest (dataStart, dataEnd) == answer);
        VerifyTestResult (HASHER::ComputeDigest (Memory::BLOB (dataStart, dataEnd).As<Streams::InputStream<Byte>> ()) == answer);
    }

}




namespace  {
    namespace Hash_CRC32 {

        using   namespace   Cryptography::Digest;

        void    DoRegressionTests_ ()
        {
            // @todo -- RETHINK IF RESULTS SB SAME REGARDLESS OF ENDIAN - NOT CONSISTENT!!!! --LGP 2015-08-26 -- AIX
            {
                // This result identical to that computed by http://www.zorc.breitbandkatze.de/crc.html -- LGP 2013-10-31
                const   char    kSrc[] = "This is a very good test of a very good test";
                DoCommonHasherTest_<Digester<Algorithm::CRC32>> ((const Byte*)kSrc, (const Byte*)kSrc + ::strlen(kSrc), 3692548919);
                DoCommonHasherTest_<Digester<Algorithm::CRC32, uint32_t>> ((const Byte*)kSrc, (const Byte*)kSrc + ::strlen(kSrc), 3692548919);
            }
        }

    }
}


namespace  {
    namespace Hash_Jenkins {

        using   namespace   Cryptography::Digest;

        void    DoRegressionTests_ ()
        {
            // @todo -- RETHINK IF RESULTS SB SAME REGARDLESS OF ENDIAN - NOT CONSISTENT!!!! --LGP 2015-08-26 -- AIX
            using   Configuration::Endian;
            using   Configuration::EndianConverter;
            using   USE_DIGESTER_     =   Digester<Algorithm::Jenkins>;
            {
                VerifyTestResult (Hash<USE_DIGESTER_> (ToLE_ (1)) == 10338022);
                VerifyTestResult (Hash<USE_DIGESTER_> ("1") == 2154528969);
                VerifyTestResult (Hash<USE_DIGESTER_> (Characters::String (L"1")) == 2154528969);
                VerifyTestResult (Hash<USE_DIGESTER_> ("1", "mysalt") == 2164173146);
                VerifyTestResult (Hash<USE_DIGESTER_> (ToLE_ (93993)) == 1748544338);
            }
            {
                const   char    kSrc[] = "This is a very good test of a very good test";
                DoCommonHasherTest_<USE_DIGESTER_> ((const Byte*)kSrc, (const Byte*)kSrc + ::strlen(kSrc), 2786528596);
            }
        }

    }
}



namespace  {
    namespace Hash_MD5 {

        using   namespace   Cryptography::Digest;

        void    DoRegressionTests_ ()
        {
            using   USE_DIGESTER_     =   Digester<Algorithm::MD5>;
            {
                const   char    kSrc[] = "This is a very good test of a very good test";
                const   char    kEncodedVal[] = "08c8888b86d6300ade93a10095a9083a";
                VerifyTestResult ((Hash<USE_DIGESTER_, string, string> (kSrc)) == kEncodedVal);
            }
        }

    }
}






namespace  {
    namespace Hash_SuperFastHash {

        using   namespace   Cryptography::Digest;

        void    DoRegressionTests_ ()
        {
            // @todo -- RETHINK IF RESULTS SB SAME REGARDLESS OF ENDIAN - NOT CONSISTENT!!!! --LGP 2015-08-26 -- AIX
            using   USE_DIGESTER_     =   Digester<Algorithm::SuperFastHash>;
            {
                VerifyTestResult (Hash<USE_DIGESTER_> (ToLE_ (1)) == 422304363);
                VerifyTestResult (Hash<USE_DIGESTER_> (ToLE_ (93993)) == 2489559407);
            }
            {
                // special case where these collide
                const   char    kSrc1[] = "        90010";
                DoCommonHasherTest_<USE_DIGESTER_> ((const Byte*)kSrc1, (const Byte*)kSrc1 + ::strlen(kSrc1), 375771507);
                const   char    kSrc2[] = "        10028";
                DoCommonHasherTest_<USE_DIGESTER_> ((const Byte*)kSrc2, (const Byte*)kSrc2 + ::strlen(kSrc2), 375771507);
            }
            {
                const   char    kSrc[] = "This is a very good test of a very good test";
                DoCommonHasherTest_<USE_DIGESTER_> ((const Byte*)kSrc, (const Byte*)kSrc + ::strlen(kSrc), 1181771593);
            }
        }

    }
}









namespace {
    namespace AllSSLEncrytionRoundtrip {
        using   namespace   Cryptography::Encoding;
        using   namespace   Cryptography::Encoding::Algorithm;

        void    DoRegressionTests_ ()
        {
#if     qHasFeature_OpenSSL
            using   Memory::BLOB;
            using   namespace   Stroika::Foundation::Cryptography::Encoding;

            auto roundTripTester_ = [] (const OpenSSLCryptoParams & cryptoParams, BLOB src) -> void {
                BLOB    encodedData = OpenSSLInputStream (cryptoParams, Direction::eEncrypt, src.As<Streams::InputStream<Byte>> ()).ReadAll ();
                BLOB    decodedData = OpenSSLInputStream (cryptoParams, Direction::eDecrypt, encodedData.As<Streams::InputStream<Byte>> ()).ReadAll ();
                VerifyTestResult (src == decodedData);
            };

            const   char    kKey1_[] = "Mr Key";
            const   char    kKey2_[] = "One Very Very Very Long key 123";
            static  const BLOB  kPassphrases_ [] = {
                BLOB ((const Byte*)kKey1_, (const Byte*)kKey1_ + ::strlen(kKey1_)),
                BLOB ((const Byte*)kKey2_, (const Byte*)kKey2_ + ::strlen(kKey2_)),
            };

            const   char    kSrc1_[] = "This is a very good test of a very good test";
            const   char    kSrc2_[] = "";
            const   char    kSrc3_[] = "We eat wiggly worms. That was a very good time to eat the worms. They are awesome!";
            const   char    kSrc4_[] = "0123456789";
            static  const BLOB  kTestMessages_ [] = {
                BLOB ((const Byte*)kSrc1_, (const Byte*)kSrc1_ + ::strlen(kSrc1_)),
                BLOB ((const Byte*)kSrc2_, (const Byte*)kSrc2_ + ::strlen(kSrc2_)),
                BLOB ((const Byte*)kSrc3_, (const Byte*)kSrc3_ + ::strlen(kSrc3_)),
#if 0
                // DEBUG WHY THIS FAILS - I THINK WE NEED TO ENABLE PADDING FOR SOME CYPHERS!
                BLOB ((const Byte*)kSrc4_, (const Byte*)kSrc4_ + ::strlen(kSrc4_)),
#endif
            };


            for (BLOB passphrase : kPassphrases_) {
                for (BLOB inputMessage : kTestMessages_) {
                    for (CipherAlgorithm ci = CipherAlgorithm::eSTART; ci != CipherAlgorithm::eEND; ci = Configuration::Inc (ci)) {
                        for (DigestAlgorithm di = DigestAlgorithm::eSTART; di != DigestAlgorithm::eEND; di = Configuration::Inc (di)) {
                            OpenSSLCryptoParams cryptoParams { ci, OpenSSL::EVP_BytesToKey  { ci, di, passphrase } };
                            roundTripTester_ (cryptoParams, inputMessage);
                        }
                    }
                }
            }
#endif
        }

    }
}







namespace {
    namespace OpenSSLDeriveKeyTests_ {
        using   namespace   Cryptography::Encoding;
        using   namespace   Cryptography::Encoding::Algorithm;

        void    DoRegressionTests_ ()
        {
#if     qHasFeature_OpenSSL
            using   Characters::String;
            using   Memory::BLOB;
            using   namespace   Stroika::Foundation::Cryptography::Encoding;

            auto check = [] (CipherAlgorithm cipherAlgorithm, DigestAlgorithm digestAlgorithm, const String & password, const DerivedKey & expected) {
                unsigned int    nRounds = 1;    // command-line tool uses this
                DerivedKey  dk = OpenSSL::EVP_BytesToKey { cipherAlgorithm, digestAlgorithm, password, nRounds };
                DbgTrace (L"dk=%s; expected=%s", Characters::ToString (dk).c_str (), Characters::ToString (expected).c_str ());
                VerifyTestResult (dk == expected);
            };

            // openssl rc4 -P -k mypass -nosalt  -md md5
            //      key=A029D0DF84EB5549C641E04A9EF389E5
            check (CipherAlgorithm::eRC4, DigestAlgorithm::eMD5, L"mypass", DerivedKey { BLOB { 0xA0, 0x29, 0xD0, 0xDF, 0x84, 0xEB, 0x55, 0x49, 0xC6, 0x41, 0xE0, 0x4A, 0x9E, 0xF3, 0x89, 0xE5 }, BLOB {} });
#endif
        }

    }
}








namespace   {
    void    DoRegressionTests_ ()
    {
        AESTest_::DoRegressionTests_ ();
        Base64Test::DoRegressionTests_ ();
        MD5Test::DoRegressionTests_ ();
        Hash_CRC32::DoRegressionTests_ ();
        Hash_Jenkins::DoRegressionTests_ ();
        Hash_MD5::DoRegressionTests_ ();
        Hash_SuperFastHash::DoRegressionTests_ ();
        AllSSLEncrytionRoundtrip::DoRegressionTests_ ();
        OpenSSLDeriveKeyTests_::DoRegressionTests_ ();
    }
}





int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
