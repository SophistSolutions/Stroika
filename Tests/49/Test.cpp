/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
#include "Stroika/Foundation/Streams/iostream/OutputStreamFromStdOStream.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Streams::iostream;

using namespace Stroika::Frameworks;

using std::byte;

#if qHasFeature_GoogleTest
namespace {
    namespace BasicBinaryInputStream_ {

        void TestBasicConstruction_ ()
        {
            {
                MemoryStream::Ptr<byte> s = MemoryStream::New<byte> ();
                EXPECT_TRUE (s != nullptr);
                EXPECT_TRUE (s.IsSeekable ());
            }
            {
                const char              kData[] = "1";
                MemoryStream::Ptr<byte> s       = MemoryStream::New<byte> (Memory::SpanReInterpretCast<const byte> (span{kData}));
                EXPECT_TRUE (s != nullptr);
                EXPECT_TRUE (s.IsSeekable ());
                byte result[100] = {byte{0}};
                EXPECT_TRUE (s.Read (span{result}).size () == 2);
                EXPECT_TRUE (to_integer<char> (result[0]) == '1');
                EXPECT_TRUE (to_integer<char> (result[1]) == '\0');
            }
        }

        void Tests_ ()
        {
            TestBasicConstruction_ ();
        }
    }
}

namespace {
    namespace BasicBinaryOutputStream_ {
        void TestBasicConstruction_ ()
        {
            {
                MemoryStream::Ptr<byte> s = MemoryStream::New<byte> ();
                EXPECT_TRUE (s != nullptr);
                EXPECT_TRUE (s.IsSeekable ());
            }
            {
                MemoryStream::Ptr<byte> s = MemoryStream::New<byte> ();
                EXPECT_TRUE (s != nullptr);
                EXPECT_TRUE (s.IsSeekable ());

                using namespace Memory;
                constexpr byte kData_[] = {3_b, 53_b, 43_b, 23_b, 3_b};
                s.Write (span{kData_});
                Memory::BLOB b = s.As<Memory::BLOB> ();
                EXPECT_TRUE (b.size () == sizeof (kData_));
                EXPECT_TRUE (b == Memory::BLOB{span{kData_}});
            }
        }

        void Tests_ ()
        {
            TestBasicConstruction_ ();
        }
    }
}

namespace {
    namespace BasicBinaryInputOutputStream_ {

        void TestBasicConstruction_ ()
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

        void Tests_ ()
        {
            TestBasicConstruction_ ();
        }
    }
}

namespace {
    namespace BinaryOutputStreamFromOStreamAdapter_ {

        void T1_ ()
        {
            {
                stringstream                          s;
                OutputStreamFromStdOStream::Ptr<byte> so       = OutputStreamFromStdOStream::New<byte> (s);
                const char                            kData_[] = "ddasdf3294234";
                so.Write (span{kData_, ::strlen (kData_)});
                EXPECT_TRUE (s.str () == kData_);
            }
        }

        void Tests_ ()
        {
            T1_ ();
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

        void Tests_ ()
        {
            Private_::T1_ ();
        }
    }
}

namespace {
    namespace TextReaderFromIterableAndString {

        void T1_ ()
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

        void Tests_ ()
        {
            T1_ ();
        }
    }
}

namespace {
    namespace TextReaderFromBLOB {

        void T1_ ()
        {
            using Characters::Character;
            using Characters::String;
            {
                Memory::BLOB    s  = Memory::BLOB::FromRaw (u8"Testing 1, 2, 3");
                TextReader::Ptr tr = TextReader::New (s);
                EXPECT_TRUE (tr.ReadAll () == L"Testing 1, 2, 3");
            }
        }

        void Tests_ ()
        {
            T1_ ();
        }
    }
}

namespace {
    namespace SharedMemoryStream_Doc_Example_Test8 {
        namespace Private_ {
            void T1_ ()
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

        void Tests_ ()
        {
            Private_::T1_ ();
        }
    }
}

namespace {
    namespace Streams_Copy_Test9 {
        namespace Private_ {
            void T1_ ()
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

        void Tests_ ()
        {
            Private_::T1_ ();
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        BasicBinaryInputStream_::Tests_ ();
        BasicBinaryOutputStream_::Tests_ ();
        BasicBinaryInputOutputStream_::Tests_ ();
        BinaryOutputStreamFromOStreamAdapter_::Tests_ ();
        TestBasicTextOutputStream_::Tests_ ();
        TextReaderFromIterableAndString::Tests_ ();
        TextReaderFromBLOB::Tests_ ();
        SharedMemoryStream_Doc_Example_Test8::Tests_ ();
        Streams_Copy_Test9::Tests_ ();
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
