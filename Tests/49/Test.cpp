/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Streams
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Memory/Span.h"
#include "Stroika/Foundation/Streams/Copy.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/OutputStream.h"
#include "Stroika/Foundation/Streams/SharedMemoryStream.h"
#include "Stroika/Foundation/Streams/TextReader.h"
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
            EXPECT_EQ (s.Read (span{result}).size (), 2u);
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
            EXPECT_TRUE (b.size () == sizeof (kData_));
            EXPECT_TRUE (b == Memory::BLOB (std::begin (kData_), std::end (kData_)));
        }
        {
            MemoryStream::Ptr<byte> s = MemoryStream::New<byte> ();
            EXPECT_TRUE (s.GetReadOffset () == 0);
            EXPECT_TRUE (s.GetWriteOffset () == 0);
            const uint8_t kData_[] = {3, 53, 43, 23, 3};
            s.Write (span{kData_});
            EXPECT_TRUE (s.GetReadOffset () == 0);
            EXPECT_TRUE (s.GetWriteOffset () == sizeof (kData_));
            byte bArr[1024];
            Verify (s.Read (span{bArr}).size () == sizeof (kData_));
            EXPECT_TRUE (s.GetReadOffset () == sizeof (kData_));
            EXPECT_TRUE (s.GetWriteOffset () == sizeof (kData_));
            EXPECT_TRUE ((Memory::BLOB{std::begin (bArr), std::begin (bArr) + s.GetReadOffset ()} ==
                          Memory::BLOB{std::begin (kData_), std::end (kData_)}));
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
            TextReader::Ptr                tr = TextReader::New (s);
            EXPECT_TRUE (tr.ReadAll () == "This");
        }
        {
            EXPECT_TRUE ((TextReader::New (String{"hello world"}).ReadAll () == "hello world"));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, TextReaderFromBLOB)
    {
        using Characters::Character;
        using Characters::String;
        {
            Memory::BLOB    s  = Memory::BLOB::FromRaw (u8"Testing 1, 2, 3");
            TextReader::Ptr tr = TextReader::New (s);
            EXPECT_TRUE (tr.ReadAll () == L"Testing 1, 2, 3");
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
                while (auto o = pipe.Read ()) {
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
        // short input stream caused issue with TextReader reading BOM, setting EOF/Fail flag which seek back didn't clear
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
            auto in  = TextReader::New (inb, nullopt, SeekableFlag::eSeekable);
            auto r   = in.ReadAll ();
            EXPECT_EQ (r.size (), 1u);
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
