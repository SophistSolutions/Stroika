/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Memory
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/Bits.h"
#include "Stroika/Foundation/Memory/ObjectFieldUtilities.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Memory/SharedByValue.h"

#include "../TestHarness/NotCopyable.h"
#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::TestHarness;

namespace {
    void Test1_Optional ()
    {
        {
            using Characters::String;
            optional<String> x;
            x = String{L"x"};
        }
        {
            optional<int> x;
            VerifyTestResult (not x.has_value ());
            x = 1;
            VerifyTestResult (x.has_value ());
            VerifyTestResult (*x == 1);
        }
        {
            // Careful about self-assignment
            optional<int> x;
            x = 3;
            x = max (*x, 1);
            VerifyTestResult (x == 3);
        }
        auto testOptionalOfThingNotCopyable = [] () {
            {
                optional<NotCopyable> n1;
                VerifyTestResult (not n1.has_value ());
                optional<NotCopyable> n2{NotCopyable ()}; // use r-value reference to move
                VerifyTestResult (n2.has_value ());
            }
            {
                [[maybe_unused]] optional<NotCopyable> a;
                optional<NotCopyable>                  a1{NotCopyable ()};
                a1 = NotCopyable ();
            }
        };
        testOptionalOfThingNotCopyable ();
        {
            optional<int> x;
            if (x) {
                VerifyTestResult (false);
            }
        }
        {
            optional<int> x;
            if (optional<int> y = x) {
                VerifyTestResult (false);
            }
        }
        {
            optional<int> x = 3;
            if (optional<int> y = x) {
                VerifyTestResult (y == 3);
            }
            else {
                VerifyTestResult (false);
            }
        }
        {
            float*  d1 = nullptr;
            double* d2 = nullptr;
            VerifyTestResult (not OptionalFromNullable (d1).has_value ());
            VerifyTestResult (not OptionalFromNullable (d2).has_value ());
        }
        {
            constexpr optional<int> x{1};
            VerifyTestResult (x == 1);
        }
        {
            optional<int>                     d;
            [[maybe_unused]] optional<double> t1 = d;                    // no warnings - this direction OK
            [[maybe_unused]] optional<double> t2 = optional<double> (d); // ""
        }
        {
            [[maybe_unused]] optional<double> d;
            //Optional<uint64_t> t1 = d;                      // should generate warning or error
            // SKIP SINCE SWITCH TO C++ optional - generates warning - optional<uint64_t> t2 = optional<uint64_t> (d); // should not
        }
        {
            optional<int> x = 1;
            VerifyTestResult (Characters::ToString (x) == L"1");
        }
        {
            // empty optional < any other value
            VerifyTestResult (optional<int>{} < -9999);
            VerifyTestResult (optional<int>{-9999} > optional<int>{});
        }
    }
    void Test2_SharedByValue ()
    {
        using Memory::BLOB;
        // par Example Usage from doc header
        SharedByValue<vector<byte>> b{BLOB::Hex ("abcd1245").Repeat (100).As<vector<byte>> ()};
        SharedByValue<vector<byte>> c = b; // copied by reference until 'c' or 'b' changed values
        VerifyTestResult (c.cget () == b.cget ());
    }
    void Test_4_Optional_Of_Mapping_Copy_Problem_ ()
    {
        using namespace Stroika::Foundation::Memory;
        using namespace Stroika::Foundation::Containers;
        Mapping<int, float> ml1, ml2;
        ml1 = ml2;

        optional<Mapping<int, float>> ol1, ol2;
        if (ol2.has_value ()) {
            ml1 = *ol2;
        }
        ol1 = ml1;
        optional<Mapping<int, float>> xxxx2 (ml1);

        // fails to compile prior to 2013-09-09
        optional<Mapping<int, float>> xxxx1 (ol1);
        // fails to compile prior to 2013-09-09
        ol1 = ol2;
    }

    // temporarily put this out here to avoid MSVC compiler bug -- LGP 2014-02-26
    // SB nested inside function where used...
    //  --LGP 2014-02-26
    namespace {
        struct X_ {
            int a = 0;
        };
        struct jimStdSP_ : std::enable_shared_from_this<jimStdSP_> {
            int                   field = 1;
            shared_ptr<jimStdSP_> doIt ()
            {
                return shared_from_this ();
            }
        };
        struct jimMIXStdSP_ : X_, std::enable_shared_from_this<jimMIXStdSP_> {
            int                      field = 1;
            shared_ptr<jimMIXStdSP_> doIt ()
            {
                return shared_from_this ();
            }
        };
    }

}

namespace {
    void Test_6_Bits_ ()
    {
        {
            VerifyTestResult (BitSubstring (0x3, 0, 1) == 1);
            VerifyTestResult (BitSubstring (0x3, 1, 2) == 1);
            VerifyTestResult (BitSubstring (0x3, 2, 3) == 0);
            VerifyTestResult (BitSubstring (0x3, 0, 3) == 0x3);
            VerifyTestResult (BitSubstring (0xff, 0, 8) == 0xff);
            VerifyTestResult (BitSubstring (0xff, 8, 16) == 0x0);
            VerifyTestResult (BitSubstring (0b10101010, 0, 1) == 0x0);
            VerifyTestResult (BitSubstring (0b10101010, 7, 8) == 0x1);
        }
        {
            VerifyTestResult (Bit (0) == 0x1);
            VerifyTestResult (Bit (1) == 0x2);
            VerifyTestResult (Bit (3) == 0x8);
            VerifyTestResult (Bit (15) == 0x8000);
            VerifyTestResult (Bit<int> (1, 2) == 0x6);
            VerifyTestResult (Bit<int> (1, 2, 15) == 0x8006);
        }
    }
}

namespace {
    void Test_7_BLOB_ ()
    {
        {
            vector<uint8_t> b  = {1, 2, 3, 4, 5};
            Memory::BLOB    bl = b;
            VerifyTestResult (bl.size () == 5 and b == bl.As<vector<uint8_t>> ());
            VerifyTestResult (bl.size () == 5 and bl.As<vector<uint8_t>> () == b);
        }
        {
            Memory::BLOB bl{1, 2, 3, 4, 5};
            VerifyTestResult (bl.size () == 5 and bl.As<vector<uint8_t>> () == (vector<uint8_t>{1, 2, 3, 4, 5}));
        }
        {
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wself-assign-overloaded\"")
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wself-move\"")
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wpragmas\"")
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wself-move\"") // PRAGMA WARNING ON GCC11 (g++ on ubuntu 20.04)but needed on i think g++ 13 and later
            Memory::BLOB bl{1, 2, 3, 4, 5};
            bl = bl; // assure self-assign OK
            bl = move (bl);
            VerifyTestResult (bl.size () == 5 and bl.As<vector<uint8_t>> () == (vector<uint8_t>{1, 2, 3, 4, 5}));
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wself-move\"")
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wself-assign-overloaded\"")
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wself-move\"")
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wpragmas\"")
        }
        {
            const char kSrc1_[] = "This is a very good test of a very good test";
            const char kSrc2_[] = "";
            const char kSrc3_[] = "We eat wiggly worms. That was a very good time to eat the worms. They are awesome!";
            const char kSrc4_[] = "0123456789";

            VerifyTestResult (Memory::BLOB ((const byte*)kSrc1_, (const byte*)kSrc1_ + ::strlen (kSrc1_)) ==
                              Memory::BLOB::FromRaw (kSrc1_, kSrc1_ + strlen (kSrc1_)));
            VerifyTestResult (Memory::BLOB ((const byte*)kSrc2_, (const byte*)kSrc2_ + ::strlen (kSrc2_)) ==
                              Memory::BLOB::FromRaw (kSrc2_, kSrc2_ + strlen (kSrc2_)));
            VerifyTestResult (Memory::BLOB ((const byte*)kSrc3_, (const byte*)kSrc3_ + ::strlen (kSrc3_)) ==
                              Memory::BLOB::FromRaw (kSrc3_, kSrc3_ + strlen (kSrc3_)));
            VerifyTestResult (Memory::BLOB ((const byte*)kSrc4_, (const byte*)kSrc4_ + ::strlen (kSrc4_)) ==
                              Memory::BLOB::FromRaw (kSrc4_, kSrc4_ + strlen (kSrc4_)));

            VerifyTestResult (Memory::BLOB ((const byte*)kSrc1_, (const byte*)kSrc1_ + ::strlen (kSrc1_)) ==
                              Memory::BLOB::FromRaw (kSrc1_, kSrc1_ + NEltsOf (kSrc1_) - 1));
            VerifyTestResult (Memory::BLOB ((const byte*)kSrc2_, (const byte*)kSrc2_ + ::strlen (kSrc2_)) ==
                              Memory::BLOB::FromRaw (kSrc2_, kSrc2_ + NEltsOf (kSrc2_) - 1));
            VerifyTestResult (Memory::BLOB ((const byte*)kSrc3_, (const byte*)kSrc3_ + ::strlen (kSrc3_)) ==
                              Memory::BLOB::FromRaw (kSrc3_, kSrc3_ + NEltsOf (kSrc3_) - 1));
            VerifyTestResult (Memory::BLOB ((const byte*)kSrc4_, (const byte*)kSrc4_ + ::strlen (kSrc4_)) ==
                              Memory::BLOB::FromRaw (kSrc4_, kSrc4_ + NEltsOf (kSrc4_) - 1));
        }
        {
            using Memory::BLOB;
            VerifyTestResult ((BLOB::FromHex ("61 70 70 6c 65 73 20 61 6e 64 20 70 65 61 72 73 0d 0a") ==
                               BLOB{0x61, 0x70, 0x70, 0x6c, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x70, 0x65, 0x61, 0x72, 0x73, 0x0d, 0x0a}));
            VerifyTestResult ((BLOB::FromHex ("4a 94 99 ac 55 f7 a2 8b 1b ca 75 62 f6 9a cf de 41 9d") ==
                               BLOB{0x4a, 0x94, 0x99, 0xac, 0x55, 0xf7, 0xa2, 0x8b, 0x1b, 0xca, 0x75, 0x62, 0xf6, 0x9a, 0xcf, 0xde, 0x41, 0x9d}));
            VerifyTestResult ((BLOB::FromHex ("68 69 20 6d 6f 6d 0d 0a") == BLOB{0x68, 0x69, 0x20, 0x6d, 0x6f, 0x6d, 0x0d, 0x0a}));
            VerifyTestResult ((BLOB::FromHex ("29 14 4a db 4e ce 20 45 09 56 e8 13 65 2f e8 d6") ==
                               BLOB{0x29, 0x14, 0x4a, 0xdb, 0x4e, 0xce, 0x20, 0x45, 0x09, 0x56, 0xe8, 0x13, 0x65, 0x2f, 0xe8, 0xd6}));
            VerifyTestResult ((BLOB::FromHex ("29144adb4ece20450956e813652fe8d6") ==
                               BLOB{0x29, 0x14, 0x4a, 0xdb, 0x4e, 0xce, 0x20, 0x45, 0x09, 0x56, 0xe8, 0x13, 0x65, 0x2f, 0xe8, 0xd6}));
            VerifyTestResult ((BLOB::FromHex ("29144adb4ece20450956e813652fe8d6").AsHex () == "29144adb4ece20450956e813652fe8d6"));
        }
    }
}

namespace {
    namespace Test9a_Buffer_ {
        void DoTest ()
        {
            {
                InlineBuffer<int> x0{0};
                InlineBuffer<int> x1{x0};
                x0 = x1;
            }
            {
                // Test using String elements, since those will test construction/reserve logic
                using Characters::String;
                InlineBuffer<String> buf1{3};
                for (int i = 0; i < 1000; i++) {
                    buf1.push_back (String{L"hi mom"});
                }
                InlineBuffer<String> buf2{buf1};
                buf1.resize (0);
            }
            {
                InlineBuffer<int> x0{4};
                InlineBuffer<int> assign2;
                assign2 = x0;
                VerifyTestResult (x0.size () == assign2.size ()); // test regression fixed 2019-03-20
                VerifyTestResult (x0.size () == 4);
            }
        }
    }
    namespace Test9b_StackBuffer_ {
        void DoTest ()
        {
            {
                StackBuffer<int> x0{0};
            }
            {
                // Test using String elements, since those will test construction/reserve logic
                using Characters::String;
                StackBuffer<String> buf1{3};
                for (int i = 0; i < 1000; i++) {
                    buf1.push_back (String{L"hi mom"});
                }
                buf1.resize (0);
            }
            {
                StackBuffer<int> x0{4};
                StackBuffer<int> assign2;
                VerifyTestResult (x0.size () == 4);
            }
        }
    }
}

namespace {
    namespace Test10_OptionalSelfAssign_ {
        void DoTest ()
        {
            {
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 7)) ||                                                         \
    (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 10))
                DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wself-assign-overloaded\""); // explicitly assigning value of variable ... to itself
#endif
                // ASSIGN
                {
                    optional<int> x;
                    x = x;
                }
                {
                    optional<Characters::String> x;
                    x = x;
                }
                {
                    optional<int> x{1};
                    x = x;
                }
                {
                    optional<Characters::String> x{L"x"};
                    x = x;
                }
            }
            // note - see https://stroika.atlassian.net/browse/STK-556 - we DON'T support Optional self-move
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 7)) ||                                                         \
    (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 10))
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wself-assign-overloaded\"");
#endif
        }
    }
}

namespace {
    namespace Test11_ObjectFieldUtilities_ {
        void DoTest ();
        namespace Private_ {
            struct X1 {
                int a;
                int b;
            };
            struct X2 {
            public:
                int a;

            private:
                int b;

            private:
                friend void Test11_ObjectFieldUtilities_::DoTest ();
            };
        }
        void DoTest ()
        {
            {
                VerifyTestResult (OffsetOf (&Private_::X1::a) == 0);
                VerifyTestResult (OffsetOf (&Private_::X1::b) >= sizeof (int));
            }
            {
                Private_::X1 t;
                static_assert (is_standard_layout_v<Private_::X1>);
                void* aAddr = &t.a;
                void* bAddr = &t.b;
                VerifyTestResult (GetObjectOwningField (aAddr, &Private_::X1::a) == &t);
                VerifyTestResult (GetObjectOwningField (bAddr, &Private_::X1::b) == &t);
            }
            {
                // Check and warning but since X2 is not standard layout, this isn't guaranteed to work
                Private_::X2 t;
                static_assert (not is_standard_layout_v<Private_::X2>);
                void* aAddr = &t.a;
                void* bAddr = &t.b;
                VerifyTestResultWarning (GetObjectOwningField (aAddr, &Private_::X2::a) == &t);
                VerifyTestResultWarning (GetObjectOwningField (bAddr, &Private_::X2::b) == &t);
            }
        }
    }
}

namespace {
    namespace Test12_OffsetOf_ {
        struct Person {
            int firstName;
            int lastName;
        };
        struct NotDefaultConstructible {
            NotDefaultConstructible () = delete;
            constexpr NotDefaultConstructible (int)
            {
            }
            int firstName{};
            int lastName{};
        };
        void DoTest ()
        {
            {
                [[maybe_unused]] size_t kOffset_ = OffsetOf (&Person::lastName);
                VerifyTestResult (OffsetOf (&Person::firstName) == 0);
            }
            {
                [[maybe_unused]] size_t kOffset_ = OffsetOf (&NotDefaultConstructible::lastName);
                VerifyTestResult (OffsetOf (&NotDefaultConstructible::firstName) == 0);
            }
#if 0
            // disabled til we can figure out a way to get this constexpr version of OffsetOf() working...
            {
                [[maybe_unused]] constexpr size_t kOffsetx_ = OffsetOf_Constexpr (&Person::lastName);
                static_assert (OffsetOf_Constexpr (&Person::firstName) == 0);
            }
#endif
        }
    }

}

namespace {
    namespace Test13_Resize_ {
        void DoTest ()
        {
            size_t           currentCapacity = 0;
            unsigned int     countOfReallocCopies{};
            constexpr size_t kCountOfResizes_ = 500 * 1024;
            for (size_t len = 0; len < kCountOfResizes_; ++len) {
                if (len > currentCapacity) {
                    size_t newCapacity = Containers::Support::ReserveTweaks::GetScaledUpCapacity (len);
                    VerifyTestResult (newCapacity >= len);
                    currentCapacity = newCapacity;
                    //DbgTrace (L"For %d (%f its log) resizes, we got %d reallocs, and allocated size=%d", len, log (len), countOfReallocCopies, newCapacity);
                    if (len > 1) {
                        VerifyTestResult (countOfReallocCopies < log (len) * 4); // grows roughly logarithmically, but factor depends on scaling in GetScaledUpCapacity
                    }
                    ++countOfReallocCopies;
                }
            }
            DbgTrace (L"For %d (%f its log) resizes, we got %d reallocs", kCountOfResizes_, log (kCountOfResizes_), countOfReallocCopies);
            VerifyTestResultWarning (5 <= countOfReallocCopies and countOfReallocCopies <= 50);
        }
    }
}

namespace {
    namespace Test14_OffsetOf_ {
        namespace Private_ {
            struct s {
                float a;
                char  b;
                char  bb;
                int   c;
                s () = delete; // no constructor
            };
#pragma pack(push, 1)
            struct s2 {
                float  a;
                char   b;
                char   bb;
                int    c;
                double d;
                char   e;
            };
#pragma pack(pop)
            struct a {
                int i;
                int j;
            };
            struct b {
                int i;
                int k;
            };
            struct ab : public a, public b {};
            DISABLE_COMPILER_MSC_WARNING_START (4121);
            DISABLE_COMPILER_MSC_WARNING_START (4324);
            struct alignas (16) al {
                float a;
                alignas (8) char b;
                char bb;
                char arr[20];
            };
#pragma pack(push, 2)
            struct al2 {
                char a;
                int  b;
                char c;
            };
#pragma pack(pop)
            DISABLE_COMPILER_MSC_WARNING_END (4121);
            DISABLE_COMPILER_MSC_WARNING_END (4324);
        }

        void DoTest ()
        {
            using namespace Private_;
            // no constructor, default aligning
            VerifyTestResult (OffsetOf (&s::a) == 0);
            VerifyTestResult (OffsetOf (&s::b) == sizeof (float));
            VerifyTestResult (OffsetOf (&s::bb) == sizeof (float) + sizeof (char));
            VerifyTestResult (OffsetOf (&s::c) == alignof (s) * 2); // aligned b with bb

            // no alignment
            VerifyTestResult (OffsetOf (&s2::a) == 0);
            VerifyTestResult (OffsetOf (&s2::b) == sizeof (float));
            VerifyTestResult (OffsetOf (&s2::bb) == sizeof (float) + sizeof (char));
            VerifyTestResult (OffsetOf (&s2::c) == sizeof (float) + sizeof (char) * 2);
            VerifyTestResult (OffsetOf (&s2::d) == sizeof (float) + sizeof (char) * 2 + sizeof (int));
            VerifyTestResult (OffsetOf (&s2::e) == sizeof (float) + sizeof (char) * 2 + sizeof (int) + sizeof (double));
            static_assert (is_standard_layout_v<s2>);
            VerifyTestResult (OffsetOf (&s2::a) == offsetof (s2, a));
            VerifyTestResult (OffsetOf (&s2::b) == offsetof (s2, b));
            VerifyTestResult (OffsetOf (&s2::bb) == offsetof (s2, bb));
            VerifyTestResult (OffsetOf (&s2::c) == offsetof (s2, c));
            VerifyTestResult (OffsetOf (&s2::d) == offsetof (s2, d));
            VerifyTestResult (OffsetOf (&s2::e) == offsetof (s2, e));

            // simply
            VerifyTestResult (OffsetOf (&a::i) == 0);
            VerifyTestResult (OffsetOf (&a::j) == sizeof (int));
            VerifyTestResult (OffsetOf (&b::i) == 0);
            VerifyTestResult (OffsetOf (&b::k) == sizeof (int));

            // other based
            //Assert (OffsetOf(&ab::j) == sizeof (int));
            //Assert (OffsetOf<ab> (&ab::k) == sizeof (int) * 3);

            // special alignments
            VerifyTestResult (OffsetOf (&al::a) == 0);
            VerifyTestResult (OffsetOf (&al::b) == 8);
            VerifyTestResult (OffsetOf (&al::bb) == 9);
            // Assert (OffsetOf (&al::arr) == 16);

            VerifyTestResult (OffsetOf (&al2::a) == 0);
            VerifyTestResult (OffsetOf (&al2::b) == 2);
            VerifyTestResult (OffsetOf (&al2::c) == 6);
        }
    }
}

namespace {
    namespace Test15_Span_ {
        void DoTest ()
        {
            {
                char buf1[1024];
                char buf2[1024];
                VerifyTestResult (not Intersects (span{buf1}, span{buf2}));
                VerifyTestResult (Intersects (span{buf1}, span{buf1}));
                VerifyTestResult (Intersects (span{buf1}.subspan (3, 10), span{buf1}.subspan (4, 10)));
            }
        }
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        Test1_Optional ();
        Test2_SharedByValue ();
        Test_4_Optional_Of_Mapping_Copy_Problem_ ();
        Test_6_Bits_ ();
        Test_7_BLOB_ ();
        Test9a_Buffer_::DoTest ();
        Test9b_StackBuffer_::DoTest ();
        Test10_OptionalSelfAssign_::DoTest ();
        Test11_ObjectFieldUtilities_::DoTest ();
        Test12_OffsetOf_::DoTest ();
        Test13_Resize_::DoTest ();
        Test14_OffsetOf_::DoTest ();
        Test15_Span_::DoTest ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
