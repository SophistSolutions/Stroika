/*
 * Copyright(c) Sophist Solutions Inc. 1990-2014.  All rights reserved
 */
//  TEST    Foundation::Memory
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Containers/Mapping.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Debug/Trace.h"

#include    "Stroika/Foundation/Memory/AnyVariantValue.h"
#include    "Stroika/Foundation/Memory/Bits.h"
#include    "Stroika/Foundation/Memory/BLOB.h"
#include    "Stroika/Foundation/Memory/Optional.h"
#include    "Stroika/Foundation/Memory/SharedByValue.h"
#include    "Stroika/Foundation/Memory/SharedPtr.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;


//TODO: DOES IT EVEN NEED TO BE SAID? THese tests are a bit sparse ;-)

namespace   {
    void    Test1_Optional ()
    {
#if     !qDebug
        {
            struct a {
                int aaa;
                int* aaap;
            };
            VerifyTestResult (sizeof (Optional<int>) == sizeof (a));
        }
#endif
        {
            Optional<int>   x;
            VerifyTestResult (x.IsMissing ());
            x = 1;
            VerifyTestResult (not x.IsMissing ());
            VerifyTestResult (x.IsPresent ());
            VerifyTestResult (*x == 1);
        } {
            // Careful about self-assignment
            Optional<int>   x;
            x = 3;
            x = max (*x, 1);
            VerifyTestResult (x == 3);
        }
        auto testOptionalOfThingNotCopyable = [] () {
            struct NotCopyable {
                NotCopyable () {}
                NotCopyable (const NotCopyable&&) {}    // but is moveable!
                NotCopyable (const NotCopyable&) = delete;
                const NotCopyable& operator= (const NotCopyable&) = delete;
            };
            Optional<NotCopyable>   n1;
            VerifyTestResult (n1.IsMissing ());
            Optional<NotCopyable>   n2 (std::move (NotCopyable ()));    // use r-value reference to move
            VerifyTestResult (n2.IsPresent ());
        };
        testOptionalOfThingNotCopyable ();
        {
            Optional<int> x;
            if (x) {
                VerifyTestResult (false);
            }
        }
        {
            Optional<int> x;
            if (Optional<int> y = x) {
                VerifyTestResult (false);
            }
        }
        {
            Optional<int> x = 3;
            if (Optional<int> y = x) {
                VerifyTestResult (y == 3);
            }
            else {
                VerifyTestResult (false);
            }
        }
        {
            float*  d1  =   nullptr;
            double* d2  =   nullptr;
            VerifyTestResult (Optional<double>::OptionalFromNullable (d1).IsMissing ());
            VerifyTestResult (Optional<double>::OptionalFromNullable (d2).IsMissing ());
        }
    }
    void    Test2_SharedByValue ()
    {
    }
    void    Test_4_Optional_Of_Mapping_Copy_Problem_ ()
    {
        using   namespace   Stroika::Foundation::Memory;
        using   namespace   Stroika::Foundation::Containers;
        Mapping<int, float>  ml1, ml2;
        ml1 = ml2;

        Optional<Mapping<int, float>>  ol1, ol2;
        if (ol2.IsPresent ()) {
            ml1 = *ol2;
        }
        ol1 = ml1;
        Optional<Mapping<int, float>>  xxxx2 (ml1);

        // fails to compile prior to 2013-09-09
        Optional<Mapping<int, float>>  xxxx1 (ol1);
        // fails to compile prior to 2013-09-09
        ol1 = ol2;
    }
    void    Test_5_AnyVariantValue_ ()
    {
        {
            VerifyTestResult (AnyVariantValue ().empty ());
            VerifyTestResult (not AnyVariantValue (1).empty ());
            VerifyTestResult (not AnyVariantValue ("1").empty ());
            //VerifyTestResult (AnyVariantValue ("1").GetType () == typeid ("1"));  // not sure why this fails but not worthy worrying about yet
            VerifyTestResult (AnyVariantValue (1).As<int> () == 1);
        }
        {
            AnyVariantValue v;
            VerifyTestResult (v.empty ());
            v = AnyVariantValue (1);
            VerifyTestResult (not v.empty ());
            VerifyTestResult (v.GetType () == typeid (1));
            VerifyTestResult (v.As<int> () == 1);
            v = AnyVariantValue (L"a");
            //VerifyTestResult (v.GetType () == typeid (L"a")); // not sure why this fails but not worthy worrying about yet
            VerifyTestResult (not v.empty ());
            v.clear ();
            VerifyTestResult (v.empty ());
            VerifyTestResult (v.GetType () == typeid (void));
        }
        {
            struct JIM {
                int a;
            };
            AnyVariantValue v;
            VerifyTestResult (v.empty ());
            v = AnyVariantValue (JIM ());
            VerifyTestResult (v.GetType () == typeid (JIM));
        }
        {
            AnyVariantValue v;
            VerifyTestResult (v.empty ());
            v = AnyVariantValue (1);
            v = v;
            VerifyTestResult (v.GetType () == typeid (1));
            VerifyTestResult (v.As<int> () == 1);
            v = AnyVariantValue (v);
            VerifyTestResult (v.GetType () == typeid (1));
            VerifyTestResult (v.As<int> () == 1);
        }
        {
            static int nCopies = 0;
            struct Copyable {
                Copyable ()
                {
                    ++nCopies;
                }
                Copyable (const Copyable&)
                {
                    ++nCopies;
                }
                ~Copyable ()
                {
                    --nCopies;
                }
                const Copyable& operator= (const Copyable&) = delete;
            };
            {
                AnyVariantValue v;
                VerifyTestResult (v.empty ());
                v = AnyVariantValue (Copyable ());
                v = v;
                v = AnyVariantValue (AnyVariantValue (v));
                v = AnyVariantValue (AnyVariantValue (Copyable ()));
                VerifyTestResult (v.GetType () == typeid (Copyable));
            }
            VerifyTestResult (0 == nCopies);
        }
    }



    // temporarily put this out here to avoid MSVC compiler bug -- LGP 2014-02-26
    // SB nested inside function where used...
    //  --LGP 2014-02-26
    namespace {
        struct X_ { int a = 0; };
        struct jimStdSP_ : std::enable_shared_from_this<jimStdSP_> {
            int field = 1;
            shared_ptr<jimStdSP_> doIt ()
            {
                return shared_from_this ();
            }
        };
        struct jimMIXStdSP_ : X_, std::enable_shared_from_this<jimMIXStdSP_> {
            int field = 1;
            shared_ptr<jimMIXStdSP_> doIt ()
            {
                return shared_from_this ();
            }
        };
        struct jimStkSP_ : Memory::enable_shared_from_this<jimStkSP_> {
            int field = 1;
            SharedPtr<jimStkSP_>  doIt ()
            {
                return shared_from_this ();
            }
        };
        struct jimMIStkSP_ : X_, Memory::enable_shared_from_this<jimMIStkSP_> {
            int field = 1;
            SharedPtr<jimMIStkSP_>  doIt ()
            {
                return shared_from_this ();
            }
        };
    }

    void    Test_6_SharedPtr ()
    {
        {
            SharedPtr<int> p (new int (3));
            VerifyTestResult (p.use_count () == 1);
            VerifyTestResult (p.unique ());
            VerifyTestResult (*p == 3);
        }
        {
            static int nCreates = 0;
            static int nDestroys = 0;
            struct COUNTED_OBJ {
                COUNTED_OBJ ()
                {
                    ++nCreates;
                }
                COUNTED_OBJ (const COUNTED_OBJ&)
                {
                    ++nCreates;
                }
                ~COUNTED_OBJ ()
                {
                    ++nDestroys;
                }
                const COUNTED_OBJ& operator= (const COUNTED_OBJ&) = delete;
            };
            struct CNT2 : COUNTED_OBJ {
            };
            {
                SharedPtr<COUNTED_OBJ> p (new COUNTED_OBJ ());
            }
            VerifyTestResult (nCreates == nDestroys);
            {
                SharedPtr<COUNTED_OBJ> p (SharedPtr<CNT2> (new CNT2 ()));
                VerifyTestResult (nCreates == nDestroys + 1);
            }
            VerifyTestResult (nCreates == nDestroys);
        }
        {
            shared_ptr<jimStdSP_> x (new jimStdSP_ ());
            shared_ptr<jimStdSP_> y = x->doIt ();
            VerifyTestResult (x == y);
        }
        {
            shared_ptr<jimMIXStdSP_> x (new jimMIXStdSP_ ());
            shared_ptr<jimMIXStdSP_> y = x->doIt ();
            shared_ptr<X_>           xx = x;
            VerifyTestResult (x == y);
        }
        {
            SharedPtr<jimStkSP_> x (new jimStkSP_ ());
            SharedPtr<jimStkSP_> y = x->doIt ();
            VerifyTestResult (x == y);
        }
        {
            SharedPtr<jimMIStkSP_> x (new jimMIStkSP_ ());
            SharedPtr<jimMIStkSP_> y = x->doIt ();
            SharedPtr<X_>          xx = x;
            VerifyTestResult (x == y);
        }
    }
}




namespace {
    void    Test_7_Bits_ ()
    {
        {
            VerifyTestResult (BitSubstring (0x3, 0, 1) == 1);
            VerifyTestResult (BitSubstring (0x3, 1, 1) == 1);
            VerifyTestResult (BitSubstring (0x3, 2, 1) == 0);
            VerifyTestResult (BitSubstring (0x3, 0, 3) == 0x3);
            VerifyTestResult (BitSubstring (0xff, 0, 8) == 0xff);
            VerifyTestResult (BitSubstring (0xff, 8, 8) == 0x0);
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
    void    Test_8_BLOB_ ()
    {
        {
            vector<Byte> b = { 1, 2, 3, 4, 5 };
            Memory::BLOB bl = b;
            VerifyTestResult (bl.size () == 5 and bl.As<vector<Byte>> () == b);
        }
        {
            Memory::BLOB bl { 1, 2, 3, 4, 5 };
            VerifyTestResult (bl.size () == 5 and bl.As<vector<Byte>> () == (vector<Byte> {1, 2, 3, 4, 5 }));
        }
        {
            const   char    kSrc1_[] = "This is a very good test of a very good test";
            const   char    kSrc2_[] = "";
            const   char    kSrc3_[] = "We eat wiggly worms. That was a very good time to eat the worms. They are awesome!";
            const   char    kSrc4_[] = "0123456789";

            VerifyTestResult  (Memory::BLOB ((const Byte*)kSrc1_, (const Byte*)kSrc1_ + ::strlen(kSrc1_)) == Memory::BLOB::Raw (kSrc1_, kSrc1_ + strlen(kSrc1_)));
            VerifyTestResult  (Memory::BLOB ((const Byte*)kSrc2_, (const Byte*)kSrc2_ + ::strlen(kSrc2_)) == Memory::BLOB::Raw (kSrc2_, kSrc2_ + strlen(kSrc2_)));
            VerifyTestResult  (Memory::BLOB ((const Byte*)kSrc3_, (const Byte*)kSrc3_ + ::strlen(kSrc3_)) == Memory::BLOB::Raw (kSrc3_, kSrc3_ + strlen(kSrc3_)));
            VerifyTestResult  (Memory::BLOB ((const Byte*)kSrc4_, (const Byte*)kSrc4_ + ::strlen(kSrc4_)) == Memory::BLOB::Raw (kSrc4_, kSrc4_ + strlen(kSrc4_)));

            VerifyTestResult  (Memory::BLOB ((const Byte*)kSrc1_, (const Byte*)kSrc1_ + ::strlen(kSrc1_)) == Memory::BLOB::Raw (kSrc1_, kSrc1_ + NEltsOf(kSrc1_) - 1));
            VerifyTestResult  (Memory::BLOB ((const Byte*)kSrc2_, (const Byte*)kSrc2_ + ::strlen(kSrc2_)) == Memory::BLOB::Raw (kSrc2_, kSrc2_ + NEltsOf(kSrc2_) - 1));
            VerifyTestResult  (Memory::BLOB ((const Byte*)kSrc3_, (const Byte*)kSrc3_ + ::strlen(kSrc3_)) == Memory::BLOB::Raw (kSrc3_, kSrc3_ + NEltsOf(kSrc3_) - 1));
            VerifyTestResult  (Memory::BLOB ((const Byte*)kSrc4_, (const Byte*)kSrc4_ + ::strlen(kSrc4_)) == Memory::BLOB::Raw (kSrc4_, kSrc4_ + NEltsOf(kSrc4_) - 1));
        }
        {
            using   Memory::BLOB;
            VerifyTestResult  ((BLOB::Hex ("61 70 70 6c 65 73 20 61 6e 64 20 70 65 61 72 73 0d 0a") == BLOB { 0x61, 0x70, 0x70, 0x6c, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x70, 0x65, 0x61, 0x72, 0x73, 0x0d, 0x0a}));
            VerifyTestResult  ((BLOB::Hex ("4a 94 99 ac 55 f7 a2 8b 1b ca 75 62 f6 9a cf de 41 9d") == BLOB { 0x4a, 0x94, 0x99, 0xac, 0x55, 0xf7, 0xa2, 0x8b, 0x1b, 0xca, 0x75, 0x62, 0xf6, 0x9a, 0xcf, 0xde, 0x41, 0x9d }));
            VerifyTestResult  ((BLOB::Hex ("68 69 20 6d 6f 6d 0d 0a") == BLOB { 0x68, 0x69, 0x20, 0x6d, 0x6f, 0x6d, 0x0d, 0x0a }));
            VerifyTestResult  ((BLOB::Hex ("29 14 4a db 4e ce 20 45 09 56 e8 13 65 2f e8 d6") == BLOB { 0x29, 0x14, 0x4a, 0xdb, 0x4e, 0xce, 0x20, 0x45, 0x09, 0x56, 0xe8, 0x13, 0x65, 0x2f, 0xe8, 0xd6 }));
            VerifyTestResult  ((BLOB::Hex ("29144adb4ece20450956e813652fe8d6") == BLOB { 0x29, 0x14, 0x4a, 0xdb, 0x4e, 0xce, 0x20, 0x45, 0x09, 0x56, 0xe8, 0x13, 0x65, 0x2f, 0xe8, 0xd6 }));
        }
    }
}


namespace {
    namespace Test9PRIVATE_ {
        template <typename T> using InPlace_Traits = Optional_Traits_Inplace_Storage<T>;
        template <typename T> using Indirect_Traits = Optional_Traits_Blockallocated_Indirect_Storage<T>;
        template    <typename OPTIONALOFT>
        void    BasicOTest_ ()
        {
            {
                OPTIONALOFT o1;
                OPTIONALOFT o2 { typename OPTIONALOFT::value_type () };
                OPTIONALOFT o3 = o1;
                o3 = o2;
                VerifyTestResult (o3 == o2);
            }
            {
                OPTIONALOFT o1 { typename OPTIONALOFT::value_type () };
                VerifyTestResult (o1.IsPresent ());
                OPTIONALOFT o2 = move (o1);
                VerifyTestResult (o2 == typename OPTIONALOFT::value_type ());
            }
        }
    }
    void    Test9_OptionalStorageTraits_ ()
    {
        using   namespace Test9PRIVATE_;
        BasicOTest_<Optional<int, InPlace_Traits<int>>> ();
        BasicOTest_<Optional<int, Indirect_Traits<int>>> ();
        BasicOTest_<Optional<wstring, InPlace_Traits<wstring>>> ();
        BasicOTest_<Optional<wstring, Indirect_Traits<wstring>>> ();
    }
}






namespace   {

    void    DoRegressionTests_ ()
    {
        Test1_Optional ();
        Test2_SharedByValue ();
        Test_4_Optional_Of_Mapping_Copy_Problem_ ();
        Test_5_AnyVariantValue_ ();
        Test_6_SharedPtr ();
        Test_7_Bits_ ();
        Test_8_BLOB_ ();
        Test9_OptionalStorageTraits_ ();
    }
}



int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}



