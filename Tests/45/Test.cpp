/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
//  TEST    Foundation::Streams
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <sstream>

#include    "Stroika/Foundation/Streams/MemoryStream.h"
#include    "Stroika/Foundation/Streams/iostream/OutputStreamFromStdOStream.h"
#include    "Stroika/Foundation/Streams/OutputStream.h"
#include    "Stroika/Foundation/Streams/TextReader.h"

#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Streams::iostream;

using   Memory::Byte;



namespace   {
    namespace   BasicBinaryInputStream_ {

        void    TestBasicConstruction_ ()
        {
            {
                MemoryStream<Byte>  s (nullptr, nullptr);
                VerifyTestResult (not s.empty ());
                VerifyTestResult (s.IsSeekable ());
            }
            {
                const char  kData[] =   "1";
                MemoryStream<Byte>  s (reinterpret_cast<const Byte*> (std::begin(kData)), reinterpret_cast<const Byte*> (std::end (kData)));
                VerifyTestResult (not s.empty ());
                VerifyTestResult (s.IsSeekable ());
                Byte    result[100] = { 0 };
                VerifyTestResult (s.Read (std::begin (result), std::end (result)) == 2);
                VerifyTestResult (result[0] == '1');
                VerifyTestResult (result[1] == '\0');
            }
        }


        void    Tests_ ()
        {
            TestBasicConstruction_ ();
        }
    }
}





namespace   {
    namespace   BasicBinaryOutputStream_ {

        void    TestBasicConstruction_ ()
        {
            {
                MemoryStream<Byte>  s;
                VerifyTestResult (not s.empty ());
                VerifyTestResult (s.IsSeekable ());
            }
            {
                MemoryStream<Byte>  s;
                VerifyTestResult (not s.empty ());
                VerifyTestResult (s.IsSeekable ());

                const Byte  kData_[] = { 3, 53, 43, 23, 3 };
                s.Write (std::begin (kData_), std::end (kData_));
                Memory::BLOB    b = s.As<Memory::BLOB> ();
                VerifyTestResult (b.size () == sizeof (kData_));
                VerifyTestResult (b == Memory::BLOB (std::begin (kData_), std::end (kData_)));
            }
        }


        void    Tests_ ()
        {
            TestBasicConstruction_ ();
        }
    }
}



namespace   {
    namespace   BasicBinaryInputOutputStream_ {

        void    TestBasicConstruction_ ()
        {
            {
                MemoryStream<Byte>  s;
                VerifyTestResult (not s.empty ());
                VerifyTestResult (s.IsSeekable ());
                VerifyTestResult (static_cast<InputStream<Byte>> (s).IsSeekable ());
                VerifyTestResult (static_cast<OutputStream<Byte>> (s).IsSeekable ());
            }
            {
                MemoryStream<Byte>  s;
                VerifyTestResult (not s.empty ());

                const Byte  kData_[] = { 3, 53, 43, 23, 3 };
                s.Write (std::begin (kData_), std::end (kData_));
                Memory::BLOB    b = s.As<Memory::BLOB> ();
                VerifyTestResult (b.size () == sizeof (kData_));
                VerifyTestResult (b == Memory::BLOB (std::begin (kData_), std::end (kData_)));
            }
            {
                MemoryStream<Byte>  s;
                VerifyTestResult (s.GetReadOffset () == 0);
                VerifyTestResult (s.GetWriteOffset () == 0);
                const Byte  kData_[] = { 3, 53, 43, 23, 3 };
                s.Write (std::begin (kData_), std::end (kData_));
                VerifyTestResult (s.GetReadOffset () == 0);
                VerifyTestResult (s.GetWriteOffset () == sizeof (kData_));
                Byte bArr[1024];
                Verify (s.Read (std::begin (bArr), std::end (bArr)) == sizeof (kData_));
                VerifyTestResult (s.GetReadOffset () == sizeof (kData_));
                VerifyTestResult (s.GetWriteOffset () == sizeof (kData_));
                VerifyTestResult (Memory::BLOB (std::begin (bArr), std::begin (bArr) + s.GetReadOffset ()) == Memory::BLOB (std::begin (kData_), std::end (kData_)));
            }
        }

        void    Tests_ ()
        {
            TestBasicConstruction_ ();
        }
    }
}




namespace   {
    namespace   BinaryOutputStreamFromOStreamAdapter_ {

        void    T1_ ()
        {
            {
                stringstream s;
                OutputStreamFromStdOStream<Memory::Byte>  so (s);
                const char kData_[] = "ddasdf3294234";
                so.Write (reinterpret_cast<const Byte*> (std::begin (kData_)), reinterpret_cast<const Byte*> (std::begin (kData_)) + strlen (kData_));
                VerifyTestResult (s.str () == kData_);
            }
        }

        void    Tests_ ()
        {
            T1_ ();
        }
    }
}








namespace   {
    namespace   TestBasicTextOutputStream_ {

        namespace  Private_ {
            using   Characters::Character;
            using   Characters::String;
            void    T1_ ()
            {
                MemoryStream<Character> out {};
                out << L"abc";
                VerifyTestResult (out.As<String> () == L"abc");
                out << L"123";
                VerifyTestResult (out.As<String> () == L"abc123");
            }
            void    T2_ ()
            {
                MemoryStream<Character> out {};
                out << L"abc";
                VerifyTestResult (out.As<String> () == L"abc");
                out << L"123";
                VerifyTestResult (out.As<String> () == L"abc123");
                out.SeekWrite (2);
                out.SeekRead (3);   // safe but irrelevant, as we dont read
                out << L"C";
                VerifyTestResult (out.As<String> () == L"abC123");
            }
        }

        void    Tests_ ()
        {
            Private_::T1_ ();
        }
    }
}







namespace   {
    namespace   TextReaderFromIterableAndString {

        void    T1_ ()
        {
            using   Characters::Character;
            using   Characters::String;
            {
                Traversal::Iterable<Character>  s = String (L"This");
                TextReader tr {s};
                VerifyTestResult (tr.ReadAll () == L"This");
            }
            {
                VerifyTestResult ((TextReader { String (L"hello world") } .ReadAll () == L"hello world"));
            }
        }

        void    Tests_ ()
        {
            T1_ ();
        }
    }
}







namespace   {
    void    DoRegressionTests_ ()
    {
        BasicBinaryInputStream_::Tests_ ();
        BasicBinaryOutputStream_::Tests_ ();
        BasicBinaryInputOutputStream_::Tests_ ();
        BinaryOutputStreamFromOStreamAdapter_::Tests_ ();
        TestBasicTextOutputStream_::Tests_ ();
        TextReaderFromIterableAndString::Tests_ ();
    }
}





int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
