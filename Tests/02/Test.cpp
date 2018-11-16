/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::Characters::Strings
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdarg>
#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Concrete/String_ExternalMemoryOwnership_ApplicationLifetime.h"
#include "Stroika/Foundation/Characters/Concrete/String_ExternalMemoryOwnership_StackLifetime.h"
#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/RegularExpression.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/String_Constant.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Configuration/Locale.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Containers/STL/Utilities.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/SmallStackBuffer.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Characters::Concrete;

using Containers::Sequence;

using std::byte;

#define qPrintTimings 0

/**
 * TODO:
 *
 *      (o) Must write ASAP a performance comparison suite for String class as part of string automated test.
 *          Print a table of results (X/Y – wstring versus String (maybe versus other string types?)).
 *          Compare inserting into start of vector<STRINGTYPE> - to test copying.
 */

namespace {
    /*
     * Test of STATIC FILE SCOPE INITIALIZATION
     *
     *      Really a test - but not a numbered test because this has to be done at file scope.
     *      Assure that static construction (file scope) works OK
     *
     *  \note   This code ATTEMPTS to detect inter-obj module problems with the file scope string objects. But its not perfect,
     *          and depends a bit on luck and undefined linker behavior. However, since we test on many platforms, the hope is
     *          that IF this code is problematic, it will fail on at least one platform.
     */
    String s_TestEmptyString_;
    String s_TestStringInit_{L"my very good test"};
    String s_TestStringConstantInit_{String_Constant{L"my very very good test"}};
    String s_TestStringAssignInit_{s_TestStringInit_};

    struct RunTest_VerifyStatics_ {
        RunTest_VerifyStatics_ ()
        {
            VerifyTestResult (s_TestEmptyString_.empty ());
            VerifyTestResult (s_TestStringInit_ == L"my very good test");
            VerifyTestResult (s_TestStringConstantInit_ == L"my very very good test");
            VerifyTestResult (s_TestStringAssignInit_ == s_TestStringInit_);
        }
    } _s_RunTest_VerifyStaticAssigns_;
}

namespace {
    unsigned ipow (unsigned n, unsigned toPow)
    {
        // quick hack since pow didnt seem to do what I want - just blindly
        // multiply and don't worry about overflow...
        unsigned result = 1;
        while (toPow-- != 0) {
            result *= n;
        }
        return (result);
    }
}

namespace {
    namespace Test2Helpers_ {
#if qDebug
        const int kLoopEnd = 1000;
#else
        const int kLoopEnd      = 2000;
#endif

        void StressTest1_ (String big)
        {
            for (size_t j = 1; j <= kLoopEnd / 50; j++) {
                String_ExternalMemoryOwnership_ApplicationLifetime a (L"a");
                for (size_t i = 0; i <= kLoopEnd; i++) {
                    big += a;
                    VerifyTestResult ((big.GetLength () - 1) == i);
                    VerifyTestResult (big[i] == 'a');
                }
                big.clear ();
            }

            String s1 = L"test strings";
            for (int i = 1; i <= kLoopEnd; i++) {
                big += s1;
                VerifyTestResult (big.GetLength () == s1.GetLength () * i);
            }
        }
        void StressTest2_ (String big)
        {
            String s1 = L"test strings";
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
        void StressTestStrings ()
        {
#if qPrintTimings
            cout << "Stress testing strings..." << endl;
            Time::DurationSecondsType t = Time::GetTickCount ();
#endif

            {
                String s (L"");
                StressTest1_ (s);
            }

#if qPrintTimings
            t = Time::GetTickCount () - t;
            cout << "finished Stress testing strings += ... time elapsed = " << t << endl;
            t = Time::GetTickCount ();
#endif

            {
                String s (L"");
                StressTest2_ (s);
            }

#if qPrintTimings
            t = Time::GetTickCount () - t;
            cout << "finished Stress testing strings + ... time elapsed = " << t << endl;
#endif
        }
#if 0
        void    StressTestBufferedStrings ()
        {
#if qPrintTimings
            cout << "Stress testing buffered strings..." << endl;
            Time::DurationSecondsType t = Time::GetTickCount ();
#endif

            {
                String_BufferedArray s (L"");
                StressTest1_ (s);
            }

#if qPrintTimings
            t = Time::GetTickCount () - t;
            cout << "finished stress testing buffered strings  += ... time elapsed = " << t << endl;
            t = Time::GetTickCount ();
#endif

            {
                String_BufferedArray s (L"");
                StressTest2_ (s);
            }

#if qPrintTimings
            t = Time::GetTickCount () - t;
            cout << "finished stress testing buffered strings + ... at " << t << endl;
#endif
        }
#endif
    }
}

namespace {
    void Test2_Helper_ (String& s1, String& s2)
    {
        VerifyTestResult (s1.GetLength () == 12);
        VerifyTestResult (String (s1).GetLength () == 12);
        VerifyTestResult (s1 == s2);
        VerifyTestResult (!(s1 != s2));
        VerifyTestResult (s1 + s1 == s2 + s2);
        VerifyTestResult ((s1 + s1).GetLength () == s1.GetLength () * 2);
        VerifyTestResult (s1[2] == 's');
        VerifyTestResult ('s' == s1[2].GetCharacterCode ());
        VerifyTestResult (s1.GetLength () == 12);

        String s3;
        s3 += s1;
        s3 += s2;

        s1 += L"\n";
        VerifyTestResult (s1.GetLength () == 13);
    }

    void Test1_ ()
    {
        /*
         * Some simple tests to start off with.
         */
        {
            VerifyTestResult (String (L"a").length () == 1);
            VerifyTestResult (String (String (L"fred") + String (L"joe")).GetLength () == 7);

            VerifyTestResult (String (L"fred") + String (L"joe") == String (L"fredjoe"));

            {
                String s1 = String (L"test strings");
                String s2 = String (L"test strings");
                Test2_Helper_ (s1, s2);
            }

            {
                String s1 (L"test strings");
                String s2 (L"test strings");

                VerifyTestResult (Character ('a') == 'a');

                Test2_Helper_ (s1, s2);
            }

            {
                String s1 = String_ExternalMemoryOwnership_ApplicationLifetime (L"test strings");
                String s2 = String_ExternalMemoryOwnership_ApplicationLifetime (L"test strings");
                Test2_Helper_ (s1, s2);
            }
        }
    }

    void Test2_ ()
    {
        Test2Helpers_::StressTestStrings ();
#if 0
        Test2Helpers_::StressTestBufferedStrings ();
#endif
    }

    void Test3_ ()
    {
        String t1;
        String t2 = t1;
        String t3 = L"a";
        String t4 = L"a";

        VerifyTestResult (t1 == L"");
        VerifyTestResult (t1 == String ());
        VerifyTestResult (t1 == String (L""));
        VerifyTestResult (t1 == t2);
        VerifyTestResult (t3 == L"a");
        VerifyTestResult (t3 == String (L"a"));
        VerifyTestResult (t4 == L"a");
#if defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 7)
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wself-assign-overloaded\""); // explicitly assigning value of variable of type 'Stroika::Foundation::Characters::String' to itself
#endif
        t1 = t1;
#if defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 7)
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wself-assign-overloaded\"");
#endif
        VerifyTestResult (t1 == L"");

        t1 += 'F';
        t1 += 'r';
        t1 += 'e';
        t1 += 'd';
        t1 += L" Flintstone";
        VerifyTestResult (t1 == L"Fred Flintstone");
        VerifyTestResult (String (L"Fred Flintstone") == t1);
        VerifyTestResult (String (L"Fred Flintstone") == t1);
        VerifyTestResult (t1 == String (L"Fred Flintstone"));
        VerifyTestResult (t2 != L"Fred Flintstone");
        VerifyTestResult (String (L"Fred Flintstone") != t2);
        VerifyTestResult (String (L"Fred Flintstone") != t2);
        VerifyTestResult (t2 != String (L"Fred Flintstone"));

        VerifyTestResult (t1.GetLength () == 15);
        t1.erase (4);
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

        String a[10];
        VerifyTestResult (a[2] == L"");
        a[3] = L"Fred";
        VerifyTestResult (a[3] == L"Fred");
        VerifyTestResult (a[2] != L"Fred");
    }

    void Test4_ ()
    {
        const wchar_t frobaz[] = L"abc";

        String  t1;
        String  t3 = L"a";
        String  t5 = String (frobaz);
        String* t6 = new String (L"xyz");
        delete (t6);

        t5 = t1;
        t1 = t5;
#if defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 7)
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wself-assign-overloaded\""); // explicitly assigning value of variable of type 'Stroika::Foundation::Characters::String' to itself
#endif
        t1 = t1;
#if defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 7)
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wself-assign-overloaded\"");
#endif
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
        VerifyTestResult (not t5.Find (L"STONE").has_value ());
        VerifyTestResult (not t5.Contains (L"SToNE"));
        VerifyTestResult (t5.Find (L"STONE", CompareOptions::eCaseInsensitive) == 5u);
        VerifyTestResult (t5.Contains (L"SToNE", CompareOptions::eCaseInsensitive));

        t1.erase (4);
        VerifyTestResult (t1.length () == 4);

        t5 = t1;
        t5.SetCharAt ('f', 0);
        t5.SetCharAt ('R', 1);
        t5.SetCharAt ('E', 2);
        t5.SetCharAt ('D', 3);

        VerifyTestResult (t5[0] == 'f');
        VerifyTestResult (t5[1] == 'R');
        VerifyTestResult (t5[2] == 'E');
        VerifyTestResult (t5[3] == 'D');
        VerifyTestResult (t5.Find ('f') == 0u);
        VerifyTestResult (t5.Find (L"f") == 0u);
        VerifyTestResult (t5.Find (L"fR") == 0u);
        VerifyTestResult (t5.Find (L"fRE") == 0u);
        VerifyTestResult (t5.Find (L"fRED") == 0u);
        VerifyTestResult (not t5.Find (L"fRD").has_value ());
        VerifyTestResult (t5.Find ('R') == 1u);
        VerifyTestResult (t5.Find ('E') == 2u);
        VerifyTestResult (t5.Find ('D') == 3u);
        VerifyTestResult (t5.Find (L"D") == 3u);

        VerifyTestResult (t5.RFind ('f') == 0u);
        VerifyTestResult (t5.RFind ('R') == 1u);
        VerifyTestResult (t5.RFind ('E') == 2u);
        VerifyTestResult (t5.RFind ('D') == 3u);
        VerifyTestResult (t5.RFind (L"D") == 3u);
        VerifyTestResult (t5.RFind (L"ED") == 2u);
        VerifyTestResult (t5.RFind (L"RED") == 1u);
        VerifyTestResult (t5.RFind (L"fRED") == 0u);
        VerifyTestResult (not t5.RFind (L"fr").has_value ());
        VerifyTestResult (t5.RFind (L"f") == 0u);

        t5.SetCharAt ('D', 0);
        t5.SetCharAt ('D', 1);
        t5.SetCharAt ('D', 2);
        t5.SetCharAt ('D', 3);
        VerifyTestResult (t5.Find ('D') == 0u);
        VerifyTestResult (t5.Find (L"D") == 0u);
        VerifyTestResult (t5.RFind ('D') == 3u);
        VerifyTestResult (t5.RFind (L"D") == 3u);

        VerifyTestResult (not t5.Find ('f').has_value ());
        VerifyTestResult (not t5.Find (L"f").has_value ());
        VerifyTestResult (not t5.RFind ('f').has_value ());
        VerifyTestResult (not t5.RFind (L"f").has_value ());

        VerifyTestResult (t5[0] == 'D');
        VerifyTestResult (t5[1] == 'D');
        VerifyTestResult (t5[2] == 'D');
        VerifyTestResult (t5[3] == 'D');
    }

    void Test5_ ()
    {
        String arr[100];
        arr[3] = L"fred";
        VerifyTestResult (arr[3] == L"fred");
        String* l = new String[100];
        l[3]      = L"FRED";
        VerifyTestResult (l[3] == L"FRED");
        VerifyTestResult (l[99] == L"");
        delete[](l);
        size_t nSlots = 100;
        l             = new String[size_t (nSlots)];
        delete[](l);
    }

    template <typename STRING>
    STRING Test6_Helper_ (const STRING& a, int depth)
    {
        STRING b = a;
        b += a;
        if (depth > 0) {
            b = Test6_Helper_<STRING> (b, depth - 1) + Test6_Helper_<STRING> (b, depth - 1);
        }
        return (b);
    }
    template <typename STRING>
    void Test6_Helper_ ([[maybe_unused]] const char* testMessage)
    {
#if qPrintTimings
        const int kRecurseDepth = 10;
#else
        const int kRecurseDepth = 8;
#endif
        STRING testString = L"some dump test";
#if qPrintTimings
        cout << "\tTYPE=" << testMessage << ": Recursive build test with depth " << kRecurseDepth << endl;
        Time::DurationSecondsType t = Time::GetTickCount ();
#endif

        STRING s = Test6_Helper_<STRING> (testString, kRecurseDepth); // returns length 114688 for depth 6
        VerifyTestResult (s.length () == (ipow (4, kRecurseDepth) * 2 * testString.length ()));

#if qPrintTimings
        t = Time::GetTickCount () - t;
        cout << "\tfinished Recursive build test. Time elapsed = " << t << " length = " << s.length () << endl;
#endif
    }
    void Test6_ ()
    {
        Test6_Helper_<String> ("Characters::String");
        Test6_Helper_<wstring> ("std::wstring");
    }
    void Test7_Comparisons_ ()
    {
        VerifyTestResult (String (L"1") <= String (L"1"));
        VerifyTestResult (L"1" <= String (L"1"));
        VerifyTestResult (String (L"1") <= L"1");
        VerifyTestResult (String (L"1") <= String (L"10"));
        VerifyTestResult (not(String (L"1") > String (L"10")));
        VerifyTestResult (not(String (L"1") >= String (L"10")));
        VerifyTestResult (String (L"1") < String (L"10"));

        VerifyTestResult (String (L"20") > String (L"11"));
        VerifyTestResult (String (L"20") >= String (L"11"));
        VerifyTestResult (not(String (L"20") < String (L"11")));
        VerifyTestResult (not(String (L"20") <= String (L"11")));
        VerifyTestResult (String (L"11") < String (L"20"));
        VerifyTestResult (String (L"11") <= String (L"20"));
        VerifyTestResult (not(String (L"11") > String (L"20")));
        VerifyTestResult (not(String (L"11") >= String (L"20")));

        VerifyTestResult (String (L"aac") > String (L"aab"));
        VerifyTestResult (String (L"aac") >= String (L"aab"));
        VerifyTestResult (not(String (L"aac") < String (L"aab")));
        VerifyTestResult (not(String (L"aac") <= String (L"aab")));

        VerifyTestResult (String (L"apple") < String (L"apples"));
        VerifyTestResult (String (L"apple") <= String (L"apples"));
        VerifyTestResult (not(String (L"apple") > String (L"apples")));
        VerifyTestResult (not(String (L"apple") >= String (L"apples")));
    }

    void Test8_ReadOnlyStrings_ ()
    {
        // NOTE - THIS TESTS String_Constant
        //  using   String_Constant =   String_ExternalMemoryOwnership_ApplicationLifetime;
        String s = String_ExternalMemoryOwnership_ApplicationLifetime (L"fred");
        VerifyTestResult (s[0] == 'f');
        s.erase (3);
        VerifyTestResult (s[0] == 'f');
        VerifyTestResult (s.GetLength () == 3);
        VerifyTestResult (s == L"fre");
        s += L"x";
        VerifyTestResult (s.GetLength () == 4);
        VerifyTestResult (s[3] == 'x');
        VerifyTestResult (s == L"frex");
        s = s.InsertAt ('x', 2);
        VerifyTestResult (s == L"frxex");
        {
            wchar_t kZero[] = L"";
            s               = s.InsertAt (std::begin (kZero), std::begin (kZero), 0);
            VerifyTestResult (s == L"frxex");
            s = s.InsertAt (std::begin (kZero), std::begin (kZero), 1);
            VerifyTestResult (s == L"frxex");
            s = s.InsertAt (std::begin (kZero), std::begin (kZero), 5);
            VerifyTestResult (s == L"frxex");
        }
    }

    void Test8_ExternalMemoryOwnershipStrings_ ()
    {
        String s = String_ExternalMemoryOwnership_ApplicationLifetime (L"fred");
        VerifyTestResult (s[0] == 'f');
        s.erase (3);
        VerifyTestResult (s[0] == 'f');
        VerifyTestResult (s.GetLength () == 3);
        s += L"x";
        VerifyTestResult (s.GetLength () == 4);
        VerifyTestResult (s[3] == 'x');
        VerifyTestResult (s == L"frex");
        s = s.InsertAt ('x', 2);
        VerifyTestResult (s == L"frxex");
    }

    namespace {
        namespace Test9Support {
            template <typename STRING>
            void DoTest1 (STRING s)
            {
                STRING t1 = s;
                for (size_t i = 0; i < 100; ++i) {
                    t1 += L"X";
                }
                STRING t2 = t1;
                if (t1 != t2) {
                    VerifyTestResult (false);
                }
            }
        }
    }
    void Test9_StringVersusStdCString_ ()
    {
        // EMBELLISH THIS MORE ONCE WE HAVE TIMING SUPPORT WORKING - SO WE CNA COMPARE PERFORMANCE - AND COME UP WITH MORE REASONABLE TESTS
        //
        //      -- LGP 2011-09-01
        Test9Support::DoTest1<String> (L"Hello");
        Test9Support::DoTest1<std::wstring> (L"Hello");
    }
    void Test10_ConvertToFromSTDStrings_ ()
    {
        const wstring kT1 = L"abcdefh124123985213129314234";
        String        t1  = kT1;
        VerifyTestResult (t1.As<wstring> () == kT1);
        VerifyTestResult (t1 == kT1);
    }
}

namespace {
    void Test11_Trim_ ()
    {
        const String kT1 = L"  abc";
        VerifyTestResult (kT1.RTrim () == kT1);
        VerifyTestResult (kT1.LTrim () == kT1.Trim ());
        VerifyTestResult (kT1.Trim () == L"abc");
        VerifyTestResult (String (L" abc ").Trim () == L"abc");

        VerifyTestResult (kT1.Trim ([](Character c) -> bool { return c.IsAlphabetic (); }) == L"  ");
        VerifyTestResult (String (L"/\n").Trim () == L"/");
    }
}

namespace {
    void Test12_CodePageConverter_ ()
    {
        wstring w = L"<PHRMode";
        using namespace Characters;
        using namespace Memory;
        CodePageConverter      cpc (kCodePage_UTF8, CodePageConverter::eHandleBOM);
        size_t                 sz = cpc.MapFromUNICODE_QuickComputeOutBufSize (w.c_str (), w.length ());
        SmallStackBuffer<char> buf (sz + 1);
        size_t                 charCnt = sz;
        cpc.MapFromUNICODE (w.c_str (), w.length (), buf, &charCnt);
        VerifyTestResult (string (buf.begin (), buf.begin () + charCnt) == "﻿<PHRMode");
    }
}

namespace {
    void Test13_ToLowerUpper_ ()
    {
        String w = L"Lewis";
        VerifyTestResult (w.ToLowerCase () == L"lewis");
        VerifyTestResult (w.ToUpperCase () == L"LEWIS");
        VerifyTestResult (w == L"Lewis");
    }
}

namespace {
    void Test14_String_StackLifetimeReadOnly_ ()
    {
        wchar_t buf[1024] = L"fred";
        {
            String_ExternalMemoryOwnership_StackLifetime s (buf);
            VerifyTestResult (s[0] == 'f');
            s.erase (3);
            VerifyTestResult (s[0] == 'f');
            VerifyTestResult (s.GetLength () == 3);
            s += L"x";
            VerifyTestResult (s.GetLength () == 4);
            VerifyTestResult (s[3] == 'x');
        }
        VerifyTestResult (::wcscmp (buf, L"fred") == 0);
    }
#if 0
    void    Test14_String_StackLifetimeReadWrite_ ()
    {
        wchar_t buf[1024]   =   L"fred";
        {
            String_ExternalMemoryOwnership_StackLifetime_ReadWrite s (buf);
            VerifyTestResult (s[0] == 'f');
            s.erase (3);
            VerifyTestResult (s[0] == 'f');
            VerifyTestResult (s.GetLength () == 3);
            s += L"x";
            VerifyTestResult (s.GetLength () == 4);
            VerifyTestResult (s[3] == 'x');
        }
        VerifyTestResult (::wcscmp (buf, L"fred") == 0);
    }
#endif
}

namespace {
    void Test15_StripAll_ ()
    {
        String w = L"Le wis";
        VerifyTestResult (w.StripAll ([](Character c) -> bool { return c.IsWhitespace (); }) == L"Lewis");

        w = L"This is a very good test    ";
        VerifyTestResult (w.StripAll ([](Character c) -> bool { return c.IsWhitespace (); }) == L"Thisisaverygoodtest");
    }
}

namespace {
    void Test16_Format_ ()
    {
        VerifyTestResult (CString::Format ("%d", 123) == "123");
        VerifyTestResult (CString::Format ("%s", "123") == "123");

        // SUBTLE - this would FAIL with vsnprintf on gcc -- See docs in Header for
        // Format string
        VerifyTestResult (CString::Format (L"%s", L"123") == L"123");
        VerifyTestResult (Format (L"%s", L"123") == L"123");

        VerifyTestResult (Format (L"%20s", L"123") == L"                 123");
        VerifyTestResult (Format (L"%.20s", L"123") == L"123");

        for (size_t i = 1; i < 1000; ++i) {
            String format = Format (L"%%%ds", static_cast<int> (i));
            VerifyTestResult (Format (format.c_str (), L"x").length () == i);
        }
        VerifyTestResult (Characters::Format (L"%d.%d%s%s", 1, 0, L"a", L"1x") == L"1.0a1x"); // 2 conseq %s%s POSIX bug fixed 2014-01-22
    }
}

namespace {
    namespace Test17_Private_ {
        void Test17_Find_ ()
        {
            VerifyTestResult (String (L"abc").Find (L"b") == 1u);
            VerifyTestResult (not String (L"abc").Find (L"x").has_value ());
            VerifyTestResult (not String (L"abc").Find (L"b", 2).has_value ());
        }
        void Test17_FindEach_ ()
        {
            {
                // @todo - Either have FindEach return Sequence or fix vector stuff!!!
                VerifyTestResult (String (L"abc").FindEach (L"b") == Containers::Sequence<size_t> ({1}).As<vector<size_t>> ());
            }
            // @todo - Either have FindEach return Sequence or fix vector stuff!!!
            VerifyTestResult (String (L"01-23-45-67-89").FindEach (L"-") == Containers::Sequence<size_t> ({2, 5, 8, 11}).As<vector<size_t>> ());
            // @todo - Either have FindEach return Sequence or fix vector stuff!!!
            VerifyTestResult (String (L"AAAA").FindEach (L"AA") == Containers::Sequence<size_t> ({0, 2}).As<vector<size_t>> ());
        }
        void Test17_ReplaceAll_ ()
        {
            VerifyTestResult (String (L"01-23-45-67-89").ReplaceAll (L"-", L"") == L"0123456789");
            VerifyTestResult (String (L"01-23-45-67-89").ReplaceAll (L"-", L"x") == L"01x23x45x67x89");
            VerifyTestResult (String (L"01-23-45-67-89").ReplaceAll (L"-", L"--") == L"01--23--45--67--89");
        }
        void Test17_RegExpMatch_ ()
        {
            {
                for (String i : Sequence<String>{L"01-23-45-67-89", L"", L"a"}) {
                    VerifyTestResult (not i.Match (RegularExpression::kNONE));
                    VerifyTestResult (i.Match (RegularExpression::kAny));
                }
            }
        }
        void Test17_RegExp_Search_ ()
        {
            {
                RegularExpression regExp (L"abc");
                String            testStr2Search = String (L"abc");
                VerifyTestResult (testStr2Search.FindEach (regExp).size () == 1);
                VerifyTestResult ((testStr2Search.FindEach (regExp)[0] == pair<size_t, size_t> (0, 3)));
            }
            {
                // Test replace crlfs
                String stringWithCRLFs = L"abc\r\ndef\r\n";
#if !qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy
                String replaced = stringWithCRLFs.ReplaceAll (RegularExpression (L"[\r\n]*"), L"");
                VerifyTestResult (replaced == L"abcdef");
#endif
            }
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
        void Test17_RegExp_ ()
        {
            Test17_Find_ ();
            Test17_FindEach_ ();
            Test17_RegExp_Search_ ();
            Test17_RegExpMatch_ ();
            VerifyTestResult ((String (L"Hello world").Find (RegularExpression (L"ello")) == pair<size_t, size_t> (1, 5)));
            vector<RegularExpressionMatch> r = String (L"<h2>Egg prices</h2>").FindEachMatch (RegularExpression (L"<h(.)>([^<]+)"));
            VerifyTestResult (r.size () == 1 and r[0].GetSubMatches ()[0] == L"2" and r[0].GetSubMatches ()[1] == L"Egg prices");
            VerifyTestResult (String (L"Hello world").ReplaceAll (RegularExpression (L"world"), L"Planet") == L"Hello Planet");
        }
        void docsTests_ ()
        {
            {
                const String_Constant kTest_{L"a=b"};
                const String_Constant kLbl2LookFor_{L"a="};
                String                tmp;
                if (kTest_.Find (kLbl2LookFor_)) {
                    tmp = String{kTest_.SubString (kLbl2LookFor_.length ())};
                }
                VerifyTestResult (tmp == L"b");
            }
            {
// SEE http://en.cppreference.com/w/cpp/regex/match_results/operator_at
// SEE http://en.cppreference.com/w/cpp/regex/regex_search
// TEST FAIULS - SEE ABOUT WHY
#if 0
#include <iostream>
#include <regex>
#include <string>
                int main() {
                    std::string lines[] = {"a=b,a=c"};

                    std::regex color_regex("a=(.*)");

                    std::smatch color_match;
                    for (const auto& line : lines) {
                        std::regex_search(line, color_match, color_regex);
                        std::cout << "matches for '" << line << "'\n";
                        // for (size_t i = 0; i < color_match.size(); ++i) {
                        //     std::ssub_match sub_match = color_match[i];
                        //     std::string sub_match_str = sub_match.str();
                        //     std::cout << i << ": " << sub_match_str << '\n';
                        //}

                        for (auto i : color_match) {
                            std::cout << i << '\n';
                        }
                    }
                }
#endif
                {
                    const String_Constant   kTest_{L"a=b,"};
                    const RegularExpression kRE_{L"a=(.*)"};
                    Sequence<String>        tmp1{kTest_.FindEachString (kRE_)};
                    VerifyTestResult (tmp1.size () == 1 and tmp1[0] == L"a=b,");
                    Sequence<RegularExpressionMatch> tmp2{kTest_.FindEachMatch (kRE_)};
                    VerifyTestResult (tmp2.size () == 1 and tmp2[0].GetFullMatch () == L"a=b," and tmp2[0].GetSubMatches () == Sequence<String>{L"b,"});
                }
                {
                    const String_Constant   kTest_{L"a=b,"};
                    const RegularExpression kRE_{L"a=(.*),"};
                    Sequence<String>        tmp1{kTest_.FindEachString (kRE_)};
                    VerifyTestResult (tmp1.size () == 1 and tmp1[0] == L"a=b,");
                    Sequence<RegularExpressionMatch> tmp2{kTest_.FindEachMatch (kRE_)};
                    VerifyTestResult (tmp2.size () == 1 and tmp2[0].GetFullMatch () == L"a=b," and tmp2[0].GetSubMatches () == Sequence<String>{L"b"});
                }

                {
                    const String_Constant   kTest_{L"a=b,"};
                    const RegularExpression kRE_{L"a=(.*)[, ]"};
                    Sequence<String>        tmp1{kTest_.FindEachString (kRE_)};
                    VerifyTestResult (tmp1.size () == 1 and tmp1[0] == L"a=b,");
                    Sequence<RegularExpressionMatch> tmp2{kTest_.FindEachMatch (kRE_)};
                    VerifyTestResult (tmp2.size () == 1 and tmp2[0].GetFullMatch () == L"a=b," and tmp2[0].GetSubMatches () == Sequence<String>{L"b"});
                }
                {
                    const String_Constant   kTest_{L"a=b, c=d"};
                    const RegularExpression kRE_{L"(.)=(.)"};
                    VerifyTestResult ((kTest_.FindEachString (kRE_) == vector<String>{L"a=b", L"c=d"}));
                }
            }
        }
    }

    void Test17_Find_ ()
    {
        Test17_Private_::Test17_ReplaceAll_ ();
        Test17_Private_::Test17_ReplaceAll_ ();
        Test17_Private_::Test17_RegExp_ ();
        Test17_Private_::docsTests_ ();
    }
}

namespace {
    void Test18_Compare_ ()
    {
        const String kHELLOWorld = String (L"Hello world");
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

namespace {
    void Test19_ConstCharStar_ ()
    {
        VerifyTestResult (wcscmp (String (L"fred").c_str (), L"fred") == 0);
        VerifyTestResult (wcscmp (String (L"0123456789abcde").c_str (), L"0123456789abcde") == 0);                                   // 15 chars
        VerifyTestResult (wcscmp (String (L"0123456789abcdef").c_str (), L"0123456789abcdef") == 0);                                 // 16 chars
        VerifyTestResult (wcscmp (String (L"0123456789abcdef0123456789abcde").c_str (), L"0123456789abcdef0123456789abcde") == 0);   // 31 chars
        VerifyTestResult (wcscmp (String (L"0123456789abcdef0123456789abcdef").c_str (), L"0123456789abcdef0123456789abcdef") == 0); // 32 chars
        {
            String tmp = L"333";
            VerifyTestResult (wcscmp (tmp.c_str (), L"333") == 0);
            tmp = L"Barny";
            VerifyTestResult (wcscmp (tmp.c_str (), L"Barny") == 0);
            tmp.SetCharAt ('c', 2);
            VerifyTestResult (wcscmp (tmp.c_str (), L"Bacny") == 0);
            String bumpRefCnt = tmp;
            tmp.SetCharAt ('d', 2);
            VerifyTestResult (wcscmp (tmp.c_str (), L"Badny") == 0);
        }
    }
}

namespace {
    void Test20_CStringHelpers_ ()
    {
        VerifyTestResult (CString::Length ("hi") == 2);
        VerifyTestResult (CString::Length (L"hi") == 2);
        {
            // This test was mostly to confirm the false-warning from valgrind
            // SEE THIS VALGRIND SUPPRESSION - wcscmp_appears_to_generate_false_warnings_gcc48_ubuntu
            wchar_t buf[3] = {'1', '2', 0};
            VerifyTestResult (::wcscmp (buf, L"12") == 0);
        }
        {
            char buf[3] = {'1', '1', '1'};
            CString::Copy (buf, NEltsOf (buf), "3");
            VerifyTestResult (::strcmp (buf, "3") == 0);
        }
        {
            wchar_t buf[3] = {'1', '1', '1'};
            CString::Copy (buf, NEltsOf (buf), L"3");
            VerifyTestResult (::wcscmp (buf, L"3") == 0);
        }
        {
            char buf[3] = {'1', '1', '1'};
            CString::Copy (buf, NEltsOf (buf), "12345");
            VerifyTestResult (::strcmp (buf, "12") == 0);
        }
        {
            wchar_t buf[3] = {'1', '1', '1'};
            CString::Copy (buf, NEltsOf (buf), L"12345");
            VerifyTestResult (::wcscmp (buf, L"12") == 0);
        }
    }
}

namespace {
    namespace Test21_StringToIntEtc_Helper_ {
        template <typename FLOAT_TYPE>
        void Verify_FloatStringRoundtripNearlyEquals_ (FLOAT_TYPE l)
        {
            VerifyTestResult (Math::NearlyEquals (l, Characters::String2Float<FLOAT_TYPE> (Float2String (l, Float2StringOptions::Precision (numeric_limits<FLOAT_TYPE>::digits10 + 1)))));
        }
    }
    void Test21_StringToIntEtc_ ()
    {
        {
            VerifyTestResult (CString::String2Int<int> ("-3") == -3);
            VerifyTestResult (CString::String2Int<int> ("3") == 3);
            VerifyTestResult (CString::String2Int<int> (wstring (L"3")) == 3);
            VerifyTestResult (String2Int<int> (String (L"3")) == 3);
        }
        {
            VerifyTestResult (CString::String2Int<int> ("") == 0);
            VerifyTestResult (String2Int<int> (L"") == 0);
            VerifyTestResult (CString::String2Int<int> (wstring (L"")) == 0);
            VerifyTestResult (String2Int<int> (String ()) == 0);
            VerifyTestResult (CString::String2Int<int> ("     ") == 0);
        }
        {
            VerifyTestResult (CString::HexString2Int ("") == 0);
            VerifyTestResult (CString::HexString2Int (L"") == 0);
            VerifyTestResult (CString::HexString2Int (wstring (L"")) == 0);
            VerifyTestResult (HexString2Int (String ()) == 0);
            VerifyTestResult (CString::HexString2Int ("     ") == 0);
        }
        {
            VerifyTestResult (isnan (String2Float<double> (String ())));
            VerifyTestResult (isnan (CString::String2Float (string ())));
            VerifyTestResult (isnan (String2Float<double> (wstring ())));
            VerifyTestResult (isnan (CString::String2Float ("")));
            VerifyTestResult (isnan (CString::String2Float (wstring (L""))));
            VerifyTestResult (isnan (String2Float<double> (String ())));
            VerifyTestResult (isnan (CString::String2Float ("     ")));
            VerifyTestResult (isnan (String2Float<double> (L"-1.#INF000000000000"))); // MSFT sometimes generates these but they arent legal INF values!
            VerifyTestResult (isnan (CString::String2Float ("-1.#INF000000000000")));
            VerifyTestResult (isnan (String2Float<double> (L"1.#INF000000000000")));
            VerifyTestResult (isnan (CString::String2Float ("1.#INF000000000000")));
            VerifyTestResult (isinf (String2Float<double> (L"INF")));
            VerifyTestResult (isinf (String2Float<double> (L"INFINITY")));
            VerifyTestResult (isinf (String2Float<double> (L"-INF")));
            VerifyTestResult (isinf (String2Float<double> (L"-INFINITY")));
            VerifyTestResult (isinf (String2Float<double> (L"+INF")));
            VerifyTestResult (isinf (String2Float<double> (L"+INFINITY")));

            VerifyTestResult (isinf (CString::String2Float (L"INF")));
            VerifyTestResult (isinf (CString::String2Float (L"INFINITY")));
            VerifyTestResult (isinf (CString::String2Float (L"-INF")));
            VerifyTestResult (isinf (CString::String2Float (L"-INFINITY")));
            VerifyTestResult (isinf (CString::String2Float (L"+INF")));
            VerifyTestResult (isinf (CString::String2Float (L"+INFINITY")));
        }
        {
            // roundtrip lossless
            Test21_StringToIntEtc_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<float>::min ());
            Test21_StringToIntEtc_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<float>::max ());
            Test21_StringToIntEtc_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<float>::lowest ());
            Test21_StringToIntEtc_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<double>::min ());
            Test21_StringToIntEtc_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<double>::max ());
            Test21_StringToIntEtc_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<double>::lowest ());
            Test21_StringToIntEtc_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<long double>::min ());
            Test21_StringToIntEtc_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<long double>::max ());
            Test21_StringToIntEtc_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<long double>::lowest ());
        }
        {
            VerifyTestResult (Math::NearlyEquals (CString::String2Float ("-44.4"), -44.4));
            VerifyTestResult (Math::NearlyEquals (CString::String2Float (L"-44.4"), -44.4));
            VerifyTestResult (Math::NearlyEquals (String2Float<double> (L"-44.4"), -44.4));
            VerifyTestResult (Math::NearlyEquals (String2Float<double> (String (L"44.4333")), 44.4333));
        }
        auto runLocaleIndepTest = []() {
            VerifyTestResult (Float2String (3000.5) == L"3000.5");
            VerifyTestResult (Float2String (30000.5) == L"30000.5");
        };
#if !qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy
        {
            // Verify change of locale has no effect on results
            locale prevLocale = locale::global (locale ("C"));
            runLocaleIndepTest ();
            locale::global (prevLocale);
        }
        {
            // Verify change of locale has no effect on results
            Configuration::ScopedUseLocale tmpLocale{Configuration::FindNamedLocale (L"en", L"us")};
            runLocaleIndepTest ();
        }
#endif
    }
}

namespace {
    void Test22_StartsWithEndsWithMatch_ ()
    {
        VerifyTestResult (String (L"abc").Match (RegularExpression (L"abc")));
        VerifyTestResult (not(String (L"abc").Match (RegularExpression (L"bc"))));
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

namespace {
    String Test23_help1_ (const wchar_t* format, va_list argsList)
    {
        return Characters::FormatV (format, argsList);
    }
    String Test23_help1_HELPER (const wchar_t* format, ...)
    {
        va_list argsList;
        va_start (argsList, format);
        String tmp = Test23_help1_ (format, argsList);
        va_end (argsList);
        return tmp;
    }
    void Test23_FormatV_ ()
    {
        VerifyTestResult (Test23_help1_HELPER (L"joe%sx", L"1") == L"joe1x");
    }
}

namespace {
    void Test24_Float2String ()
    {
        VerifyTestResult (Float2String (0.0) == L"0");
        VerifyTestResult (Float2String (3000.5) == L"3000.5");
        VerifyTestResult (Float2String (3000.500) == L"3000.5");
        VerifyTestResult (Float2String (3.1234, Characters::Float2StringOptions::Precision (2)) == L"3.1");
        VerifyTestResult (Float2String (3.1234, Characters::Float2StringOptions::Precision (3)) == L"3.12");
        VerifyTestResult (Float2String (31.234, Characters::Float2StringOptions::Precision (3)) == L"31.2");
        VerifyTestResult (Float2String (30707548160.0) == L"3.07075e+10");
    }
}

namespace {
    void Test25_RemoveAt_ ()
    {
        String x = L"123";
        x        = x.RemoveAt (1);
        VerifyTestResult (x == L"13");
        x = x.RemoveAt (0, 2);
        VerifyTestResult (x.empty ());
    }
}

namespace {
    void Test26_Iteration_ ()
    {
        {
            String x = L"123";
            int    i = 0;
            for (auto c : x) {
                VerifyTestResult (c == x[i]);
                i++;
            }
            VerifyTestResult (i == 3);
        }
        {
            // Current (may want to rethink since differs from containers) spec is that iteration 'captures' value logically
            // at the time we start iterating
            // -- LGP 2013-12-17
            String x = L"123";
            int    i = 0;
            for (auto c : x) {
                VerifyTestResult (c == x[i]);
                i++;
                x += L"9";
            }
            VerifyTestResult (i == 3);
        }
    }
}

namespace {
    void Test27_Repeat_ ()
    {
        {
            String x;
            String r = x.Repeat (5);
            VerifyTestResult (r.length () == 0);
        }
        {
            String x = L"123";
            String r = x.Repeat (3);
            VerifyTestResult (r.length () == 9);
            VerifyTestResult (r.SubString (3, 6) == L"123");
        }
    }
}

namespace {
    void Test28_ReplacementForStripTrailingCharIfAny_ ()
    {
        auto StripTrailingCharIfAny = [](const String& s, const Character& c) -> String {
            return s.EndsWith (c) ? s.SubString (0, -1) : s;
        };
        VerifyTestResult (StripTrailingCharIfAny (L"xxx", '.') == L"xxx");
        VerifyTestResult (StripTrailingCharIfAny (L"xxx.", '.') == L"xxx");
        VerifyTestResult (StripTrailingCharIfAny (L"xxx..", '.') == L"xxx.");
    }
}

namespace {
    void Test29_StringWithSequenceOfCharacter_ ()
    {
        {
            String              initialString = L"012345";
            Sequence<Character> s1            = Sequence<Character> (initialString); // THIS NEEDS TO BE MORE SEEMLESS
            s1.SetAt (3, 'E');
            VerifyTestResult (String (s1) == L"012E45");
        }
    }
}

namespace {
    void Test30_LimitLength_ ()
    {
        VerifyTestResult (String (L"12345").LimitLength (3) == L"12\u2026");
        VerifyTestResult (String (L"12345").LimitLength (5) == L"12345");
    }
}

namespace {
    void Test31_OperatorINSERT_ostream_ ()
    {
        using namespace std;
        wstringstream out;
        out << String (L"abc");
        VerifyTestResult (out.str () == L"abc");
    }
}

namespace {
    void Test32_StringBuilder_ ()
    {
        {
            StringBuilder out;
            out << L"hi mom";
            VerifyTestResult (out.str () == L"hi mom");
            out += L".";
            VerifyTestResult (out.str () == L"hi mom.");
            VerifyTestResult (static_cast<String> (out) == L"hi mom.");
            VerifyTestResult (out.As<String> () == L"hi mom.");
            VerifyTestResult (out.As<wstring> () == L"hi mom.");
        }
        {
            StringBuilder out{L"x"};
            VerifyTestResult (static_cast<String> (out) == L"x");
            VerifyTestResult (out.As<String> () == L"x");
        }
    }
}

namespace {
    void Test33_Append_ ()
    {
        String    result;
        Character buf[]{'a', 'b', 'c', 'd'};
        for (int i = 0; i < 10; ++i) {
            result.Append (std::begin (buf), std::begin (buf) + NEltsOf (buf));
        }
        VerifyTestResult (result.size () == 4 * 10);
        VerifyTestResult (result == L"abcdabcdabcdabcdabcdabcdabcdabcdabcdabcd");
        Verify (L"a" + String () == L"a"); // we had bug in v2.0a100 and earlier with null string on RHS of operator+
    }
}

namespace {
    void Test44_LocaleUNICODEConversions_ ()
    {
#if !qCompilerAndStdLib_locale_name_string_return_bogus_lengthBuggy
#if !qCompilerAndStdLib_locale_constructor_byname_asserterror_Buggy
        auto testRoundtrip = [](const char* localName, const string& localMBString, const wstring& wideStr) {
            bool initializedLocale = false;
            try {
                locale l{localName};
                initializedLocale = true;
                VerifyTestResult (String::FromNarrowString (localMBString, l) == wideStr);
                VerifyTestResult (String (wideStr).AsNarrowString (l) == localMBString);
            }
            catch (const runtime_error&) {
                // https://en.cppreference.com/w/cpp/locale/locale/locale says must throw std::runtime_error if invalid locale
                // if no such locale, just skip the test...
                VerifyTestResult (not initializedLocale); // else means throw from conversion which would be bad
            }
            catch (...) {
                DbgTrace (L"error: %s", Characters::ToString (current_exception ()).c_str ());
                VerifyTestResult (false); // I think docs say must throw std::runtime_error
            }
        };
        //testRoundtrip ("en_US.utf8", u8"z\u00df\u6c34\U0001d10b", L"zß水𝄋");
        testRoundtrip ("C", "fred", L"fred");
        testRoundtrip ("en_US.utf8", "\x7a\xc3\x9f\xe6\xb0\xb4\xf0\x9d\x84\x8b", L"zß水𝄋");
#endif
#endif
    }
}

namespace {
    void Test45_Tokenize_ ()
    {
        using Containers::Set;
        {
            String           t{L"ABC DEF G"};
            Sequence<String> tt{t.Tokenize ()};
            VerifyTestResult (tt.size () == 3);
            VerifyTestResult (tt[1] == L"DEF");
        }
        {
            String t{L"foo=   7"};
            auto   tt = t.Tokenize (Containers::Set<Character>{'='});
            VerifyTestResult (tt.length () == 2);
            VerifyTestResult (tt[0] == L"foo");
            VerifyTestResult (tt[1] == L"7");
        }
        {
            String t{L"           \n foo=   7"};
            auto   tt = t.Tokenize (Containers::Set<Character>{'='});
            VerifyTestResult (tt.length () == 2);
            VerifyTestResult (tt[1] == L"7");
        }
        {
            String t{L"MemTotal:        3082000 kB"};
            auto   tt = t.Tokenize (Containers::Set<Character>{':', ' ', '\t'});
            VerifyTestResult (tt.length () == 3);
            VerifyTestResult (tt[0] == L"MemTotal");
            VerifyTestResult (tt[1] == L"3082000");
            VerifyTestResult (tt[2] == L"kB");
        }
    }
}

namespace {
    void Test46_CompareLHSRHS_ ()
    {
        const wchar_t*     i = L"One";
        Characters::String n = L"Two";
        if (i == n) {
            VerifyTestResult (false);
        }
        if (n == i) {
            VerifyTestResult (false);
        }
        VerifyTestResult (i != n);
        VerifyTestResult (n != i);
    }
}

namespace {
    void Test47_SubString_ ()
    {
        {
            String tmp{L"This is good"};
            VerifyTestResult (tmp.SubString (5) == L"is good");
        }
        {
            const String_Constant kTest_{L"a=b"};
            const String_Constant kLbl2LookFor_{L"a="};
            String                tmp;
            if (kTest_.Find (kLbl2LookFor_)) {
                tmp = String{kTest_.SubString (kLbl2LookFor_.length ())};
            }
            VerifyTestResult (tmp == L"b");
        }
    }
}

namespace {
    void Test48_ToString_ ()
    {
        VerifyTestResult (ToString (3) == L"3");
        VerifyTestResult (ToString (3u) == L"0x3");
        VerifyTestResult (ToString (1.0).StartsWith (L"1"));
        VerifyTestResult (ToString (L"abc") == L"'abc'");
        VerifyTestResult (ToString (String (L"abc")) == L"'abc'");
        VerifyTestResult (ToString (initializer_list<int>{3, 4, 5}) == L"[ 3, 4, 5 ]");
        {
            constexpr byte kSample_[] = {byte{0x34}, byte{0x55}, byte{0x1f}};
            Memory::BLOB   b{begin (kSample_), end (kSample_)};
            VerifyTestResult (b.ToString () == L"[3 bytes: 34551f]");
            VerifyTestResult (Characters::ToString (b) == L"[3 bytes: 34551f]");
        }
        {
            constexpr int ca[3]{1, 3, 5};
            int           a[3]{1, 3, 5};
            VerifyTestResult (Characters::ToString (ca) == L"[ 1, 3, 5 ]");
            VerifyTestResult (Characters::ToString (a) == L"[ 1, 3, 5 ]");
        }
    }
}

namespace {
    void Test49_SetOfStringCTORIssue_ ()
    {
        Memory::Optional<String> optString{String ()};
        Containers::Set<String>  s{*optString};
    }
}

namespace {
    void Test50_Utf8Conversions_ ()
    {
        {
            VerifyTestResult (string (u8"phred") == String::FromUTF8 (string (u8"phred")).AsUTF8 ());
            VerifyTestResult (string (u8"שלום") == String::FromUTF8 (string (u8"שלום")).AsUTF8 ());
        }
        {
            VerifyTestResult (string (u8"phred") == String::FromUTF8 (u8"phred").AsUTF8 ());
            VerifyTestResult (string (u8"שלום") == String::FromUTF8 (u8"שלום").AsUTF8 ());
        }
        {
            StringBuilder tmp;
            tmp.Append (String::FromUTF8 (u8"€"));
            Verify (tmp.str () == L"€");
        }
        {
            // Intentionally no operator+ etc stuff for utf8 because type of u8"" is same as "", so no way to overload
        }
    }
}

namespace {
    void Test51_Utf16Conversions_ ()
    {
        {
            VerifyTestResult (u16string (u"phred") == String (u16string (u"phred")).AsUTF16 ());
            VerifyTestResult (u16string (u"שלום") == String (u16string (u"שלום")).AsUTF16 ());
        }
        {
            VerifyTestResult (u16string (u"phred") == String (u"phred").AsUTF16 ());
            VerifyTestResult (u16string (u"שלום") == String (u"שלום").AsUTF16 ());
        }
        {
            String tmp;
            tmp = tmp + u"phred";
            tmp += u"שלום";
            Verify (tmp == u"phredשלום");
        }
        {
            StringBuilder tmp;
            tmp += u"שלום";
            Verify (tmp.str () == u"שלום");
        }
        {
            StringBuilder tmp;
            tmp.Append (u"שלום");
            Verify (tmp.str () == u"שלום");
            //Verify (tmp.length () == 4);
        }
        {
            StringBuilder tmp;
            tmp += u"שלום";
            Verify (tmp.str () == u"שלום");
        }
        {
            StringBuilder tmp;
            tmp.Append (u"€");
            Verify (tmp.str () == u"€");
        }
        {
            // test surrogates
            // From https://en.wikipedia.org/wiki/UTF-16
            // http://www.fileformat.info/info/unicode/char/2008a/index.htm
            static const u16string kTestWithSurrogates_ = u16string (u"\U0002008A");
            VerifyTestResult (kTestWithSurrogates_.size () == 2);
            VerifyTestResult (kTestWithSurrogates_ == String (kTestWithSurrogates_).AsUTF16 ());
            // For now (as of v2.1d10) - Stroika stores strings as 'wchar_t' so the length of this will depend on if wchar_t is 2bytes or 4
            if constexpr (sizeof (wchar_t) == sizeof (char16_t)) {
                VerifyTestResult (String (kTestWithSurrogates_).length () == 2); // surrogate  surrogate pairs
            }
            if constexpr (sizeof (wchar_t) == sizeof (char32_t)) {
                VerifyTestResult (String (kTestWithSurrogates_).length () == 1); // surrogate  surrogate pairs
            }
        }
    }
}

namespace {
    void Test52_Utf32Conversions_ ()
    {
        {
            VerifyTestResult (u32string (U"phred") == String (u32string (U"phred")).AsUTF32 ());
            VerifyTestResult (u32string (U"שלום") == String (u32string (U"שלום")).AsUTF32 ());
        }
        {
            VerifyTestResult (u32string (U"phred") == String (U"phred").AsUTF32 ());
            VerifyTestResult (u32string (U"שלום") == String (U"שלום").AsUTF32 ());
        }
        {
            StringBuilder tmp;
            tmp += U"שלום";
            Verify (tmp.str () == U"שלום");
            // Verify (tmp.length () == 4);
        }
        {
            StringBuilder tmp;
            tmp.Append (U"שלום");
            Verify (tmp.str () == U"שלום");
        }
        {
            StringBuilder tmp;
            tmp += U"שלום";
            Verify (tmp.str () == U"שלום");
        }
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        Test1_ ();
        Test2_ ();
        Test3_ ();
        Test4_ ();
        Test5_ ();
        Test6_ ();
        Test7_Comparisons_ ();
        Test8_ReadOnlyStrings_ ();
        Test8_ExternalMemoryOwnershipStrings_ ();
        Test9_StringVersusStdCString_ ();
        Test10_ConvertToFromSTDStrings_ ();
        Test11_Trim_ ();
        Test12_CodePageConverter_ ();
        Test13_ToLowerUpper_ ();
        Test14_String_StackLifetimeReadOnly_ ();
#if 0
        Test14_String_StackLifetimeReadWrite_ ();
#endif
        Test15_StripAll_ ();
        Test16_Format_ ();
        Test17_Find_ ();
        Test18_Compare_ ();
        Test19_ConstCharStar_ ();
        Test20_CStringHelpers_ ();
        Test21_StringToIntEtc_ ();
        Test22_StartsWithEndsWithMatch_ ();
        Test23_FormatV_ ();
        Test24_Float2String ();
        Test25_RemoveAt_ ();
        Test26_Iteration_ ();
        Test27_Repeat_ ();
        Test28_ReplacementForStripTrailingCharIfAny_ ();
        Test29_StringWithSequenceOfCharacter_ ();
        Test30_LimitLength_ ();
        Test31_OperatorINSERT_ostream_ ();
        Test32_StringBuilder_ ();
        Test33_Append_ ();
        Test44_LocaleUNICODEConversions_ ();
        Test45_Tokenize_ ();
        Test46_CompareLHSRHS_ ();
        Test47_SubString_ ();
        Test48_ToString_ ();
        Test49_SetOfStringCTORIssue_ ();
        Test50_Utf8Conversions_ ();
        Test51_Utf16Conversions_ ();
        Test52_Utf32Conversions_ ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
