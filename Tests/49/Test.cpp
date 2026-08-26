/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::Streams
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstring>
#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Streams/BinaryToText.h"
#include "Stroika/Foundation/Streams/BufferedInputStream.h"
#include "Stroika/Foundation/Streams/Copy.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/OutputStream.h"
#include "Stroika/Foundation/Streams/SharedMemoryStream.h"
#include "Stroika/Foundation/Streams/ToSeekableInputStream.h"
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
                out << "abc";
                EXPECT_TRUE (out.As<String> () == "abc");
                out << "123";
                EXPECT_TRUE (out.As<String> () == "abc123");
            }
            void T2_ ()
            {
                MemoryStream::Ptr<Character> out = MemoryStream::New<Character> ();
                out << "abc";
                EXPECT_TRUE (out.As<String> () == "abc");
                out << "123";
                EXPECT_TRUE (out.As<String> () == "abc123");
                out.SeekWrite (2);
                out.SeekRead (3); // safe but irrelevant, as we don't read
                out << "C";
                EXPECT_TRUE (out.As<String> () == "abC123");
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
        in << "abc";
        EXPECT_TRUE (in.As<String> () == "abc");

        MemoryStream::Ptr<Character> out = MemoryStream::New<Character> ();
        Streams::CopyAll<Character> (in, out);
        EXPECT_TRUE (out.As<String> () == "abc");
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
namespace {
    constexpr char   kABCs_[]  = "abcdefghijklmnopqrstuvwxyz";
    constexpr size_t kABCsLen_ = sizeof (kABCs_) - 1; // no trailing NUL

    /*
     *  ToSeekableInputStream::New () returns its argument unchanged when that argument is already
     *  seekable, so the source stream MUST be non-seekable or none of the caching code these tests
     *  exercise is even reached. Reads it all forward (which is what accumulates the cache), then
     *  seeks back to the start, ready to re-read out of that cache.
     */
    InputStream::Ptr<byte> MkSeekableOverCachedABCs_ (stringstream& backing)
    {
        InputStream::Ptr<byte> nonSeekable = InputStreamFromStdIStream::New<byte> (backing, eNotSeekable);
        EXPECT_FALSE (nonSeekable.IsSeekable ());
        InputStream::Ptr<byte> s = ToSeekableInputStream::New<byte> (nonSeekable);
        EXPECT_TRUE (s.IsSeekable ());
        byte   all[kABCsLen_];
        size_t nRead = 0;
        while (nRead < kABCsLen_) {
            span<byte> r = s.ReadBlocking (span{all + nRead, kABCsLen_ - nRead});
            if (r.empty ()) {
                break;
            }
            nRead += r.size ();
        }
        EXPECT_EQ (nRead, kABCsLen_);
        EXPECT_EQ (std::memcmp (all, kABCs_, kABCsLen_), 0);
        EXPECT_EQ (s.GetOffset (), kABCsLen_);
        EXPECT_EQ (s.Seek (0), 0u);
        return s;
    }

    // Read until EOF in pieces of at most 'chunk' elements; returns the total read.
    size_t ReadAllInPieces_ (const InputStream::Ptr<byte>& s, span<byte> into, size_t chunk)
    {
        size_t n = 0;
        while (n < into.size ()) {
            size_t want = into.size () - n;
            if (want > chunk) {
                want = chunk;
            }
            span<byte> r = s.ReadBlocking (into.subspan (n, want));
            if (r.empty ()) {
                break;
            }
            n += r.size ();
        }
        return n;
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, ToSeekableInputStream_ReReadIntoLargerBuffer_)
    {
        Debug::TraceContextBumper ctx{"ToSeekableInputStream_ReReadIntoLargerBuffer_"};
        // A Read must never report more elements than exist, nor read past the end of the cache to produce them.
        stringstream           backing{kABCs_};
        InputStream::Ptr<byte> s = MkSeekableOverCachedABCs_ (backing);
        byte                   tooBig[kABCsLen_ * 4];
        span<byte>             got = s.ReadBlocking (span{tooBig});
        EXPECT_EQ (got.size (), kABCsLen_);
        EXPECT_EQ (std::memcmp (tooBig, kABCs_, kABCsLen_), 0);
        EXPECT_EQ (s.GetOffset (), kABCsLen_);
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, ToSeekableInputStream_ReReadIntoSmallerBuffer_)
    {
        Debug::TraceContextBumper ctx{"ToSeekableInputStream_ReReadIntoSmallerBuffer_"};
        // A Read must never produce more elements than the caller asked for.
        stringstream           backing{kABCs_};
        InputStream::Ptr<byte> s = MkSeekableOverCachedABCs_ (backing);
        byte                   tooSmall[4];
        span<byte>             got = s.ReadBlocking (span{tooSmall});
        EXPECT_EQ (got.size (), sizeof (tooSmall));
        EXPECT_EQ (std::memcmp (tooSmall, kABCs_, sizeof (tooSmall)), 0);
        EXPECT_EQ (s.GetOffset (), sizeof (tooSmall));

        // and the rest of the cache still reads back correctly after that short read
        byte       rest[kABCsLen_];
        span<byte> got2 = s.ReadBlocking (span{rest});
        EXPECT_EQ (got2.size (), kABCsLen_ - sizeof (tooSmall));
        EXPECT_EQ (std::memcmp (rest, kABCs_ + sizeof (tooSmall), kABCsLen_ - sizeof (tooSmall)), 0);
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, BufferedInputStream_SeekableOverSeekable_CanSeek_)
    {
        Debug::TraceContextBumper ctx{"BufferedInputStream_SeekableOverSeekable_CanSeek_"};
        // A stream that answers IsSeekable () true must actually support Seek ().
        InputStream::Ptr<byte> s = BufferedInputStream::New<byte> (MemoryStream::New<byte> (as_bytes (span{kABCs_, kABCsLen_})));
        EXPECT_TRUE (s.IsSeekable ());
        byte first[10];
        EXPECT_EQ (s.ReadBlocking (span{first}).size (), sizeof (first));
        EXPECT_EQ (std::memcmp (first, kABCs_, sizeof (first)), 0);
        EXPECT_EQ (s.GetOffset (), sizeof (first));
        EXPECT_EQ (s.Seek (0), 0u);
        EXPECT_EQ (s.GetOffset (), 0u);
        byte again[10];
        EXPECT_EQ (s.ReadBlocking (span{again}).size (), sizeof (again));
        EXPECT_EQ (std::memcmp (again, first, sizeof (first)), 0);
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, BufferedInputStream_UnSeekable_ReadsAndTracksOffset_)
    {
        Debug::TraceContextBumper ctx{"BufferedInputStream_UnSeekable_ReadsAndTracksOffset_"};
        // The eNotSeekable rep buffers upstream reads; it must still deliver every element in
        // order, and report the offset the CALLER is at - not how far it has pre-read upstream.
        stringstream backing{kABCs_};
        InputStream::Ptr<byte> s = BufferedInputStream::New<byte> (InputStreamFromStdIStream::New<byte> (backing, eNotSeekable), eNotSeekable);
        EXPECT_FALSE (s.IsSeekable ());
        byte head[4];
        EXPECT_EQ (s.ReadBlocking (span{head}).size (), sizeof (head));
        EXPECT_EQ (std::memcmp (head, kABCs_, sizeof (head)), 0);
        EXPECT_EQ (s.GetOffset (), sizeof (head));
        // the rest, in small pieces, all served out of the one buffer already filled upstream
        byte   rest[kABCsLen_];
        size_t n = ReadAllInPieces_ (s, span{rest, kABCsLen_ - sizeof (head)}, 3);
        EXPECT_EQ (n, kABCsLen_ - sizeof (head));
        EXPECT_EQ (std::memcmp (rest, kABCs_ + sizeof (head), n), 0);
        EXPECT_EQ (s.GetOffset (), kABCsLen_);
        // at EOF. (IsAtEOF () is not askable here - it peeks, which needs seekability)
        byte past[4];
        EXPECT_TRUE (s.ReadBlocking (span{past}).empty ());
        EXPECT_EQ (s.GetOffset (), kABCsLen_);
    }
}

namespace {
    GTEST_TEST (Foundation_Streams, ToSeekableInputStream_SeekPastEnd_)
    {
        Debug::TraceContextBumper ctx{"ToSeekableInputStream_SeekPastEnd_"};
        // Seeking beyond the end cannot be satisfied - the wrapper can only cache what upstream
        // actually has - so it must report that, NOT spin forever waiting for data that will
        // never arrive.
        stringstream           backing{kABCs_};
        InputStream::Ptr<byte> s = ToSeekableInputStream::New<byte> (InputStreamFromStdIStream::New<byte> (backing, eNotSeekable));
        EXPECT_TRUE (s.IsSeekable ());
        EXPECT_ANY_THROW (s.Seek (kABCsLen_ * 10));
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
