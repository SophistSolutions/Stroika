/*
 * Copyright(c) Records For Living, Inc. 2004-2012.  All rights reserved
 */
//  TEST    Foundation::Streams
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Streams/BasicBinaryInputStream.h"
#include    "Stroika/Foundation/Streams/BasicBinaryOutputStream.h"

#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Streams;




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
                BasicBinaryInputStream  s (reinterpret_cast<const Byte*> (StartOfArray(kData)), reinterpret_cast<const Byte*> (EndOfArray(kData)));
                VerifyTestResult (not s.empty ());
                VerifyTestResult (s.IsSeekable ());
                Byte    result[100] = { 0 };
                VerifyTestResult (s.Read (StartOfArray (result), EndOfArray (result)) == 2);
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
                s.Write (StartOfArray (kData_), EndOfArray (kData_));
#if 0
                Memory::BLOB    b = s.As<Memory::BLOB> ();
                VerifyTestResult (b.size () == sizeof (kData_));
                VerifyTestResult (b == Memory::BLOB (StartOfArray (kData_), EndOfArray (kData_));
#endif
            }
        }


                          void    Tests_ ()
        {
            TestBasicConstruction_ ();
        }
    }
}







namespace   {
    void    DoRegressionTests_ ()
    {
        BasicBinaryInputStream_::Tests_ ();
        BasicBinaryOutputStream_::Tests_ ();
    }
}





int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}
