/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Memory
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"

#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Memory/Bits.h"
#include "Stroika/Foundation/Memory/ObjectFieldUtilities.h"
#include "Stroika/Foundation/Memory/Optional.h"
#include "Stroika/Foundation/Memory/SharedByValue.h"

#include "Stroika/Frameworks/Test/ArchtypeClasses.h"
#include "Stroika/Frameworks/Test/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;

using Test::ArchtypeClasses::OnlyDefaultConstructibleAndMoveable;

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Foundation_Memory_, Test1_Optional_)
    {
        {
            using Characters::String;
            optional<String> x;
            x = String{L"x"};
        }
        {
            optional<int> x;
            EXPECT_TRUE (not x.has_value ());
            x = 1;
            EXPECT_TRUE (x.has_value ());
            EXPECT_TRUE (*x == 1);
        }
        {
            // Careful about self-assignment
            optional<int> x;
            x = 3;
            x = max (*x, 1);
            EXPECT_TRUE (x == 3);
        }
        auto testOptionalOfThingNotCopyable = [] () {
            {
                optional<OnlyDefaultConstructibleAndMoveable> n1;
                EXPECT_TRUE (not n1.has_value ());
                optional<OnlyDefaultConstructibleAndMoveable> n2{OnlyDefaultConstructibleAndMoveable ()}; // use r-value reference to move
                EXPECT_TRUE (n2.has_value ());
            }
            {
                [[maybe_unused]] optional<OnlyDefaultConstructibleAndMoveable> a;
                optional<OnlyDefaultConstructibleAndMoveable>                  a1{OnlyDefaultConstructibleAndMoveable ()};
                a1 = OnlyDefaultConstructibleAndMoveable ();
            }
        };
        testOptionalOfThingNotCopyable ();
        {
            optional<int> x;
            if (x) {
                EXPECT_TRUE (false);
            }
        }
        {
            optional<int> x;
            if (optional<int> y = x) {
                EXPECT_TRUE (false);
            }
        }
        {
            optional<int> x = 3;
            if (optional<int> y = x) {
                EXPECT_TRUE (y == 3);
            }
            else {
                EXPECT_TRUE (false);
            }
        }
        {
            float*  d1 = nullptr;
            double* d2 = nullptr;
            EXPECT_TRUE (not OptionalFromNullable (d1).has_value ());
            EXPECT_TRUE (not OptionalFromNullable (d2).has_value ());
        }
        {
            constexpr optional<int> x{1};
            EXPECT_EQ (x, 1);
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
            EXPECT_TRUE (Characters::ToString (x) == L"1");
        }
        {
            // empty optional < any other value
            EXPECT_TRUE (optional<int>{} < -9999);
            EXPECT_TRUE (optional<int>{-9999} > optional<int>{});
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Memory_, Test2_SharedByValue)
    {
        using Memory::BLOB;
        // par Example Usage from doc header
        SharedByValue<vector<byte>> b{BLOB::FromHex ("abcd1245").Repeat (100).As<vector<byte>> ()};
        SharedByValue<vector<byte>> c = b; // copied by reference until 'c' or 'b' changed values
        EXPECT_TRUE (c.cget () == b.cget ());
    }
}

namespace {
    GTEST_TEST (Foundation_Memory_, Test_4_Optional_Of_Mapping_Copy_Problem_)
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
}

namespace {
    GTEST_TEST (Foundation_Memory_, Test_6_Bits_)
    {
        //TEST now if compiler bug gone and works... --LGP 2024-05-25
        //
        // temporarily put this out here to avoid MSVC compiler bug -- LGP 2014-02-26
        // SB nested inside function where used...
        //  --LGP 2014-02-26
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
        {
            EXPECT_EQ (Bit (0), 0x1u);
            EXPECT_EQ (Bit (1), 0x2u);
            EXPECT_EQ (Bit (3), 0x8u);
            EXPECT_EQ (Bit (15), 0x8000u);
            EXPECT_EQ (Bit<int> (1, 2), 6);
            EXPECT_EQ (Bit<int> (1, 2, 15), 0x8006);
        }
        {
            EXPECT_EQ (BitSubstring (0x3, 0, 1), 1);
            EXPECT_EQ (BitSubstring (0x3, 1, 2), 1);
            EXPECT_EQ (BitSubstring (0x3, 2, 3), 0);
            EXPECT_EQ (BitSubstring (0x3, 0, 3), 0x3);
            EXPECT_EQ (BitSubstring (0xff, 0, 8), 0xff);
            EXPECT_EQ (BitSubstring (0xff, 8, 16), 0x0);
            EXPECT_EQ (BitSubstring (0b10101010, 0, 1), 0x0);
            EXPECT_EQ (BitSubstring (0b10101010, 7, 8), 0x1);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Memory_, Test_7_BLOB_)
    {
        {
            vector<uint8_t> b  = {1, 2, 3, 4, 5};
            Memory::BLOB    bl = b;
            EXPECT_TRUE (bl.size () == 5 and b == bl.As<vector<uint8_t>> ());
            EXPECT_TRUE (bl.size () == 5 and bl.As<vector<uint8_t>> () == b);
        }
        {
            Memory::BLOB bl{1, 2, 3, 4, 5};
            EXPECT_TRUE (bl.size () == 5 and bl.As<vector<uint8_t>> () == (vector<uint8_t>{1, 2, 3, 4, 5}));
        }
        {
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wself-assign-overloaded\"")
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wself-move\"")
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wpragmas\"")
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wself-move\"") // PRAGMA WARNING ON GCC11 (g++ on ubuntu 20.04)but needed on i think g++ 13 and later
            Memory::BLOB bl{1, 2, 3, 4, 5};
            bl = bl; // assure self-assign OK
            bl = move (bl);
            EXPECT_TRUE (bl.size () == 5 and bl.As<vector<uint8_t>> () == (vector<uint8_t>{1, 2, 3, 4, 5}));
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

            EXPECT_TRUE (Memory::BLOB ((const byte*)kSrc1_, (const byte*)kSrc1_ + ::strlen (kSrc1_)) ==
                         Memory::BLOB::FromRaw (kSrc1_, kSrc1_ + strlen (kSrc1_)));
            EXPECT_TRUE (Memory::BLOB ((const byte*)kSrc2_, (const byte*)kSrc2_ + ::strlen (kSrc2_)) ==
                         Memory::BLOB::FromRaw (kSrc2_, kSrc2_ + strlen (kSrc2_)));
            EXPECT_TRUE (Memory::BLOB ((const byte*)kSrc3_, (const byte*)kSrc3_ + ::strlen (kSrc3_)) ==
                         Memory::BLOB::FromRaw (kSrc3_, kSrc3_ + strlen (kSrc3_)));
            EXPECT_TRUE (Memory::BLOB ((const byte*)kSrc4_, (const byte*)kSrc4_ + ::strlen (kSrc4_)) ==
                         Memory::BLOB::FromRaw (kSrc4_, kSrc4_ + strlen (kSrc4_)));

            EXPECT_TRUE (Memory::BLOB ((const byte*)kSrc1_, (const byte*)kSrc1_ + ::strlen (kSrc1_)) ==
                         Memory::BLOB::FromRaw (kSrc1_, kSrc1_ + NEltsOf (kSrc1_) - 1));
            EXPECT_TRUE (Memory::BLOB ((const byte*)kSrc2_, (const byte*)kSrc2_ + ::strlen (kSrc2_)) ==
                         Memory::BLOB::FromRaw (kSrc2_, kSrc2_ + NEltsOf (kSrc2_) - 1));
            EXPECT_TRUE (Memory::BLOB ((const byte*)kSrc3_, (const byte*)kSrc3_ + ::strlen (kSrc3_)) ==
                         Memory::BLOB::FromRaw (kSrc3_, kSrc3_ + NEltsOf (kSrc3_) - 1));
            EXPECT_TRUE (Memory::BLOB ((const byte*)kSrc4_, (const byte*)kSrc4_ + ::strlen (kSrc4_)) ==
                         Memory::BLOB::FromRaw (kSrc4_, kSrc4_ + NEltsOf (kSrc4_) - 1));
        }
        {
            using Memory::BLOB;
            EXPECT_TRUE ((BLOB::FromHex ("61 70 70 6c 65 73 20 61 6e 64 20 70 65 61 72 73 0d 0a") ==
                          BLOB{0x61, 0x70, 0x70, 0x6c, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x70, 0x65, 0x61, 0x72, 0x73, 0x0d, 0x0a}));
            EXPECT_TRUE ((BLOB::FromHex ("4a 94 99 ac 55 f7 a2 8b 1b ca 75 62 f6 9a cf de 41 9d") ==
                          BLOB{0x4a, 0x94, 0x99, 0xac, 0x55, 0xf7, 0xa2, 0x8b, 0x1b, 0xca, 0x75, 0x62, 0xf6, 0x9a, 0xcf, 0xde, 0x41, 0x9d}));
            EXPECT_TRUE ((BLOB::FromHex ("68 69 20 6d 6f 6d 0d 0a") == BLOB{0x68, 0x69, 0x20, 0x6d, 0x6f, 0x6d, 0x0d, 0x0a}));
            EXPECT_TRUE ((BLOB::FromHex ("29 14 4a db 4e ce 20 45 09 56 e8 13 65 2f e8 d6") ==
                          BLOB{0x29, 0x14, 0x4a, 0xdb, 0x4e, 0xce, 0x20, 0x45, 0x09, 0x56, 0xe8, 0x13, 0x65, 0x2f, 0xe8, 0xd6}));
            EXPECT_TRUE ((BLOB::FromHex ("29144adb4ece20450956e813652fe8d6") ==
                          BLOB{0x29, 0x14, 0x4a, 0xdb, 0x4e, 0xce, 0x20, 0x45, 0x09, 0x56, 0xe8, 0x13, 0x65, 0x2f, 0xe8, 0xd6}));
            EXPECT_TRUE ((BLOB::FromHex ("29144adb4ece20450956e813652fe8d6").AsHex () == "29144adb4ece20450956e813652fe8d6"));
        }
        {
            using Memory::BLOB;
            EXPECT_TRUE ((BLOB::FromBase64 ("aGVsbG8=") == BLOB{'h', 'e', 'l', 'l', 'o'}));
            EXPECT_TRUE ((BLOB{'h', 'e', 'l', 'l', 'o'}.AsBase64 () == "aGVsbG8="));
        }
        {
            using Memory::BLOB;
            constexpr char kRawBytes_[] = "jwv8YQUAAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAA/45JREFU\r\n"
                                          "eF5M/XeTpPl1pQlmFcAWM99tbeavWZvtXRsba8EeatUECRIgAEKLKqC0zqpKrTNDeoRrLUNmVoGa\r\n"
                                          "7Nk12w8wa9PdBOrd5znXAzYkg1Hp4f4qf3/nPffcc+996ezsefNF8/K15pcvXXv55V9c++LlL13j\r\n";
            auto           b1           = BLOB::FromBase64 (kRawBytes_);
            auto           b2s          = b1.AsBase64 ();
            auto           b2           = BLOB::FromBase64 (b2s);
            EXPECT_EQ (b1, b2);
        }
        {
            using namespace Memory;
            auto b = "hello"_blob;
            EXPECT_EQ (b.size (), 5u);
            EXPECT_EQ (b[0], static_cast<byte> ('h'));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Memory_, Test9a_Buffer_)
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
                buf1.push_back (String{"hi mom"});
            }
            InlineBuffer<String> buf2{buf1};
            buf1.resize (0);
        }
        {
            InlineBuffer<int> x0{4};
            InlineBuffer<int> assign2;
            assign2 = x0;
            EXPECT_TRUE (x0.size () == assign2.size ()); // test regression fixed 2019-03-20
            EXPECT_TRUE (x0.size () == 4);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Memory_, Test9b_StackBuffer_)
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
            EXPECT_EQ (x0.size (), 4u);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Memory_, Test10_OptionalSelfAssign_)
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
        // note - see http://stroika-bugs.sophists.com/browse/STK-556 - we DON'T support Optional self-move
#if (defined(__clang_major__) && !defined(__APPLE__) && (__clang_major__ >= 7)) ||                                                         \
    (defined(__clang_major__) && defined(__APPLE__) && (__clang_major__ >= 10))
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wself-assign-overloaded\"");
#endif
    }
}

namespace {
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

        public:
            void* getBAddr ()
            {
                return &b;
            }
            static auto getBAddrOffset ()
            {
                return &X2::b;
            }

        private:
            FRIEND_TEST (Foundation_Memory_, Test11_ObjectFieldUtilities_);
        };
    }
    GTEST_TEST (Foundation_Memory_, Test11_ObjectFieldUtilities_)
    {
        {
            EXPECT_TRUE (OffsetOf (&Private_::X1::a) == 0);
            EXPECT_TRUE (OffsetOf (&Private_::X1::b) >= sizeof (int));
        }
        {
            Private_::X1 t;
            static_assert (is_standard_layout_v<Private_::X1>);
            void* aAddr = &t.a;
            void* bAddr = &t.b;
            EXPECT_TRUE (GetObjectOwningField (aAddr, &Private_::X1::a) == &t);
            EXPECT_TRUE (GetObjectOwningField (bAddr, &Private_::X1::b) == &t);
        }
        {
            // Check and warning but since X2 is not standard layout, this isn't guaranteed to work
            Private_::X2 t;
            static_assert (not is_standard_layout_v<Private_::X2>);
            void* aAddr = &t.a;
            void* bAddr = t.getBAddr ();
            VerifyTestResultWarning (GetObjectOwningField (aAddr, &Private_::X2::a) == &t);
            VerifyTestResultWarning (GetObjectOwningField (bAddr, Private_::X2::getBAddrOffset ()) == &t);
        }
    }
}

namespace {
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
    GTEST_TEST (Foundation_Memory_, Test12_OffsetOf_)
    {
        {
            [[maybe_unused]] size_t kOffset_ = OffsetOf (&Person::lastName);
            EXPECT_TRUE (OffsetOf (&Person::firstName) == 0);
        }
        {
            [[maybe_unused]] size_t kOffset_ = OffsetOf (&NotDefaultConstructible::lastName);
            EXPECT_TRUE (OffsetOf (&NotDefaultConstructible::firstName) == 0);
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

namespace {
    GTEST_TEST (Foundation_Memory_, Test13_Resize_)
    {
        size_t           currentCapacity = 0;
        unsigned int     countOfReallocCopies{};
        constexpr size_t kCountOfResizes_ = 500 * 1024;
        for (size_t len = 0; len < kCountOfResizes_; ++len) {
            if (len > currentCapacity) {
                size_t newCapacity = Containers::Support::ReserveTweaks::GetScaledUpCapacity (len);
                EXPECT_TRUE (newCapacity >= len);
                currentCapacity = newCapacity;
                //DbgTrace (L"For %d (%f its log) resizes, we got %d reallocs, and allocated size=%d", len, log (len), countOfReallocCopies, newCapacity);
                if (len > 1) {
                    EXPECT_TRUE (countOfReallocCopies < log (len) * 4); // grows roughly logarithmically, but factor depends on scaling in GetScaledUpCapacity
                }
                ++countOfReallocCopies;
            }
        }
        DbgTrace (L"For {} ({} its log) resizes, we got {} reallocs"_f, kCountOfResizes_, log (kCountOfResizes_), countOfReallocCopies);
        VerifyTestResultWarning (5 <= countOfReallocCopies and countOfReallocCopies <= 50);
    }
}

namespace {
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

    GTEST_TEST (Foundation_Memory_, Test14_OffsetOf_)
    {
        using namespace Private_;
        // no constructor, default aligning
        EXPECT_TRUE (OffsetOf (&s::a) == 0);
        EXPECT_TRUE (OffsetOf (&s::b) == sizeof (float));
        EXPECT_TRUE (OffsetOf (&s::bb) == sizeof (float) + sizeof (char));
        EXPECT_TRUE (OffsetOf (&s::c) == alignof (s) * 2); // aligned b with bb

        // no alignment
        EXPECT_TRUE (OffsetOf (&s2::a) == 0);
        EXPECT_TRUE (OffsetOf (&s2::b) == sizeof (float));
        EXPECT_TRUE (OffsetOf (&s2::bb) == sizeof (float) + sizeof (char));
        EXPECT_TRUE (OffsetOf (&s2::c) == sizeof (float) + sizeof (char) * 2);
        EXPECT_TRUE (OffsetOf (&s2::d) == sizeof (float) + sizeof (char) * 2 + sizeof (int));
        EXPECT_TRUE (OffsetOf (&s2::e) == sizeof (float) + sizeof (char) * 2 + sizeof (int) + sizeof (double));
        static_assert (is_standard_layout_v<s2>);
        EXPECT_TRUE (OffsetOf (&s2::a) == offsetof (s2, a));
        EXPECT_TRUE (OffsetOf (&s2::b) == offsetof (s2, b));
        EXPECT_TRUE (OffsetOf (&s2::bb) == offsetof (s2, bb));
        EXPECT_TRUE (OffsetOf (&s2::c) == offsetof (s2, c));
        EXPECT_TRUE (OffsetOf (&s2::d) == offsetof (s2, d));
        EXPECT_TRUE (OffsetOf (&s2::e) == offsetof (s2, e));

        // simply
        EXPECT_TRUE (OffsetOf (&a::i) == 0);
        EXPECT_TRUE (OffsetOf (&a::j) == sizeof (int));
        EXPECT_TRUE (OffsetOf (&b::i) == 0);
        EXPECT_TRUE (OffsetOf (&b::k) == sizeof (int));

        // other based
        //Assert (OffsetOf(&ab::j) == sizeof (int));
        //Assert (OffsetOf<ab> (&ab::k) == sizeof (int) * 3);

        // special alignments
        EXPECT_TRUE (OffsetOf (&al::a) == 0);
        EXPECT_TRUE (OffsetOf (&al::b) == 8);
        EXPECT_TRUE (OffsetOf (&al::bb) == 9);
        // Assert (OffsetOf (&al::arr) == 16);

        EXPECT_TRUE (OffsetOf (&al2::a) == 0);
        EXPECT_TRUE (OffsetOf (&al2::b) == 2);
        EXPECT_TRUE (OffsetOf (&al2::c) == 6);
    }
}

namespace {
    GTEST_TEST (Foundation_Memory_, Test15_Span_)
    {
        {
            char buf1[1024];
            char buf2[1024];
            EXPECT_TRUE (not Intersects (span{buf1}, span{buf2}));
            EXPECT_TRUE (Intersects (span{buf1}, span{buf1}));
            EXPECT_TRUE (Intersects (span{buf1}.subspan (3, 10), span{buf1}.subspan (4, 10)));
        }
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
