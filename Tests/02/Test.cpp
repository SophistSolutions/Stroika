/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
#include "Stroika/Foundation/Common/Locale.h"
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

//tmphack while experimenting with new std format code
#include "Stroika/Foundation/IO/Network/CIDR.h"
#include "Stroika/Foundation/IO/Network/URI.h"

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

}

namespace {
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
}

namespace {
    GTEST_TEST (Foundation_Characters, Test2_)
    {
        Debug::TraceContextBumper ctx{"Test2_"};
        Test2Helpers_::StressTestStrings ();
#if 0
        Test2Helpers_::StressTestBufferedStrings ();
#endif
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, Test3_)
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
        EXPECT_EQ (t1, "Fred Flintstone");
        EXPECT_EQ (String{"Fred Flintstone"}, t1);
        EXPECT_EQ (String{"Fred Flintstone"}, t1);
        EXPECT_EQ (t1, String{"Fred Flintstone"});
        EXPECT_TRUE (t2 != "Fred Flintstone");
        EXPECT_TRUE (String{"Fred Flintstone"} != t2);
        EXPECT_TRUE (String{"Fred Flintstone"} != t2);
        EXPECT_TRUE (t2 != String{L"Fred Flintstone"});

        EXPECT_EQ (t1.size (), 15u);
        t1.erase (4);
        EXPECT_EQ (t1.size (), 4u);
        EXPECT_EQ (t1, "Fred");

        EXPECT_EQ (t1[0], 'F');
        EXPECT_EQ (t1[1], 'r');
        EXPECT_EQ (t1[2], 'e');
        EXPECT_EQ (t1[3], 'd');

        String a[10];
        EXPECT_EQ (a[2], "");
        a[3] = "Fred";
        EXPECT_EQ (a[3], "Fred");
        EXPECT_TRUE (a[2] != "Fred");
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, Test4_)
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
}

namespace {
    GTEST_TEST (Foundation_Characters, Test5_)
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
}

namespace {
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
    GTEST_TEST (Foundation_Characters, Test6_)
    {
        using namespace Test6_PRIVATE_;
        Debug::TraceContextBumper ctx{"Test6_"};
        Test6_Helper_<String> ("Characters::String");
        Test6_Helper_<wstring> ("std::wstring");
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, Comparisons_)
    {
        Debug::TraceContextBumper ctx{"Comparisons_"};
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
}

namespace {
    GTEST_TEST (Foundation_Characters, ReadOnlyStrings_)
    {
        Debug::TraceContextBumper ctx{"ReadOnlyStrings_"};
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
}

namespace {
    GTEST_TEST (Foundation_Characters, ExternalMemoryOwnershipStrings_)
    {
        Debug::TraceContextBumper ctx{"ExternalMemoryOwnershipStrings_"};
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
}

namespace {
    namespace Test9Support {
        template <typename STRING>
        void DoTest1 (STRING s)
        {
            STRING           t1 = s;
            constexpr size_t kMaxCount_{100}; // NOTE - see http://stroika-bugs.sophists.com/browse/STK-996
            for (size_t i = 0; i < kMaxCount_; ++i) {
                t1 += L"X";
            }
            STRING t2 = t1;
            if (t1 != t2) {
                EXPECT_TRUE (false);
            }
        }
    }
    GTEST_TEST (Foundation_Characters, StringVersusStdCString_)
    {
        Debug::TraceContextBumper ctx{"StringVersusStdCString_"};
        // EMBELLISH THIS MORE ONCE WE HAVE TIMING SUPPORT WORKING - SO WE CNA COMPARE PERFORMANCE - AND COME UP WITH MORE REASONABLE TESTS
        //
        //      -- LGP 2011-09-01
        Test9Support::DoTest1<String> ("Hello");
        Test9Support::DoTest1<std::wstring> (L"Hello");
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, ConvertToFromSTDStrings_)
    {
        Debug::TraceContextBumper ctx{"ConvertToFromSTDStrings_"};
        const wstring             kT1 = L"abcdefh124123985213129314234";
        String                    t1  = kT1;
        EXPECT_EQ (t1.As<wstring> (), kT1);
        EXPECT_EQ (t1, kT1);
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, Trim_)
    {
        Debug::TraceContextBumper ctx{"Trim_"};
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
    GTEST_TEST (Foundation_Characters, CodePageConverter_)
    {
        Debug::TraceContextBumper ctx{"CodePageConverter_"};
        wstring                   w = L"<PHRMode";
        using namespace Characters;
        using namespace Memory;
        EXPECT_EQ ((CodeCvt<wchar_t>{WellKnownCodePages::kUTF8}.String2Bytes<string> (span{w})), "<PHRMode");
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, ToLowerUpper_)
    {
        Debug::TraceContextBumper ctx{"ToLowerUpper_"};
        String                    w = "Lewis";
        EXPECT_EQ (w.ToLowerCase (), "lewis");
        EXPECT_EQ (w.ToUpperCase (), "LEWIS");
        EXPECT_EQ (w, "Lewis");
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, StripAll_)
    {
        Debug::TraceContextBumper ctx{"StripAll_"};
        String                    w = "Le wis";
        EXPECT_EQ (w.StripAll ([] (Character c) -> bool { return c.IsWhitespace (); }), "Lewis");

        w = "This is a very good test    ";
        EXPECT_EQ (w.StripAll ([] (Character c) -> bool { return c.IsWhitespace (); }), "Thisisaverygoodtest");
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, Format_)
    {
        Debug::TraceContextBumper ctx{"Format_"};
        EXPECT_TRUE (CString::Format ("%d", 123) == "123");
        EXPECT_TRUE (CString::Format ("%s", "123") == "123");

        // SUBTLE - this would FAIL with vsnprintf on gcc -- See docs in Header for
        // Format string
        EXPECT_TRUE (CString::Format (L"%s", L"123") == L"123");
        EXPECT_EQ (Format ("{}"_f, L"123"), L"123");

        DbgTrace ("{:20}"_f, L"123");
        EXPECT_EQ (Format ("{:>20}"_f, L"123"), "                 123");
        EXPECT_EQ (Format ("{:.20}"_f, L"123"), "123");

#if 0
        // @todo somethign similar with new style string formatters
        for (size_t i = 1; i < 1000; ++i) {
            String format = Format (L"%%%ds", static_cast<int> (i));
            EXPECT_TRUE (Format (format.c_str (), L"x").length () == i);
        }
#endif
        EXPECT_EQ (Characters::Format ("{}.{}{}{}"_f, 1, 0, L"a", L"1x"), "1.0a1x"); // 2 conseq %s%s POSIX bug fixed 2014-01-22
    }
}

namespace {
    namespace TestFIND_Private_ {
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
                EXPECT_EQ (replaced, "abcdef");
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
    GTEST_TEST (Foundation_Characters, Find_)
    {
        Debug::TraceContextBumper ctx{"Find_"};
        TestFIND_Private_::Test17_ReplaceAll_ ();
        TestFIND_Private_::Test17_ReplaceAll_ ();
        TestFIND_Private_::Test17_RegExp_ ();
        TestFIND_Private_::docsTests_ ();
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, Compare_)
    {
        Debug::TraceContextBumper ctx{"Compare_"};
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
    GTEST_TEST (Foundation_Characters, ConstCharStar_)
    {
        Debug::TraceContextBumper ctx{"ConstCharStar_"};
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
    GTEST_TEST (Foundation_Characters, CStringHelpers_)
    {
        Debug::TraceContextBumper ctx{"CStringHelpers_"};
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
            if constexpr (qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy || qCompilerAndStdLib_isinf_Valgrind_Buggy) {
                auto f = Characters::FloatConversion::ToFloat<FLOAT_TYPE> (FloatConversion::ToString (l, FloatConversion::Precision::kFull));
                if (not Math::NearlyEquals (l, f)) {
                    if (Debug::IsRunningUnderValgrind () and qCompilerAndStdLib_isinf_Valgrind_Buggy) {
                        Stroika::Frameworks::Test::WarnTestIssue ("ToFloat(ToString({})) not properly roundtripping under valgrind: {}; note isinf({})={}, and isinf(f)={}"_f(
                                                                      l, f, l, isinf (l), isinf (f))
                                                                      .template As<wstring> ()
                                                                      .c_str ());
                        return;
                    }
                    if (qCompilerAndStdLib_from_chars_and_tochars_FP_Precision_Buggy) {
                        Stroika::Frameworks::Test::WarnTestIssue (
                            "ToFloat(ToString({})) not properly roundtripping: {}"_f(l, f).template As<wstring> ().c_str ());
                        return;
                    }
                }
            }
            EXPECT_TRUE (Math::NearlyEquals (
                l, Characters::FloatConversion::ToFloat<FLOAT_TYPE> (FloatConversion::ToString (l, FloatConversion::Precision::kFull))));
        }
    }
    GTEST_TEST (Foundation_Characters, StringNumericConversions_)
    {
        Debug::TraceContextBumper ctx{"StringNumericConversions_"};
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
            // http://stroika-bugs.sophists.com/browse/STK-966
            auto doTest = [] (const wstring& w, long double wVal) {
                StringBuilder sb1;
                sb1 << w;
                Memory::StackBuffer<wchar_t> ignoreBuf1;
                span<const wchar_t>          tmpData1 = sb1.GetData (&ignoreBuf1);
                double                       ff2      = Characters::FloatConversion::ToFloat<double> (tmpData1);
                DbgTrace ("*in reader:  ff2-> {}"_f, ff2);
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

                        // This test is an ABJECT FAILURE - http://stroika-bugs.sophists.com/browse/STK-747 but LOW PRIORITY
                    };
                    checkJapaneseNumbers ();
                    if (auto ln = Common::FindLocaleNameQuietly ("ja"sv, "JP"sv)) {
                        Common::ScopedUseLocale tmpLocale{locale{ln->AsNarrowSDKString (eIgnoreErrors).c_str ()}};
                        checkJapaneseNumbers ();
                    }
                }

                {
                    // European comma/period confusion
                    // BROKEN - see http://stroika-bugs.sophists.com/browse/STK-748
                    // See https://docs.oracle.com/cd/E19455-01/806-0169/overview-9/index.html
                    if (auto ln = Common::FindLocaleNameQuietly (L"en", L"US")) {
                        Common::ScopedUseLocale tmpLocale{locale{ln->AsNarrowSDKString (eIgnoreErrors).c_str ()}};
                        EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat<double> ("100.1"), 100.1));
                        [[maybe_unused]] auto i2 = FloatConversion::ToFloat<double> ("967,295.01");
                        //EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat<double> (L"967,295.01") , 967295.01));     -- http://stroika-bugs.sophists.com/browse/STK-748
                    }
                    if (auto ln = Common::FindLocaleNameQuietly (L"es", L"ES")) {
                        Common::ScopedUseLocale tmpLocale{locale{ln->AsNarrowSDKString (eIgnoreErrors).c_str ()}};
                        //DbgTrace ("using locale %s", locale{}.name ().c_str ());
                        //DbgTrace (L"decimal separator: %c", std::use_facet<std::numpunct<char>> (locale{}).decimal_point ());
                        EXPECT_TRUE (std::use_facet<std::numpunct<char>> (locale{}).decimal_point () == ',');
                        EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat<double> (L"100,1"), 100.1));
                        [[maybe_unused]] auto i2 = FloatConversion::ToFloat<double> (L"967.295,01");
                        //EXPECT_TRUE (Math::NearlyEquals (FloatConversion::ToFloat<double> (L"967.295,01") , 967295.01)); -- http://stroika-bugs.sophists.com/browse/STK-748
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
            EXPECT_EQ (FloatConversion::ToString (3000.5), "3000.5");
            EXPECT_EQ (FloatConversion::ToString (30000.5), "30000.5");
        };
        {
            // Verify change of locale has no effect on results
            locale prevLocale = locale::global (locale{"C"});
            runLocaleIndepTest ();
            locale::global (prevLocale);
        }
        try {
            // Verify change of locale has no effect on results
            Common::ScopedUseLocale tmpLocale{Common::FindNamedLocale ("en"sv, "us"sv)};
            runLocaleIndepTest ();
        }
        catch ([[maybe_unused]] const Common::LocaleNotFoundException& e) {
            Stroika::Frameworks::Test::WarnTestIssue ("Skipping test cuz missing locale");
        }
        {
            EXPECT_EQ (String2Int ("0587:c413:5500:0000:0000:0000:0001]:60000"), 0);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, StartsWithEndsWithMatch_)
    {
        Debug::TraceContextBumper ctx{"StartsWithEndsWithMatch_"};
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
    GTEST_TEST (Foundation_Characters, FormatV_)
    {
        using namespace Test23_PRIVATE_;
        Debug::TraceContextBumper ctx{"FormatV_"};
        EXPECT_EQ (Test23_help1_HELPER (L"joe%sx", L"1"), L"joe1x");
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, Float2String)
    {
        Debug::TraceContextBumper ctx{"Float2String"};
        EXPECT_EQ (FloatConversion::ToString (0.0), "0");
        EXPECT_EQ (FloatConversion::ToString (3000.5), "3000.5");
        EXPECT_EQ (FloatConversion::ToString (3000.500), "3000.5");
        EXPECT_EQ (FloatConversion::ToString (3.1234, Characters::FloatConversion::Precision{2}), "3.1");
        EXPECT_EQ (FloatConversion::ToString (3.1234, Characters::FloatConversion::Precision{3}), "3.12");
        EXPECT_EQ (FloatConversion::ToString (31.234, Characters::FloatConversion::Precision{3}), "31.2");
        EXPECT_EQ (FloatConversion::ToString (30707548160.0), "3.07075e+10");
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, RemoveAt_)
    {
        Debug::TraceContextBumper ctx{"RemoveAt_"};
        String                    x = "123";
        x                           = x.RemoveAt (1);
        EXPECT_EQ (x, "13");
        x = x.RemoveAt (0, 2);
        EXPECT_TRUE (x.empty ());
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, Iteration_)
    {
        Debug::TraceContextBumper ctx{"Iteration_"};
        {
            String x = "123";
            int    i = 0;
            for (auto c : x) {
                EXPECT_EQ (c, x[i]);
                ++i;
            }
            EXPECT_EQ (i, 3);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, Repeat_)
    {
        Debug::TraceContextBumper ctx{"Repeat_"};
        {
            String x;
            String r = x.Repeat (5);
            EXPECT_TRUE (r.length () == 0);
        }
        {
            String x = "123";
            String r = x.Repeat (3);
            EXPECT_EQ (r.length (), 9u);
            EXPECT_EQ (r.SubString (3, 6), "123");
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, ReplacementForStripTrailingCharIfAny_)
    {
        Debug::TraceContextBumper ctx{"ReplacementForStripTrailingCharIfAny_"};
        auto StripTrailingCharIfAny = [] (const String& s, const Character& c) -> String { return s.EndsWith (c) ? s.SubString (0, -1) : s; };
        EXPECT_TRUE (StripTrailingCharIfAny ("xxx", '.') == "xxx");
        EXPECT_TRUE (StripTrailingCharIfAny ("xxx.", '.') == "xxx");
        EXPECT_TRUE (StripTrailingCharIfAny ("xxx..", '.') == "xxx.");
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, StringWithSequenceOfCharacter_)
    {
        Debug::TraceContextBumper ctx{"StringWithSequenceOfCharacter_"};
        {
            String              initialString = "012345";
            Sequence<Character> s1            = Sequence<Character>{initialString};
            s1.SetAt (3, 'E');
            EXPECT_EQ (String{s1}, "012E45");
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, LimitLength_)
    {
        Debug::TraceContextBumper ctx{"LimitLength_"};
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
    GTEST_TEST (Foundation_Characters, OperatorINSERT_ostream_)
    {
        Debug::TraceContextBumper ctx{"OperatorINSERT_ostream_"};
        wstringstream             out;
        out << String{"abc"};
        EXPECT_TRUE (out.str () == L"abc");
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, StringBuilder_)
    {
        Debug::TraceContextBumper ctx{"StringBuilder_"};
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
    GTEST_TEST (Foundation_Characters, Append_)
    {
        Debug::TraceContextBumper ctx{"Append_"};
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
    GTEST_TEST (Foundation_Characters, LocaleUNICODEConversions_)
    {
        Debug::TraceContextBumper ctx{"LocaleUNICODEConversions_"};
        EXPECT_TRUE (String{"abcdefgjij"}.AsNarrowSDKString (eIgnoreErrors) == "abcdefgjij"); // Failed due to bug in CodePageConverter::MapFromUNICODE before v3.0d2
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
                DbgTrace ("e={}"_f, Characters::ToString (current_exception ()));
#if qCompilerAndStdLib_locale_utf8_string_convert_Buggy
// sigh - fails to convert unicode characters
#else
                EXPECT_TRUE (not initializedLocale); // else means throw from conversion which would be bad
#endif
            }
            catch (...) {
                DbgTrace ("error: "_f, Characters::ToString (current_exception ()));
                EXPECT_TRUE (false); // I think docs say must throw std::runtime_error
            }
        };
        //testRoundtrip ("en_US.utf8", u8"z\u00df\u6c34\U0001d10b", L"zß水𝄋");
        testRoundtrip ("C", "fred", L"fred");
        testRoundtrip ("en_US.utf8", "\x7a\xc3\x9f\xe6\xb0\xb4\xf0\x9d\x84\x8b", L"zß水𝄋");
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, Tokenize_)
    {
        Debug::TraceContextBumper ctx{"Tokenize_"};
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
    GTEST_TEST (Foundation_Characters, CompareLHSRHS_)
    {
        Debug::TraceContextBumper ctx{"CompareLHSRHS_"};
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
    GTEST_TEST (Foundation_Characters, SubString_)
    {
        Debug::TraceContextBumper ctx{"SubString_"};
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
    GTEST_TEST (Foundation_Characters, ToString_)
    {
        Debug::TraceContextBumper ctx{"ToString_"};
        EXPECT_EQ (ToString (3), "3");
        EXPECT_EQ (ToString (3u, ios_base::hex), "0x3");
        EXPECT_EQ (ToString (3u), "3");
        EXPECT_TRUE (ToString (1.0).StartsWith ("1"));
        EXPECT_EQ (ToString ("abc"), "abc");
        EXPECT_EQ (ToString (String{"abc"}), "abc");
        EXPECT_EQ (ToString ((initializer_list<int>{3, 4, 5})), "[ 3, 4, 5 ]");
        {
            using namespace Memory;
            constexpr byte kSample_[] = {0x94_b, 0x55_b, 0x1f_b}; // note format may change - and if ascii - format is different
            Memory::BLOB   b{begin (kSample_), end (kSample_)};
            EXPECT_EQ (b.ToString (), "[3 bytes: 94551f]");
            EXPECT_EQ (Characters::ToString (b), "[3 bytes: 94551f]");
        }
        {
            constexpr int ca[3]{1, 3, 5};
            int           a[3]{1, 3, 5};
            EXPECT_EQ (Characters::ToString (ca), "[ 1, 3, 5 ]");
            EXPECT_EQ (Characters::ToString (a), "[ 1, 3, 5 ]");
        }
        {
            EXPECT_EQ (Characters::ToString (atomic<int>{3}), "3");
        }
        {
            auto                    sp = make_shared<int> (3);
            [[maybe_unused]] String a  = "{}"_f(sp);
            DbgTrace ("a={}"_f, a);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, SetOfStringCTORIssue_)
    {
        Debug::TraceContextBumper ctx{"SetOfStringCTORIssue_"};
        optional<String>          optString{String{}};
        Containers::Set<String>   s{*optString};
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, UnicodeStringLiterals_)
    {
        Debug::TraceContextBumper ctx{"UnicodeStringLiterals_"};
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
                EXPECT_TRUE (a[3].GetCharacterCode () == 0x00b5);
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
    GTEST_TEST (Foundation_Characters, Utf8Conversions_)
    {
        Debug::TraceContextBumper ctx{"Utf8Conversions_"};
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
        { // clang-format off
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
    GTEST_TEST (Foundation_Characters, Utf16Conversions_)
    {
        Debug::TraceContextBumper ctx{"Utf16Conversions_"};
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
                constexpr auto ch = Character{char32_t (0x10000)};
                EXPECT_TRUE ((ch.IsSurrogatePair () and ch.GetSurrogatePair () == pair{static_cast<char16_t> (0xD800), static_cast<char16_t> (0xDC00)}));
                EXPECT_TRUE ((ch == Character{0xD800, 0xDC00}));
            }
            {
                constexpr auto ch = Character{char32_t (0x10E6D)};
                EXPECT_TRUE ((ch.IsSurrogatePair () and ch.GetSurrogatePair () == pair{static_cast<char16_t> (0xD803), static_cast<char16_t> (0xDE6D)}));
                EXPECT_TRUE ((ch == Character{0xD803, 0xDE6D}));
            }
            {
                constexpr auto ch = Character{char32_t (0x1D11E)};
                EXPECT_TRUE ((ch.IsSurrogatePair () and ch.GetSurrogatePair () == pair{static_cast<char16_t> (0xD834), static_cast<char16_t> (0xDD1E)}));
                EXPECT_TRUE ((ch == Character{0xD834, 0xDD1E}));
            }
            {
                constexpr auto ch = Character{char32_t (0x10FFFF)};
                EXPECT_TRUE ((ch.IsSurrogatePair () and ch.GetSurrogatePair () == pair{static_cast<char16_t> (0xDBFF), static_cast<char16_t> (0xDFFF)}));
                EXPECT_TRUE ((ch == Character{0xDBFF, 0xDFFF}));
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, Utf32Conversions_)
    {
        Debug::TraceContextBumper ctx{"Utf32Conversions_"};
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
    GTEST_TEST (Foundation_Characters, vswprintf_on_2_strings_longish_Buggy_)
    {
        Debug::TraceContextBumper ctx{"vswprintf_on_2_strings_longish_Buggy_"};
        String                    b = L"…";
        if constexpr (not qCompiler_vswprintf_on_elispisStr_Buggy) {
            try {
                String x = Characters::Format ("{}"_f, b);
                EXPECT_TRUE (x == b);
            }
            catch (...) {
                EXPECT_TRUE (false); // means we have the bug...
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, StringAs_)
    {
        Debug::TraceContextBumper ctx{"StringAs_"};
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
    GTEST_TEST (Foundation_Characters, StringAscii_CTORs_)
    {
        Debug::TraceContextBumper ctx{"StringAscii_CTORs_"};
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
    GTEST_TEST (Foundation_Characters, StdStringMoveCTORs_)
    {
        Debug::TraceContextBumper ctx{"StdStringMoveCTORs_"};
        {
            wstring stuff{L"abc"};
            String  a{move (stuff)};
            EXPECT_TRUE (a == "abc");
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Characters, Latin1_Tests_)
    {
        Debug::TraceContextBumper ctx{"Latin1_Tests_"};
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
                EXPECT_EQ (latin1AsStr[i].GetCharacterCode (), static_cast<unsigned char> (nonAsciiLatin1Test[i]));
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
    GTEST_TEST (Foundation_Characters, CodeCVT_)
    {
        Debug::TraceContextBumper ctx{"CodeCVT_"};
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
            DISABLE_COMPILER_MSC_WARNING_START (4996)
            // CodeCvt between char16_t characters <-> UTF-8 using std::codecvt<char16_t, char8_t, std::mbstate_t>
            CodeCvt<char16_t> codeCvt2 = CodeCvt<char16_t>::mkFromStdCodeCvt<std::codecvt<char16_t, char8_t, std::mbstate_t>> ();
            DISABLE_COMPILER_MSC_WARNING_END (4996)
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
                DISABLE_COMPILER_MSC_WARNING_START (4996)
                CodeCvt<wchar_t> codeCvt3a =
                    CodeCvt<wchar_t>::mkFromStdCodeCvt<std::codecvt_byname<wchar_t, char, std::mbstate_t>> ({}, "en_US.UTF8");
                CodeCvt<char32_t> codeCvt3b =
                    CodeCvt<char32_t>::mkFromStdCodeCvt<std::codecvt_byname<char32_t, char8_t, std::mbstate_t>> ({}, "en_US.UTF8");
                CodeCvt<char16_t> codeCvt3c =
                    CodeCvt<char16_t>::mkFromStdCodeCvt<std::codecvt_byname<char16_t, char8_t, std::mbstate_t>> ({}, "en_US.UTF8");
                DISABLE_COMPILER_MSC_WARNING_END (4996)
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
    GTEST_TEST (Foundation_Characters, New_Format)
    {
        Debug::TraceContextBumper ctx{"New_Format"};

        struct foo {
            String ToString () const
            {
                return "foo as a string"sv;
            }
        };

        using Common::StdCompat::format; // only needed while supporting fmtlib/ cuz not std::format not available

        string  a1 = format ("{}", 1);
        wstring a2 = format (L"{}", 1);
        EXPECT_EQ (a1, "1");

        EXPECT_EQ ("a{}a"_f(3), "a3a");

        String a3 = Format ("{}"_f, 3);
        String a4 = Format ("{}"_f, a3);
        String a5 = Format (L"{}"_f, IO::Network::URI{"http://www.sophists.com"});
        DbgTrace ("a5={}"_f, Characters::ToString (a5));
        String a5a = Format ("{}"_f, IO::Network::URI{"http://www.sophists.com"});
        DbgTrace ("a5a={}"_f, Characters::ToString (a5));
        String a6 = Format (L"{}"_f, IO::Network::CIDR{"192.168.244.0/24"});
        DbgTrace ("a6={}"_f, Characters::ToString (a6));
        {
            auto url = IO::Network::URI{"http://www.sophists.com"};
            DbgTrace ("u = {}"_f, Characters::ToString (url));
            DbgTrace (L"u = {}"_f, url);
            DbgTrace ("u = {}"_f, url);
            // Each will output "u = http://www.sophists.com
        }
        String a7 = Format ("{}"_f, foo{});
        DbgTrace ("a7={}"_f, Characters::ToString (a7));
        DbgTrace (L"yippie: {}"_f, foo{});
        EXPECT_EQ (a7, "foo as a string"sv);
        DbgTrace (L"t2:  cidr= {}, s1={}, s2={}"_f, IO::Network::CIDR{"192.168.244.0/24"}, L"s1", String{"s2"});
        DbgTrace ("t2:  cidr= {}, s1={}, s2={}"_f, IO::Network::CIDR{"192.168.244.0/24"}, L"s1", String{"s2"});
        // @todo add mapping for wstring/string even for std  case using my ToString mechanism (details elude me - must be careful to avoid introducing ambiguity)
        //        DbgTrace2 (L"t2:  s1={}, s2={}, s3={}",  "s1", String{"s2"}, string{"s3"});
        DbgTrace ("t2: cidr= {}, s1={}, s2={}, s3={}"_f, IO::Network::CIDR{"192.168.244.0/24"}, L"s1", String{"s2"}, wstring{L"s3"});
        //         DbgTrace2 (L"t2: cidr= {}, s1={}, s2={}, s3={}", IO::Network::CIDR{"192.168.244.0/24"}, "s1", String{"s2"}, string{"s3"});

        auto pp = "{}"_f(1);
        DbgTrace (L"pp={}"_f, pp);
        EXPECT_EQ (pp, "1");

        {
            String big = "1"_k.Repeat (1000);
            EXPECT_EQ (big.size (), 1000u);
            String a = Characters::Format ("BIG shortened '{:.10}'"_f, big);
            DbgTrace ("a={}"_f, a);
            EXPECT_EQ (a.size (), 26u);
            EXPECT_EQ (a, "BIG shortened '1111111111'");
        }
        {
            // EXPECT_EQ ("{}"_f(atomic<int>{3}), "3");    // DOES NOT work because formattable requires T be copyable, and stdc++ requires atomic<T> not copyable
        }
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
