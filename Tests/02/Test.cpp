/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Characters
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cstdarg>
#include <iostream>
#include <sstream>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/RegularExpression.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Configuration/Locale.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Containers/STL/Utilities.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Time/Realtime.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Characters::Concrete;

using namespace Stroika::Frameworks;

using Containers::Sequence;

using std::byte;

#define qPrintTimings 0

#if qHasFeature_GoogleTest
/**
 * TODO:
 *
 *      (o) Must write ASAP a performance comparison suite for String class as part of string automated test.
 *          Print a table of results (X/Y – wstring versus String (maybe versus other string types?)).
 *          Compare inserting into start of vector<STRINGTYPE> - to test copying.
 */

namespace {
    namespace SampleUNICODE_ {

        namespace RawConstants_ {

            namespace Heb1_ {
                // שלום
                constexpr char8_t  k8_[]    = u8"\u1513\u1500\u1493\u1501";
                constexpr char16_t k16_[]   = u"\u1513\u1500\u1493\u1501";
                constexpr char32_t k32_[]   = U"\u1513\u1500\u1493\u1501";
                constexpr wchar_t  kWide_[] = L"\u1513\u1500\u1493\u1501";
            }
        }
    }
}

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
    String s_TestStringInit_{"my very good test"};
    String s_TestStringConstantInit_{String::FromStringConstant (L"my very very good test")};
    String s_TestStringAssignInit_{s_TestStringInit_};

    struct RunTest_VerifyStatics_ {
        RunTest_VerifyStatics_ ()
        {
            EXPECT_TRUE (s_TestEmptyString_.empty ());
            EXPECT_TRUE (s_TestStringInit_ == "my very good test");
            EXPECT_TRUE (s_TestStringConstantInit_ == "my very very good test");
            EXPECT_TRUE (s_TestStringAssignInit_ == s_TestStringInit_);
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
        const int kLoopEnd = 2000;
#endif

        void StressTest1_ (String big)
        {
            for (size_t j = 1; j <= kLoopEnd / 50; j++) {
                String a = "a"_k;
                for (size_t i = 0; i <= kLoopEnd; ++i) {
                    big += a;
                    EXPECT_TRUE ((big.size () - 1) == i);
                    EXPECT_TRUE (big[i] == 'a');
                }
                big.clear ();
            }

            String s1 = "test strings";
            for (int i = 1; i <= kLoopEnd; ++i) {
                big += s1;
                EXPECT_TRUE (big.size () == s1.size () * i);
            }
        }
        void StressTest2_ (String big)
        {
            String s1 = "test strings";
            for (int i = 1; i <= kLoopEnd; ++i) {
                big = big + s1;
                EXPECT_TRUE (big.size () == s1.size () * i);
#if 0
                for (int j = 0; j < big.size (); ++j) {
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
            Time::TimePointSeconds t = Time::GetTickCount ();
#endif

            {
                String s ("");
                StressTest1_ (s);
            }

#if qPrintTimings
            t = Time::GetTickCount () - t;
            cout << "finished Stress testing strings += ... time elapsed = " << t << endl;
            t = Time::GetTickCount ();
#endif

            {
                String s ("");
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
            Time::TimePointSeconds t = Time::GetTickCount ();
#endif

            {
                String_BufferedArray s ("");
                StressTest1_ (s);
            }

#if qPrintTimings
            t = Time::GetTickCount () - t;
            cout << "finished stress testing buffered strings  += ... time elapsed = " << t << endl;
            t = Time::GetTickCount ();
#endif

            {
                String_BufferedArray s ("");
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
        EXPECT_TRUE (s1.size () == 12);
        EXPECT_TRUE (String{s1}.size () == 12);
        EXPECT_TRUE (s1 == s2);
        EXPECT_TRUE (!(s1 != s2));
        EXPECT_TRUE (s1 + s1 == s2 + s2);
        EXPECT_TRUE ((s1 + s1).size () == s1.size () * 2);
        EXPECT_TRUE (s1[2] == 's');
        EXPECT_TRUE ('s' == s1[2].GetCharacterCode ());
        EXPECT_TRUE (s1.size () == 12);

        String s3;
        s3 += s1;
        s3 += s2;

        s1 += "\n";
        EXPECT_TRUE (s1.size () == 13);
    }

    GTEST_TEST (Foundation_Characters, Test1)
    {
        Debug::TraceContextBumper ctx{"Test1_"};
        /*
         * Some simple tests to start off with.
         */
        {
            EXPECT_TRUE (String{"a"}.length () == 1);
            EXPECT_TRUE (String{String{"fred"} + String{"joe"}}.size () == 7);

            EXPECT_TRUE (String{"fred"} + String{"joe"} == String{"fredjoe"});

            {
                String s1 = String{"test strings"};
                String s2 = String{"test strings"};
                Test2_Helper_ (s1, s2);
            }

            {
                String s1{"test strings"};
                String s2{"test strings"};

                EXPECT_TRUE (Character{'a'} == 'a');

                Test2_Helper_ (s1, s2);
            }

            {
                String s1 = String::FromStringConstant (L"test strings");
                String s2 = String::FromStringConstant ("test strings");
                Test2_Helper_ (s1, s2);
            }
        }
    }

    void Test2_ ()
    {
        Debug::TraceContextBumper ctx{"Test2_"};
        Test2Helpers_::StressTestStrings ();
#if 0
        Test2Helpers_::StressTestBufferedStrings ();
#endif
    }

    void Test3_ ()
    {
        Debug::TraceContextBumper ctx{"Test3_"};
        String                    t1;
        String                    t2 = t1;
        String                    t3 = "a";
        String                    t4 = "a";

        EXPECT_TRUE (t1 == "");
        EXPECT_TRUE (t1 == String{});
        EXPECT_TRUE (t1 == String{""});
        EXPECT_TRUE (t1 == t2);
        EXPECT_TRUE (t3 == "a");
        EXPECT_TRUE (t3 == String{"a"});
        EXPECT_TRUE (t4 == "a");
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 7)) ||                                                         \
    (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 10))
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wself-assign-overloaded\""); // explicitly assigning value of variable of type 'Stroika::Foundation::Characters::String' to itself
#endif
        t1 = t1;
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 7)) ||                                                         \
    (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 10))
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wself-assign-overloaded\"");
#endif
        EXPECT_TRUE (t1 == "");

        t1 += 'F';
        t1 += 'r';
        t1 += 'e';
        t1 += 'd';
        t1 += " Flintstone";
        EXPECT_TRUE (t1 == "Fred Flintstone");
        EXPECT_TRUE (String{"Fred Flintstone"} == t1);
        EXPECT_TRUE (String{"Fred Flintstone"} == t1);
        EXPECT_TRUE (t1 == String{"Fred Flintstone"});
        EXPECT_TRUE (t2 != "Fred Flintstone");
        EXPECT_TRUE (String{"Fred Flintstone"} != t2);
        EXPECT_TRUE (String{"Fred Flintstone"} != t2);
        EXPECT_TRUE (t2 != String{L"Fred Flintstone"});

        EXPECT_TRUE (t1.size () == 15);
        t1.erase (4);
        EXPECT_TRUE (t1.size () == 4);
        EXPECT_TRUE (t1 == "Fred");

        EXPECT_TRUE (t1[0] == 'F');
        EXPECT_TRUE (t1[1] == 'r');
        EXPECT_TRUE (t1[2] == 'e');
        EXPECT_TRUE (t1[3] == 'd');

        EXPECT_TRUE (t1[0] == 'F');
        EXPECT_TRUE (t1[1] == 'r');
        EXPECT_TRUE (t1[2] == 'e');
        EXPECT_TRUE (t1[3] == 'd');

        String a[10];
        EXPECT_TRUE (a[2] == "");
        a[3] = "Fred";
        EXPECT_TRUE (a[3] == "Fred");
        EXPECT_TRUE (a[2] != "Fred");
    }

    void Test4_ ()
    {
        Debug::TraceContextBumper ctx{"Test4_"};
        const wchar_t             frobaz[] = L"abc";

        String  t1;
        String  t3 = "a";
        String  t5 = String{frobaz};
        String* t6 = new String{"xyz"};
        delete (t6);

        t5 = t1;
        t1 = t5;
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 7)) ||                                                         \
    (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 10))
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wself-assign-overloaded\""); // explicitly assigning value of variable of type 'Stroika::Foundation::Characters::String' to itself
#endif
        t1 = t1;
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 7)) ||                                                         \
    (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 10))
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wself-assign-overloaded\"");
#endif
        EXPECT_TRUE (t1 == "");
        EXPECT_TRUE (t5 == "");

        t1 += 'F';
        t1 += 'r';
        t1 += 'e';
        t1 += 'd';
        t1 += " Flintstone";
        EXPECT_TRUE (t1 == "Fred Flintstone");
        t5 = t1.SubString (5, 5 + 10);
        EXPECT_TRUE (t5 == "Flintstone");
        EXPECT_TRUE (not t5.Find ("STONE").has_value ());
        EXPECT_TRUE (not t5.Contains ("SToNE"));
        EXPECT_TRUE (t5.Find ("STONE", eCaseInsensitive) == 5u);
        EXPECT_TRUE (t5.Contains ("SToNE", eCaseInsensitive));

        t1.erase (4);
        EXPECT_TRUE (t1.length () == 4);

        t5 = t1;
        t5.SetCharAt ('f', 0);
        t5.SetCharAt ('R', 1);
        t5.SetCharAt ('E', 2);
        t5.SetCharAt ('D', 3);

        EXPECT_TRUE (t5[0] == 'f');
        EXPECT_TRUE (t5[1] == 'R');
        EXPECT_TRUE (t5[2] == 'E');
        EXPECT_TRUE (t5[3] == 'D');
        EXPECT_TRUE (t5.Find ('f') == 0u);
        EXPECT_TRUE (t5.Find ("f") == 0u);
        EXPECT_TRUE (t5.Find ("fR") == 0u);
        EXPECT_TRUE (t5.Find ("fRE") == 0u);
        EXPECT_TRUE (t5.Find ("fRED") == 0u);
        EXPECT_TRUE (not t5.Find ("fRD").has_value ());
        EXPECT_TRUE (t5.Find ('R') == 1u);
        EXPECT_TRUE (t5.Find ('E') == 2u);
        EXPECT_TRUE (t5.Find ('D') == 3u);
        EXPECT_TRUE (t5.Find ("D") == 3u);

        EXPECT_TRUE (t5.RFind ('f') == 0u);
        EXPECT_TRUE (t5.RFind ('R') == 1u);
        EXPECT_TRUE (t5.RFind ('E') == 2u);
        EXPECT_TRUE (t5.RFind ('D') == 3u);
        EXPECT_TRUE (t5.RFind ("D") == 3u);
        EXPECT_TRUE (t5.RFind ("ED") == 2u);
        EXPECT_TRUE (t5.RFind ("RED") == 1u);
        EXPECT_TRUE (t5.RFind ("fRED") == 0u);
        EXPECT_TRUE (not t5.RFind ("fr").has_value ());
        EXPECT_TRUE (t5.RFind ("f") == 0u);

        t5.SetCharAt ('D', 0);
        t5.SetCharAt ('D', 1);
        t5.SetCharAt ('D', 2);
        t5.SetCharAt ('D', 3);
        EXPECT_TRUE (t5.Find ('D') == 0u);
        EXPECT_TRUE (t5.Find ("D") == 0u);
        EXPECT_TRUE (t5.RFind ('D') == 3u);
        EXPECT_TRUE (t5.RFind ("D") == 3u);

        EXPECT_TRUE (not t5.Find ('f').has_value ());
        EXPECT_TRUE (not t5.Find ("f").has_value ());
        EXPECT_TRUE (not t5.RFind ('f').has_value ());
        EXPECT_TRUE (not t5.RFind ("f").has_value ());

        EXPECT_TRUE (t5[0] == 'D');
        EXPECT_TRUE (t5[1] == 'D');
        EXPECT_TRUE (t5[2] == 'D');
        EXPECT_TRUE (t5[3] == 'D');
    }

    void Test5_ ()
    {
        Debug::TraceContextBumper ctx{"Test5_"};
        String                    arr[100];
        arr[3] = "fred";
        EXPECT_TRUE (arr[3] == "fred");
        String* l = new String[100];
        l[3]      = "FRED";
        EXPECT_TRUE (l[3] == "FRED");
        EXPECT_TRUE (l[99] == "");
        delete[] (l);
        size_t nSlots = 100;
        l             = new String[size_t (nSlots)];
        delete[] (l);
    }

    namespace Test6_PRIVATE_ {
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
            Debug::TraceContextBumper ctx{"Test6_Helper_"};
#if qPrintTimings
            constexpr int kRecurseDepth = 10;
#else
#ifdef __arm__
            constexpr bool kArm_ = true;
#else
            constexpr bool kArm_ = false;
#endif
            constexpr int kRecurseDepth = (kArm_ and Debug::kBuiltWithAddressSanitizer) ? 7 : 8; // reduction only needed with sanitizer on rasberrypi
#endif
            STRING testString = L"some dump test";
#if qPrintTimings
            cout << "\tTYPE=" << testMessage << ": Recursive build test with depth " << kRecurseDepth << endl;
            Time::TimePointSeconds t = Time::GetTickCount ();
#endif

            STRING s = Test6_Helper_<STRING> (testString, kRecurseDepth); // returns length 114688 for depth 6
            EXPECT_TRUE (s.length () == (ipow (4, kRecurseDepth) * 2 * testString.length ()));

#if qPrintTimings
            t = Time::GetTickCount () - t;
            cout << "\tfinished Recursive build test. Time elapsed = " << t << " length = " << s.length () << endl;
#endif
        }
    }
    void Test6_ ()
    {
        using namespace Test6_PRIVATE_;
        Debug::TraceContextBumper ctx{"Test6_"};
        Test6_Helper_<String> ("Characters::String");
        Test6_Helper_<wstring> ("std::wstring");
    }
    void Test7_Comparisons_ ()
    {
        Debug::TraceContextBumper ctx{"Test7_Comparisons_"};
        EXPECT_TRUE (String{"1"} <= String{"1"});
        EXPECT_TRUE ("1" <= String{"1"});
        EXPECT_TRUE (String{"1"} <= "1");
        EXPECT_TRUE (String{"1"} <= String{"10"});
        EXPECT_TRUE (not(String{"1"} > String{"10"}));
        EXPECT_TRUE (not(String{"1"} > String{"10"}));
        EXPECT_TRUE (not(String{"1"} >= String{"10"}));
        EXPECT_TRUE (String{"1"} < String{"10"});

        EXPECT_TRUE (String{"20"} > String{"11"});
        EXPECT_TRUE (String{"20"} >= String{"11"});
        EXPECT_TRUE (not(String{"20"} < String{"11"}));
        EXPECT_TRUE (not(String{"20"} <= String{"11"}));
        EXPECT_TRUE (String{"11"} < String{"20"});
        EXPECT_TRUE (String{"11"} <= String{"20"});
        EXPECT_TRUE (not(String{"11"} > String{"20"}));
        EXPECT_TRUE (not(String{"11"} >= String{"20"}));

        EXPECT_TRUE (String{"aac"} > String{"aab"});
        EXPECT_TRUE (String{"aac"} >= String{"aab"});
        EXPECT_TRUE (not(String{"aac"} < String{"aab"}));
        EXPECT_TRUE (not(String{"aac"} <= String{"aab"}));

        EXPECT_TRUE (String{"apple"} < String{"apples"});
        EXPECT_TRUE (String{"apple"} <= String{"apples"});
        EXPECT_TRUE (not(String{"apple"} > String{"apples"}));
        EXPECT_TRUE (not(String{"apple"} >= String{"apples"}));
    }

    void Test8_ReadOnlyStrings_ ()
    {
        Debug::TraceContextBumper ctx{"Test8_ReadOnlyStrings_"};
        // NOTE - THIS TESTS String_Constant
        //  using   String_Constant =   String_ExternalMemoryOwnership_ApplicationLifetime;
        String s = String::FromStringConstant ("fred");
        EXPECT_TRUE (s[0] == 'f');
        s.erase (3);
        EXPECT_TRUE (s[0] == 'f');
        EXPECT_TRUE (s.size () == 3);
        EXPECT_TRUE (s == "fre");
        s += "x";
        EXPECT_TRUE (s.size () == 4);
        EXPECT_TRUE (s[3] == 'x');
        EXPECT_TRUE (s == "frex");
        s = s.InsertAt ('x', 2);
        EXPECT_TRUE (s == "frxex");
        {
            wchar_t kZero[] = L"";
            s               = s.InsertAt (span{std::begin (kZero), std::begin (kZero)}, 0);
            EXPECT_TRUE (s == "frxex");
            s = s.InsertAt (span{std::begin (kZero), std::begin (kZero)}, 1);
            EXPECT_TRUE (s == "frxex");
            s = s.InsertAt (span{std::begin (kZero), std::begin (kZero)}, 5);
            EXPECT_TRUE (s == "frxex");
        }
    }

    void Test8_ExternalMemoryOwnershipStrings_ ()
    {
        Debug::TraceContextBumper ctx{"Test8_ExternalMemoryOwnershipStrings_"};
        String                    s = String::FromStringConstant ("fred");
        EXPECT_TRUE (s[0] == 'f');
        s.erase (3);
        EXPECT_TRUE (s[0] == 'f');
        EXPECT_TRUE (s.size () == 3);
        s += "x";
        EXPECT_TRUE (s.size () == 4);
        EXPECT_TRUE (s[3] == 'x');
        EXPECT_TRUE (s == "frex");
        s = s.InsertAt ('x', 2);
        EXPECT_TRUE (s == "frxex");
    }

    namespace {
        namespace Test9Support {
            template <typename STRING>
            void DoTest1 (STRING s)
            {
                STRING           t1 = s;
                constexpr size_t kMaxCount_{100}; // NOTE - see https://stroika.atlassian.net/browse/STK-996
                for (size_t i = 0; i < kMaxCount_; ++i) {
                    t1 += L"X";
                }
                STRING t2 = t1;
                if (t1 != t2) {
                    EXPECT_TRUE (false);
                }
            }
        }
    }
    void Test9_StringVersusStdCString_ ()
    {
        Debug::TraceContextBumper ctx{"Test9_StringVersusStdCString_"};
        // EMBELLISH THIS MORE ONCE WE HAVE TIMING SUPPORT WORKING - SO WE CNA COMPARE PERFORMANCE - AND COME UP WITH MORE REASONABLE TESTS
        //
        //      -- LGP 2011-09-01
        Test9Support::DoTest1<String> ("Hello");
        Test9Support::DoTest1<std::wstring> (L"Hello");
    }
    void Test10_ConvertToFromSTDStrings_ ()
    {
        Debug::TraceContextBumper ctx{"Test10_ConvertToFromSTDStrings_"};
        const wstring             kT1 = L"abcdefh124123985213129314234";
        String                    t1  = kT1;
        EXPECT_TRUE (t1.As<wstring> () == kT1);
        EXPECT_TRUE (t1 == kT1);
    }
}

namespace {
    void Test11_Trim_ ()
    {
        Debug::TraceContextBumper ctx{"Test11_Trim_"};
        const String              kT1 = "  abc";
        EXPECT_TRUE (kT1.RTrim () == kT1);
        EXPECT_TRUE (kT1.LTrim () == kT1.Trim ());
        EXPECT_TRUE (kT1.Trim () == "abc");
        EXPECT_TRUE (String{" abc "}.Trim () == "abc");

        EXPECT_TRUE (kT1.Trim ([] (Character c) -> bool { return c.IsAlphabetic (); }) == L"  ");
        EXPECT_TRUE (String{"/\n"}.Trim () == "/");
    }
}

namespace {
    void Test12_CodePageConverter_ ()
    {
        Debug::TraceContextBumper ctx{"Test12_CodePageConverter_"};
        wstring                   w = L"<PHRMode";
        using namespace Characters;
        using namespace Memory;
        EXPECT_TRUE ((CodeCvt<wchar_t>{WellKnownCodePages::kUTF8}.String2Bytes<string> (span{w}) == "<PHRMode"));
    }
}

namespace {
    void Test13_ToLowerUpper_ ()
    {
        Debug::TraceContextBumper ctx{"Test13_ToLowerUpper_"};
        String                    w = "Lewis";
        EXPECT_TRUE (w.ToLowerCase () == "lewis");
        EXPECT_TRUE (w.ToUpperCase () == "LEWIS");
        EXPECT_TRUE (w == "Lewis");
    }
}

namespace {
    void Test15_StripAll_ ()
    {
        Debug::TraceContextBumper ctx{"Test15_StripAll_"};
        String                    w = "Le wis";
        EXPECT_TRUE (w.StripAll ([] (Character c) -> bool { return c.IsWhitespace (); }) == "Lewis");

        w = "This is a very good test    ";
        EXPECT_TRUE (w.StripAll ([] (Character c) -> bool { return c.IsWhitespace (); }) == "Thisisaverygoodtest");
    }
}

namespace {
    void Test16_Format_ ()
    {
        Debug::TraceContextBumper ctx{"Test16_Format_"};
        EXPECT_TRUE (CString::Format ("%d", 123) == "123");
        EXPECT_TRUE (CString::Format ("%s", "123") == "123");

        // SUBTLE - this would FAIL with vsnprintf on gcc -- See docs in Header for
        // Format string
        EXPECT_TRUE (CString::Format (L"%s", L"123") == L"123");
        EXPECT_TRUE (Format (L"%s", L"123") == L"123");

        EXPECT_TRUE (Format (L"%20s", L"123") == L"                 123");
        EXPECT_TRUE (Format (L"%.20s", L"123") == L"123");

        for (size_t i = 1; i < 1000; ++i) {
            String format = Format (L"%%%ds", static_cast<int> (i));
            EXPECT_TRUE (Format (format.c_str (), L"x").length () == i);
        }
        EXPECT_TRUE (Characters::Format (L"%d.%d%s%s", 1, 0, L"a", L"1x") == L"1.0a1x"); // 2 conseq %s%s POSIX bug fixed 2014-01-22
    }
}

namespace {
    namespace Test17_Private_ {
        void Test17_Find_ ()
        {
            EXPECT_TRUE (String{"abc"}.Find ("b") == 1u);
            EXPECT_TRUE (not String{"abc"}.Find ("x").has_value ());
            EXPECT_TRUE (not String{"abc"}.Find ("b", 2).has_value ());
        }
        void Test17_FindEach_ ()
        {
            {
                // @todo - Either have FindEach return Sequence or fix vector stuff!!!
                EXPECT_TRUE (String{"abc"}.FindEach ("b") == Containers::Sequence<size_t> ({1}).As<vector<size_t>> ());
            }
            // @todo - Either have FindEach return Sequence or fix vector stuff!!!
            EXPECT_TRUE (String{"01-23-45-67-89"}.FindEach ("-") == Containers::Sequence<size_t> ({2, 5, 8, 11}).As<vector<size_t>> ());
            // @todo - Either have FindEach return Sequence or fix vector stuff!!!
            EXPECT_TRUE (String{"AAAA"}.FindEach ("AA") == Containers::Sequence<size_t> ({0, 2}).As<vector<size_t>> ());
        }
        void Test17_ReplaceAll_ ()
        {
            EXPECT_TRUE (String{"01-23-45-67-89"}.ReplaceAll ("-", "") == "0123456789");
            EXPECT_TRUE (String{"01-23-45-67-89"}.ReplaceAll ("-", "x") == "01x23x45x67x89");
            EXPECT_TRUE (String{"01-23-45-67-89"}.ReplaceAll ("-", "--") == "01--23--45--67--89");
        }
        void Test17_Replace_ ()
        {
            EXPECT_TRUE (String{u8"abc"}.Replace (1, 2, "B") == "aBc");
        }
        void Test17_RegExpMatch_ ()
        {
            {
                for (String i : Sequence<String>{"01-23-45-67-89", "", "a"}) {
                    EXPECT_TRUE (not i.Matches (RegularExpression::kNONE));
                    EXPECT_TRUE (i.Matches (RegularExpression::kAny));
                }
            }
            {
                // Test from String::Match () docs
                static const RegularExpression kSonosRE_{"([0-9.:]*)( - .*)"_RegEx};
                static const String            kTestStr_{"192.168.244.104 - Sonos Play:5"};
                EXPECT_TRUE (kTestStr_.Matches (kSonosRE_));
                optional<String> match1;
                EXPECT_TRUE (kTestStr_.Matches (kSonosRE_, &match1) and match1 == "192.168.244.104");
                optional<String> match2;
                EXPECT_TRUE (kTestStr_.Matches (kSonosRE_, &match1, &match2) and match1 == "192.168.244.104" and match2 == " - Sonos Play:5");
            }
        }
        void Test17_RegExp_Search_ ()
        {
            {
                RegularExpression regExp{"abc"};
                String            testStr2Search = String{"abc"};
                EXPECT_TRUE (testStr2Search.FindEach (regExp).size () == 1);
                EXPECT_TRUE ((testStr2Search.FindEach (regExp)[0] == pair<size_t, size_t> (0, 3)));
            }
            {
                // Test replace crlfs
                String stringWithCRLFs = "abc\r\ndef\r\n";
#if !qCompilerAndStdLib_regexp_Compile_bracket_set_Star_Buggy
                String replaced = stringWithCRLFs.ReplaceAll (RegularExpression{"[\r\n]*"}, "");
                EXPECT_TRUE (replaced == "abcdef");
#endif
            }
#if 0
// not sure why this didn't work! -
            {
                String  abc     =   String {"abc"};
                String  abcabc  =   String {"abc abc"};
                EXPECT_TRUE (abcabc.Search (abc).size () == 2);
                EXPECT_TRUE ((abcabc.Search (abc)[0] == pair<size_t, size_t> (0, abc.length ())));
                EXPECT_TRUE ((abcabc.Search (abc)[1] == pair<size_t, size_t> (3, abc.length ())));
            }
#endif
        }
        void Test17_RegExp_ ()
        {
            Test17_Find_ ();
            Test17_FindEach_ ();
            Test17_RegExp_Search_ ();
            Test17_RegExpMatch_ ();
            Test17_Replace_ ();
            EXPECT_TRUE ((String{"Hello world"}.Find (RegularExpression{"ello"}) == pair<size_t, size_t> (1, 5)));
            vector<RegularExpressionMatch> r = String{"<h2>Egg prices</h2>"}.FindEachMatch (RegularExpression{"<h(.)>([^<]+)"});
            EXPECT_TRUE (r.size () == 1 and r[0].GetSubMatches ()[0] == "2" and r[0].GetSubMatches ()[1] == "Egg prices");
            EXPECT_TRUE (String{"Hello world"}.ReplaceAll (RegularExpression{"world"}, "Planet") == "Hello Planet");
        }
        void docsTests_ ()
        {
            {
                const String kTest_ = String::FromStringConstant ("a=b");
                const String kLbl2LookFor_{"a="_k};
                String       tmp;
                if (kTest_.Find (kLbl2LookFor_)) {
                    tmp = String{kTest_.SubString (kLbl2LookFor_.length ())};
                }
                EXPECT_TRUE (tmp == "b");
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

                        for (const auto& i : color_match) {
                            std::cout << i << '\n';
                        }
                    }
                }
#endif
                {
                    const String            kTest_{"a=b,"_k};
                    const RegularExpression kRE_{"a=(.*)"};
                    Sequence<String>        tmp1{kTest_.FindEachString (kRE_)};
                    EXPECT_TRUE (tmp1.size () == 1 and tmp1[0] == "a=b,");
                    Sequence<RegularExpressionMatch> tmp2{kTest_.FindEachMatch (kRE_)};
                    EXPECT_TRUE (tmp2.size () == 1 and tmp2[0].GetFullMatch () == "a=b," and tmp2[0].GetSubMatches () == Sequence<String>{"b,"});
                }
                {
                    const String            kTest_ = String::FromStringConstant ("a=b,");
                    const RegularExpression kRE_{"a=(.*),"};
                    Sequence<String>        tmp1{kTest_.FindEachString (kRE_)};
                    EXPECT_TRUE (tmp1.size () == 1 and tmp1[0] == "a=b,");
                    Sequence<RegularExpressionMatch> tmp2{kTest_.FindEachMatch (kRE_)};
                    EXPECT_TRUE (tmp2.size () == 1 and tmp2[0].GetFullMatch () == "a=b," and tmp2[0].GetSubMatches () == Sequence<String>{"b"});
                }

                {
                    const String            kTest_ = String::FromStringConstant ("a=b,");
                    const RegularExpression kRE_{"a=(.*)[, ]"};
                    Sequence<String>        tmp1{kTest_.FindEachString (kRE_)};
                    EXPECT_TRUE (tmp1.size () == 1 and tmp1[0] == "a=b,");
                    Sequence<RegularExpressionMatch> tmp2{kTest_.FindEachMatch (kRE_)};
                    EXPECT_TRUE (tmp2.size () == 1 and tmp2[0].GetFullMatch () == "a=b," and tmp2[0].GetSubMatches () == Sequence<String>{"b"});
                }
                {
                    const String            kTest_{"a=b, c=d"sv};
                    const RegularExpression kRE_{"(.)=(.)"};
                    EXPECT_TRUE ((kTest_.FindEachString (kRE_) == vector<String>{"a=b", "c=d"}));
                }
            }
        }
    }

    void Test17_Find_ ()
    {
        Debug::TraceContextBumper ctx{"Test17_Find_"};
        Test17_Private_::Test17_ReplaceAll_ ();
        Test17_Private_::Test17_ReplaceAll_ ();
        Test17_Private_::Test17_RegExp_ ();
        Test17_Private_::docsTests_ ();
    }
}

namespace {
    void Test18_Compare_ ()
    {
        Debug::TraceContextBumper ctx{"Test18_Compare_"};
        const String              kHELLOWorld = String{"Hello world"};
        EXPECT_TRUE ((String::ThreeWayComparer{eWithCase}(kHELLOWorld, kHELLOWorld) == 0));
        EXPECT_TRUE ((String::ThreeWayComparer{eWithCase}(kHELLOWorld, String{"Hello world"}) == 0));
        EXPECT_TRUE ((String::ThreeWayComparer{eWithCase}(kHELLOWorld, kHELLOWorld.ToLowerCase ()) <= 0));
        EXPECT_TRUE ((String::ThreeWayComparer{eCaseInsensitive}(kHELLOWorld, kHELLOWorld.ToLowerCase ()) == 0));
        EXPECT_TRUE ((String::ThreeWayComparer{eCaseInsensitive}("fred", "fredy") < 0));
        EXPECT_TRUE ((String::ThreeWayComparer{eCaseInsensitive}("fred", "Fredy") < 0));
        EXPECT_TRUE ((String::ThreeWayComparer{eCaseInsensitive}("Fred", "fredy") < 0));
        EXPECT_TRUE ((String::ThreeWayComparer{eWithCase}("fred", "fredy") < 0));
        EXPECT_TRUE ((String::ThreeWayComparer{eWithCase}("fred", "Fredy") > 0));
        EXPECT_TRUE ((String::ThreeWayComparer{eWithCase}("Fred", "fredy") < 0));
    }
}

namespace {
    void Test19_ConstCharStar_ ()
    {
        Debug::TraceContextBumper ctx{"Test19_ConstCharStar_"};
        EXPECT_TRUE (wcscmp (String{"fred"}.c_str (), L"fred") == 0);
        EXPECT_TRUE (wcscmp (String{"0123456789abcde"}.c_str (), L"0123456789abcde") == 0);                                   // 15 chars
        EXPECT_TRUE (wcscmp (String{"0123456789abcdef"}.c_str (), L"0123456789abcdef") == 0);                                 // 16 chars
        EXPECT_TRUE (wcscmp (String{"0123456789abcdef0123456789abcde"}.c_str (), L"0123456789abcdef0123456789abcde") == 0);   // 31 chars
        EXPECT_TRUE (wcscmp (String{"0123456789abcdef0123456789abcdef"}.c_str (), L"0123456789abcdef0123456789abcdef") == 0); // 32 chars
        {
            String tmp = "333";
            EXPECT_TRUE (wcscmp (tmp.c_str (), L"333") == 0);
            tmp = "Barny";
            EXPECT_TRUE (wcscmp (tmp.c_str (), L"Barny") == 0);
            tmp.SetCharAt ('c', 2);
            EXPECT_TRUE (wcscmp (tmp.c_str (), L"Bacny") == 0);
            String bumpRefCnt = tmp;
            tmp.SetCharAt ('d', 2);
            EXPECT_TRUE (wcscmp (tmp.c_str (), L"Badny") == 0);
        }
    }
}

namespace {
    void Test20_CStringHelpers_ ()
    {
        Debug::TraceContextBumper ctx{"Test20_CStringHelpers_"};
        EXPECT_TRUE (CString::Length ("hi") == 2);
        EXPECT_TRUE (CString::Length (L"hi") == 2);
        {
            // This test was mostly to confirm the false-warning from valgrind
            // SEE THIS VALGRIND SUPPRESSION - wcscmp_appears_to_generate_false_warnings_gcc48_ubuntu
            wchar_t buf[3] = {'1', '2', 0};
            EXPECT_TRUE (::wcscmp (buf, L"12") == 0);
        }
        {
            char buf[3] = {'1', '1', '1'};
            CString::Copy (buf, Memory::NEltsOf (buf), "3");
            EXPECT_TRUE (::strcmp (buf, "3") == 0);
        }
        {
            wchar_t buf[3] = {'1', '1', '1'};
            CString::Copy (buf, Memory::NEltsOf (buf), L"3");
            EXPECT_TRUE (::wcscmp (buf, L"3") == 0);
        }
        {
            char buf[3] = {'1', '1', '1'};
            CString::Copy (buf, Memory::NEltsOf (buf), "12345");
            EXPECT_TRUE (::strcmp (buf, "12") == 0);
        }
        {
            wchar_t buf[3] = {'1', '1', '1'};
            CString::Copy (buf, Memory::NEltsOf (buf), L"12345");
            EXPECT_TRUE (::wcscmp (buf, L"12") == 0);
        }
    }
}

namespace {
    namespace Test21_StringNumericConversions_Helper_ {
        template <typename FLOAT_TYPE>
        void Verify_FloatStringRoundtripNearlyEquals_ (FLOAT_TYPE l)
        {
            if constexpr (qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy) {
                EXPECT_TRUE (Math::NearlyEquals (l, Characters::FloatConversion::ToFloat<FLOAT_TYPE> (FloatConversion::ToString (
                                                        l, FloatConversion::Precision{numeric_limits<FLOAT_TYPE>::digits10 + 2}))));
            }
            else {
                EXPECT_TRUE (Math::NearlyEquals (l, Characters::FloatConversion::ToFloat<FLOAT_TYPE> (FloatConversion::ToString (
                                                        l, FloatConversion::Precision{numeric_limits<FLOAT_TYPE>::digits10 + 1}))));
            }
        }
    }
    void Test21_StringNumericConversions_ ()
    {
        Debug::TraceContextBumper ctx{"Test21_StringNumericConversions_"};
        {
            EXPECT_TRUE (CString::String2Int<int> ("-3") == -3);
            EXPECT_TRUE (CString::String2Int<int> ("3") == 3);
            EXPECT_TRUE (CString::String2Int<int> (wstring{L"3"}) == 3);
            EXPECT_TRUE (String2Int<int> (String{"3"}) == 3);
        }
        {
            EXPECT_TRUE (CString::String2Int<int> ("") == 0);
            EXPECT_TRUE (String2Int<int> ("") == 0);
            EXPECT_TRUE (CString::String2Int<int> (wstring{L""}) == 0);
            EXPECT_TRUE (String2Int<int> (String{}) == 0);
            EXPECT_TRUE (CString::String2Int<int> ("     ") == 0);
        }
        {
            EXPECT_TRUE (CString::HexString2Int ("") == 0);
            EXPECT_TRUE (CString::HexString2Int (L"") == 0);
            EXPECT_TRUE (CString::HexString2Int (wstring{L""}) == 0);
            EXPECT_TRUE (HexString2Int (String{}) == 0);
            EXPECT_TRUE (CString::HexString2Int ("     ") == 0);
        }
        {
            // https://stroika.atlassian.net/browse/STK-966
            auto doTest = [] (const wstring& w, long double wVal) {
                StringBuilder sb1;
                sb1 << w;
                Memory::StackBuffer<wchar_t> ignoreBuf1;
                span<const wchar_t>          tmpData1 = sb1.GetData (&ignoreBuf1);
                double                       ff2      = Characters::FloatConversion::ToFloat<double> (tmpData1);
                DbgTrace (L"*in reader:  ff2-> %f", ff2);
                EXPECT_TRUE (Math::NearlyEquals (ff2, wVal, 0.001));
            };
            for (int i = 1; i < 10; ++i) {
                doTest (L"405.1", 405.1);
                doTest (L"4405.2", 4405.2);
                doTest (L"44905.3", 44905.3);
            }
        }
        {
            EXPECT_TRUE (isnan (FloatConversion::ToFloat<double> (String{})));
            EXPECT_TRUE (isnan (FloatConversion::ToFloat (string{})));
            EXPECT_TRUE (isnan (FloatConversion::ToFloat<double> (wstring{})));
            EXPECT_TRUE (isnan (FloatConversion::ToFloat ("")));
            EXPECT_TRUE (isnan (FloatConversion::ToFloat (wstring{L""})));
            EXPECT_TRUE (isnan (FloatConversion::ToFloat<double> (String{})));
            EXPECT_TRUE (isnan (FloatConversion::ToFloat ("     ")));
            EXPECT_TRUE (isnan (FloatConversion::ToFloat<double> (L"-1.#INF000000000000"))); // MSFT sometimes generates these but they arent legal INF values!
            {
                String remainder;
                EXPECT_TRUE (FloatConversion::ToFloat<double> (L"-INF  f", &remainder) == FloatConversion::ToFloat<double> (L"-INF"));
                EXPECT_TRUE (remainder == L"  f");
            }
            {
                // leading/trailing space (or other bad trailing characters) produces NAN for no &remainder overload
                EXPECT_TRUE (isinf (FloatConversion::ToFloat<double> (L"INF")));
                EXPECT_TRUE (not isnan (FloatConversion::ToFloat<double> (L"INF")));
                EXPECT_TRUE (isinf (FloatConversion::ToFloat<double> (L"iNF")));
                EXPECT_TRUE (isnan (FloatConversion::ToFloat<double> (L" INF")));
                EXPECT_TRUE (isnan (FloatConversion::ToFloat<double> (L"INF ")));
                // leading/trailing space produces NAN for WITH &remainder overload produces right value
                String remainder;
                EXPECT_TRUE (FloatConversion::ToFloat<double> (L"-INF  f", &remainder) == FloatConversion::ToFloat<double> (L"-INF"));
                EXPECT_TRUE (remainder == L"  f");
                EXPECT_TRUE (FloatConversion::ToFloat<double> (L" INF  f", &remainder) == FloatConversion::ToFloat<double> (L"INF"));
                EXPECT_TRUE (remainder == L"  f");
                EXPECT_TRUE (FloatConversion::ToFloat<double> (L" 3.0 ", &remainder) == 3.0);
                EXPECT_TRUE (remainder == L" ");
            }
            EXPECT_TRUE (isnan (FloatConversion::ToFloat ("-1.#INF000000000000")));
            EXPECT_TRUE (isnan (FloatConversion::ToFloat<double> (L"1.#INF000000000000")));
            EXPECT_TRUE (isnan (FloatConversion::ToFloat ("1.#INF000000000000")));
            EXPECT_TRUE (isinf (FloatConversion::ToFloat<double> (L"INF")));
            EXPECT_TRUE (isinf (FloatConversion::ToFloat<double> (L"INFINITY")));
            EXPECT_TRUE (isinf (FloatConversion::ToFloat<double> (L"-INF")));
            EXPECT_TRUE (isinf (FloatConversion::ToFloat<double> (L"-INFINITY")));
            EXPECT_TRUE (isinf (FloatConversion::ToFloat<double> (L"+INF")));
            EXPECT_TRUE (isinf (FloatConversion::ToFloat<double> (L"+INFINITY")));

            {
                /*
                 *  numeric conversions I18n
                 */
                {
                    auto checkJapaneseNumbers = [] () {
                        // From https://www.fluentin3months.com/japanese-numbers/
                        const String kNumber8a_ = L"\u306F\u3061";       //  はち
                        const String kNumber8b_ = L"\u516B";             //  八
                        const String kNumber8c_ = L"\u3084\u3063\u3064"; //  やっつ
                        const String kNumber8d_ = L"\u516B\u3064";       //  八つ

                        [[maybe_unused]] auto xa = FloatConversion::ToFloat<double> (kNumber8a_);
                        [[maybe_unused]] auto xb = FloatConversion::ToFloat<double> (kNumber8b_);
                        [[maybe_unused]] auto xc = FloatConversion::ToFloat<double> (kNumber8c_);
                        [[maybe_unused]] auto xd = FloatConversion::ToFloat<double> (kNumber8d_);
                        // https://www.intmath.com/blog/mathematics/unicode-characters-for-chinese-and-japanese-numbers-1482
                        const String          kNumber1_ = L"\u4e00"; //      一
                        const String          kNumber2_ = L"\u4e8c"; //      二
                        [[maybe_unused]] auto x1        = FloatConversion::ToFloat<double> (kNumber1_);
                        [[maybe_unused]] auto x2        = FloatConversion::ToFloat<double> (kNumber2_);

                        // This test is an ABJECT FAILURE - https://stroika.atlassian.net/browse/STK-747 but LOW PRIORITY
                    };
                    checkJapaneseNumbers ();
                    if (auto ln = Configuration::FindLocaleNameQuietly (L"ja", L"JP")) {
                        Configuration::ScopedUseLocale tmpLocale{locale{ln->AsNarrowSDKString ().c_str ()}};
                        checkJapaneseNumbers ();
                    }
                }

                {
                    // European comma/period confusion
                    // BROKEN - see https://stroika.atlassian.net/browse/STK-748
                    // See https://docs.oracle.com/cd/E19455-01/806-0169/overview-9/index.html
                    if (auto ln = Configuration::FindLocaleNameQuietly (L"en", L"US")) {
                        Configuration::ScopedUseLocale tmpLocale{locale{ln->AsNarrowSDKString ().c_str ()}};
                        EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat<double> ("100.1"), 100.1));
                        [[maybe_unused]] auto i2 = FloatConversion::ToFloat<double> ("967,295.01");
                        //EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat<double> (L"967,295.01") , 967295.01));     -- https://stroika.atlassian.net/browse/STK-748
                    }
                    if (auto ln = Configuration::FindLocaleNameQuietly (L"es", L"ES")) {
                        Configuration::ScopedUseLocale tmpLocale{locale{ln->AsNarrowSDKString ().c_str ()}};
                        //DbgTrace ("using locale %s", locale{}.name ().c_str ());
                        //DbgTrace (L"decimal separator: %c", std::use_facet<std::numpunct<char>> (locale{}).decimal_point ());
                        EXPECT_TRUE (std::use_facet<std::numpunct<char>> (locale{}).decimal_point () == ',');
                        EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat<double> (L"100,1"), 100.1));
                        [[maybe_unused]] auto i2 = FloatConversion::ToFloat<double> (L"967.295,01");
                        //EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat<double> (L"967.295,01") , 967295.01)); -- https://stroika.atlassian.net/browse/STK-748
                    }
                }
            }

            EXPECT_TRUE (isinf (FloatConversion::ToFloat (L"INF")));
            EXPECT_TRUE (isinf (FloatConversion::ToFloat (L"INFINITY")));
            EXPECT_TRUE (isinf (FloatConversion::ToFloat (L"-INF")));
            EXPECT_TRUE (isinf (FloatConversion::ToFloat (L"-INFINITY")));
            EXPECT_TRUE (isinf (FloatConversion::ToFloat (L"+INF")));
            EXPECT_TRUE (isinf (FloatConversion::ToFloat (L"+INFINITY")));
        }
        {
            // roundtrip lossless
            Test21_StringNumericConversions_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<float>::min ());
            Test21_StringNumericConversions_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<float>::max ());
            Test21_StringNumericConversions_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<float>::lowest ());
            Test21_StringNumericConversions_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<double>::min ());
            Test21_StringNumericConversions_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<double>::max ());
            Test21_StringNumericConversions_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<double>::lowest ());
            Test21_StringNumericConversions_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<long double>::min ());
            Test21_StringNumericConversions_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<long double>::max ());
            Test21_StringNumericConversions_Helper_::Verify_FloatStringRoundtripNearlyEquals_ (numeric_limits<long double>::lowest ());
        }
        {
            EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat ("3"), 3.0));
            EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat (L"3"), 3.0));
            EXPECT_TRUE (Math::NearlyEquals (Characters::FloatConversion::ToFloat (L"3"), 3.0));
            EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat ("-44.4"), -44.4));
            EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat (L"-44.4"), -44.4));
            EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat<double> (L"-44.4"), -44.4));
            EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat<double> (String{L"44.4333"}), 44.4333));
        }
        [[maybe_unused]] auto runLocaleIndepTest = [] () {
            EXPECT_TRUE (FloatConversion::ToString (3000.5) == "3000.5");
            EXPECT_TRUE (FloatConversion::ToString (30000.5) == "30000.5");
        };
        {
            // Verify change of locale has no effect on results
            locale prevLocale = locale::global (locale{"C"});
            runLocaleIndepTest ();
            locale::global (prevLocale);
        }
        {
            // Verify change of locale has no effect on results
            Configuration::ScopedUseLocale tmpLocale{Configuration::FindNamedLocale ("en", "us")};
            runLocaleIndepTest ();
        }
        {
            EXPECT_TRUE (String2Int ("0587:c413:5500:0000:0000:0000:0001]:60000") == 0);
        }
    }
}

namespace {
    void Test22_StartsWithEndsWithMatch_ ()
    {
        Debug::TraceContextBumper ctx{"Test22_StartsWithEndsWithMatch_"};
        EXPECT_TRUE (String{"abc"}.Matches (RegularExpression{"abc"}));
        EXPECT_TRUE (not(String{"abc"}.Matches (RegularExpression{"bc"})));
        EXPECT_TRUE (String{"abc"}.Matches (RegularExpression{".*bc"}));
        EXPECT_TRUE (not String{"abc"}.Matches (RegularExpression{"b.*c"}));
        EXPECT_TRUE (not String{"Hello world"}.Matches (RegularExpression{"ello"}));
        EXPECT_TRUE (String{"abc"}.StartsWith ("AB", eCaseInsensitive));
        EXPECT_TRUE (not String{"abc"}.StartsWith ("AB", eWithCase));
        EXPECT_TRUE (String{"abc"}.EndsWith ("bc", eCaseInsensitive));
        EXPECT_TRUE (String{"abc"}.EndsWith ("bc", eWithCase));
        EXPECT_TRUE (String{"abc"}.EndsWith ("BC", eCaseInsensitive));
        EXPECT_TRUE (not String{"abc"}.EndsWith ("BC", eWithCase));
    }
}

namespace {
    namespace Test23_PRIVATE_ {
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
    }
    void Test23_FormatV_ ()
    {
        using namespace Test23_PRIVATE_;
        Debug::TraceContextBumper ctx{"Test23_FormatV_"};
        EXPECT_TRUE (Test23_help1_HELPER (L"joe%sx", L"1") == L"joe1x");
    }
}

namespace {
    void Test24_Float2String ()
    {
        Debug::TraceContextBumper ctx{"Test24_Float2String"};
        EXPECT_TRUE (FloatConversion::ToString (0.0) == "0");
        EXPECT_TRUE (FloatConversion::ToString (3000.5) == "3000.5");
        EXPECT_TRUE (FloatConversion::ToString (3000.500) == "3000.5");
        EXPECT_TRUE (FloatConversion::ToString (3.1234, Characters::FloatConversion::Precision{2}) == "3.1");
        EXPECT_TRUE (FloatConversion::ToString (3.1234, Characters::FloatConversion::Precision{3}) == "3.12");
        EXPECT_TRUE (FloatConversion::ToString (31.234, Characters::FloatConversion::Precision{3}) == "31.2");
        EXPECT_TRUE (FloatConversion::ToString (30707548160.0) == "3.07075e+10");
    }
}

namespace {
    void Test25_RemoveAt_ ()
    {
        Debug::TraceContextBumper ctx{"Test25_RemoveAt_"};
        String                    x = "123";
        x                           = x.RemoveAt (1);
        EXPECT_TRUE (x == "13");
        x = x.RemoveAt (0, 2);
        EXPECT_TRUE (x.empty ());
    }
}

namespace {
    void Test26_Iteration_ ()
    {
        Debug::TraceContextBumper ctx{"Test26_Iteration_"};
        {
            String x = "123";
            int    i = 0;
            for (auto c : x) {
                EXPECT_TRUE (c == x[i]);
                ++i;
            }
            EXPECT_TRUE (i == 3);
        }
    }
}

namespace {
    void Test27_Repeat_ ()
    {
        Debug::TraceContextBumper ctx{"Test27_Repeat_"};
        {
            String x;
            String r = x.Repeat (5);
            EXPECT_TRUE (r.length () == 0);
        }
        {
            String x = "123";
            String r = x.Repeat (3);
            EXPECT_TRUE (r.length () == 9);
            EXPECT_TRUE (r.SubString (3, 6) == "123");
        }
    }
}

namespace {
    void Test28_ReplacementForStripTrailingCharIfAny_ ()
    {
        Debug::TraceContextBumper ctx{"Test28_ReplacementForStripTrailingCharIfAny_"};
        auto StripTrailingCharIfAny = [] (const String& s, const Character& c) -> String { return s.EndsWith (c) ? s.SubString (0, -1) : s; };
        EXPECT_TRUE (StripTrailingCharIfAny ("xxx", '.') == "xxx");
        EXPECT_TRUE (StripTrailingCharIfAny ("xxx.", '.') == "xxx");
        EXPECT_TRUE (StripTrailingCharIfAny ("xxx..", '.') == "xxx.");
    }
}

namespace {
    void Test29_StringWithSequenceOfCharacter_ ()
    {
        Debug::TraceContextBumper ctx{"Test29_StringWithSequenceOfCharacter_"};
        {
            String              initialString = "012345";
            Sequence<Character> s1            = Sequence<Character> (initialString); // THIS NEEDS TO BE MORE SEEMLESS
            s1.SetAt (3, 'E');
            EXPECT_TRUE (String{s1} == "012E45");
        }
    }
}

namespace {
    void Test30_LimitLength_ ()
    {
        Debug::TraceContextBumper ctx{"Test30_LimitLength_"};
        if constexpr (qCompiler_vswprintf_on_elispisStr_Buggy) {
            EXPECT_TRUE (String{"12345"}.LimitLength (3) == "...");
        }
        else {
            EXPECT_TRUE (String{"12345"}.LimitLength (3) == L"12\u2026");
        }
        EXPECT_TRUE (String{"12345"}.LimitLength (5) == "12345");
    }
}

namespace {
    void Test31_OperatorINSERT_ostream_ ()
    {
        Debug::TraceContextBumper ctx{"Test31_OperatorINSERT_ostream_"};
        wstringstream             out;
        out << String{"abc"};
        EXPECT_TRUE (out.str () == L"abc");
    }
}

namespace {
    void Test32_StringBuilder_ ()
    {
        Debug::TraceContextBumper ctx{"Test32_StringBuilder_"};
        {
            StringBuilder out;
            out << L"hi mom";
            EXPECT_TRUE (out.str () == "hi mom");
            out += ".";
            EXPECT_TRUE (out.str () == "hi mom.");
            EXPECT_TRUE (out.As<String> () == "hi mom.");
            EXPECT_TRUE (out.As<wstring> () == L"hi mom.");
        }
        {
            StringBuilder out{"x"};
            EXPECT_TRUE (out.As<String> () == "x");
        }
    }
}

namespace {
    void Test33_Append_ ()
    {
        Debug::TraceContextBumper ctx{"Test33_Append_"};
        String                    result;
        Character                 buf[]{'a', 'b', 'c', 'd'};
        for (int i = 0; i < 10; ++i) {
            result.Append (std::begin (buf), std::begin (buf) + Memory::NEltsOf (buf));
        }
        EXPECT_TRUE (result.size () == 4 * 10);
        EXPECT_TRUE (result == "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcd");
        Verify (L"a" + String{} == "a"); // we had bug in v2.0a100 and earlier with null string on RHS of operator+
    }
}

namespace {
    void Test44_LocaleUNICODEConversions_ ()
    {
        Debug::TraceContextBumper ctx{"Test44_LocaleUNICODEConversions_"};
        EXPECT_TRUE (String{"abcdefgjij"}.AsNarrowSDKString () == "abcdefgjij"); // Failed due to bug in CodePageConverter::MapFromUNICODE before v3.0d2
        auto testRoundtrip = [] (const char* localName, const string& localMBString, const wstring& wideStr) {
            bool initializedLocale = false;
            try {
                locale l{localName};
                initializedLocale = true;
                EXPECT_TRUE (String::FromNarrowString (localMBString, l) == wideStr);
                EXPECT_TRUE (String{wideStr}.AsNarrowString (l) == localMBString);
            }
            catch (const runtime_error&) {
                // https://en.cppreference.com/w/cpp/locale/locale/locale says must throw std::runtime_error if invalid locale
                // if no such locale, just skip the test...
                DbgTrace (L"e=%s", Characters::ToString (current_exception ()).c_str ());
#if qCompilerAndStdLib_locale_utf8_string_convert_Buggy
// sigh - fails to convert unicode characters
#else
                EXPECT_TRUE (not initializedLocale);                                             // else means throw from conversion which would be bad
#endif
            }
            catch (...) {
                DbgTrace (L"error: %s", Characters::ToString (current_exception ()).c_str ());
                EXPECT_TRUE (false); // I think docs say must throw std::runtime_error
            }
        };
        //testRoundtrip ("en_US.utf8", u8"z\u00df\u6c34\U0001d10b", L"zß水𝄋");
        testRoundtrip ("C", "fred", L"fred");
        testRoundtrip ("en_US.utf8", "\x7a\xc3\x9f\xe6\xb0\xb4\xf0\x9d\x84\x8b", L"zß水𝄋");
    }
}

namespace {
    void Test45_Tokenize_ ()
    {
        Debug::TraceContextBumper ctx{"Test45_Tokenize_"};
        using Containers::Set;
        {
            String           t{"ABC DEF G"};
            Sequence<String> tt{t.Tokenize ()};
            EXPECT_TRUE (tt.size () == 3);
            EXPECT_TRUE (tt[1] == "DEF");
        }
        {
            String t{"foo=   7"};
            auto   tt = t.Tokenize (Containers::Set<Character>{'='});
            EXPECT_TRUE (tt.length () == 2);
            EXPECT_TRUE (tt[0] == "foo");
            EXPECT_TRUE (tt[1] == "7");
        }
        {
            String t{"           \n foo=   7"};
            auto   tt = t.Tokenize ({'='});
            EXPECT_TRUE (tt.length () == 2);
            EXPECT_TRUE (tt[1] == "7");
        }
        {
            String t{"MemTotal:        3082000 kB"};
            auto   tt = t.Tokenize ({':', ' ', '\t'});
            EXPECT_TRUE (tt.length () == 3);
            EXPECT_TRUE (tt[0] == "MemTotal");
            EXPECT_TRUE (tt[1] == "3082000");
            EXPECT_TRUE (tt[2] == "kB");
        }
    }
}

namespace {
    void Test46_CompareLHSRHS_ ()
    {
        Debug::TraceContextBumper ctx{"Test46_CompareLHSRHS_"};
        const wchar_t*            i = L"One";
        Characters::String        n = "Two";
        if (i == n) {
            EXPECT_TRUE (false);
        }
        if (n == i) {
            EXPECT_TRUE (false);
        }
        EXPECT_TRUE (i != n);
        EXPECT_TRUE (n != i);
    }
}

namespace {
    void Test47_SubString_ ()
    {
        Debug::TraceContextBumper ctx{"Test47_SubString_"};
        {
            String tmp{"This is good"};
            EXPECT_TRUE (tmp.SubString (5) == "is good");
        }
        {
            const String kTest_{"a=b"_k};
            const String kLbl2LookFor_{"a="_k};
            String       tmp;
            if (kTest_.Find (kLbl2LookFor_)) {
                tmp = String{kTest_.SubString (kLbl2LookFor_.length ())};
            }
            EXPECT_TRUE (tmp == "b");
        }
    }
}

namespace {
    void Test48_ToString_ ()
    {
        Debug::TraceContextBumper ctx{"Test48_ToString_"};
        EXPECT_TRUE (ToString (3) == "3");
        EXPECT_TRUE (ToString (3u, ios_base::hex) == "0x3");
        EXPECT_TRUE (ToString (3u) == "3");
        EXPECT_TRUE (ToString (1.0).StartsWith ("1"));
        EXPECT_TRUE (ToString ("abc") == "'abc'");
        EXPECT_TRUE (ToString (String{"abc"}) == "'abc'");
        EXPECT_TRUE (ToString (initializer_list<int>{3, 4, 5}) == "[ 3, 4, 5 ]");
        {
            using namespace Memory;
            constexpr byte kSample_[] = {0x34_b, 0x55_b, 0x1f_b};
            Memory::BLOB   b{begin (kSample_), end (kSample_)};
            EXPECT_TRUE (b.ToString () == "[3 bytes: 34551f]");
            EXPECT_TRUE (Characters::ToString (b) == "[3 bytes: 34551f]");
        }
        {
            constexpr int ca[3]{1, 3, 5};
            int           a[3]{1, 3, 5};
            EXPECT_TRUE (Characters::ToString (ca) == "[ 1, 3, 5 ]");
            EXPECT_TRUE (Characters::ToString (a) == "[ 1, 3, 5 ]");
        }
    }
}

namespace {
    void Test49_SetOfStringCTORIssue_ ()
    {
        Debug::TraceContextBumper ctx{"Test49_SetOfStringCTORIssue_"};
        optional<String>          optString{String{}};
        Containers::Set<String>   s{*optString};
    }
}

namespace {
    void Test50a_UnicodeStringLiterals_ ()
    {
        {
            const char16_t microChars16[] = u"\u00B5";
            const char32_t microChars32[] = U"\U000000B5";
            EXPECT_TRUE (0x00B5 == microChars16[0]);
            EXPECT_TRUE (0x00B5 == microChars32[0]);
            EXPECT_TRUE (sizeof (microChars16) == 2 * 2);
            EXPECT_TRUE (sizeof (microChars32) == 2 * 4);
            EXPECT_TRUE (String{microChars16}.size () == 1);
            EXPECT_TRUE (String{microChars32}.size () == 1);
        }
        {
            // Argument must be "45 µs"
            auto test45mus = [] (const String& a) {
                {
                    Memory::StackBuffer<wchar_t> ignored1;
                    auto                         rhsSpan = a.GetData (&ignored1);
                    EXPECT_TRUE (rhsSpan.size () == 5);
                }
                String b;
                b += a;
                EXPECT_TRUE (a == b);
                EXPECT_TRUE (a.size () == 5);
                EXPECT_TRUE (a[3] == 0x00b5);
                EXPECT_TRUE (a[4] == 's');
            };
            test45mus (String{L"45 \u00b5s"});
            test45mus (String{u"45 \u00b5s"});
            test45mus (String{U"45 \u00b5s"});
            test45mus (String{u8"45 \u00b5s"}); // sadly u8"µ" doesn't work everywhere cuz using file BOM apparently not standardized, and using u8 apparantly has no impact on charset interpretation of successive source bytes

            {
                // Bug in UTF conversion discovered in RFL UMLS data conversion -- LGP 2023-12-02
                {
                    u16string l = u"Fluorouracil\u00c2\u00ae"; // ORIG TEST
                    String    t1{l};
                    String    t2{t1.AsUTF8 ()};
                    EXPECT_TRUE (t1 == t2);
                }
                {
                    // narrowed/simplified test
                    u16string test     = u"\u00c2";
                    u8string  testAsU8 = String{test}.AsUTF8 ();
                    EXPECT_TRUE (testAsU8 == u8"\u00c2"); //u8"\xc382"); // according to https://www.utf8-chartable.de/
                    String    readingThatUTF8{testAsU8};
                    u16string ttt = readingThatUTF8.AsUTF16 ();
                    EXPECT_TRUE (readingThatUTF8.length () == 1);
                    EXPECT_TRUE (readingThatUTF8 == test);
                }
            }
        }
    }
}

namespace {
    void Test50_Utf8Conversions_ ()
    {
        Debug::TraceContextBumper ctx{"Test50_Utf8Conversions_"};
        {
            EXPECT_TRUE (String::FromUTF8 (u8"phred") == String{"phred"});
            // Need char8_t to use this constructor
            EXPECT_TRUE (String{u8"phred"} == String{"phred"});
        }
        {
            StringBuilder tmp;
            tmp.Append (String::FromUTF8 (u8"€"));
            Verify (tmp.str () == L"€");
        }
        {
            EXPECT_TRUE (u8"שלום" == String::FromUTF8 (u8"שלום").AsUTF8<u8string> ());
            EXPECT_TRUE (u8string{u8"phred"} == String::FromUTF8 (u8string{u8"phred"}).AsUTF8<u8string> ());
            EXPECT_TRUE (u8string{u8"שלום"} == String::FromUTF8 (u8string{u8"שלום"}).AsUTF8<u8string> ());
        }
        {
            EXPECT_TRUE (u8string{u8"phred"} == String::FromUTF8 (u8"phred").AsUTF8<u8string> ());
            EXPECT_TRUE (u8string{u8"שלום"} == String::FromUTF8 (u8"שלום").AsUTF8<u8string> ());
        }
        {// clang-format off
            // examples from https://en.wikipedia.org/wiki/UTF-8#Encoding
            {
                char8_t dollarSign[] = u8"$";
                EXPECT_TRUE (sizeof (dollarSign) == 2);
                EXPECT_TRUE (dollarSign[0] == 0x0024);
                EXPECT_TRUE (UTFConvert::NextCharacter (span<const char8_t>{dollarSign}) == 1u);
                EXPECT_TRUE (dollarSign[*UTFConvert::NextCharacter (span<const char8_t>{dollarSign})] == '\0');
                EXPECT_TRUE (UTFConvert::ComputeCharacterLength (span<const char8_t>{dollarSign}) == 2u); // Nul byte
            }
            {
                //char8_t poundSign[] = u8"£";
                char8_t poundSign[] = u8"\u00a3";
                static_assert (sizeof (poundSign) == 3);
                EXPECT_TRUE (UTFConvert::NextCharacter (span<const char8_t>{poundSign}) == 2u);
                EXPECT_TRUE (poundSign[*UTFConvert::NextCharacter (span<const char8_t>{poundSign})] == '\0');
                EXPECT_TRUE (UTFConvert::ComputeCharacterLength (span<const char8_t>{poundSign}) == 2u); // Nul byte
            }
            {
                char8_t devanagari[] = u8"\u0939";
                //char8_t devanagari[] = u8"ह";
                static_assert (sizeof (devanagari) == 4);
                EXPECT_TRUE (UTFConvert::NextCharacter (span<const char8_t>{devanagari}) == 3u);
                EXPECT_TRUE (devanagari[*UTFConvert::NextCharacter (span<const char8_t>{devanagari})] == '\0');
                EXPECT_TRUE (UTFConvert::ComputeCharacterLength (span<const char8_t>{devanagari}) == 2u); // Nul byte
            }
            {
                char8_t euroSign[] = u8"\u0939";
                //char8_t euroSign[] = u8"€";
                static_assert (sizeof (euroSign) == 4);
                EXPECT_TRUE (UTFConvert::NextCharacter (span<const char8_t>{euroSign}) == 3u);
                EXPECT_TRUE (euroSign[*UTFConvert::NextCharacter (span<const char8_t>{euroSign})] == '\0');
                EXPECT_TRUE (UTFConvert::ComputeCharacterLength (span<const char8_t>{euroSign}) == 2u); // Nul byte
            }
            {
                char8_t hangull[] = u8"\uD55C";
                //char8_t hangull[] = u8"한";
                static_assert (sizeof (hangull) == 4);
                EXPECT_TRUE (UTFConvert::NextCharacter (span<const char8_t>{hangull}) == 3u);
                EXPECT_TRUE (hangull[*UTFConvert::NextCharacter (span<const char8_t>{hangull})] == '\0');
                EXPECT_TRUE (UTFConvert::ComputeCharacterLength (span<const char8_t>{hangull}) == 2u); // Nul byte
            }
            {
                char8_t hwair[] = u8"\U00010348";
                //char8_t hwair[] = u8"𐍈";
                static_assert (sizeof (hwair) == 5);
                EXPECT_TRUE (UTFConvert::NextCharacter (span<const char8_t>{hwair}) == 4u);
                EXPECT_TRUE (hwair[*UTFConvert::NextCharacter (span<const char8_t>{hwair})] == '\0');
                EXPECT_TRUE (UTFConvert::ComputeCharacterLength (span<const char8_t>{hwair}) == 2u); // Nul byte
            }
        }
        {
            // Assure we properly document/TREAT the case of 'SOURCE EXHAUSTED'
                //char8_t poundSign[] = u8"£";
                char8_t poundSign[] = u8"\u00a3";
                static_assert (sizeof (poundSign) == 3);
                EXPECT_TRUE (UTFConvert::NextCharacter (span<const char8_t>{poundSign}) == 2u);
                UTFConvert c;
                Character outBuf[1024];
                auto a = c.ConvertQuietly (span<const char8_t>{&poundSign[0], 1}, span<Character> {outBuf});
                EXPECT_TRUE (a.fSourceConsumed == 0 and a.fTargetProduced == 0);
                EXPECT_TRUE (a.fStatus == UTFConvert::ConversionStatusFlag::sourceExhausted );
                try {
                    c.Convert (span<const char8_t>{&poundSign[0], 1}, span<Character> {outBuf});
                    EXPECT_TRUE (false);   // not reached
                }
                catch (...) {
                    // good
                }
        }
        {
            // Intentionally no operator+ etc stuff for utf8 because type of u8"" is same as "", so no way to overload
        }
    }
// clang-format on
}

namespace {
    void Test51_Utf16Conversions_ ()
    {
        Debug::TraceContextBumper ctx{"Test51_Utf16Conversions_"};
        {
            EXPECT_TRUE (u16string{u"phred"} == String{u16string{u"phred"}}.AsUTF16 ());
            EXPECT_TRUE (u16string{u"שלום"} == String{u16string{u"שלום"}}.AsUTF16 ()); // @todo CORRECT but misleading since file encoding doesnt match these characters
        }
        {
            EXPECT_TRUE (u16string{u"phred"} == String{u"phred"}.AsUTF16 ());
            EXPECT_TRUE (u16string{u"שלום"} == String{u"שלום"}.AsUTF16 ());
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
            static const u16string kTestWithSurrogates_ = u16string{u"\U0002008A"};
            EXPECT_TRUE (kTestWithSurrogates_.size () == 2);
            EXPECT_TRUE (kTestWithSurrogates_ == String{kTestWithSurrogates_}.AsUTF16 ());
            EXPECT_TRUE (String{kTestWithSurrogates_}.length () == 1);
            EXPECT_TRUE (String{kTestWithSurrogates_}.AsUTF16 ().length () == 2);
        }
        {
            // https://www.informit.com/articles/article.aspx?p=2274038&seqNum=10 SURROGATE SAMPLES
            {
                constexpr auto ch = Character{0x10000};
                EXPECT_TRUE ((ch.IsSurrogatePair () and ch.GetSurrogatePair () == pair{static_cast<char16_t> (0xD800), static_cast<char16_t> (0xDC00)}));
                EXPECT_TRUE ((ch == Character{0xD800, 0xDC00}));
            }
            {
                constexpr auto ch = Character{0x10E6D};
                EXPECT_TRUE ((ch.IsSurrogatePair () and ch.GetSurrogatePair () == pair{static_cast<char16_t> (0xD803), static_cast<char16_t> (0xDE6D)}));
                EXPECT_TRUE ((ch == Character{0xD803, 0xDE6D}));
            }
            {
                constexpr auto ch = Character{0x1D11E};
                EXPECT_TRUE ((ch.IsSurrogatePair () and ch.GetSurrogatePair () == pair{static_cast<char16_t> (0xD834), static_cast<char16_t> (0xDD1E)}));
                EXPECT_TRUE ((ch == Character{0xD834, 0xDD1E}));
            }
            {
                constexpr auto ch = Character{0x10FFFF};
                EXPECT_TRUE ((ch.IsSurrogatePair () and ch.GetSurrogatePair () == pair{static_cast<char16_t> (0xDBFF), static_cast<char16_t> (0xDFFF)}));
                EXPECT_TRUE ((ch == Character{0xDBFF, 0xDFFF}));
            }
        }
    }
}

namespace {
    void Test52_Utf32Conversions_ ()
    {
        Debug::TraceContextBumper ctx{"Test52_Utf32Conversions_"};
        {
            EXPECT_TRUE (u32string{U"phred"} == String{u32string{U"phred"}}.AsUTF32 ());
            EXPECT_TRUE (u32string{U"שלום"} == String{u32string{U"שלום"}}.AsUTF32 ());
            //            EXPECT_TRUE (u32string{U"שלום"}.size () == 4);
            //           EXPECT_TRUE (String{u32string{U"שלום"}}.size () == 4);
        }
        {
            EXPECT_TRUE (u32string{U"phred"} == String{U"phred"}.AsUTF32 ());
            EXPECT_TRUE (u32string{U"שלום"} == String{U"שלום"}.AsUTF32 ());
            EXPECT_TRUE (String{U"שלום"} == String{u"שלום"});
        }
        {
            using namespace SampleUNICODE_::RawConstants_;
            EXPECT_TRUE (String{Heb1_::k8_}.size () == 4);
            EXPECT_TRUE (String{Heb1_::k16_}.size () == 4);
            EXPECT_TRUE (String{Heb1_::k32_}.size () == 4);
            EXPECT_TRUE (String{Heb1_::kWide_}.size () == 4);
            EXPECT_TRUE (String{Heb1_::k8_} == String{Heb1_::k16_} and String{Heb1_::k16_} == String{Heb1_::k32_} and
                         String{Heb1_::k32_} == String{Heb1_::kWide_});
            StringBuilder tmp;
            tmp += Heb1_::k8_;
            EXPECT_TRUE (tmp.length () == 4);
            EXPECT_TRUE (tmp.str () == Heb1_::k8_);
            EXPECT_TRUE (tmp.str () == Heb1_::kWide_);
        }
        {
            StringBuilder tmp;
            tmp.Append (u8"שלום");
            //   EXPECT_TRUE (tmp.size () == 4);
            EXPECT_TRUE (tmp.str () == u8"שלום");
        }
        {
            StringBuilder tmp;
            tmp += U"שלום";
            EXPECT_TRUE (tmp.str () == U"שלום");
        }
    }
}

namespace {
    void Test53_vswprintf_on_2_strings_longish_Buggy_ ()
    {
        Debug::TraceContextBumper ctx{"Test53_vswprintf_on_2_strings_longish_Buggy_"};
        String                    b = L"…";
        if constexpr (not qCompiler_vswprintf_on_elispisStr_Buggy) {
            try {
                String x = Characters::Format (L"%s", b.c_str ());
                EXPECT_TRUE (x == b);
            }
            catch (...) {
                EXPECT_TRUE (false); // means we have the bug...
            }
        }
    }
}

namespace {
    void Test54_StringAs_ ()
    {
        Debug::TraceContextBumper ctx{"Test54_StringAs_"};
        EXPECT_TRUE (String{"hi mom"}.AsASCII () == "hi mom");
        {
            try {
                string a2 = String{u"שלום"}.AsASCII (); // throws
                EXPECT_TRUE (false);                    // not reached
            }
            catch (...) {
                // good
            }
        }
    }
}

namespace {
    void Test55_StringAscii_CTORs_ ()
    {
        Debug::TraceContextBumper ctx{"Test55_StringAscii_CTORs_"};
        (void)String{"a"};
        try {
            (void)String{"\x81"};
            EXPECT_TRUE (false); // not reached
        }
        catch (...) {
            // good
        }
    }
}

namespace {
    void Test56_StdStringMoveCTORs_ ()
    {
        Debug::TraceContextBumper ctx{"Test56_StdStringMoveCTORs_"};
        {
            wstring stuff{L"abc"};
            String  a{move (stuff)};
            EXPECT_TRUE (a == "abc");
        }
    }
}

namespace {
    void Test57_Latin1_Tests_ ()
    {
        Debug::TraceContextBumper ctx{"Test57_Latin1_Tests_"};
        /**
         *  Be careful with signed/unsigned issues using Latin1 characters and string
         *      A few random characters selected from https://en.wikipedia.org/wiki/Latin-1_Supplement
         */
        string plainAsciiTest = "fred";
        string nonAsciiLatin1Test;
        {
            constexpr unsigned char kMicroSign            = 0xb5;
            constexpr unsigned char kCedilla              = 0xb8;
            constexpr unsigned char kInvertedQuestionMark = 0xbf;
#if defined(_MSC_VER)
            // other compilers don't seem to allow this check, and one compiler enuf to verify I have the right codes
            static_assert (kMicroSign == static_cast<unsigned char> ('µ'));
            static_assert (kCedilla == static_cast<unsigned char> ('¸'));
            static_assert (kInvertedQuestionMark == static_cast<unsigned char> ('¿'));
#endif
            nonAsciiLatin1Test = string{{(char)kMicroSign, (char)kCedilla, (char)kInvertedQuestionMark}};
            Assert (nonAsciiLatin1Test.length () == 3 and static_cast<unsigned char> (nonAsciiLatin1Test[1]) == kCedilla);
        }
        {
            EXPECT_TRUE (String{plainAsciiTest} == String{plainAsciiTest});
            try {
                [[maybe_unused]] auto ignored = String{nonAsciiLatin1Test};
                EXPECT_TRUE (false);
            }
            catch (...) {
                // good
            }
            String latin1AsStr = String::FromLatin1 (nonAsciiLatin1Test);
            EXPECT_TRUE (latin1AsStr.length () == nonAsciiLatin1Test.length ());
            for (size_t i = 0; i < nonAsciiLatin1Test.length (); ++i) {
                EXPECT_TRUE (latin1AsStr[i] == static_cast<unsigned char> (nonAsciiLatin1Test[i]));
            }
        }
        {
            String aaa = L"\u00b5"; // see https://www.cogsci.ed.ac.uk/~richard/utf-8.cgi?input=b5&mode=hex
            Assert (aaa.length () == 1);
            u8string aaa8 = aaa.As<u8string> ();
            Assert (aaa8.length () == 2 and aaa8[0] == 0xc2 and aaa8[1] == 0xb5);
        }
    }
}

namespace {
    void Test58_CodeCVT_ ()
    {
        Debug::TraceContextBumper ctx{"Test58_CodeCVT_"};
        auto                      codeCvtChar16Test = [] (CodeCvt<char16_t> ccvt) {
            constexpr char16_t        someRandomText[] = u"hello mom";
            span<const char16_t>      someRandomTextSpan{someRandomText, Characters::CString::Length (someRandomText)};
            Memory::StackBuffer<byte> targetBuf{ccvt.ComputeTargetByteBufferSize (someRandomTextSpan)};
            auto                      b = ccvt.Characters2Bytes (someRandomTextSpan, span{targetBuf});
            EXPECT_TRUE (b.size () == 9 and b[0] == static_cast<byte> ('h'));
        };
        {
            // simple fast converter from char16_t characters <-> UTF-8
            CodeCvt<char16_t> codeCvt1{};
            codeCvtChar16Test (codeCvt1);
            // CodeCvt between char16_t characters <-> UTF-8 using std::codecvt<char16_t, char8_t, std::mbstate_t>
            CodeCvt<char16_t> codeCvt2 = CodeCvt<char16_t>::mkFromStdCodeCvt<std::codecvt<char16_t, char8_t, std::mbstate_t>> ();
            codeCvtChar16Test (codeCvt2);

            auto hasLocale = [] (const string& n) {
                try {
                    locale{n};
                    return true;
                }
                catch (...) {
                    return false;
                }
            };
            // see https://en.cppreference.com/w/cpp/locale/codecvt_byname for cases not deprecated
            if (hasLocale ("en_US.UTF8")) {
                // Now using codecvt_byname (locale converter)
                CodeCvt<wchar_t> codeCvt3a =
                    CodeCvt<wchar_t>::mkFromStdCodeCvt<std::codecvt_byname<wchar_t, char, std::mbstate_t>> ({}, "en_US.UTF8");
                CodeCvt<char32_t> codeCvt3b =
                    CodeCvt<char32_t>::mkFromStdCodeCvt<std::codecvt_byname<char32_t, char8_t, std::mbstate_t>> ({}, "en_US.UTF8");
                CodeCvt<char16_t> codeCvt3c =
                    CodeCvt<char16_t>::mkFromStdCodeCvt<std::codecvt_byname<char16_t, char8_t, std::mbstate_t>> ({}, "en_US.UTF8");
                codeCvtChar16Test (codeCvt3c);
                CodeCvt<char16_t> codeCvt4c = CodeCvt<char16_t>{locale{"en_US.UTF8"}};
                codeCvtChar16Test (codeCvt4c);
            }
            if (hasLocale ("en_US.UTF8")) {
                CodeCvt<>         cc{locale{"en_US.UTF8"}};
                constexpr char8_t someRandomText[] = u8"hello mom";
                span<const byte>  someRandomTextBinarySpan =
                    as_bytes (span<const char8_t>{someRandomText, Characters::CString::Length (someRandomText)});
                Memory::StackBuffer<Character> buf{cc.ComputeTargetCharacterBufferSize (someRandomTextBinarySpan)};
                auto                           b = cc.Bytes2Characters (&someRandomTextBinarySpan, span{buf});
                EXPECT_TRUE (someRandomTextBinarySpan.size () == 0); // ALL CONSUMED
                EXPECT_TRUE (b.size () == 9 and b[0] == 'h');
            }
        }
    }
}
namespace {
    GTEST_TEST (Foundation_Characters, all)
    {
        Debug::TraceContextBumper ctx{"DoRegressionTests_"};
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
        Test15_StripAll_ ();
        Test16_Format_ ();
        Test17_Find_ ();
        Test18_Compare_ ();
        Test19_ConstCharStar_ ();
        Test20_CStringHelpers_ ();
        Test21_StringNumericConversions_ ();
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
        Test50a_UnicodeStringLiterals_ ();
        Test50_Utf8Conversions_ ();
        Test51_Utf16Conversions_ ();
        Test52_Utf32Conversions_ ();
        Test53_vswprintf_on_2_strings_longish_Buggy_ ();
        Test54_StringAs_ ();
        Test55_StringAscii_CTORs_ ();
        Test56_StdStringMoveCTORs_ ();
        Test57_Latin1_Tests_ ();
        Test58_CodeCVT_ ();
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
