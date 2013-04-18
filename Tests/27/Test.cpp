/*
 * Copyright(c) Records For Living, Inc. 2004-2012.  All rights reserved
 */
//  TEST    Foundation::Streams
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <sstream>

#include    "Stroika/Foundation/Streams/BasicBinaryInputStream.h"
#include    "Stroika/Foundation/Streams/BasicBinaryOutputStream.h"
#include    "Stroika/Foundation/Streams/iostream/BinaryInputStreamFromIStreamAdapter.h"
#include    "Stroika/Foundation/Streams/iostream/BinaryOutputStreamFromOStreamAdapter.h"
#include    "Stroika/Foundation/Streams/iostream/TextInputStreamFromIStreamAdapter.h"
#include    "Stroika/Foundation/Streams/ExternallyOwnedMemoryBinaryInputStream.h"

#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;
using   namespace   Stroika::Foundation::Streams::iostream;




namespace   {
    namespace   BasicBinaryInputStream_ {

        void    TestBasicConstruction_ ()
        {
            {
                BasicBinaryInputStream  s (nullptr, nullptr);
                VerifyTestResult (not s.empty ());
                VerifyTestResult (s.IsSeekable ());
            }
            {
                const char  kData[] =   "1";
                BasicBinaryInputStream  s (reinterpret_cast<const Byte*> (std::begin(kData)), reinterpret_cast<const Byte*> (std::end (kData)));
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
                BasicBinaryOutputStream  s;
                VerifyTestResult (not s.empty ());
                VerifyTestResult (s.IsSeekable ());
            }
            {
                BasicBinaryOutputStream  s;
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
    namespace   BinaryOutputStreamFromOStreamAdapter_ {

        void    T1_ ()
        {
            {
                stringstream s;
                BinaryOutputStreamFromOStreamAdapter  so (s);
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
    void    DoRegressionTests_ ()
    {
        BasicBinaryInputStream_::Tests_ ();
        BasicBinaryOutputStream_::Tests_ ();
        BinaryOutputStreamFromOStreamAdapter_::Tests_ ();
    }
}





int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
