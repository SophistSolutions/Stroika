/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Streams
#include "Stroika/Foundation/StroikaPreComp.h"

#include <sstream>

#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Streams/Copy.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/OutputStream.h"
#include "Stroika/Foundation/Streams/SharedMemoryStream.h"
#include "Stroika/Foundation/Streams/TextReader.h"
#include "Stroika/Foundation/Streams/iostream/OutputStreamFromStdOStream.h"

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Streams;
using namespace Stroika::Foundation::Streams::iostream;

using std::byte;

namespace {
    namespace BasicBinaryInputStream_ {

        void TestBasicConstruction_ ()
        {
            {
                MemoryStream<byte>::Ptr s = MemoryStream<byte>::New (nullptr, nullptr);
                VerifyTestResult (s != nullptr);
                VerifyTestResult (s.IsSeekable ());
            }
            {
                const char              kData[] = "1";
                MemoryStream<byte>::Ptr s       = MemoryStream<byte>::New (reinterpret_cast<const byte*> (std::begin (kData)), reinterpret_cast<const byte*> (std::end (kData)));
                VerifyTestResult (s != nullptr);
                VerifyTestResult (s.IsSeekable ());
                byte result[100] = {byte{0}};
                VerifyTestResult (s.Read (std::begin (result), std::end (result)) == 2);
                VerifyTestResult (to_integer<char> (result[0]) == '1');
                VerifyTestResult (to_integer<char> (result[1]) == '\0');
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
                MemoryStream<byte>::Ptr s = MemoryStream<byte>::New ();
                VerifyTestResult (s != nullptr);
                VerifyTestResult (s.IsSeekable ());
            }
            {
                MemoryStream<byte>::Ptr s = MemoryStream<byte>::New ();
                VerifyTestResult (s != nullptr);
                VerifyTestResult (s.IsSeekable ());

                constexpr byte kData_[] = {byte{3}, byte{53}, byte{43}, byte{23}, byte{3}};
                s.Write (std::begin (kData_), std::end (kData_));
                Memory::BLOB b = s.As<Memory::BLOB> ();
                VerifyTestResult (b.size () == sizeof (kData_));
                VerifyTestResult (b == Memory::BLOB (std::begin (kData_), std::end (kData_)));
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
                MemoryStream<byte>::Ptr s = MemoryStream<byte>::New ();
                VerifyTestResult (s != nullptr);
                VerifyTestResult (s.IsSeekable ());
                VerifyTestResult (static_cast<InputStream<byte>::Ptr> (s).IsSeekable ());
                VerifyTestResult (static_cast<OutputStream<byte>::Ptr> (s).IsSeekable ());
            }
            {
                MemoryStream<byte>::Ptr s = MemoryStream<byte>::New ();
                VerifyTestResult (s != nullptr);

                const uint8_t kData_[] = {3, 53, 43, 23, 3};
                s.Write (std::begin (kData_), std::end (kData_));
                Memory::BLOB b = s.As<Memory::BLOB> ();
                VerifyTestResult (b.size () == sizeof (kData_));
                VerifyTestResult (b == Memory::BLOB (std::begin (kData_), std::end (kData_)));
            }
            {
                MemoryStream<byte>::Ptr s = MemoryStream<byte>::New ();
                VerifyTestResult (s.GetReadOffset () == 0);
                VerifyTestResult (s.GetWriteOffset () == 0);
                const uint8_t kData_[] = {3, 53, 43, 23, 3};
                s.Write (std::begin (kData_), std::end (kData_));
                VerifyTestResult (s.GetReadOffset () == 0);
                VerifyTestResult (s.GetWriteOffset () == sizeof (kData_));
                byte bArr[1024];
                Verify (s.Read (std::begin (bArr), std::end (bArr)) == sizeof (kData_));
                VerifyTestResult (s.GetReadOffset () == sizeof (kData_));
                VerifyTestResult (s.GetWriteOffset () == sizeof (kData_));
                VerifyTestResult (Memory::BLOB (std::begin (bArr), std::begin (bArr) + s.GetReadOffset ()) == Memory::BLOB (std::begin (kData_), std::end (kData_)));
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
                stringstream                                  s;
                OutputStreamFromStdOStream<Memory::byte>::Ptr so       = OutputStreamFromStdOStream<Memory::byte>::New (s);
                const char                                    kData_[] = "ddasdf3294234";
                so.Write (reinterpret_cast<const byte*> (std::begin (kData_)), reinterpret_cast<const byte*> (std::begin (kData_)) + strlen (kData_));
                VerifyTestResult (s.str () == kData_);
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
                MemoryStream<Character>::Ptr out = MemoryStream<Character>::New ();
                out << L"abc";
                VerifyTestResult (out.As<String> () == L"abc");
                out << L"123";
                VerifyTestResult (out.As<String> () == L"abc123");
            }
            void T2_ ()
            {
                MemoryStream<Character>::Ptr out = MemoryStream<Character>::New ();
                out << L"abc";
                VerifyTestResult (out.As<String> () == L"abc");
                out << L"123";
                VerifyTestResult (out.As<String> () == L"abc123");
                out.SeekWrite (2);
                out.SeekRead (3); // safe but irrelevant, as we don't read
                out << L"C";
                VerifyTestResult (out.As<String> () == L"abC123");
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
                Traversal::Iterable<Character> s  = String (L"This");
                TextReader::Ptr                tr = TextReader::New (s);
                VerifyTestResult (tr.ReadAll () == L"This");
            }
            {
                VerifyTestResult ((TextReader::New (String{L"hello world"}).ReadAll () == L"hello world"));
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
                Memory::BLOB    s  = Memory::BLOB::Raw (u8"Testing 1, 2, 3");
                TextReader::Ptr tr = TextReader::New (s);
                VerifyTestResult (tr.ReadAll () == L"Testing 1, 2, 3");
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
                SharedMemoryStream<unsigned int>::Ptr pipe = SharedMemoryStream<unsigned int>::New ();
                unsigned                              sum{};
                static constexpr unsigned int         kStartWith{1};
                static constexpr unsigned int         kUpToInclusive_{1000};
                Thread::Ptr                           consumer = Thread::New ([&]() {
                    while (auto o = pipe.Read ()) {
                        sum += *o;
                    }
                },
                                                    Thread::eAutoStart);
                Thread::Ptr                           producer = Thread::New ([&]() {
                    for (unsigned int i = kStartWith; i <= kUpToInclusive_; i++) {
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
                MemoryStream<Character>::Ptr in = MemoryStream<Character>::New ();
                in << L"abc";
                VerifyTestResult (in.As<String> () == L"abc");

                MemoryStream<Character>::Ptr out = MemoryStream<Character>::New ();
                Streams::CopyAll<Character> (in, out);
                VerifyTestResult (out.As<String> () == L"abc");
            }
        }

        void Tests_ ()
        {
            Private_::T1_ ();
        }
    }
}

namespace {
    void DoRegressionTests_ ()
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

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
