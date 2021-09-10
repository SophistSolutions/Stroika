/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
//  TEST    Foundation::Cryptography
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#if qPlatform_Windows
#include <windows.h>
#if qHasFeature_ATLMFC
#include <atlenc.h>
#endif
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/GUID.h"
#include "Stroika/Foundation/Configuration/Endian.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Cryptography/Digest/Algorithm/CRC32.h"
#include "Stroika/Foundation/Cryptography/Digest/Algorithm/Jenkins.h"
#include "Stroika/Foundation/Cryptography/Digest/Algorithm/MD5.h"
#include "Stroika/Foundation/Cryptography/Digest/Algorithm/SuperFastHash.h"
#include "Stroika/Foundation/Cryptography/Digest/Hash.h"
#include "Stroika/Foundation/Cryptography/Encoding/Algorithm/AES.h"
#include "Stroika/Foundation/Cryptography/Encoding/Algorithm/Base64.h"
#include "Stroika/Foundation/Cryptography/Encoding/Algorithm/RC4.h"
#include "Stroika/Foundation/Cryptography/Encoding/OpenSSLCryptoStream.h"
#include "Stroika/Foundation/Cryptography/Format.h"
#include "Stroika/Foundation/Cryptography/OpenSSL/LibraryContext.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/IO/Network/InternetAddress.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/SmallStackBuffer.h"
#include "Stroika/Foundation/Streams/ExternallyOwnedMemoryInputStream.h"
#include "Stroika/Foundation/Streams/iostream/SerializeItemToBLOB.h"

#include "../TestHarness/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Streams;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    uint32_t ToLE_ (uint32_t n)
    {
        using Configuration::Endian;
        using Configuration::EndianConverter;
        return EndianConverter<uint32_t> (n, Configuration::GetEndianness (), Endian::eLittle);
    }
}

namespace {
    namespace Base64Test {
        namespace PRIVATE_ {

#if qPlatform_Windows && qHasFeature_ATLMFC
            namespace {
                using Encoding::Algorithm::LineBreak;
                vector<byte> DecodeBase64_ATL_ (const string& s)
                {
                    int                            dataSize1 = ATL::Base64DecodeGetRequiredLength (static_cast<int> (s.length ()));
                    Memory::SmallStackBuffer<byte> buf1 (dataSize1);
                    if (ATL::Base64Decode (s.c_str (), static_cast<int> (s.length ()), reinterpret_cast<BYTE*> (buf1.begin ()), &dataSize1)) {
                        return vector<byte> (buf1.begin (), buf1.begin () + dataSize1);
                    }
                    return vector<byte> ();
                }
                string EncodeBase64_ATL_ (const vector<byte>& b, LineBreak lb)
                {
                    size_t totalSize = b.size ();
                    if (totalSize != 0) {
                        Memory::SmallStackBuffer<char> relBuf (0);
                        int                            relEncodedSize = ATL::Base64EncodeGetRequiredLength (static_cast<int> (totalSize));
                        relBuf.GrowToSize (relEncodedSize);
                        VerifyTestResult (ATL::Base64Encode (reinterpret_cast<const BYTE*> (Containers::Start (b)), static_cast<int> (totalSize), relBuf, &relEncodedSize));
                        relBuf[relEncodedSize] = '\0';
                        if (lb == LineBreak::eCRLF_LB) {
                            return (static_cast<const char*> (relBuf));
                        }
                        else {
                            VerifyTestResult (lb == LineBreak::eLF_LB);
                            string result;
                            result.reserve (relEncodedSize);
                            for (int i = 0; i < relEncodedSize; ++i) {
                                if (relBuf[i] == '\r') {
                                    //
                                    result.push_back ('\n');
                                    ++i; // skip LF
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

            namespace {
                inline void VERIFY_ATL_ENCODEBASE64_ ([[maybe_unused]] const vector<byte>& bytes)
                {
#if qPlatform_Windows && qHasFeature_ATLMFC
                    VerifyTestResult (Encoding::Algorithm::EncodeBase64 (ExternallyOwnedMemoryInputStream<byte>::New (begin (bytes), end (bytes)), LineBreak::eCRLF_LB) == EncodeBase64_ATL_ (bytes, LineBreak::eCRLF_LB));
                    VerifyTestResult (Encoding::Algorithm::EncodeBase64 (ExternallyOwnedMemoryInputStream<byte>::New (begin (bytes), end (bytes)), LineBreak::eLF_LB) == EncodeBase64_ATL_ (bytes, LineBreak::eLF_LB));
#endif
                }
                inline void VERIFY_ATL_DECODE_ ()
                {
#if qPlatform_Windows
#else
#endif
                }
            }

            namespace {
                void VERIFY_ENCODE_DECODE_BASE64_IDEMPOTENT_ (const vector<byte>& bytes)
                {
                    VerifyTestResult (Encoding::Algorithm::DecodeBase64 (Encoding::Algorithm::EncodeBase64 (ExternallyOwnedMemoryInputStream<byte>::New (begin (bytes), end (bytes)))) == bytes);
                }
            }

            namespace {
                void DO_ONE_REGTEST_BASE64_ (const string& base64EncodedString, const vector<byte>& originalUnEncodedBytes)
                {
                    VerifyTestResult (Encoding::Algorithm::EncodeBase64 (ExternallyOwnedMemoryInputStream<byte>::New (begin (originalUnEncodedBytes), end (originalUnEncodedBytes))) == base64EncodedString);
                    VerifyTestResult (Encoding::Algorithm::DecodeBase64 (base64EncodedString) == originalUnEncodedBytes);
                    VERIFY_ATL_ENCODEBASE64_ (originalUnEncodedBytes);
                    VERIFY_ENCODE_DECODE_BASE64_IDEMPOTENT_ (originalUnEncodedBytes);
                }
            }
        }
        void DoRegressionTests_ ()
        {
            Debug::TraceContextBumper ctx{"Base64Test::DoRegressionTests_"};

            {
                const char kSrc[] =
                    "This is a good test\r\n"
                    "We eat wiggly worms.\r\n"
                    "\r\n"
                    "That is a very good thing.****^^^#$#AS\r\n";
                const char kEncodedVal[] = "VGhpcyBpcyBhIGdvb2QgdGVzdA0KV2UgZWF0IHdpZ2dseSB3b3Jtcy4NCg0KVGhhdCBpcyBhIHZl\r\ncnkgZ29vZCB0aGluZy4qKioqXl5eIyQjQVMNCg==";
                PRIVATE_::DO_ONE_REGTEST_BASE64_ (kEncodedVal, vector<byte> ((const byte*)kSrc, (const byte*)kSrc + ::strlen (kSrc)));
            }

            {
                const char kSrc[] =
                    "{\\rtf1 \\ansi {\\fonttbl {\\f0 \\fnil \\fcharset163 Times New Roman;}}{\\colortbl \\red0\\green0\\blue0;}\r\n"
                    "{\\*\\listtable{\\list \\listtemplateid12382 {\\listlevel \\levelnfc23 \\leveljc0 \\levelfollow0 \\levelstartat1 \\levelindent0 {\\leveltext \\levelnfc23 \\leveltemplateid17421 \\'01\\u8226  ?;}\\f0 \\fi-360 \\li720 \\jclisttab \\tx720 }\\listid292 }}\r\n"
                    "{\\*\\listoverridetable{\\listoverride \\listid292 \\listoverridecount0 \\ls1 }}\r\n"
                    "{\\*\\generator Sophist Solutions, Inc. Led RTF IO Engine - 3.1b2x;}\\pard \\plain \\f0 \\fs24 \\cf0 Had hay fever today. Not terrible, but several times. And I think a bit yesterda\r\n"
                    "y.}";
                const char kEncodedVal[] =
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
                    "dCB5ZXN0ZXJkYQ0KeS59";
                PRIVATE_::DO_ONE_REGTEST_BASE64_ (kEncodedVal, vector<byte> ((const byte*)kSrc, (const byte*)kSrc + ::strlen (kSrc)));
            }

            {
                const char kSrc[]        = "()'asdf***Adasdf a";
                const char kEncodedVal[] = "KCknYXNkZioqKkFkYXNkZiBh";
                PRIVATE_::DO_ONE_REGTEST_BASE64_ (kEncodedVal, vector<byte> ((const byte*)kSrc, (const byte*)kSrc + ::strlen (kSrc)));
            }
        }
    }
}

namespace {
    namespace MD5Test {
        void DoRegressionTests_ ()
        {
            Debug::TraceContextBumper ctx{"MD5Test::DoRegressionTests_"};

            // really this is a test of high level tools used in orig Cryptography::MD5 module, but these are really
            // generic utilities...
            using DIGESTER_ = Digest::Digester<Digest::Algorithm::MD5>;
            {
                const char kSrc[]        = "This is a very good test of a very good test";
                const char kEncodedVal[] = "08c8888b86d6300ade93a10095a9083a";
                VerifyTestResult (Format<string> (Digest::ComputeDigest<Digest::Algorithm::MD5> ((const byte*)kSrc, (const byte*)kSrc + ::strlen (kSrc))) == kEncodedVal);
            }
            {
                int    tmp       = 3;
                string digestStr = Format<string> (Digest::ComputeDigest<Digest::Algorithm::MD5> (Streams::iostream::SerializeItemToBLOB (tmp)));
                VerifyTestResult (digestStr == "eccbc87e4b5ce2fe28308fd9f2a7baf3");
            }
            {
                int tmp = 3;
                VerifyTestResult ((Digest::Hash<int, DIGESTER_, string>{}(tmp) == "edcfae989540fd42e4b8556d5b723bb6"));
            }
        }
    }
}

namespace {
    namespace HashMisc {
        using namespace Cryptography::Digest;

        void DoRegressionTests_ ()
        {
            Debug::TraceContextBumper ctx{"HashMisc::DoRegressionTests_"};

            using namespace Characters;
            {
                VerifyTestResult ((Hash<String>{}(L"x") != Hash<String>{}(L"y"))); // practically this should never fail if not absolutely required
                VerifyTestResult ((Hash<String>{L"somesalt"}(L"x") != Hash<String>{}(L"x")));
                VerifyTestResult ((Hash<String>{L"somesalt"}(L"x") == Hash<String>{L"somesalt"}(L"x")));

                VerifyTestResult ((Hash<String, DefaultHashDigester, DefaultHashDigester::ReturnType>{}(L"x") == Hash<String>{}(L"x")));
                struct altStringSerializer {
                    auto operator() (const String& s) { return s.empty () ? Memory::BLOB{} : Memory::BLOB ((const byte*)s.c_str (), (const byte*)s.c_str () + 1); };
                };
                //constexpr auto altStringSerializer = [] (const String& s) { return s.empty () ? Memory::BLOB{} : Memory::BLOB ((const byte*)s.c_str (), (const byte*)s.c_str () + 1); };
                VerifyTestResult ((Hash<String, DefaultHashDigester, DefaultHashDigester::ReturnType, altStringSerializer>{}(L"xxx") != Hash<String>{}(L"xxx")));
                VerifyTestResult ((Hash<String, DefaultHashDigester, DefaultHashDigester::ReturnType, altStringSerializer>{}(L"x1") == Hash<String, DefaultHashDigester, DefaultHashDigester::ReturnType, altStringSerializer>{}(L"x2")));
            }
            {
                auto ec1{make_error_code (std::errc::already_connected)};
                auto ec2{make_error_code (std::errc::directory_not_empty)};
                auto hasher = Hash<error_code, SystemHashDigester<error_code>>{};
                VerifyTestResultWarning (hasher (ec1) != hasher (ec2));
                VerifyTestResult ((hasher (ec1) == hash<error_code>{}(ec1)));
                VerifyTestResult ((hasher (ec2) == hash<error_code>{}(ec2)));
            }
            {
                using namespace IO::Network;
                auto hasher = Hash<InternetAddress>{};
                VerifyTestResultWarning ((hasher (InternetAddress{L"192.168.243.3"}) != hasher (InternetAddress{L"192.168.243.4"})));
            }
        }
    }
}

namespace {
    namespace DigestAltResults {
        using namespace Cryptography::Digest;

        void DoRegressionTests_ ()
        {
            Debug::TraceContextBumper ctx{"DigestAltResults::DoRegressionTests_"};
            // Excercise uses of ConvertResult()
            {
                using namespace DataExchange;
                using namespace IO::Network;
                auto                 digesterWithDefaultResult  = Digester<Digest::Algorithm::SuperFastHash>{};
                auto                 digesterWithResult_uint8_t = Digester<Digest::Algorithm::SuperFastHash, uint8_t>{};
                Memory::BLOB         value2Hash                 = DefaultSerializer<InternetAddress>{}(InternetAddress{L"192.168.244.33"});
                auto                 h1                         = digesterWithDefaultResult (value2Hash);
                uint8_t              h2                         = digesterWithResult_uint8_t (value2Hash);
                std::array<byte, 40> h3                         = ComputeDigest<Digest::Algorithm::SuperFastHash, std::array<byte, 40>> (value2Hash);

                /*
                 *  NOTE - basically ALL these tests vary on a number of parameters.
                 *      o   default serializer for string depends on sizeof wchar_t
                 *      o   some values involve casts of integers of byte array data which depends on endianness
                 *      but otherwise I dont think these values should float/vary (thus the VerifyTestResult tests).
                 */
                if (sizeof (wchar_t) == 2) {
                    VerifyTestResult (h1 == 1969025828);                                                                 // before Stroika 2.1b10 (2512011991)
                    VerifyTestResult (h2 == 36);                                                                         // before Stroika 2.1b10 (215)
                    VerifyTestResult (h3[0] == std::byte{0x24} and h3[1] == std::byte{0xf3} and h3[39] == std::byte{0}); // value before Stroika 2.1b10 215, 66,... 0
                    if (Configuration ::GetEndianness () == Configuration::Endian::eX86) {
                        VerifyTestResult ((Digester<Digest::Algorithm::SuperFastHash, string>{}(value2Hash) == "0x1969025828")); // value before Stroika 2.1b10 ("0x2512011991")
                    }
                }
                else if (sizeof (wchar_t) == 4) {
                    VerifyTestResult (h1 == 4259971568);                                                              // before Stroika 2.1b10 3490201358
                    VerifyTestResult (h2 == 240);                                                                     // before Stroika 2.1b10 (14)
                    VerifyTestResult (h3[0] == std::byte{0xf0} and h3[1] == std::byte{1} and h3[39] == std::byte{0}); // value before Stroika 2.1b10 14, 63,... 0
                    if (Configuration ::GetEndianness () == Configuration::Endian::eX86) {
                        VerifyTestResult ((Digester<Digest::Algorithm::SuperFastHash, string>{}(value2Hash) == "0x4259971568")); // value before Stroika 2.1b10 ("0x3490201358")
                    }
                }
            }

            {
                // copy array to smaller type
                using namespace DataExchange;
                using namespace IO::Network;
                auto         digesterWithDefaultResult  = Digester<Digest::Algorithm::MD5>{};
                auto         digesterWithResult_uint8_t = Digester<Digest::Algorithm::MD5, uint8_t>{};
                Memory::BLOB value2Hash                 = DefaultSerializer<InternetAddress>{}(InternetAddress{L"192.168.244.33"});
                auto         h1                         = digesterWithDefaultResult (value2Hash);
                uint8_t      h2                         = digesterWithResult_uint8_t (value2Hash);
                VerifyTestResult (h2 == h1[0]);
                auto digesterWithResult_string = Digester<Digest::Algorithm::MD5, string>{};

                /*
                 *  NOTE - basically ALL these tests vary on a number of parameters.
                 *      o   default serializer for string depends on sizeof wchar_t
                 */
                if (sizeof (wchar_t) == 2) {
                    VerifyTestResult (digesterWithResult_string (value2Hash) == "06d0b6f01666443614f2502b44386721");
                    VerifyTestResult ((Digester<Digest::Algorithm::MD5, String>{}(value2Hash) == L"06d0b6f01666443614f2502b44386721"));
                }
                if (sizeof (wchar_t) == 4) {
                    VerifyTestResult (digesterWithResult_string (value2Hash) == "32e6a57de2f9324edfc93863838d9015");
                    VerifyTestResult ((Digester<Digest::Algorithm::MD5, String>{}(value2Hash) == L"32e6a57de2f9324edfc93863838d9015"));
                }
            }
            {
                using namespace IO::Network;
                auto                 hasherWithDefaultResult  = Hash<InternetAddress, Digester<Digest::Algorithm::SuperFastHash>>{};
                auto                 hasherWithResult_uint8_t = Hash<InternetAddress, Digester<Digest::Algorithm::SuperFastHash>, uint8_t>{};
                auto                 value2Hash               = InternetAddress{L"192.168.244.33"};
                auto                 h1                       = hasherWithDefaultResult (value2Hash);
                uint8_t              h2                       = hasherWithResult_uint8_t (value2Hash);
                auto                 hasherWithResult_array40 = Hash<InternetAddress, Digester<Digest::Algorithm::SuperFastHash>, std::array<byte, 40>>{};
                std::array<byte, 40> h3                       = hasherWithResult_array40 (value2Hash);

                /*
                 *  NOTE - basically ALL these tests vary on a number of parameters.
                 *      o   default serializer for string depends on sizeof wchar_t
                 */
                if (sizeof (wchar_t) == 2) {
                    VerifyTestResult (h1 == 1969025828);                                                                 // experimentally derived values but they shouldn't float (before Stroika 2.1b10 was 2512011991)
                    VerifyTestResult (h2 == 36);                                                                         // before Stroika 2.1b10, value was 215
                    VerifyTestResult (h3[0] == std::byte{0x24} and h3[1] == std::byte{0xf3} and h3[39] == std::byte{0}); // before Stroika 2.1b10, value was 215, 66, ...0
                }
                if (sizeof (wchar_t) == 4) {
                    VerifyTestResult (h1 == 4259971568);                                                              // experimentally derived values but they shouldn't float (before Stroika 2.1b10 was 3490201358)
                    VerifyTestResult (h2 == 240);                                                                     // before Stroika 2.1b10, value was 14
                    VerifyTestResult (h3[0] == std::byte{0xf0} and h3[1] == std::byte{1} and h3[39] == std::byte{0}); // before Stroika 2.1b10, value was 14, 63, ...0
                }
            }
        }
    }
}

namespace {
    template <typename DIGESTER>
    void DoCommonDigesterTest_ (const byte* dataStart, const byte* dataEnd, uint32_t answer)
    {
        VerifyTestResult (DIGESTER{}(dataStart, dataEnd) == answer);
        VerifyTestResult (DIGESTER{}(Memory::BLOB (dataStart, dataEnd).As<Streams::InputStream<byte>::Ptr> ()) == answer);
    }
}

namespace {
    namespace Digest_CRC32_ {

        using namespace Cryptography::Digest;

        void DoRegressionTests_ ()
        {
            Debug::TraceContextBumper ctx{"Digest_CRC32_::DoRegressionTests_"};

            // @todo -- RETHINK IF RESULTS SB SAME REGARDLESS OF ENDIAN - NOT CONSISTENT!!!! --LGP 2015-08-26 -- AIX
            {
                // This result identical to that computed by http://www.zorc.breitbandkatze.de/crc.html -- LGP 2013-10-31
                const char kSrc[] = "This is a very good test of a very good test";
                DoCommonDigesterTest_<Digester<Algorithm::CRC32>> ((const byte*)kSrc, (const byte*)kSrc + ::strlen (kSrc), 3692548919);
                DoCommonDigesterTest_<Digester<Algorithm::CRC32, uint32_t>> ((const byte*)kSrc, (const byte*)kSrc + ::strlen (kSrc), 3692548919);
            }
        }
    }
}

namespace {
    namespace Digest_Jenkins_ {

        using namespace Cryptography::Digest;

        void DoRegressionTests_ ()
        {
            Debug::TraceContextBumper ctx{"Digest_Jenkins_::DoRegressionTests_"};

            using Configuration::Endian;
            using Configuration::EndianConverter;
            using USE_DIGESTER_ = Digester<Algorithm::Jenkins>;
            {
                VerifyTestResult ((Cryptography::Digest::Hash<int, USE_DIGESTER_>{}(ToLE_ (1)) == 10338022));
                VerifyTestResult ((Cryptography::Digest::Hash<string, USE_DIGESTER_>{}("1") == 2154528969));
                // DefaultSerializer<String> MAY produce different byte patterns depending on sizeof (wchar_t)
                if (sizeof (wchar_t) == 2) {
                    VerifyTestResult ((Cryptography::Digest::Hash<Characters::String, USE_DIGESTER_>{}(L"1") == 1243634960));
                }
                else if (sizeof (wchar_t) == 4) {
                    VerifyTestResult ((Cryptography::Digest::Hash<Characters::String, USE_DIGESTER_>{}(L"1") == 2036516137));
                }
                VerifyTestResult ((Cryptography::Digest::Hash<string, USE_DIGESTER_>{"mysalt"}("1") == 1355707049));
                VerifyTestResult ((Cryptography::Digest::Hash<int, USE_DIGESTER_>{}(ToLE_ (93993)) == 1748544338));
            }
            {
                const char kSrc[] = "This is a very good test of a very good test";
                DoCommonDigesterTest_<USE_DIGESTER_> ((const byte*)kSrc, (const byte*)kSrc + ::strlen (kSrc), 2786528596);
            }
        }
    }
}

namespace {
    namespace Digester_MD5_ {

        using namespace Cryptography::Digest;

        void DoRegressionTests_ ()
        {
            Debug::TraceContextBumper ctx{"Digester_MD5_::DoRegressionTests_"};

            using USE_DIGESTER_ = Digester<Algorithm::MD5>;
            {
                const char kSrc[]        = "This is a very good test of a very good test";
                const char kEncodedVal[] = "08c8888b86d6300ade93a10095a9083a";
                VerifyTestResult ((Cryptography::Digest::Hash<string, USE_DIGESTER_, string>{}(kSrc)) == kEncodedVal);
            }
            {
                using namespace Characters; // fails due to qCompilerAndStdLib_SpaceshipAutoGenForOpEqualsForCommonGUID_Buggy
                VerifyTestResult (Cryptography::Format<String> (Hash<string, USE_DIGESTER_>{}("x")) == L"9dd4e461268c8034f5c8564e155c67a6");
                VerifyTestResult (Cryptography::Format<string> (Hash<string, USE_DIGESTER_>{}("x")) == "9dd4e461268c8034f5c8564e155c67a6");
                VerifyTestResult ((Common::GUID{Hash<string, USE_DIGESTER_>{}("x")} == Common::GUID{L"61e4d49d-8c26-3480-f5c8-564e155c67a6"}));
                VerifyTestResult ((Hash<string, USE_DIGESTER_, Common::GUID>{}("x") == Common::GUID{L"61e4d49d-8c26-3480-f5c8-564e155c67a6"}));
            }
        }
    }
}

namespace {
    namespace Digester_SuperFastHash_ {

        using namespace Cryptography::Digest;

        void DoRegressionTests_ ()
        {
            Debug::TraceContextBumper ctx{"Digester_SuperFastHash_::DoRegressionTests_"};

            // @todo -- RETHINK IF RESULTS SB SAME REGARDLESS OF ENDIAN - NOT CONSISTENT!!!! --LGP 2015-08-26 -- AIX
            using USE_DIGESTER_ = Digester<Algorithm::SuperFastHash>;
            {
                VerifyTestResult ((Cryptography::Digest::Hash<int, USE_DIGESTER_>{}(ToLE_ (1)) == 3282063817u));     // value before 2.1b10 was 422304363
                VerifyTestResult ((Cryptography::Digest::Hash<int, USE_DIGESTER_>{}(ToLE_ (93993)) == 2783293987u)); // value before 2.1b10 was 2489559407
            }
            {
                // special case where these collide (USED TO BUT NO LONGER DO DUE TO CHANGE IN what amounts to SEED in 2.1b10)
                const char kSrc1[] = "        90010";
                DoCommonDigesterTest_<USE_DIGESTER_> ((const byte*)kSrc1, (const byte*)kSrc1 + ::strlen (kSrc1), 4253059698u); // value before 2.1b10 was 375771507
                const char kSrc2[] = "        10028";
                DoCommonDigesterTest_<USE_DIGESTER_> ((const byte*)kSrc2, (const byte*)kSrc2 + ::strlen (kSrc2), 1644096207u); // value before 2.1b10 was 375771507
            }
            {
                const char kSrc[] = "This is a very good test of a very good test";
                DoCommonDigesterTest_<USE_DIGESTER_> ((const byte*)kSrc, (const byte*)kSrc + ::strlen (kSrc), 1796287867u); // value before 2.1b10 was 1181771593
            }
        }
    }
}

namespace {
    namespace EnumerateOpenSSLAlgorithmsInContexts_ {
        void DoRegressionTests_ ()
        {
#if qHasFeature_OpenSSL
            Debug::TraceContextBumper ctx{"EnumerateOpenSSLAlgorithmsInContexts_::DoRegressionTests_"};
            Set<String>               defaultContextAvailableCipherAlgorithms = Set<String>{OpenSSL::LibraryContext::sDefault.pAvailableCipherAlgorithms ().Select<String> ([] (auto i) { return i.pName (); })};
            Set<String>               defaultContextStandardCipherAlgorithms  = Set<String>{OpenSSL::LibraryContext::sDefault.pStandardCipherAlgorithms ().Select<String> ([] (auto i) { return i.pName (); })};
            Set<String>               defaultContextAvailableDigestAlgorithms = Set<String>{OpenSSL::LibraryContext::sDefault.pAvailableDigestAlgorithms ().Select<String> ([] (auto i) { return i.pName (); })};
            Set<String>               defaultContextStandardDigestAlgorithms  = Set<String>{OpenSSL::LibraryContext::sDefault.pStandardDigestAlgorithms ().Select<String> ([] (auto i) { return i.pName (); })};

            DbgTrace (L"defaultContextAvailableCipherAlgorithms = #%d %s", defaultContextAvailableCipherAlgorithms.size (), Characters::ToString (defaultContextAvailableCipherAlgorithms).c_str ());
            DbgTrace (L"defaultContextStandardCipherAlgorithms = #%d %s", defaultContextStandardCipherAlgorithms.size (), Characters::ToString (defaultContextStandardCipherAlgorithms).c_str ());
            DbgTrace (L"defaultContextAvailableDigestAlgorithms = #%d %s", defaultContextAvailableDigestAlgorithms.size (), Characters::ToString (defaultContextAvailableDigestAlgorithms).c_str ());
            DbgTrace (L"defaultContextStandardDigestAlgorithms = #%d %s", defaultContextStandardDigestAlgorithms.size (), Characters::ToString (defaultContextStandardDigestAlgorithms).c_str ());

            // worth noting if these fail
            if (not defaultContextAvailableCipherAlgorithms.Contains (defaultContextStandardCipherAlgorithms)) {
                Stroika::TestHarness::WarnTestIssue (Characters::Format (L"defaultContextAvailableCipherAlgorithms missing standard algoritmhs: %s", Characters::ToString (defaultContextStandardCipherAlgorithms - defaultContextAvailableCipherAlgorithms).c_str ()).c_str ());
            }
            if (not defaultContextAvailableDigestAlgorithms.Contains (defaultContextStandardDigestAlgorithms)) {
                Stroika::TestHarness::WarnTestIssue (Characters::Format (L"defaultContextAvailableDigestAlgorithms missing standard algoritmhs: %s", Characters::ToString (defaultContextStandardDigestAlgorithms - defaultContextAvailableDigestAlgorithms).c_str ()).c_str ());
            }
            // for openssl v3 could also check with legacy provider loaded...
#endif
        }
    }
}

namespace {
    namespace AllSSLEncrytionRoundtrip {
        using namespace Cryptography::Encoding;
        using namespace Cryptography::Encoding::Algorithm;

        void DoRegressionTests_ ()
        {
            Debug::TraceContextBumper ctx{"AllSSLEncrytionRoundtrip::DoRegressionTests_"};

#if qHasFeature_OpenSSL
            using Memory::BLOB;
            using namespace Stroika::Foundation::Cryptography::Encoding;

            auto roundTripTester_ = [] (const OpenSSLCryptoParams& cryptoParams, BLOB src) -> void {
                BLOB encodedData = OpenSSLInputStream::New (cryptoParams, Direction::eEncrypt, src.As<Streams::InputStream<byte>::Ptr> ()).ReadAll ();
                BLOB decodedData = OpenSSLInputStream::New (cryptoParams, Direction::eDecrypt, encodedData.As<Streams::InputStream<byte>::Ptr> ()).ReadAll ();
                VerifyTestResult (src == decodedData);
            };

            const char        kKey1_[]        = "Mr Key";
            const char        kKey2_[]        = "One Very Very Very Long key 123";
            static const BLOB kPassphrases_[] = {
                BLOB::Raw (kKey1_, NEltsOf (kKey1_) - 1),
                BLOB::Raw (kKey2_, NEltsOf (kKey2_) - 1),
            };

            const char kSrc1_[] = "This is a very good test of a very good test";
            const char kSrc2_[] = "";
            const char kSrc3_[] = "We eat wiggly worms. That was a very good time to eat the worms. They are awesome!";
            const char kSrc4_[] = "0123456789";

            static const BLOB kTestMessages_[] = {
                BLOB::Raw (kSrc1_, NEltsOf (kSrc1_) - 1),
                BLOB::Raw (kSrc2_, NEltsOf (kSrc2_) - 1),
                BLOB::Raw (kSrc3_, NEltsOf (kSrc3_) - 1),
                BLOB::Raw (kSrc4_, NEltsOf (kSrc4_) - 1)};

            Set<String> providers2Try{OpenSSL::LibraryContext::kDefaultProvider};
            if (OPENSSL_VERSION_NUMBER >= 0x30000000L) {
                providers2Try += OpenSSL::LibraryContext::kLegacyProvider;
            }
            for (String provider : providers2Try) {
#if qCompiler_Sanitizer_ASAN_With_OpenSSL3_LoadLegacyProvider_Buggy
                optional<OpenSSL::LibraryContext::TemporarilyAddProvider> providerAdder;
                if (not(Debug::kBuiltWithAddressSanitizer and provider == OpenSSL::LibraryContext::kLegacyProvider)) {
                    providerAdder = OpenSSL::LibraryContext::TemporarilyAddProvider{&OpenSSL::LibraryContext::sDefault, provider};
                }
#else
                OpenSSL::LibraryContext::TemporarilyAddProvider providerAdder{&OpenSSL::LibraryContext::sDefault, provider};
#endif
                unsigned int nCipherTests{};
                unsigned int nFailures{};
                Set<String>  failingCiphers;
                for (BLOB passphrase : kPassphrases_) {
                    for (BLOB inputMessage : kTestMessages_) {
                        for (CipherAlgorithm ci : OpenSSL::LibraryContext::sDefault.pAvailableCipherAlgorithms ()) {
                            for (DigestAlgorithm di : OpenSSL::LibraryContext::sDefault.pAvailableDigestAlgorithms ()) {
                                nCipherTests++;
                                try {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                                    Debug::TraceContextBumper ctx{L"roundtriptesting", L"ci=%s, di=%s", Characters::ToString (ci).c_str (), Characters::ToString (di).c_str ()};
#endif
                                    OpenSSLCryptoParams cryptoParams{ci, OpenSSL::EVP_BytesToKey{ci, di, passphrase}};
                                    roundTripTester_ (cryptoParams, inputMessage);
                                }
                                catch (...) {
                                    nFailures++;
                                    failingCiphers.Add (Characters::ToString (ci));
                                    DbgTrace (L"For Test (%s, %s): Ignorning exception: %s", Characters::ToString (ci).c_str (), Characters::ToString (di).c_str (), Characters::ToString (current_exception ()).c_str ());
                                }
                            }
                        }
                    }
                }
                if (nFailures != 0) {
                    Set<String>              allCiphers{OpenSSL::LibraryContext::sDefault.pAvailableCipherAlgorithms ().Select<String> ([] (auto i) { return i.pName (); })};
                    Set<String>              passingCiphers              = allCiphers - failingCiphers;
                    static const Set<String> kLastSeenAllFailingCiphers_ = {L"AES-128-OCB", L"AES-128-XTS", L"AES-192-OCB", L"AES-256-OCB", L"AES-256-XTS", L"ARIA-128-CCM", L"ARIA-128-GCM", L"ARIA-192-CCM", L"ARIA-192-GCM", L"ARIA-256-CCM", L"ARIA-256-GCM", L"id-aes128-CCM", L"id-aes128-GCM", L"id-aes128-wrap", L"id-aes128-wrap-pad", L"id-aes192-CCM", L"id-aes192-GCM", L"id-aes192-wrap", L"id-aes192-wrap-pad", L"id-aes256-CCM", L"id-aes256-GCM", L"id-aes256-wrap", L"id-aes256-wrap-pad", L"id-smime-alg-CMS3DESwrap"};
                    if (kLastSeenAllFailingCiphers_ != failingCiphers) {
                        Stroika::TestHarness::WarnTestIssue (Characters::Format (L"For provider=%s, nCipherTests=%d, nFailures=%d, failingCiphers=%s, passing-ciphrs=%s", provider.c_str (), nCipherTests, nFailures, Characters::ToString (failingCiphers).c_str (), Characters::ToString (passingCiphers).c_str ()).c_str ());
                    }
                    static const Set<String> kStandardCipherAlgorithmNames{OpenSSL::LibraryContext::sDefault.pStandardCipherAlgorithms ().Select<String> ([] (auto i) { return i.pName (); })};
                    if (failingCiphers ^ kStandardCipherAlgorithmNames) {
                        Stroika::TestHarness::WarnTestIssue (Characters::Format (L"For provider=%s, some standard ciphers failed: %s", provider.c_str (), Characters::ToString (failingCiphers ^ kStandardCipherAlgorithmNames).c_str ()).c_str ());
                    }
                }
            }
#endif
        }
    }
}

namespace {
    namespace OpenSSLDeriveKeyTests_ {
        using namespace Cryptography::Encoding;
        using namespace Cryptography::Encoding::Algorithm;

        void DoRegressionTests_ ()
        {
            Debug::TraceContextBumper ctx{"OpenSSLDeriveKeyTests_::DoRegressionTests_"};

#if qHasFeature_OpenSSL
            using Characters::String;
            using Memory::BLOB;
            using namespace Stroika::Foundation::Cryptography::Encoding;

            auto checkNoSalt = [] (CipherAlgorithm cipherAlgorithm, DigestAlgorithm digestAlgorithm, const String& password, const DerivedKey& expected) {
                unsigned int nRounds = 1; // command-line tool uses this
                DerivedKey   dk      = OpenSSL::EVP_BytesToKey{cipherAlgorithm, digestAlgorithm, password, nRounds};
                DbgTrace (L"dk=%s; expected=%s", Characters::ToString (dk).c_str (), Characters::ToString (expected).c_str ());
                VerifyTestResult (dk == expected);
            };
            auto checkWithSalt = [] (CipherAlgorithm cipherAlgorithm, DigestAlgorithm digestAlgorithm, const String& password, const BLOB& salt, const DerivedKey& expected) {
                unsigned int nRounds = 1; // command-line tool uses this
                DerivedKey   dk      = OpenSSL::EVP_BytesToKey{cipherAlgorithm, digestAlgorithm, password, nRounds, salt};
                DbgTrace (L"dk=%s; expected=%s", Characters::ToString (dk).c_str (), Characters::ToString (expected).c_str ());
                VerifyTestResult (dk == expected);
            };

            // openssl rc4 -P -k mypass -md md5 -nosalt
            //      key=A029D0DF84EB5549C641E04A9EF389E5
            checkNoSalt (OpenSSL::CipherAlgorithms::kRC4, OpenSSL::DigestAlgorithms::kMD5, L"mypass", DerivedKey{BLOB::Hex ("A029D0DF84EB5549C641E04A9EF389E5"), BLOB{}});

            // openssl rc4 -P -k mypass  -md md5 -S 0102030405060708
            //  salt=0102030405060708
            //  key=56BDFF04895C5D16F5E3F68737000092
            checkWithSalt (OpenSSL::CipherAlgorithms::kRC4, OpenSSL::DigestAlgorithms::kMD5, L"mypass", BLOB::Hex ("0102030405060708"), DerivedKey{BLOB::Hex ("56BDFF04895C5D16F5E3F68737000092"), BLOB{}});

            // openssl blowfish -P -k mypass -md sha1 -nosalt
            //      key=E727D1464AE12436E899A726DA5B2F11
            //      iv =D8381B26923E0415
            checkNoSalt (OpenSSL::CipherAlgorithms::kBlowfish, OpenSSL::DigestAlgorithms::kSHA1, L"mypass", DerivedKey{BLOB::Hex ("E727D1464AE12436E899A726DA5B2F11"), BLOB::Hex ("D8381B26923E0415")});

            // openssl aes-256-cbc -P -k mypass -md md5 -nosalt
            //      key=A029D0DF84EB5549C641E04A9EF389E5A10CE9C4682486F8622F2F18E7291367
            //      iv =541F477059FAEFD57328A0B0D22F2A20
            checkNoSalt (OpenSSL::CipherAlgorithms::kAES_256_CBC, OpenSSL::DigestAlgorithms::kMD5, L"mypass", DerivedKey{BLOB::Hex ("A029D0DF84EB5549C641E04A9EF389E5A10CE9C4682486F8622F2F18E7291367"), BLOB::Hex ("541F477059FAEFD57328A0B0D22F2A20")});

            // openssl aes-128-ofb -P -k mypass -md sha1 -S 1122334455667788
            //      salt=1122334455667788
            //      key=36237DC4B90DD237329731E85EE5BB5A
            //      iv =35F1A763D974A002DB1721B8F25498E6
            checkWithSalt (OpenSSL::CipherAlgorithms::kAES_128_OFB, OpenSSL::DigestAlgorithms::kSHA1, L"mypass", BLOB::Hex ("1122334455667788"), DerivedKey{BLOB::Hex ("36237DC4B90DD237329731E85EE5BB5A"), BLOB::Hex ("35F1A763D974A002DB1721B8F25498E6")});
#endif
        }
    }
}

namespace {
    namespace OpenSSLEncryptDecryptTests_ {
        using namespace Cryptography::Encoding;
        using namespace Cryptography::Encoding::Algorithm;

        void DoRegressionTests_ ()
        {
            Debug::TraceContextBumper ctx{"OpenSSLEncryptDecryptTests_::DoRegressionTests_"};

#if qHasFeature_OpenSSL
            using Characters::String;
            using Memory::BLOB;
            using namespace Stroika::Foundation::Cryptography::Encoding;

            auto checkNoSalt = [] (CipherAlgorithm cipherAlgorithm, DigestAlgorithm digestAlgorithm, const String& password, const BLOB& src, const BLOB& expected) {
                if (OpenSSL::LibraryContext::sDefault.pAvailableCipherAlgorithms ().Contains (cipherAlgorithm)) {
                    unsigned int        nRounds = 1; // command-line tool uses this
                    OpenSSLCryptoParams cryptoParams{cipherAlgorithm, OpenSSL::EVP_BytesToKey{cipherAlgorithm, digestAlgorithm, password, nRounds}};
                    DbgTrace (L"dk=%s", Characters::ToString (OpenSSL::EVP_BytesToKey{cipherAlgorithm, digestAlgorithm, password, nRounds}).c_str ());
                    BLOB encodedData = OpenSSLInputStream::New (cryptoParams, Direction::eEncrypt, src.As<Streams::InputStream<byte>::Ptr> ()).ReadAll ();
                    BLOB decodedData = OpenSSLInputStream::New (cryptoParams, Direction::eDecrypt, encodedData.As<Streams::InputStream<byte>::Ptr> ()).ReadAll ();
                    DbgTrace (L"src=%s; encodedData=%s; expected=%s; decodedData=%s", Characters::ToString (src).c_str (), Characters::ToString (encodedData).c_str (), Characters::ToString (expected).c_str (), Characters::ToString (decodedData).c_str ());
                    VerifyTestResult (encodedData == expected);
                    VerifyTestResult (src == decodedData);
                }
            };

            //  echo apples and pears| od -t x1 --width=64
            //      0000000 61 70 70 6c 65 73 20 61 6e 64 20 70 65 61 72 73 0d 0a
            //
            //  echo apples and pears| openssl rc4 -md md5 -e -k abc -nosalt -P
            //      key=900150983CD24FB0D6963F7D28E17F72
            //
            //  echo apples and pears| openssl rc4 -md md5 -e -k abc -nosalt | openssl rc4 -md md5 -d -k abc -nosalt
            //      apples and pears
            //
            //  echo apples and pears| openssl rc4 -md md5 -e -k abc -nosalt | od -t x1 --width=64
            //      0000000 4a 94 99 ac 55 f7 a2 8b 1b ca 75 62 f6 9a cf de 41 9d
            //
            checkNoSalt (OpenSSL::CipherAlgorithms::kRC4, OpenSSL::DigestAlgorithms::kMD5, L"abc", BLOB::Hex ("61 70 70 6c 65 73 20 61 6e 64 20 70 65 61 72 73 0d 0a"), BLOB::Hex ("4a 94 99 ac 55 f7 a2 8b 1b ca 75 62 f6 9a cf de 41 9d"));

            //  echo hi mom| od -t x1 --width=64
            //      0000000 68 69 20 6d 6f 6d 0d 0a
            //  echo hi mom| openssl bf -e -k aaa -nosalt | openssl bf -d -k aaa -nosalt
            //      hi mom
            //  echo hi mom| openssl bf -md md5 -k aaa -nosalt | od -t x1 --width=64
            //      0000000 29 14 4a db 4e ce 20 45 09 56 e8 13 65 2f e8 d6
            checkNoSalt (OpenSSL::CipherAlgorithms::kBlowfish, OpenSSL::DigestAlgorithms::kMD5, L"aaa", BLOB::Hex ("68 69 20 6d 6f 6d 0d 0a"), BLOB::Hex ("29 14 4a db 4e ce 20 45 09 56 e8 13 65 2f e8 d6"sv));

            //  echo hi mom| od -t x1 --width=64
            //      0000000 68 69 20 6d 6f 6d 0d 0a
            //   echo hi mom| openssl aes-128-cbc -md md5 -k aaa -nosalt | od -t x1 --width=64
            //      0000000 6b 95 c9 eb 68 5e c3 7f 4f e4 86 99 55 1d 05 53
            checkNoSalt (OpenSSL::CipherAlgorithms::kAES_128_CBC, OpenSSL::DigestAlgorithms::kMD5, L"aaa", BLOB::Hex ("68 69 20 6d 6f 6d 0d 0a"sv), BLOB::Hex ("6b 95 c9 eb 68 5e c3 7f 4f e4 86 99 55 1d 05 53"));
#endif
        }
    }
}

namespace {
    namespace AESTest_ {
        using namespace Cryptography::Encoding;
        using namespace Cryptography::Encoding::Algorithm;

        void DoRegressionTests_ ()
        {
            Debug::TraceContextBumper ctx{"AESTest_::DoRegressionTests_"};

            {
                /**
                 *      echo -n "This is a very good test of a very good test" | od -t x1 --width=100
                 *          0000000 2d 6e 20 22 54 68 69 73 20 69 73 20 61 20 76 65 72 79 20 67 6f 6f 64 20 74 65 73 74 20 6f 66 20 61 20 76 65 72 79 20 67 6f 6f 64 20 74 65 73 74 22 20 0d 0a
                 *
                 *      echo -n "This is a very good test of a very good test" | openssl enc -k password -e -aes-256-cbc -nosalt | od -t x1 --width=100
                 *          0000000 62 d2 eb f6 ee 92 4f 7f 1d 5e 70 d0 dc 90 cc 3a b2 37 f5 d6 2c e4 42 d9 34 50 5b 6c fc 89 5b da c9 ab 29 5b ef d2 87 b6 07 0f df 55 f5 43 21 7b 0c cc 4a 2f d6 d8 25 d7 73 ed a9 1c 48 15 96 cd
                 */
                const Memory::BLOB srcText   = Memory::BLOB::Hex ("2d 6e 20 22 54 68 69 73 20 69 73 20 61 20 76 65 72 79 20 67 6f 6f 64 20 74 65 73 74 20 6f 66 20 61 20 76 65 72 79 20 67 6f 6f 64 20 74 65 73 74 22 20 0d 0a");
                const Memory::BLOB encResult = Memory::BLOB::Hex ("62 d2 eb f6 ee 92 4f 7f 1d 5e 70 d0 dc 90 cc 3a b2 37 f5 d6 2c e4 42 d9 34 50 5b 6c fc 89 5b da c9 ab 29 5b ef d2 87 b6 07 0f df 55 f5 43 21 7b 0c cc 4a 2f d6 d8 25 d7 73 ed a9 1c 48 15 96 cd");
#if qHasFeature_OpenSSL
                const OpenSSL::DerivedKey kDerivedKey = OpenSSL::EVP_BytesToKey{OpenSSL::CipherAlgorithms::kAES_256_CBC (), OpenSSL::DigestAlgorithms::kMD5, "password"};
                // HORRIBLE MESS OF AN API! -
                // @todo - FIX
                VerifyTestResult (EncodeAES (kDerivedKey, srcText, AESOptions::e256_CBC) == encResult);
                VerifyTestResult (DecodeAES (kDerivedKey, encResult, AESOptions::e256_CBC) == srcText);
#endif
            }
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        Debug::TraceContextBumper ctx{"DoRegressionTests_"};
        Base64Test::DoRegressionTests_ ();
        MD5Test::DoRegressionTests_ ();
        HashMisc::DoRegressionTests_ ();
        DigestAltResults::DoRegressionTests_ ();
        Digest_CRC32_::DoRegressionTests_ ();
        Digest_Jenkins_::DoRegressionTests_ ();
        Digester_MD5_::DoRegressionTests_ ();
        Digester_SuperFastHash_::DoRegressionTests_ ();
        EnumerateOpenSSLAlgorithmsInContexts_::DoRegressionTests_ ();
        AllSSLEncrytionRoundtrip::DoRegressionTests_ ();
        OpenSSLDeriveKeyTests_::DoRegressionTests_ ();
        OpenSSLEncryptDecryptTests_::DoRegressionTests_ ();
        AESTest_::DoRegressionTests_ ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
