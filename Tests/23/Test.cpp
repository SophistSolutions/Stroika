/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
//  TEST    Foundation::DataExchangeFormat::ObjectVariantMapper
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Configuration/Locale.h"
#include    "Stroika/Foundation/DataExchangeFormat/BadFormatException.h"
#include    "Stroika/Foundation/DataExchangeFormat/ObjectVariantMapper.h"
#include    "Stroika/Foundation/DataExchangeFormat/JSON/Reader.h"
#include    "Stroika/Foundation/DataExchangeFormat/JSON/Writer.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/IO/FileSystem/BinaryFileInputStream.h"
#include    "Stroika/Foundation/IO/FileSystem/BinaryFileOutputStream.h"
#include    "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include    "Stroika/Foundation/Memory/VariantValue.h"
#include    "Stroika/Foundation/Streams/BasicBinaryInputOutputStream.h"
#include    "Stroika/Foundation/Math/Common.h"
#include    "Stroika/Foundation/Time/DateTime.h"
#include    "Stroika/Foundation/Time/Duration.h"
#include    "Stroika/Foundation/Traversal/Range.h"

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchangeFormat;

using   Memory::Byte;
using   Memory::VariantValue;
using   Time::Date;
using   Time::DateTime;
using   Time::Duration;
using   Time::TimeOfDay;





namespace   {
    namespace DoRegressionTests_BasicDataRoundtrips_1_ {
        template    <typename T>
        void    RoundTripTest_ (T v)
        {
            Memory::VariantValue mv =   v;
            VerifyTestResult (mv.As<T> () == v);
        }
        template    <typename T>
        void    RoundTripMinMax_ ()
        {
            RoundTripTest_ (numeric_limits<T>::lowest ());
            RoundTripTest_ (numeric_limits<T>::max ());
        }
        void    DoAll ()
        {
            RoundTripMinMax_<int> ();
            RoundTripMinMax_<unsigned int> ();
            RoundTripMinMax_<long> ();
            RoundTripMinMax_<unsigned long> ();
            RoundTripMinMax_<long long> ();
            RoundTripMinMax_<unsigned long long> ();
            RoundTripMinMax_<int8_t> ();
            RoundTripMinMax_<uint8_t> ();
            RoundTripMinMax_<int16_t> ();
            RoundTripMinMax_<uint16_t> ();
            RoundTripMinMax_<int32_t> ();
            RoundTripMinMax_<uint32_t> ();
            RoundTripMinMax_<int64_t> ();
            RoundTripMinMax_<uint64_t> ();
            RoundTripMinMax_<float> ();
            RoundTripMinMax_<float> ();
            RoundTripMinMax_<double> ();
            RoundTripMinMax_<double> ();
            //enum class Fred { Barny };
            //RoundTripMinMax_<Fred> ();
            RoundTripTest_<int> (3);
        }
    }
}

namespace   {
    void    DoRegressionTests_SimpleMapToFromJSON_2_ ()
    {
        const bool kWrite2FileAsWell_ = true;      // just for debugging

        struct SharedContactsConfig_ {
            bool                        fEnabled;
            DateTime                    fLastSynchronizedAt;
            Mapping<String, String>     fThisPHRsIDToSharedContactID;

            SharedContactsConfig_ ()
                : fEnabled (false)
                , fLastSynchronizedAt ()
                , fThisPHRsIDToSharedContactID () {
            }

            bool operator== (const SharedContactsConfig_& rhs) const {
                return fEnabled == rhs.fEnabled and
                       fLastSynchronizedAt == rhs.fLastSynchronizedAt and
                       fThisPHRsIDToSharedContactID == rhs.fThisPHRsIDToSharedContactID
                       ;
            }
        };

        ObjectVariantMapper mapper;

        // register each of your mappable (even private) types
#if     qCompilerAndStdLib_Supports_initializer_lists
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> ( {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fEnabled, L"Enabled"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fLastSynchronizedAt, L"Last-Synchronized-At"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fThisPHRsIDToSharedContactID, L"This-HR-ContactID-To-SharedContactID-Map"),
        }));
#else
        ObjectVariantMapper::StructureFieldInfo kInfo[] = {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fEnabled, L"Enabled"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fLastSynchronizedAt, L"Last-Synchronized-At"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fThisPHRsIDToSharedContactID, L"This-HR-ContactID-To-SharedContactID-Map"),
        };
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> (std::begin (kInfo), std::end (kInfo)));
#endif

        bool newEnabled = true;
        SharedContactsConfig_   tmp;
        tmp.fEnabled = newEnabled;
        tmp.fThisPHRsIDToSharedContactID.Add (L"A", L"B");
        tmp.fLastSynchronizedAt = DateTime (Time::Date (Time::Year (1998), Time::MonthOfYear::eApril, Time::DayOfMonth::e11), Time::TimeOfDay::Parse (L"3pm", locale::classic ()));

        VariantValue v = mapper.FromObject  (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::BasicBinaryInputOutputStream   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            IO::FileSystem::BinaryFileOutputStream tmp (IO::FileSystem::WellKnownLocations::GetTemporary () + L"t.txt");
            JSON::Writer ().Write (v, tmp);
        }

        if (kWrite2FileAsWell_) {
            IO::FileSystem::BinaryFileInputStream tmp (IO::FileSystem::WellKnownLocations::GetTemporary () + L"t.txt");
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmp));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}

namespace   {
    void    DoRegressionTests_SimpleMapToFromJSON_3_ ()
    {
        const bool kWrite2FileAsWell_ = true;      // just for debugging

        struct SharedContactsConfig_ {
            int fInt1;
            unsigned long long fInt2;
            long long fInt3;
            int32_t fInt4;

            SharedContactsConfig_ ()
                : fInt1 (0)
                , fInt2 (0)
                , fInt3 (0)
                , fInt4 (0) {
            }

            bool operator== (const SharedContactsConfig_& rhs) const {
                return
                    fInt1 == rhs.fInt1 and
                    fInt2 == rhs.fInt2 and
                    fInt3 == rhs.fInt3 and
                    fInt4 == rhs.fInt4
                    ;
            }
        };

        ObjectVariantMapper mapper;

        // register each of your mappable (even private) types
#if     qCompilerAndStdLib_Supports_initializer_lists
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> ( {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fInt1, L"Int1"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fInt2, L"Int2"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fInt3, L"Int3"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fInt4, L"Int4"),
        }));
#else
        ObjectVariantMapper::StructureFieldInfo kInfo[] = {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fInt1, L"Int1"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fInt2, L"Int2"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fInt3, L"Int3"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fInt4, L"Int4"),
        };
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> (std::begin (kInfo), std::end (kInfo)));
#endif

        SharedContactsConfig_   tmp;
        tmp.fInt1 = 2;
        tmp.fInt2 = numeric_limits<decltype (tmp.fInt2)>::max ();
        tmp.fInt3 = numeric_limits<decltype (tmp.fInt3)>::max ();
        tmp.fInt4 = numeric_limits<decltype (tmp.fInt4)>::min ();

        VariantValue v = mapper.FromObject  (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::BasicBinaryInputOutputStream   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"t.txt";
            JSON::Writer ().Write (v, IO::FileSystem::BinaryFileOutputStream (fileName));
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (IO::FileSystem::BinaryFileInputStream (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}




namespace   {
    void    DoRegressionTests_SimpleMapRangeTypes_4_ ()
    {
        using   namespace Traversal;
        const bool kWrite2FileAsWell_ = true;      // just for debugging

        struct SharedContactsConfig_ {
            Range<int>  fIntRange;

            SharedContactsConfig_ ()
                : fIntRange (-3, 99) {
            }

            bool operator== (const SharedContactsConfig_& rhs) const {
                return
                    fIntRange == rhs.fIntRange
                    ;
            }
        };

        ObjectVariantMapper mapper;

        mapper.Add (ObjectVariantMapper::MakeCommonSerializer_Range<Range<int>> ());
#if     qCompilerAndStdLib_Supports_initializer_lists
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> ( {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fIntRange, L"fIntRange"),
        }));
#else
        ObjectVariantMapper::StructureFieldInfo kInfo[] = {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fIntRange, L"fIntRange"),
        };
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> (std::begin (kInfo), std::end (kInfo)));
#endif

        SharedContactsConfig_   tmp;
        tmp.fIntRange = Range<int> (1, 10);
        VariantValue v = mapper.FromObject  (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::BasicBinaryInputOutputStream   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"y.txt";
            JSON::Writer ().Write (v, IO::FileSystem::BinaryFileOutputStream (fileName));
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (IO::FileSystem::BinaryFileInputStream (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}







namespace   {
    void    DoRegressionTests_SimpleEnumTypes_5_ ()
    {
        using   namespace Traversal;
        const bool kWrite2FileAsWell_ = true;      // just for debugging

        enum class Fred {
            a = -3,
            b,
            c,
            d,
            e,
            f,
            g,
            h,

            Stroika_Define_Enum_Bounds (a, h)
        };
        struct SharedContactsConfig_ {
            Fred fEnum1;

            SharedContactsConfig_ ()
                : fEnum1 (Fred::a) {
            }

            bool operator== (const SharedContactsConfig_& rhs) const {
                return
                    fEnum1 == rhs.fEnum1
                    ;
            }
        };

        ObjectVariantMapper mapper;

        mapper.Add (ObjectVariantMapper::MakeCommonSerializer_Enumeration<Fred> ());
#if     qCompilerAndStdLib_Supports_initializer_lists
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> ( {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fEnum1, L"fEnum1"),
        }));
#else
        ObjectVariantMapper::StructureFieldInfo kInfo[] = {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fEnum1, L"fEnum1"),
        };
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> (std::begin (kInfo), std::end (kInfo)));
#endif

        SharedContactsConfig_   tmp;
        tmp.fEnum1 = Fred::b;
        VariantValue v = mapper.FromObject  (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::BasicBinaryInputOutputStream   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"5.txt";
            JSON::Writer ().Write (v, IO::FileSystem::BinaryFileOutputStream (fileName));
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (IO::FileSystem::BinaryFileInputStream (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}







namespace   {
    void    DoRegressionTests_DurationsDateTime_6_ ()
    {
        using   namespace Traversal;
        const bool kWrite2FileAsWell_ = true;      // just for debugging

        struct SharedContactsConfig_ {
            Duration fDuration1;
            DateTime fDateTime1;
            DateTime fDate1;
            TimeOfDay fTimeOfDay1;

            SharedContactsConfig_ ()
                : fDuration1 (chrono::milliseconds (200))
                , fDateTime1 ()
                , fDate1 ()
                , fTimeOfDay1 () {
            }

            bool operator== (const SharedContactsConfig_& rhs) const {
                return
                    fDuration1 == rhs.fDuration1
                    and fDateTime1 == rhs.fDateTime1
                    and fDate1 == rhs.fDate1
                    and fTimeOfDay1 == rhs.fTimeOfDay1
                    ;
            }
        };

        ObjectVariantMapper mapper;
#if     qCompilerAndStdLib_Supports_initializer_lists
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> ( {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fDuration1, L"fDuration1"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fDateTime1, L"fDateTime1"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fDate1, L"fDate1"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fTimeOfDay1, L"fTimeOfDay1"),
        }));
#else
        ObjectVariantMapper::StructureFieldInfo kInfo[] = {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fDuration1, L"fDuration1"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fDateTime1, L"fDateTime1"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fDate1, L"fDate1"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fTimeOfDay1, L"fTimeOfDay1"),
        };
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> (std::begin (kInfo), std::end (kInfo)));
#endif

        SharedContactsConfig_   tmp;
        tmp.fDate1 = Date (Time::Year (2001), Time::MonthOfYear::eFebruary, Time::DayOfMonth::e12);
        tmp.fDateTime1 = DateTime (Date (Time::Year (2001), Time::MonthOfYear::eFebruary, Time::DayOfMonth::e12), Time::TimeOfDay::Parse (L"3pm", locale::classic ()));
        tmp.fTimeOfDay1 = tmp.fDateTime1.GetTimeOfDay ();
        tmp.fTimeOfDay1 = TimeOfDay (tmp.fTimeOfDay1.GetAsSecondsCount () + 60);
        VariantValue v = mapper.FromObject  (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::BasicBinaryInputOutputStream   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"6.txt";
            JSON::Writer ().Write (v, IO::FileSystem::BinaryFileOutputStream (fileName));
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (IO::FileSystem::BinaryFileInputStream (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}






namespace   {
    void    DoRegressionTests_VariantValue_7_ ()
    {
        using   namespace Traversal;
        const bool kWrite2FileAsWell_ = true;      // just for debugging

        struct SharedContactsConfig_ {
            VariantValue fVV1;

            SharedContactsConfig_ ()
                : fVV1 () {
            }

            bool operator== (const SharedContactsConfig_& rhs) const {
                return
                    fVV1 == rhs.fVV1
                    ;
            }
        };

        ObjectVariantMapper mapper;
#if     qCompilerAndStdLib_Supports_initializer_lists
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> ( {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fVV1, L"fVV1"),
        }));
#else
        ObjectVariantMapper::StructureFieldInfo kInfo[] = {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fVV1, L"fVV1"),
        };
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<ObjectVariantMapper::StructureFieldInfo> (std::begin (kInfo), std::end (kInfo)));
#endif

        SharedContactsConfig_   tmp;
        tmp.fVV1 = Date (Time::Year (2001), Time::MonthOfYear::eFebruary, Time::DayOfMonth::e12);
        VariantValue v = mapper.FromObject  (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::BasicBinaryInputOutputStream   tmpStream;
        JSON::Writer ().Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            String fileName = IO::FileSystem::WellKnownLocations::GetTemporary () + L"7.txt";
            JSON::Writer ().Write (v, IO::FileSystem::BinaryFileOutputStream (fileName));
            SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (IO::FileSystem::BinaryFileInputStream (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.ToObject<SharedContactsConfig_> (JSON::Reader ().Read (tmpStream));
//  @todo - FIX COMPARE OF VARIANTS !!!!
//        VerifyTestResult (tmp2 == tmp);
    }
}







namespace   {
    void    DoRegressionTests_ ()
    {
        DoRegressionTests_BasicDataRoundtrips_1_::DoAll ();
        DoRegressionTests_SimpleMapToFromJSON_2_ ();
        DoRegressionTests_SimpleMapToFromJSON_3_ ();
        DoRegressionTests_SimpleMapRangeTypes_4_ ();
        DoRegressionTests_SimpleEnumTypes_5_ ();
        DoRegressionTests_DurationsDateTime_6_ ();
        DoRegressionTests_VariantValue_7_ ();
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

