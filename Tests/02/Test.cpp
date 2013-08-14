/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
//  TEST    Foundation::Characters::Strings
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <iostream>

#include    "Stroika/Foundation/Characters/CString/Utilities.h"
#include    "Stroika/Foundation/Characters/Format.h"
#include    "Stroika/Foundation/Characters/RegularExpression.h"
#include    "Stroika/Foundation/Characters/String.h"
#include    "Stroika/Foundation/Characters/Concrete/String_BufferedArray.h"
#include    "Stroika/Foundation/Characters/Concrete/String_ExternalMemoryOwnership_StackLifetime_ReadOnly.h"
#include    "Stroika/Foundation/Characters/Concrete/String_ExternalMemoryOwnership_StackLifetime_ReadWrite.h"
#include    "Stroika/Foundation/Characters/Concrete/String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly.h"
#include    "Stroika/Foundation/Containers/Common.h"
#include    "Stroika/Foundation/Configuration/Locale.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Memory/SmallStackBuffer.h"
#include    "Stroika/Foundation/Time/Realtime.h"

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Characters::Concrete;


#define qPrintTimings   0



/**
 * TODO:
 *
 *      (o) Must write ASAP a performance comparison suite for String class as part of string automated test.
 *          Print a table of results (X/Y – wstring versus String (maybe versus other string types?)).
 *          Compare inserting into start of vector<STRINGTYPE> - to test copying.
 */







namespace   {
    unsigned ipow (unsigned n, unsigned toPow)
    {
        // quick hack since pow didnt seem to do what I want - just blindly
        // multiply and dont worry about overflow...
        unsigned result = 1;
        while (toPow-- != 0) {
            result *= n;
        }
        return (result);
    }
}

namespace   {
    namespace   Test2Helpers_ {
#if qDebug
        const   int kLoopEnd = 1000;
#else
        const   int kLoopEnd = 2000;
#endif

        void    StressTest1_ (String big)
        {
            for (int j = 1; j <= kLoopEnd / 50; j++) {
                String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly a (L"a");
                for (int i = 0; i <= kLoopEnd; i++) {
                    big += a;
                    VerifyTestResult ((big.GetLength () - 1) == i);
                    VerifyTestResult (big[i] == 'a');
                }
                big.SetLength (0);
            }

            String  s1  =   L"test strings";
            for (int i = 1; i <= kLoopEnd; i++) {
                big += s1;
                VerifyTestResult (big.GetLength () == s1.GetLength () * i);
            }
        }
        void    StressTest2_ (String big)
        {
            String  s1  =   L"test strings";
            for (int i = 1; i <= kLoopEnd; i++) {
                big = big + s1;
                VerifyTestResult (big.GetLength () == s1.GetLength () * i);
#if 0
                for (int j = 0; j < big.GetLength (); ++j) {
                    Character c = big[j];
                    int breahere = 1;
                }
#endif
            }
        }
        void    StressTestStrings ()
        {
#if     qPrintTimings
            cout <<  "Stress testing strings..." << endl;
            Time::DurationSecondsType   t   =   Time::GetTickCount ();
#endif

            {
                String s (L"");
                StressTest1_ (s);
            }

#if     qPrintTimings
            t = Time::GetTickCount () - t;
            cout << "finished Stress testing strings += ... time elapsed = " << t << endl;
            t   =   Time::GetTickCount ();
#endif

            {
                String s (L"");
                StressTest2_ (s);
            }

#if     qPrintTimings
            t = Time::GetTickCount () - t;
            cout << "finished Stress testing strings + ... time elapsed = " << t << endl;
#endif
        }
        void    StressTestBufferedStrings ()
        {
#if     qPrintTimings
            cout << "Stress testing buffered strings..." << endl;
            Time::DurationSecondsType t = Time::GetTickCount ();
#endif

            {
                String_BufferedArray s (L"");
                StressTest1_ (s);
            }

#if     qPrintTimings
            t = Time::GetTickCount () - t;
            cout << "finished stress testing buffered strings  += ... time elapsed = " << t << endl;
            t = Time::GetTickCount ();
#endif

            {
                String_BufferedArray s (L"");
                StressTest2_ (s);
            }

#if     qPrintTimings
            t = Time::GetTickCount () - t;
            cout << "finished stress testing buffered strings + ... at " << t << endl;
#endif
        }
    }
}










namespace   {
    void    Test2_Helper_ (String& s1, String& s2)
    {
        VerifyTestResult (s1.GetLength () == 12);
        VerifyTestResult (String (s1).GetLength () == 12);
        VerifyTestResult (s1 == s2);
        VerifyTestResult (! (s1 != s2));
        VerifyTestResult (s1 + s1 == s2 + s2);
        VerifyTestResult ((s1 + s1).GetLength () == s1.GetLength () * 2);
        VerifyTestResult (s1[2] == 's');
        VerifyTestResult ('s' == s1[2]);
        VerifyTestResult (s1.GetLength () == 12);

        String s3;
        s3 += s1;
        s3 += s2;

        s1 += L"\n";
        VerifyTestResult (s1.GetLength () == 13);
    }

    void    Test1_ ()
    {
        /*
         * Some simple tests to start off with.
         */
        {
            VerifyTestResult (String (L"a").length () == 1);
            VerifyTestResult (String (String (L"fred") + String (L"joe")).GetLength () == 7);

            VerifyTestResult (String (L"fred") + String (L"joe") == String (L"fredjoe"));
            {
                String  s1  =   String_BufferedArray (L"test strings");
                String  s2  =   String_BufferedArray (L"test strings");
                Test2_Helper_ (s1, s2);
            }

            {
                String_BufferedArray s1 (L"test strings");
                String_BufferedArray s2 (L"test strings");

                VerifyTestResult (Character ('a') == 'a');

                Test2_Helper_ (s1, s2);
            }

            {
                String  s1  =   String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (L"test strings");
                String  s2  =   String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly (L"test strings");
                Test2_Helper_ (s1, s2);
            }
        }
    }

    void    Test2_ ()
    {
        Test2Helpers_::StressTestStrings ();
        Test2Helpers_::StressTestBufferedStrings ();
    }

    void    Test3_ ()
    {
        String  t1;
        String  t2  =   t1;
        String  t3  =   L"a";
        String  t4  =   L"a";

        VerifyTestResult (t1 == L"");
        VerifyTestResult (t1 == String ());
        VerifyTestResult (t1 == String (L""));
        VerifyTestResult (t1 == t2);
        VerifyTestResult (t3 == L"a");
        VerifyTestResult (t3 == String(L"a"));
        VerifyTestResult (t4 == L"a");
        t1 = t1;
        VerifyTestResult (t1 == L"");

        t1 += 'F';
        t1 += 'r';
        t1 += 'e';
        t1 += 'd';
        t1 += L" Flintstone";
        VerifyTestResult (t1 == L"Fred Flintstone");
        VerifyTestResult (L"Fred Flintstone" == t1);
        VerifyTestResult (String (L"Fred Flintstone") == t1);
        VerifyTestResult (t1 == String (L"Fred Flintstone"));
        VerifyTestResult (t2 != L"Fred Flintstone");
        VerifyTestResult (L"Fred Flintstone" != t2);
        VerifyTestResult (String (L"Fred Flintstone") != t2);
        VerifyTestResult (t2 != String (L"Fred Flintstone"));

        VerifyTestResult (t1.GetLength () == 15);
        t1.SetLength (20);
        VerifyTestResult (t1.GetLength () == 20);
        t1.SetLength (4);
        VerifyTestResult (t1.GetLength () == 4);
        VerifyTestResult (t1 == L"Fred");

        VerifyTestResult (t1[0] == 'F');
        VerifyTestResult (t1[1] == 'r');
        VerifyTestResult (t1[2] == 'e');
        VerifyTestResult (t1[3] == 'd');

        VerifyTestResult (t1[0] == 'F');
        VerifyTestResult (t1[1] == 'r');
        VerifyTestResult (t1[2] == 'e');
        VerifyTestResult (t1[3] == 'd');

        String  a[10];
        VerifyTestResult (a[2] == L"");
        a [3] = L"Fred";
        VerifyTestResult (a[3] == L"Fred");
        VerifyTestResult (a[2] != L"Fred");
    }

    void    Test4_ ()
    {
        const   wchar_t frobaz[]    =   L"abc";

        String  t1;
        String  t3  =   L"a";
        String  t5  =   String (frobaz);
        String* t6  =   new String (L"xyz");
        delete (t6);

        t5 = t1;
        t1 = t5;
        t1 = t1;
        VerifyTestResult (t1 == L"");
        VerifyTestResult (t5 == L"");

        t1 += 'F';
        t1 += 'r';
        t1 += 'e';
        t1 += 'd';
        t1 += L" Flintstone";
        VerifyTestResult (t1 == L"Fred Flintstone");
        t5 = t1.SubString (5, 5 + 10);
        VerifyTestResult (t5 == L"Flintstone");
        VerifyTestResult (t5.Find (L"STONE") == String::kBadIndex);
        VerifyTestResult (not t5.Contains (L"SToNE"));
        VerifyTestResult (t5.Find (L"STONE", CompareOptions::eCaseInsensitive) == 5);
        VerifyTestResult (t5.Contains (L"SToNE", CompareOptions::eCaseInsensitive));

        t1.SetLength (20);
        t1.SetLength (4);

        t5 = t1;
        t5.SetCharAt ('f', 0);
        t5.SetCharAt ('R', 1);
        t5.SetCharAt ('E', 2);
        t5.SetCharAt ('D', 3);

        VerifyTestResult (t5[0] == 'f');
        VerifyTestResult (t5[1] == 'R');
        VerifyTestResult (t5[2] == 'E');
        VerifyTestResult (t5[3] == 'D');
        VerifyTestResult (t5.Find ('f') == 0);
        VerifyTestResult (t5.Find (L"f") == 0);
        VerifyTestResult (t5.Find (L"fR") == 0);
        VerifyTestResult (t5.Find (L"fRE") == 0);
        VerifyTestResult (t5.Find (L"fRED") == 0);
        VerifyTestResult (t5.Find (L"fRD") == String::kBadIndex);
        VerifyTestResult (t5.Find ('R') == 1);
        VerifyTestResult (t5.Find ('E') == 2);
        VerifyTestResult (t5.Find ('D') == 3);
        VerifyTestResult (t5.Find (L"D") == 3);

        VerifyTestResult (t5.RFind ('f') == 0);
        VerifyTestResult (t5.RFind ('R') == 1);
        VerifyTestResult (t5.RFind ('E') == 2);
        VerifyTestResult (t5.RFind ('D') == 3);
        VerifyTestResult (t5.RFind (L"D") == 3);
        VerifyTestResult (t5.RFind (L"ED") == 2);
        VerifyTestResult (t5.RFind (L"RED") == 1);
        VerifyTestResult (t5.RFind (L"fRED") == 0);
        VerifyTestResult (t5.RFind (L"fr") == String::kBadIndex);
        VerifyTestResult (t5.RFind (L"f") == 0);

        t5.SetCharAt ('D', 0);
        t5.SetCharAt ('D', 1);
        t5.SetCharAt ('D', 2);
        t5.SetCharAt ('D', 3);
        VerifyTestResult (t5.Find ('D') == 0);
        VerifyTestResult (t5.Find (L"D") == 0);
        VerifyTestResult (t5.RFind ('D') == 3);
        VerifyTestResult (t5.RFind (L"D") == 3);

        VerifyTestResult (t5.Find ('f') == String::kBadIndex);
        VerifyTestResult (t5.Find (L"f") == String::kBadIndex);
        VerifyTestResult (t5.RFind ('f') == String::kBadIndex);
        VerifyTestResult (t5.RFind (L"f") == String::kBadIndex);

        VerifyTestResult (t5[0] == 'D');
        VerifyTestResult (t5[1] == 'D');
        VerifyTestResult (t5[2] == 'D');
        VerifyTestResult (t5[3] == 'D');
    }

    void    Test5_ ()
    {
        String arr [100];
        arr [3] = L"fred";
        VerifyTestResult (arr[3] == L"fred");
        String* l   =   new String [100];
        l[3] = L"FRED";
        VerifyTestResult (l[3] == L"FRED");
        VerifyTestResult (l[99] == L"");
        delete[] (l);
        size_t nSlots = 100;
        l   =   new String [size_t (nSlots)];
        delete[] (l);
    }

    template    <typename   STRING>
    STRING  Test6_Helper_ (const STRING& a, int depth)
    {
        STRING  b = a;
        b += a;
        if (depth > 0) {
            b = Test6_Helper_<STRING> (b, depth - 1) + Test6_Helper_<STRING> (b, depth - 1);
        }
        return (b);
    }
    template    <typename   STRING>
    void    Test6_Helper_ (const char* testMessage)
    {
#if     qPrintTimings
        const   int kRecurseDepth = 10;
#else
        const   int kRecurseDepth = 8;
#endif
        STRING  testString = L"some dump test";
#if     qPrintTimings
        cout << "\tTYPE=" << testMessage << ": Recursive build test with depth " << kRecurseDepth << endl;
        Time::DurationSecondsType t = Time::GetTickCount ();
#endif

        STRING s = Test6_Helper_<STRING> (testString, kRecurseDepth);   // returns length 114688 for depth 6
        VerifyTestResult (s.length () ==  (ipow (4, kRecurseDepth) * 2 * testString.length ()));

#if     qPrintTimings
        t = Time::GetTickCount () - t;
        cout << "\tfinished Recursive build test. Time elapsed = " << t << " length = " << s.length () << endl;
#endif
    }
    void    Test6_ ()
    {
        Test6_Helper_<String> ("Characters::String");
        Test6_Helper_<wstring> ("std::wstring");
    }
    void    Test7_ ()
    {
        VerifyTestResult (String (L"1") <= String (L"1"));
        VerifyTestResult (String (L"1") <= String (L"10"));
        VerifyTestResult (not (String (L"1") > String (L"10")));
        VerifyTestResult (not (String (L"1") >= String (L"10")));
        VerifyTestResult (String (L"1") < String (L"10"));

        VerifyTestResult (String (L"20") > String (L"11"));
        VerifyTestResult (String (L"20") >= String (L"11"));
        VerifyTestResult (not (String (L"20") < String (L"11")));
        VerifyTestResult (not (String (L"20") <= String (L"11")));
        VerifyTestResult (String (L"11") < String (L"20"));
        VerifyTestResult (String (L"11") <= String (L"20"));
        VerifyTestResult (not (String (L"11") > String (L"20")));
        VerifyTestResult (not (String (L"11") >= String (L"20")));

        VerifyTestResult (String (L"aac") > String (L"aab"));
        VerifyTestResult (String (L"aac") >= String (L"aab"));
        VerifyTestResult (not (String (L"aac") < String (L"aab")));
        VerifyTestResult (not (String (L"aac") <= String (L"aab")));

        VerifyTestResult (String (L"apple") < String (L"apples"));
        VerifyTestResult (String (L"apple") <= String (L"apples"));
        VerifyTestResult (not (String (L"apple") > String (L"apples")));
        VerifyTestResult (not (String (L"apple") >= String (L"apples")));
    }

    void    Test8_ReadOnlyStrings_ ()
    {
        String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly s (L"fred");
        VerifyTestResult (s[0] == 'f');
        s.SetLength (3);
        VerifyTestResult (s[0] == 'f');
        VerifyTestResult (s.GetLength () == 3);
        s += L"x";
        VerifyTestResult (s.GetLength () == 4);
        VerifyTestResult (s[3] == 'x');
        VerifyTestResult (s == L"frex");
        s.InsertAt ('x', 2);
        VerifyTestResult (s == L"frxex");
        {
            wchar_t kZero[] =   L"";
            s.InsertAt (std::begin (kZero), std::begin (kZero), 0);
            VerifyTestResult (s == L"frxex");
            s.InsertAt (std::begin (kZero), std::begin (kZero), 1);
            VerifyTestResult (s == L"frxex");
            s.InsertAt (std::begin (kZero), std::begin (kZero), 5);
            VerifyTestResult (s == L"frxex");
        }
    }

    void    Test8_ExternalMemoryOwnershipStrings_ ()
    {
        String_ExternalMemoryOwnership_ApplicationLifetime_ReadOnly s (L"fred");
        VerifyTestResult (s[0] == 'f');
        s.SetLength (3);
        VerifyTestResult (s[0] == 'f');
        VerifyTestResult (s.GetLength () == 3);
        s += L"x";
        VerifyTestResult (s.GetLength () == 4);
        VerifyTestResult (s[3] == 'x');
        VerifyTestResult (s == L"frex");
        s.InsertAt ('x', 2);
        VerifyTestResult (s == L"frxex");
    }

    namespace   {
        namespace   Test9Support {
            template    <typename   STRING>
            void    DoTest1 (STRING s)
            {
                STRING  t1  =   s;
                for (size_t i = 0; i < 100; ++i) {
                    t1 += L"X";
                }
                STRING  t2  =   t1;
                if (t1 != t2) {
                    VerifyTestResult (false);
                }
            }
        }
    }
    void    Test9_StringVersusStdCString_ ()
    {
        // EMBELLISH THIS MORE ONCE WE HAVE TIMING SUPPORT WORKING - SO WE CNA COMPARE PERFORMANCE - AND COME UP WITH MORE REASONABLE TESTS
        //
        //      -- LGP 2011-09-01
        Test9Support::DoTest1<String> (L"Hello");
        Test9Support::DoTest1<std::wstring> (L"Hello");
    }
    void    Test10_ConvertToFromSTDStrings_ ()
    {
        const   wstring kT1 =   L"abcdefh124123985213129314234";
        String  t1  =   kT1;
        VerifyTestResult (t1.As<wstring> () == kT1);
        VerifyTestResult (t1 == kT1);
    }
}


namespace   {
#if     !qCompilerAndStdLib_Supports_lambda_default_argument || !qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
    static  bool    Test11_TRIM_ISALPHA (Character c)               {       return c.IsAlphabetic ();       }
#endif
    void    Test11_Trim_ ()
    {
        const   String  kT1 =   L"  abc";
        VerifyTestResult (kT1.RTrim () == kT1);
        VerifyTestResult (kT1.LTrim () == kT1.Trim ());
        VerifyTestResult (kT1.Trim () == L"abc");

#if     qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
        VerifyTestResult (kT1.Trim ([] (Character c) -> bool { return c.IsAlphabetic (); }) == L"  ");
#else
        VerifyTestResult (kT1.Trim (Test11_TRIM_ISALPHA) == L"  ");
#endif
    }
}



namespace   {
    void    Test12_CodePageConverter_ ()
    {
        wstring w = L"<PHRMode";
        using namespace Characters;
        using namespace Memory;
        CodePageConverter   cpc (kCodePage_UTF8, CodePageConverter::eHandleBOM);
        size_t              sz  =       cpc.MapFromUNICODE_QuickComputeOutBufSize (w.c_str (), w.length ());
        SmallStackBuffer<char>  buf (sz + 1);
        size_t  charCnt =   sz;
        cpc.MapFromUNICODE (w.c_str (), w.length (), buf, &charCnt);
        VerifyTestResult (string (buf.begin (), buf.begin () + charCnt) == "﻿<PHRMode");
    }
}





namespace   {
    void    Test13_ToLowerUpper_ ()
    {
        String w = L"Lewis";
        VerifyTestResult (w.ToLowerCase () == L"lewis");
        VerifyTestResult (w.ToUpperCase () == L"LEWIS");
        VerifyTestResult (w == L"Lewis");
    }
}



namespace   {
    void    Test14_String_StackLifetimeReadOnly_ ()
    {
        wchar_t buf[1024]   =   L"fred";
        {
            String_ExternalMemoryOwnership_StackLifetime_ReadOnly s (buf);
            VerifyTestResult (s[0] == 'f');
            s.SetLength (3);
            VerifyTestResult (s[0] == 'f');
            VerifyTestResult (s.GetLength () == 3);
            s += L"x";
            VerifyTestResult (s.GetLength () == 4);
            VerifyTestResult (s[3] == 'x');
        }
        VerifyTestResult (::wcscmp (buf, L"fred") == 0);
    }
    void    Test14_String_StackLifetimeReadWrite_ ()
    {
        wchar_t buf[1024]   =   L"fred";
        {
            String_ExternalMemoryOwnership_StackLifetime_ReadWrite s (buf);
            VerifyTestResult (s[0] == 'f');
            s.SetLength (3);
            VerifyTestResult (s[0] == 'f');
            VerifyTestResult (s.GetLength () == 3);
            s += L"x";
            VerifyTestResult (s.GetLength () == 4);
            VerifyTestResult (s[3] == 'x');
        }
        VerifyTestResult (::wcscmp (buf, L"fred") == 0);
    }

}




namespace   {
#if     !qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
    static  bool    Test15_STRIPALLTEST_ (Character c)              {       return c.IsWhitespace ();       }
#endif
    void    Test15_StripAll_ ()
    {
        String w = L"Le wis";
#if     qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
        VerifyTestResult (w.StripAll ([](Character c) -> bool { return c.IsWhitespace (); }) == L"Lewis");
#else
        VerifyTestResult (w.StripAll (Test15_STRIPALLTEST_) == L"Lewis");
#endif

        w = L"This is a very good test    ";
#if     qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
        VerifyTestResult (w.StripAll ([](Character c) -> bool { return c.IsWhitespace (); }) == L"Thisisaverygoodtest");
#else
        VerifyTestResult (w.StripAll (Test15_STRIPALLTEST_) == L"Thisisaverygoodtest");
#endif
    }
}




namespace   {
    void    Test16_Format_ ()
    {
        VerifyTestResult (Format ("%d", 123) == "123");
        VerifyTestResult (Format ("%s", "123") == "123");

        // SUBTLE - this would FAIL with vsnprintf on gcc -- See docs in Header for
        // Format string
        VerifyTestResult (Format (L"%s", L"123") == L"123");

        VerifyTestResult (Format (L"%20s", L"123") == L"                 123");
        VerifyTestResult (Format (L"%.20s", L"123") == L"123");

        for (int i = 1; i < 1000; ++i) {
            String  format  =   Format (L"%%%ds", i);
            VerifyTestResult (Format (format.As<wstring> ().c_str (), L"x").length () == i);
        }
    }
}

namespace   {
    void    Test17_Find_ ()
    {
        VerifyTestResult (String (L"abc").Find (L"b") == 1);
        VerifyTestResult (String (L"abc").Find (L"x") == String::kBadIndex);
        VerifyTestResult (String (L"abc").Find (L"b", 2) == String::kBadIndex);
    }
    void    Test17_RegExp_Search_ ()
    {
#if     qCompilerAndStdLib_Supports_regex
        {
            RegularExpression   regExp (L"abc");
            String              testStr2Search  =   String (L"abc");
            VerifyTestResult (testStr2Search.FindEach (regExp).size () == 1);
            VerifyTestResult ((testStr2Search.FindEach (regExp)[0] == pair<size_t, size_t> (0, 3)));
        } {
            // Test replace crlfs
            String  stringWithCRLFs =   L"abc\r\ndef\r\n";
            String  replaced        =   stringWithCRLFs.ReplaceAll (RegularExpression (L"[\r\n]*"), L"");
            VerifyTestResult (replaced == L"abcdef");
        }
#endif
#if 0
// not sure why this didn't work! -
        {
            String  abc     =   String (L"abc");
            String  abcabc  =   String (L"abc abc");
            VerifyTestResult (abcabc.Search (abc).size () == 2);
            VerifyTestResult ((abcabc.Search (abc)[0] == pair<size_t, size_t> (0, abc.length ())));
            VerifyTestResult ((abcabc.Search (abc)[1] == pair<size_t, size_t> (3, abc.length ())));
        }
#endif
    }
    void    Test17_RegExp_ ()
    {
        Test17_Find_ ();
        Test17_RegExp_Search_ ();
#if     qCompilerAndStdLib_Supports_regex
        VerifyTestResult ((String (L"Hello world").Find (RegularExpression (L"ello", RegularExpression::SyntaxType::eECMAScript)) == pair<size_t, size_t> (1, 5)));
        vector<String>  r   =   String (L"<h2>Egg prices</h2>").FindEachString (RegularExpression (L"<h(.)>([^<]+)", RegularExpression::SyntaxType::eECMAScript));
        VerifyTestResult (r.size () == 3 and r[1] == L"2" and r[2] == L"Egg prices");
        VerifyTestResult (String (L"Hello world").ReplaceAll (RegularExpression (L"world"), L"Planet") == L"Hello Planet");
#endif
    }
}





namespace   {
    void    Test18_Compare_ ()
    {
        const   String  kHELLOWorld =   String (L"Hello world");
        VerifyTestResult (kHELLOWorld.Compare (kHELLOWorld, CompareOptions::eWithCase) == 0);
        VerifyTestResult (kHELLOWorld.Compare (String (L"Hello world"), CompareOptions::eWithCase) == 0);

        VerifyTestResult (kHELLOWorld.Compare (kHELLOWorld.ToLowerCase (), CompareOptions::eWithCase) < 0);
        VerifyTestResult (kHELLOWorld.Compare (kHELLOWorld.ToLowerCase (), CompareOptions::eCaseInsensitive) == 0);
        VerifyTestResult (String (L"fred").Compare (L"fredy", CompareOptions::eCaseInsensitive) < 0);
        VerifyTestResult (String (L"fred").Compare (L"Fredy", CompareOptions::eCaseInsensitive) < 0);
        VerifyTestResult (String (L"Fred").Compare (L"fredy", CompareOptions::eCaseInsensitive) < 0);
        VerifyTestResult (String (L"fred").Compare (L"fredy", CompareOptions::eWithCase) < 0);
        VerifyTestResult (String (L"fred").Compare (L"Fredy", CompareOptions::eWithCase) > 0);
        VerifyTestResult (String (L"Fred").Compare (L"fredy", CompareOptions::eWithCase) < 0);
    }
}





namespace   {
    void    Test19_ConstCharStar_ ()
    {
        VerifyTestResult (wcscmp (String (L"fred").c_str (), L"fred") == 0);
        {
            String  tmp =   L"333";
            VerifyTestResult (wcscmp (tmp.c_str (), L"333") == 0);
            tmp = L"Barny";
            VerifyTestResult (wcscmp (tmp.c_str (), L"Barny") == 0);
            tmp.SetCharAt ('c', 2);
            VerifyTestResult (wcscmp (tmp.c_str (), L"Bacny") == 0);
            String  bumpRefCnt  =   tmp;
            tmp.SetCharAt ('d', 2);
            VerifyTestResult (wcscmp (tmp.c_str (), L"Badny") == 0);
        }
    }
}


namespace   {
    void    Test20_CStringHelpers_ ()
    {
        VerifyTestResult (CString::Length ("hi") == 2);
        VerifyTestResult (CString::Length (L"hi") == 2);

        {
            char    buf[3] = {'1', '1', '1' };
            CString::Copy (buf, NEltsOf (buf), "3");
            VerifyTestResult (::strcmp (buf, "3") == 0);
        }
        {
            wchar_t buf[3] = {'1', '1', '1' };
            CString::Copy (buf, NEltsOf (buf), L"3");
            VerifyTestResult (::wcscmp (buf, L"3") == 0);
        }


        {
            char    buf[3] = {'1', '1', '1' };
            CString::Copy (buf, NEltsOf (buf), "12345");
            VerifyTestResult (::strcmp (buf, "12") == 0);
        }
        {
            wchar_t buf[3] = {'1', '1', '1' };
            CString::Copy (buf, NEltsOf (buf), L"12345");
            VerifyTestResult (::wcscmp (buf, L"12") == 0);
        }

    }
}

namespace {
    void    Test21_StringToIntEtc_ ()
    {
        {
            VerifyTestResult (String2Int ("-3") == -3);
            VerifyTestResult (String2Int ("3") == 3);
            VerifyTestResult (String2Int (wstring (L"3")) == 3);
            VerifyTestResult (String2Int (String (L"3")) == 3);
        }
        {
            VerifyTestResult (String2Int ("") == 0);
            VerifyTestResult (String2Int (L"") == 0);
            VerifyTestResult (String2Int (wstring (L"")) == 0);
            VerifyTestResult (String2Int (String ()) == 0);
            VerifyTestResult (String2Int ("     ") == 0);
        }
        {
            VerifyTestResult (HexString2Int ("") == 0);
            VerifyTestResult (HexString2Int (L"") == 0);
            VerifyTestResult (HexString2Int (wstring (L"")) == 0);
            VerifyTestResult (HexString2Int (String ()) == 0);
            VerifyTestResult (HexString2Int ("     ") == 0);
        }
        {
            VerifyTestResult (std::isnan (String2Float (String ())));
            VerifyTestResult (std::isnan (String2Float (string ())));
            VerifyTestResult (std::isnan (String2Float (wstring ())));
            VerifyTestResult (std::isnan (String2Float ("")));
            VerifyTestResult (std::isnan (String2Float (wstring (L""))));
            VerifyTestResult (std::isnan (String2Float (String ())));
            VerifyTestResult (std::isnan (String2Float ("     ")));
        }
        {
            VerifyTestResult (Math::NearlyEquals (String2Float ("-44.4"), -44.4));
            VerifyTestResult (Math::NearlyEquals (String2Float (L"-44.4"), -44.4));
            VerifyTestResult (Math::NearlyEquals (String2Float (String (L"44.4333")), 44.4333));
        }
        auto runLocaleIndepTest = [] () {
            VerifyTestResult (Float2String (3000.5) == L"3000.5");
            VerifyTestResult (Float2String (30000.5) == L"30000.5");
        };
        {
            // Verify change of locale has no effect on results
            locale  prevLocale  =   locale::global (locale ("C"));
            runLocaleIndepTest ();
            locale::global (prevLocale);
        }
        {
            // Verify change of locale has no effect on results
            locale  prevLocale  =   locale::global (Configuration::FindNamedLocale (L"en", L"us"));
            runLocaleIndepTest ();
            locale::global (prevLocale);
        }
    }
}


namespace {
    void    Test22_StartsWithEndsWithMatch_ ()
    {
        VerifyTestResult (String (L"abc").Match (RegularExpression (L"abc")));
        VerifyTestResult (not (String (L"abc").Match (RegularExpression (L"bc"))));
        VerifyTestResult (String (L"abc").Match (RegularExpression (L".*bc")));
        VerifyTestResult (not String (L"abc").Match (RegularExpression (L"b.*c")));
        VerifyTestResult (not String (L"Hello world").Match (RegularExpression (L"ello")));
        VerifyTestResult (String (L"abc").StartsWith (L"AB", CompareOptions::eCaseInsensitive));
        VerifyTestResult (not String (L"abc").StartsWith (L"AB", CompareOptions::eWithCase));
        VerifyTestResult (String (L"abc").EndsWith (L"bc", CompareOptions::eCaseInsensitive));
        VerifyTestResult (String (L"abc").EndsWith (L"bc", CompareOptions::eWithCase));
        VerifyTestResult (String (L"abc").EndsWith (L"BC", CompareOptions::eCaseInsensitive));
        VerifyTestResult (not String (L"abc").EndsWith (L"BC", CompareOptions::eWithCase));
    }

}


namespace   {

    void    DoRegressionTests_ ()
    {
        Test1_ ();
        Test2_ ();
        Test3_ ();
        Test4_ ();
        Test5_ ();
        Test6_ ();
        Test7_ ();
        Test8_ReadOnlyStrings_ ();
        Test8_ExternalMemoryOwnershipStrings_ ();
        Test9_StringVersusStdCString_ ();
        Test10_ConvertToFromSTDStrings_ ();
        Test11_Trim_ ();
        Test12_CodePageConverter_ ();
        Test13_ToLowerUpper_ ();
        Test14_String_StackLifetimeReadOnly_ ();
        Test14_String_StackLifetimeReadWrite_ ();
        Test15_StripAll_ ();
        Test16_Format_ ();
        Test17_Find_ ();
        Test17_RegExp_ ();
        Test18_Compare_ ();
        Test19_ConstCharStar_ ();
        Test20_CStringHelpers_ ();
        Test21_StringToIntEtc_ ();
        Test22_StartsWithEndsWithMatch_ ();
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

