/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
//  TEST    Foundation::Streams
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/Copy.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/OutputStream.h"
#include "Stroika/Foundation/Streams/SharedMemoryStream.h"
#include "Stroika/Foundation/Streams/iostream/InputStreamFromStdIStream.h"
#include "Stroika/Foundation/Streams/iostream/OutputStreamFromStdOStream.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Streams::iostream;

using namespace Stroika::Frameworks;

using std::byte;

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Foundation_Streams, BasicBinaryInputStream_)
    {
        {
            MemoryStream::Ptr<byte> s = MemoryStream::New<byte> ();
            EXPECT_NE (s, nullptr);
            EXPECT_TRUE (s.IsSeekable ());
        }
        {
            const char              kData[] = "1";
            MemoryStream::Ptr<byte> s       = MemoryStream::New<byte> (as_bytes (span{kData}));
            EXPECT_NE (s, nullptr);
            EXPECT_TRUE (s.IsSeekable ());
            byte result[100] = {byte{0}};
            EXPECT_EQ (s.ReadBlocking (span{result}).size (), 2u);
            EXPECT_EQ (to_integer<char> (result[0]), '1');
            EXPECT_EQ (to_integer<char> (result[1]), '\0');
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, BasicBinaryOutputStream_)
    {
        {
            MemoryStream::Ptr<byte> s = MemoryStream::New<byte> ();
            EXPECT_NE (s, nullptr);
            EXPECT_TRUE (s.IsSeekable ());
        }
        {
            MemoryStream::Ptr<byte> s = MemoryStream::New<byte> ();
            EXPECT_NE (s, nullptr);
            EXPECT_TRUE (s.IsSeekable ());

            using namespace Memory;
            constexpr byte kData_[] = {3_b, 53_b, 43_b, 23_b, 3_b};
            s.Write (span{kData_});
            Memory::BLOB b = s.As<Memory::BLOB> ();
            EXPECT_TRUE (b.size () == sizeof (kData_));
            EXPECT_TRUE (b == Memory::BLOB{span{kData_}});
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, BasicBinaryInputOutputStream_)
    {
        {
            MemoryStream::Ptr<byte> s = MemoryStream::New<byte> ();
            EXPECT_TRUE (s != nullptr);
            EXPECT_TRUE (s.IsSeekable ());
            EXPECT_TRUE (static_cast<InputStream::Ptr<byte>> (s).IsSeekable ());
            EXPECT_TRUE (static_cast<OutputStream::Ptr<byte>> (s).IsSeekable ());
        }
        {
            MemoryStream::Ptr<byte> s = MemoryStream::New<byte> ();
            EXPECT_TRUE (s != nullptr);

            const uint8_t kData_[] = {3, 53, 43, 23, 3};
            s.Write (span{kData_});
            Memory::BLOB b = s.As<Memory::BLOB> ();
            EXPECT_EQ (b.size (), sizeof (kData_));
            EXPECT_EQ (b, Memory::BLOB (std::begin (kData_), std::end (kData_)));
        }
        {
            MemoryStream::Ptr<byte> s = MemoryStream::New<byte> ();
            EXPECT_EQ (s.GetReadOffset (), 0);
            EXPECT_EQ (s.GetWriteOffset (), 0);
            const uint8_t kData_[] = {3, 53, 43, 23, 3};
            s.Write (span{kData_});
            EXPECT_EQ (s.GetReadOffset (), 0u);
            EXPECT_EQ (s.GetWriteOffset (), sizeof (kData_));
            byte bArr[1024];
            Verify (s.ReadBlocking (span{bArr}).size () == sizeof (kData_));
            EXPECT_EQ (s.GetReadOffset (), sizeof (kData_));
            EXPECT_EQ (s.GetWriteOffset (), sizeof (kData_));
            EXPECT_EQ ((Memory::BLOB{std::begin (bArr), std::begin (bArr) + s.GetReadOffset ()}),
                       (Memory::BLOB{std::begin (kData_), std::end (kData_)}));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, BinaryOutputStreamFromOStreamAdapter_)
    {
        {
            stringstream                          s;
            OutputStreamFromStdOStream::Ptr<byte> so       = OutputStreamFromStdOStream::New<byte> (s);
            const char                            kData_[] = "ddasdf3294234";
            so.Write (span{kData_, ::strlen (kData_)});
            EXPECT_TRUE (s.str () == kData_);
        }
    }
}

namespace {
    namespace TestBasicTextOutputStream_ {
        namespace Private_ {
            using Characters::Character;
            using Characters::String;
            void T1_ ()
            {
                MemoryStream::Ptr<Character> out = MemoryStream::New<Character> ();
                out << L"abc";
                EXPECT_TRUE (out.As<String> () == L"abc");
                out << L"123";
                EXPECT_TRUE (out.As<String> () == L"abc123");
            }
            void T2_ ()
            {
                MemoryStream::Ptr<Character> out = MemoryStream::New<Character> ();
                out << L"abc";
                EXPECT_TRUE (out.As<String> () == L"abc");
                out << L"123";
                EXPECT_TRUE (out.As<String> () == L"abc123");
                out.SeekWrite (2);
                out.SeekRead (3); // safe but irrelevant, as we don't read
                out << L"C";
                EXPECT_TRUE (out.As<String> () == L"abC123");
            }
        }
    }
    GTEST_TEST (Foundation_Streams, TestBasicTextOutputStream_)
    {
        using namespace TestBasicTextOutputStream_;
        Private_::T1_ ();
        Private_::T2_ ();
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, TextReaderFromIterableAndString)
    {
        using Characters::Character;
        using Characters::String;
        {
            Traversal::Iterable<Character> s  = String{"This"};
            BinaryToText::Reader::Ptr      tr = BinaryToText::Reader::New (s);
            EXPECT_TRUE (tr.ReadAll () == "This");
        }
        {
            EXPECT_TRUE ((BinaryToText::Reader::New (String{"hello world"}).ReadAll () == "hello world"));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, TextReaderFromBLOB)
    {
        using Characters::Character;
        using Characters::String;
        {
            Memory::BLOB              s  = Memory::BLOB::FromRaw (u8"Testing 1, 2, 3");
            BinaryToText::Reader::Ptr tr = BinaryToText::Reader::New (s);
            EXPECT_EQ (tr.ReadAll (), "Testing 1, 2, 3");
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, SharedMemoryStream_Doc_Example_Test8)
    {
        using namespace Execution;
        SharedMemoryStream::Ptr<unsigned int> pipe = SharedMemoryStream::New<unsigned int> ();
        unsigned                              sum{};
        static constexpr unsigned int         kStartWith{1};
        static constexpr unsigned int         kUpToInclusive_{1000};
        Thread::Ptr                           consumer = Thread::New (
            [&] () {
                while (auto o = pipe.ReadBlocking ()) {
                    sum += *o;
                }
            },
            Thread::eAutoStart);
        Thread::Ptr producer = Thread::New (
            [&] () {
                for (unsigned int i = kStartWith; i <= kUpToInclusive_; ++i) {
                    pipe.Write (i);
                };
                pipe.CloseWrite (); // critical or consumer hangs on final read
            },
            Thread::eAutoStart);
        Thread::WaitForDone ({consumer, producer});
        Assert (sum == (1 + kUpToInclusive_) * (kUpToInclusive_ - 1 + 1) / 2); // not a race
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, Streams_Copy_Test9)
    {
        using Characters::Character;
        using Characters::String;
        MemoryStream::Ptr<Character> in = MemoryStream::New<Character> ();
        in << L"abc";
        EXPECT_TRUE (in.As<String> () == L"abc");

        MemoryStream::Ptr<Character> out = MemoryStream::New<Character> ();
        Streams::CopyAll<Character> (in, out);
        EXPECT_TRUE (out.As<String> () == L"abc");
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, IOStreamSeekBug)
    {
        // short input stream caused issue with BinaryToText::Reader reading BOM, setting EOF/Fail flag which seek back didn't clear
        {
            stringstream tmp;
            tmp << "3";
            auto in = Streams::iostream::InputStreamFromStdIStream::New<byte> (tmp);

            auto r = in.ReadAll ();
            EXPECT_EQ (r.size (), 1u);
        }
        {
            stringstream tmp;
            tmp << "3";
            auto inb = Streams::iostream::InputStreamFromStdIStream::New<byte> (tmp);
            auto in  = BinaryToText::Reader::New (inb, nullopt, SeekableFlag::eSeekable);
            auto r   = in.ReadAll ();
            EXPECT_EQ (r.size (), 1u);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, TextReaderBug)
    {
        Debug::TraceContextBumper ctx{"TextReaderBug"};
        {
            constexpr unsigned char TESTOUT_UTF16[] = {0x50, 0x00, 0x72, 0x00, 0x61, 0x00, 0x61, 0x00, 0x74, 0x00, 0x20, 0x00, 0x36,
                                                       0x00, 0x2e, 0x00, 0x34, 0x00, 0x2e, 0x00, 0x32, 0x00, 0x33, 0x00, 0x20, 0x00,
                                                       0x28, 0x00, 0x4f, 0x00, 0x63, 0x00, 0x74, 0x00, 0x6f, 0x00, 0x62, 0x00, 0x65,
                                                       0x00, 0x72, 0x00, 0x20, 0x00, 0x32, 0x00, 0x37, 0x00, 0x20, 0x00, 0x32, 0x00,
                                                       0x30, 0x00, 0x32, 0x00, 0x34, 0x00, 0x29, 0x00, 0x0d, 0x00, 0x0a, 0x00};
            auto tr = Streams::BinaryToText::Reader::New (Memory::BLOB{span{TESTOUT_UTF16}}, Characters::UnicodeExternalEncodings::eUTF16_LE);
            auto s = tr.ReadAll ();
            // Triggered TWO bugs - the CodeCvt.inl - r.fSourceConsumed * sizeof (SERIALIZED_CHAR_T) issue, and
            // a Bug with BLOB stream code (BLOB :: As<binarystream> () needs to hold onto shared_ptr refcnt of BLOB
            // cuz in above test - it goes out of scope while STREAM still in use.
            EXPECT_EQ (s, "Praat 6.4.23 (October 27 2024)\r\n");
        }
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
