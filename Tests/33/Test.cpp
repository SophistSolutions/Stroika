﻿/*
* Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
*/
//  TEST    Foundation::DataExchangeFormat::ObjectVariantMapper
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Configuration/Locale.h"
#include "Stroika/Foundation/Containers/Bijection.h"
#include "Stroika/Foundation/Containers/SortedMultiSet.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include "Stroika/Foundation/IO/Network/CIDR.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"
#include "Stroika/Foundation/Traversal/Range.h"

#include "../TestHarness/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::DataExchange;

using Time::Date;
using Time::DateTime;
using Time::Duration;
using Time::TimeOfDay;

namespace {
    namespace DoRegressionTests_BasicDataRoundtrips_1_ {
        template <typename T>
        void RoundTripTest_ (T v)
        {
            VariantValue mv = v;
            VerifyTestResult (mv.As<T> () == v);
        }
        template <typename T>
        void RoundTripMinMax_ ()
        {
            RoundTripTest_ (numeric_limits<T>::lowest ());
            RoundTripTest_ (numeric_limits<T>::max ());
        }
        void DoAll ()
        {
            Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_BasicDataRoundtrips_1_"};
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

namespace {
    void DoRegressionTests_SimpleMapToFromJSON_2_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_SimpleMapToFromJSON_2_"};
        const bool                kWrite2FileAsWell_ = true; // just for debugging

        struct SharedContactsConfig_ {
            bool                      fEnabled{false};
            optional<DateTime>        fLastSynchronizedAt;
            Mapping<String, String>   fThisPHRsIDToSharedContactID;
            Bijection<String, String> fThisPHRsIDToSharedContactID2;

            SharedContactsConfig_ () = default;

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                return fEnabled == rhs.fEnabled and fLastSynchronizedAt == rhs.fLastSynchronizedAt and
                       fThisPHRsIDToSharedContactID == rhs.fThisPHRsIDToSharedContactID and
                       fThisPHRsIDToSharedContactID2 == rhs.fThisPHRsIDToSharedContactID2;
            }
        };

        ObjectVariantMapper mapper;

        mapper.AddCommonType<Bijection<String, String>> ();

        // register each of your mappable (even private) types
        mapper.AddClass<SharedContactsConfig_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"Enabled", StructFieldMetaInfo{&SharedContactsConfig_::fEnabled}},
            {L"Last-Synchronized-At", StructFieldMetaInfo{&SharedContactsConfig_::fLastSynchronizedAt}},
            {L"This-HR-ContactID-To-SharedContactID-Map", StructFieldMetaInfo{&SharedContactsConfig_::fThisPHRsIDToSharedContactID}},
            {L"This-HR-ContactID-To-SharedContactID-Bijection", StructFieldMetaInfo{&SharedContactsConfig_::fThisPHRsIDToSharedContactID2}},
        });

        bool                  newEnabled = true;
        SharedContactsConfig_ tmp;
        tmp.fEnabled = newEnabled;
        tmp.fThisPHRsIDToSharedContactID.Add (L"A", L"B");
        tmp.fThisPHRsIDToSharedContactID2.Add (L"A", L"B");
        tmp.fLastSynchronizedAt =
            DateTime{Time::Date{Time::Year{1998}, Time::April, Time::day{11}}, Time::TimeOfDay::Parse (L"3pm", locale::classic ())};

        VariantValue v = mapper.FromObject (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
        Variant::JSON::Writer{}.Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            IO::FileSystem::FileOutputStream::Ptr tmpFileStream =
                IO::FileSystem::FileOutputStream::New (IO::FileSystem::WellKnownLocations::GetTemporary () / "t.txt");
            Variant::JSON::Writer{}.Write (v, tmpFileStream);
        }

        if (kWrite2FileAsWell_) {
            IO::FileSystem::FileInputStream::Ptr tmpFileStream =
                IO::FileSystem::FileInputStream::New (IO::FileSystem::WellKnownLocations::GetTemporary () / "t.txt");
            SharedContactsConfig_ tmp2 = mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (tmpFileStream));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_ tmp2 = mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}

namespace {
    void DoRegressionTests_SimpleMapToFromJSON_3_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_SimpleMapToFromJSON_3_"};
        const bool                kWrite2FileAsWell_ = true; // just for debugging

        struct SharedContactsConfig_ {
            int                fInt1;
            unsigned long long fInt2;
            long long          fInt3;
            int32_t            fInt4;

            SharedContactsConfig_ ()
                : fInt1 (0)
                , fInt2 (0)
                , fInt3 (0)
                , fInt4 (0)
            {
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                return fInt1 == rhs.fInt1 and fInt2 == rhs.fInt2 and fInt3 == rhs.fInt3 and fInt4 == rhs.fInt4;
            }
        };

        ObjectVariantMapper mapper;

        // register each of your mappable (even private) types
        mapper.AddClass<SharedContactsConfig_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"Int1", StructFieldMetaInfo{&SharedContactsConfig_::fInt1}},
            {L"Int2", StructFieldMetaInfo{&SharedContactsConfig_::fInt2}},
            {L"Int3", StructFieldMetaInfo{&SharedContactsConfig_::fInt3}},
            {L"Int4", StructFieldMetaInfo{&SharedContactsConfig_::fInt4}},
        });

        SharedContactsConfig_ tmp;
        tmp.fInt1 = 2;
        tmp.fInt2 = numeric_limits<decltype (tmp.fInt2)>::max ();
        tmp.fInt3 = numeric_limits<decltype (tmp.fInt3)>::max ();
        tmp.fInt4 = numeric_limits<decltype (tmp.fInt4)>::min ();

        VariantValue v = mapper.FromObject (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
        Variant::JSON::Writer{}.Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            filesystem::path fileName = IO::FileSystem::WellKnownLocations::GetTemporary () / "t.txt"sv;
            Variant::JSON::Writer{}.Write (v, IO::FileSystem::FileOutputStream::New (fileName));
            [[maybe_unused]] SharedContactsConfig_ tmp2 =
                mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (IO::FileSystem::FileInputStream::New (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_ tmp2 = mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}

namespace {
    void DoRegressionTests_SimpleMapRangeTypes_4_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_SimpleMapRangeTypes_4_"};
        using namespace Traversal;
        const bool kWrite2FileAsWell_ = true; // just for debugging

        struct SharedContactsConfig_ {
            Range<int>         fIntRange;
            DiscreteRange<int> fDiscIntRange2;

            SharedContactsConfig_ ()
                : fIntRange (-3, 99)
                , fDiscIntRange2 (4, 19)
            {
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                return fIntRange == rhs.fIntRange and fDiscIntRange2 == rhs.fDiscIntRange2;
            }
        };

        ObjectVariantMapper mapper;

        mapper.AddCommonType<Range<int>> ();
        mapper.AddCommonType<DiscreteRange<int>> ();

        mapper.AddClass<SharedContactsConfig_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"fIntRange", StructFieldMetaInfo{&SharedContactsConfig_::fIntRange}},
            {L"fDiscIntRange2", StructFieldMetaInfo{&SharedContactsConfig_::fDiscIntRange2}},
        });

        SharedContactsConfig_ tmp;
        tmp.fIntRange      = Range<int> (1, 10);
        tmp.fDiscIntRange2 = DiscreteRange<int> (38, 39);
        VariantValue v     = mapper.FromObject (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
        Variant::JSON::Writer{}.Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            filesystem::path fileName = IO::FileSystem::WellKnownLocations::GetTemporary () / "4.txt"sv;
            Variant::JSON::Writer{}.Write (v, IO::FileSystem::FileOutputStream::New (fileName));
            [[maybe_unused]] SharedContactsConfig_ tmp2 =
                mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (IO::FileSystem::FileInputStream::New (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_ tmp2 = mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}

namespace {
    void DoRegressionTests_SimpleEnumTypes_5_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_SimpleEnumTypes_5_"};
        using namespace Traversal;
        const bool kWrite2FileAsWell_ = true; // just for debugging

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
        const Configuration::EnumNames<Fred> Fred_NAMES = {
            {Fred::a, L"a"}, {Fred::b, L"b"}, {Fred::c, L"c"}, {Fred::d, L"d"},
            {Fred::e, L"e"}, {Fred::f, L"f"}, {Fred::g, L"g"}, {Fred::h, L"h"},
        };
        struct SharedContactsConfig_ {
            Fred fEnum1;

            SharedContactsConfig_ ()
                : fEnum1 (Fred::a)
            {
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                return fEnum1 == rhs.fEnum1;
            }
        };

        {
            ObjectVariantMapper mapper;

            mapper.Add (ObjectVariantMapper::MakeCommonSerializer_NamedEnumerations<Fred> (Fred_NAMES));
            mapper.AddClass<SharedContactsConfig_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                {L"fEnum1", StructFieldMetaInfo{&SharedContactsConfig_::fEnum1}},
            });

            SharedContactsConfig_ tmp;
            tmp.fEnum1     = Fred::b;
            VariantValue v = mapper.FromObject (tmp);

            // at this point - we should have VariantValue object with "Enabled" field.
            // This can then be serialized using

            Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
            Variant::JSON::Writer{}.Write (v, tmpStream);

            if (kWrite2FileAsWell_) {
                filesystem::path fileName = IO::FileSystem::WellKnownLocations::GetTemporary () / "5.txt"sv;
                Variant::JSON::Writer{}.Write (v, IO::FileSystem::FileOutputStream::New (fileName));
                [[maybe_unused]] SharedContactsConfig_ tmp2 =
                    mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (IO::FileSystem::FileInputStream::New (fileName)));
            }

            // THEN deserialized, and mapped back to C++ object form
            SharedContactsConfig_ tmp2 = mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (tmpStream));
            VerifyTestResult (tmp2 == tmp);
        }

        {
            ObjectVariantMapper mapper;

            mapper.Add (mapper.MakeCommonSerializer_NamedEnumerations<Fred> (Bijection<Fred, String> (Fred_NAMES)));
            mapper.AddClass<SharedContactsConfig_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                {L"fEnum1", StructFieldMetaInfo{&SharedContactsConfig_::fEnum1}},
            });

            SharedContactsConfig_ tmp;
            tmp.fEnum1     = Fred::b;
            VariantValue v = mapper.FromObject (tmp);

            // at this point - we should have VariantValue object with "Enabled" field.
            // This can then be serialized using

            Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
            Variant::JSON::Writer{}.Write (v, tmpStream);

            if (kWrite2FileAsWell_) {
                filesystem::path fileName = IO::FileSystem::WellKnownLocations::GetTemporary () / "5.txt"sv;
                Variant::JSON::Writer{}.Write (v, IO::FileSystem::FileOutputStream::New (fileName));
                [[maybe_unused]] SharedContactsConfig_ tmp2 =
                    mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (IO::FileSystem::FileInputStream::New (fileName)));
            }

            // THEN deserialized, and mapped back to C++ object form
            SharedContactsConfig_ tmp2 = mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (tmpStream));
            VerifyTestResult (tmp2 == tmp);
        }
    }
}

namespace {
    void DoRegressionTests_DurationsDateTime_6_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_DurationsDateTime_6_"};
        using namespace Traversal;
        const bool kWrite2FileAsWell_ = true; // just for debugging

        struct SharedContactsConfig_ {
            Duration            fDuration1;
            optional<DateTime>  fDateTime1;
            optional<DateTime>  fDate1;
            optional<TimeOfDay> fTimeOfDay1;

            SharedContactsConfig_ ()
                : fDuration1 (chrono::milliseconds (200))
            {
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                return fDuration1 == rhs.fDuration1 and fDateTime1 == rhs.fDateTime1 and fDate1 == rhs.fDate1 and fTimeOfDay1 == rhs.fTimeOfDay1;
            }
        };

        ObjectVariantMapper mapper;
        mapper.AddClass<SharedContactsConfig_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"fDuration1", StructFieldMetaInfo{&SharedContactsConfig_::fDuration1}},
            {L"fDateTime1", StructFieldMetaInfo{&SharedContactsConfig_::fDateTime1}},
            {L"fDate1", StructFieldMetaInfo{&SharedContactsConfig_::fDate1}},
            {L"fTimeOfDay1", StructFieldMetaInfo{&SharedContactsConfig_::fTimeOfDay1}},
        });

        SharedContactsConfig_ tmp;
        tmp.fDate1 = Date{Time::Year{2001}, Time::February, Time::day{12}};
        tmp.fDateTime1 = DateTime{Date{Time::Year{2001}, Time::February, Time::day{12}}, Time::TimeOfDay::Parse (L"3pm", locale::classic ())};
        tmp.fTimeOfDay1 = tmp.fDateTime1->GetTimeOfDay ();
        Assert (tmp.fTimeOfDay1.has_value ());
        tmp.fTimeOfDay1 = TimeOfDay{tmp.fTimeOfDay1->GetAsSecondsCount () + 60};
        VariantValue v  = mapper.FromObject (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
        Variant::JSON::Writer{}.Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            filesystem::path fileName = IO::FileSystem::WellKnownLocations::GetTemporary () / "6.txt"sv;
            Variant::JSON::Writer{}.Write (v, IO::FileSystem::FileOutputStream::New (fileName));
            [[maybe_unused]] SharedContactsConfig_ tmp2 =
                mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (IO::FileSystem::FileInputStream::New (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_ tmp2 = mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}

namespace {
    void DoRegressionTests_VariantValue_7_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_VariantValue_7_"};
        using namespace Traversal;
        const bool kWrite2FileAsWell_ = true; // just for debugging

        struct SharedContactsConfig_ {
            VariantValue fVV1;
            SharedContactsConfig_ ()                                 = default;
            bool operator== (const SharedContactsConfig_& rhs) const = default;
        };

        ObjectVariantMapper mapper;
        mapper.AddClass<SharedContactsConfig_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"fVV1", StructFieldMetaInfo{&SharedContactsConfig_::fVV1}},
        });

        SharedContactsConfig_ tmp;
        tmp.fVV1       = Date{Time::Year{2001}, Time::February, Time::day{12}};
        VariantValue v = mapper.FromObject (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
        Variant::JSON::Writer{}.Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            filesystem::path fileName = IO::FileSystem::WellKnownLocations::GetTemporary () / "7.txt"sv;
            Variant::JSON::Writer{}.Write (v, IO::FileSystem::FileOutputStream::New (fileName));
            [[maybe_unused]] SharedContactsConfig_ tmp2 =
                mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (IO::FileSystem::FileInputStream::New (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_ tmp2 = mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}

namespace {
    void DoRegressionTests_MakeCommonSerializer_8_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_MakeCommonSerializer_8_"};
        using namespace Traversal;
        const bool kWrite2FileAsWell_ = true; // just for debugging

        struct SharedContactsConfig_ {
            int                fInt1;
            optional<int>      fInt2;
            Mapping<int, int>  fMapping1;
            Sequence<int>      fSequence1;
            int                fBasicArray1[5];
            Set<int>           fSet1_;
            vector<int>        fVector1_;
            pair<int, String>  fPair1_;
            tuple<int, String> fTuple2_;

            SharedContactsConfig_ ()
                : fInt1 (3)
            {
                memset (&fBasicArray1, 0, sizeof (fBasicArray1));
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                if (memcmp (fBasicArray1, rhs.fBasicArray1, sizeof (fBasicArray1)) != 0) {
                    return false;
                }
                return fInt1 == rhs.fInt1 and fInt2 == rhs.fInt2 and fMapping1 == rhs.fMapping1 and fSequence1 == rhs.fSequence1 and
                       fSet1_ == rhs.fSet1_ and fVector1_ == rhs.fVector1_ and fPair1_ == rhs.fPair1_ and fTuple2_ == rhs.fTuple2_;
            }
        };

        ObjectVariantMapper mapper;

        mapper.AddCommonType<optional<int>> ();
        mapper.Add (ObjectVariantMapper::MakeCommonSerializer<Mapping<int, int>> ());
        mapper.Add (ObjectVariantMapper::MakeCommonSerializer<Sequence<int>> ());
        mapper.Add (ObjectVariantMapper::MakeCommonSerializer<vector<int>> ());
        mapper.Add (ObjectVariantMapper::MakeCommonSerializer<Set<int>> ());
        mapper.Add (ObjectVariantMapper::MakeCommonSerializer<int[5]> ());
        mapper.Add (ObjectVariantMapper::MakeCommonSerializer<pair<int, String>> ());
        mapper.Add (ObjectVariantMapper::MakeCommonSerializer<tuple<int, String>> ());

        mapper.AddClass<SharedContactsConfig_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"fInt1", StructFieldMetaInfo{&SharedContactsConfig_::fInt1}},
            {L"fInt2", StructFieldMetaInfo{&SharedContactsConfig_::fInt2}},
            {L"fMapping1", StructFieldMetaInfo{&SharedContactsConfig_::fMapping1}},
            {L"fSequence1", StructFieldMetaInfo{&SharedContactsConfig_::fSequence1}},
            {L"fBasicArray1", StructFieldMetaInfo{&SharedContactsConfig_::fBasicArray1}},
            {L"fSet1_", StructFieldMetaInfo{&SharedContactsConfig_::fSet1_}},
            {L"fVector1_", StructFieldMetaInfo{&SharedContactsConfig_::fVector1_}},
            {L"fPair1_", StructFieldMetaInfo{&SharedContactsConfig_::fPair1_}},
            {L"fTuple2", StructFieldMetaInfo{&SharedContactsConfig_::fTuple2_}},
        });

        SharedContactsConfig_ tmp;
        tmp.fInt1 = 4;
        tmp.fInt2 = 6;
        tmp.fSequence1.Append (19);
        tmp.fMapping1.Add (3, 5);
        tmp.fBasicArray1[3] = 5;
        tmp.fSet1_.Add (193);
        tmp.fVector1_.push_back (3);
        tmp.fVector1_.push_back (-91);
        tmp.fPair1_    = pair<int, String>{3, L"test"};
        tmp.fTuple2_   = tuple<int, String>{4, L"test4"};
        VariantValue v = mapper.FromObject (tmp);

        Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
        Variant::JSON::Writer{}.Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            filesystem::path fileName = IO::FileSystem::WellKnownLocations::GetTemporary () / "8.txt"sv;
            Variant::JSON::Writer{}.Write (v, IO::FileSystem::FileOutputStream::New (fileName));
            [[maybe_unused]] SharedContactsConfig_ tmp2 =
                mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (IO::FileSystem::FileInputStream::New (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_ tmp2 = mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}

namespace {
    void DoRegressionTests_Subclass_9_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_Subclass_9_"};
        using namespace Traversal;

        struct BaseObj_ {
            int  fVV1{};
            bool operator== (const BaseObj_& rhs) const
            {
                return fVV1 == rhs.fVV1;
            }
        };

        struct Derived_ : BaseObj_ {
            int  fVV2{};
            bool operator== (const Derived_& rhs) const
            {
                return BaseObj_::operator== (rhs) and fVV2 == rhs.fVV2;
            }
        };

        ObjectVariantMapper mapper;
        mapper.AddClass<BaseObj_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"fVV1", StructFieldMetaInfo{&BaseObj_::fVV1}},
        });
        mapper.AddSubClass<Derived_, BaseObj_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"fVV2", StructFieldMetaInfo{&Derived_::fVV2}},
        });

        Derived_ tmp;
        tmp.fVV1       = 55;
        tmp.fVV2       = 345;
        VariantValue v = mapper.FromObject (tmp);

        Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
        Variant::JSON::Writer{}.Write (v, tmpStream);

        // THEN deserialized, and mapped back to C++ object form
        Derived_ tmp2 = mapper.ToObject<Derived_> (Variant::JSON::Reader{}.Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}

namespace {
    void DoRegressionTests_FileTypeConverterOverride_10_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_FileTypeConverterOverride_10_"};
        using namespace Traversal;
        const bool kWrite2FileAsWell_ = true; // just for debugging

        struct SharedContactsConfig_ {
            int               fInt1;
            optional<int>     fInt2;
            Mapping<int, int> fMapping1;
            Sequence<int>     fSequence1;
            int               fBasicArray1[5];
            Set<int>          fSet1_;
            vector<int>       fVector1_;
            IO::Network::URI  fURL1_;
            IO::Network::URI  fURL2_;

            SharedContactsConfig_ ()
                : fInt1 (3)
            {
                memset (&fBasicArray1, 0, sizeof (fBasicArray1));
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                if (memcmp (fBasicArray1, rhs.fBasicArray1, sizeof (fBasicArray1)) != 0) {
                    return false;
                }
                return fInt1 == rhs.fInt1 and fInt2 == rhs.fInt2 and fMapping1 == rhs.fMapping1 and fSequence1 == rhs.fSequence1 and
                       fSet1_ == rhs.fSet1_ and fVector1_ == rhs.fVector1_ and fURL1_ == rhs.fURL1_ and fURL2_ == rhs.fURL2_;
            }
        };

        ObjectVariantMapper mapper;

        mapper.AddCommonType<optional<int>> ();
        mapper.AddCommonType<Mapping<int, int>> ();
        mapper.AddCommonType<Sequence<int>> ();
        mapper.AddCommonType<vector<int>> ();

        mapper.AddClass<SharedContactsConfig_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {L"fInt1", StructFieldMetaInfo{&SharedContactsConfig_::fInt1}},
            {L"fInt2", StructFieldMetaInfo{&SharedContactsConfig_::fInt2}, ObjectVariantMapper::StructFieldInfo::eOmitNullFields},
            {L"fMapping1", StructFieldMetaInfo{&SharedContactsConfig_::fMapping1}},
            {L"fSequence1", StructFieldMetaInfo{&SharedContactsConfig_::fSequence1}},
            {L"fBasicArray1", StructFieldMetaInfo{&SharedContactsConfig_::fBasicArray1}, ObjectVariantMapper::MakeCommonSerializer<int[5]> ()},
            {L"fSet1_", StructFieldMetaInfo{&SharedContactsConfig_::fSet1_}, ObjectVariantMapper::MakeCommonSerializer<Set<int>> ()},
            {L"fVector1_", StructFieldMetaInfo{&SharedContactsConfig_::fVector1_}},
            {L"fURL1_", StructFieldMetaInfo{&SharedContactsConfig_::fURL1_}, ObjectVariantMapper::MakeCommonSerializer<IO::Network::URI> ()},
            {L"fURL2_", StructFieldMetaInfo{&SharedContactsConfig_::fURL2_}, ObjectVariantMapper::MakeCommonSerializer<IO::Network::URI> ()},
        });

        SharedContactsConfig_ tmp;
        tmp.fInt1 = 4;
        tmp.fInt2 = 6;
        tmp.fSequence1.Append (19);
        tmp.fMapping1.Add (3, 5);
        tmp.fBasicArray1[3] = 5;
        tmp.fSet1_.Add (193);
        tmp.fVector1_.push_back (3);
        tmp.fVector1_.push_back (-91);
        tmp.fURL1_ = IO::Network::URI{L"http://localhost:3344/fred"};
        tmp.fURL2_ = IO::Network::URI{L"http://localhost:1234"};

        VariantValue v = mapper.FromObject (tmp);

        Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
        Variant::JSON::Writer{}.Write (v, tmpStream);

        if (kWrite2FileAsWell_) {
            filesystem::path fileName = IO::FileSystem::WellKnownLocations::GetTemporary () / "10.txt"sv;
            Variant::JSON::Writer{}.Write (v, IO::FileSystem::FileOutputStream::New (fileName));
            [[maybe_unused]] SharedContactsConfig_ tmp2 =
                mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (IO::FileSystem::FileInputStream::New (fileName)));
        }

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_ tmp2 = mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}

namespace {

    void DoRegressionTests_CustomMapper_11_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_CustomMapper_11_"};
        {
            struct RGBColor {
                uint8_t red;
                uint8_t green;
                uint8_t blue;
                bool    operator== (const RGBColor& rhs) const
                {
                    return red == rhs.red and green == rhs.green and blue == rhs.blue;
                }
            };

            ObjectVariantMapper mapper;

            mapper.Add<RGBColor> (
                [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const RGBColor* obj) -> VariantValue {
                    return L"#" + Characters::Format (L"%2x%2x%2x", obj->red, obj->green, obj->blue);
                },
                [] ([[maybe_unused]] const ObjectVariantMapper& mapper, const VariantValue& d, RGBColor* intoObj) -> void {
                    String tmpInBuf = d.As<String> ();
                    if (tmpInBuf.length () != 7) {
                        Execution::Throw (DataExchange::BadFormatException{L"RGBColor sb length 7"});
                    }
                    if (tmpInBuf[0] != '#') {
                        Execution::Throw (DataExchange::BadFormatException{L"RGBColor must start with #"});
                    }
                    auto readColorComponent = [] (const wchar_t* start, const wchar_t* end) -> uint8_t {
                        wchar_t buf[1024];
                        Require (end - start < static_cast<ptrdiff_t> (Memory::NEltsOf (buf)));
                        memcpy (buf, start, (end - start) * sizeof (wchar_t));
                        buf[(end - start)] = '\0';
                        wchar_t* e         = nullptr;
                        auto     result    = std::wcstoul (buf, &e, 16);
                        if (e != buf + 2) {
                            Execution::Throw (DataExchange::BadFormatException (L"expected 6 hex bytes"));
                        }
                        Assert (result <= 255);
                        return static_cast<uint8_t> (result);
                    };
                    intoObj->red   = readColorComponent (tmpInBuf.c_str () + 1, tmpInBuf.c_str () + 3);
                    intoObj->green = readColorComponent (tmpInBuf.c_str () + 3, tmpInBuf.c_str () + 5);
                    intoObj->blue  = readColorComponent (tmpInBuf.c_str () + 5, tmpInBuf.c_str () + 7);
                });

            RGBColor     tmp = RGBColor{255, 255, 255};
            VariantValue v   = mapper.FromObject (tmp);

            Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
            Variant::JSON::Writer{}.Write (v, tmpStream);

            // THEN deserialized, and mapped back to C++ object form
            RGBColor tmp2 = mapper.ToObject<RGBColor> (Variant::JSON::Reader{}.Read (tmpStream));
            VerifyTestResult (tmp2 == tmp);
        }
    }
}

namespace {
    void DoRegressionTests_MakeCommonSerializer_EnumAsInt_12_ ()
    {
        Debug::TraceContextBumper ctx{L"{}::DoRegressionTests_MakeCommonSerializer_EnumAsInt_12_"};
        using namespace Traversal;
        const bool kWrite2FileAsWell_ = true; // just for debugging

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
                : fEnum1{Fred::a}
            {
            }

            bool operator== (const SharedContactsConfig_& rhs) const
            {
                return fEnum1 == rhs.fEnum1;
            }
        };

        {
            ObjectVariantMapper mapper;

            mapper.Add (ObjectVariantMapper::MakeCommonSerializer_EnumAsInt<Fred> ());
            mapper.AddClass<SharedContactsConfig_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                {L"fEnum1", StructFieldMetaInfo{&SharedContactsConfig_::fEnum1}},
            });

            SharedContactsConfig_ tmp;
            tmp.fEnum1     = Fred::b;
            VariantValue v = mapper.FromObject (tmp);

            // at this point - we should have VariantValue object with "Enabled" field.
            // This can then be serialized using

            Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
            Variant::JSON::Writer{}.Write (v, tmpStream);

            if (kWrite2FileAsWell_) {
                filesystem::path fileName = IO::FileSystem::WellKnownLocations::GetTemporary () / "12.txt"sv;
                Variant::JSON::Writer{}.Write (v, IO::FileSystem::FileOutputStream::New (fileName));
                [[maybe_unused]] SharedContactsConfig_ tmp2 =
                    mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (IO::FileSystem::FileInputStream::New (fileName)));
            }

            // THEN deserialized, and mapped back to C++ object form
            SharedContactsConfig_ tmp2 = mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (tmpStream));
            VerifyTestResult (tmp2 == tmp);
        }
    }
}

namespace {
    namespace Test13_ObjVarMapperAndGUID_ {
        void DoIt ()
        {
            ObjectVariantMapper m;
            Common::GUID        g = Common::GUID::GenerateNew ();
            VerifyTestResult (m.ToObject<Common::GUID> (m.FromObject (g)) == g);
        }
    }
}

namespace {
    // https://stroika.atlassian.net/browse/STK-909
    namespace Test14_ObjVarMapperAsStringVsToString_STK_909_ {
        void DoIt ()
        {
            ObjectVariantMapper mapper;
            struct T {
                IO::Network::InternetAddress    ia;
                optional<IO::Network::CIDR>     cidr;
                DataExchange::InternetMediaType mediaType;
                bool                            operator== (const T& rhs) const
                {
                    return ia == rhs.ia and cidr == rhs.cidr and mediaType == rhs.mediaType;
                }
            };
            mapper.AddCommonType<IO::Network::CIDR> ();
            mapper.AddCommonType<optional<IO::Network::CIDR>> ();
            mapper.AddClass<T> ({
                {L"ia", StructFieldMetaInfo{&T::ia}},
                {L"cidr", StructFieldMetaInfo{&T::cidr}},
                {L"mediaType", StructFieldMetaInfo{&T::mediaType}},
            });
            T g1{IO::Network::V4::kLocalhost, IO::Network::CIDR{IO::Network::V6::kAddrAny, 64}, DataExchange::InternetMediaTypes::kJPEG};
            VerifyTestResult (mapper.ToObject<T> (mapper.FromObject (g1)) == g1);
            T g2{IO::Network::V4::kLocalhost, IO::Network::CIDR{IO::Network::V4::kLocalhost, 16}, DataExchange::InternetMediaTypes::kGIF};
            VerifyTestResult (mapper.ToObject<T> (mapper.FromObject (g2)) == g2);
            T g3{IO::Network::V4::kLocalhost, IO::Network::CIDR{IO::Network::InternetAddress{"192.22.4.4"}, 9}, DataExchange::InternetMediaTypes::kGIF};
            VerifyTestResult (mapper.ToObject<T> (mapper.FromObject (g3)) == g3);
        }
    }
}

namespace {
    namespace Test15_JIRA_951_ObjectMapper_SortedMultiset_ {
        void DoIt ()
        {
            using Common::CountedValue;
            using Containers::MultiSet;
            {
                // Allow CountedValue to be default constructed (when its T type is default constructible)
                [[maybe_unused]] CountedValue<int> x1;
            }
            {
                // Allow Adder<> to work with Containers::MultiSet
                using T = int;
                MultiSet<T> x2;
                Containers::Adapters::Adder<MultiSet<T>>::Add (&x2, CountedValue<T>{3, 4});
            }
            {
                ObjectVariantMapper mapper;
                MultiSet<int>       s1;
                mapper.AddCommonType<CountedValue<int>> ();
                mapper.Add (ObjectVariantMapper::MakeCommonSerializer_WithAdder<MultiSet<int>> ());
                s1.Add (2);
                s1.Add (2);
                s1.Add (3);
                VariantValue  sAsVariant         = mapper.FromObject (s1);
                MultiSet<int> mappedBackToObject = mapper.ToObject<MultiSet<int>> (sAsVariant);
                VerifyTestResult (s1 == mappedBackToObject);
            }
            {
                ObjectVariantMapper mapper;
                MultiSet<int>       s1;
                mapper.AddCommonType<CountedValue<int>> ();
                mapper.AddCommonType<MultiSet<int>> ();
                s1.Add (2);
                s1.Add (2);
                s1.Add (3);
                VariantValue  sAsVariant         = mapper.FromObject (s1);
                MultiSet<int> mappedBackToObject = mapper.ToObject<MultiSet<int>> (sAsVariant);
                VerifyTestResult (s1 == mappedBackToObject);
            }
            {
                ObjectVariantMapper mapper;
                SortedMultiSet<int> s1;
                mapper.AddCommonType<CountedValue<int>> ();
                mapper.AddCommonType<SortedMultiSet<int>> ();
                s1.Add (2);
                s1.Add (2);
                s1.Add (3);
                VariantValue        sAsVariant         = mapper.FromObject (s1);
                SortedMultiSet<int> mappedBackToObject = mapper.ToObject<SortedMultiSet<int>> (sAsVariant);
                VerifyTestResult (s1 == mappedBackToObject);
            }
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        DoRegressionTests_BasicDataRoundtrips_1_::DoAll ();
        DoRegressionTests_SimpleMapToFromJSON_2_ ();
        DoRegressionTests_SimpleMapToFromJSON_3_ ();
        DoRegressionTests_SimpleMapRangeTypes_4_ ();
        DoRegressionTests_SimpleEnumTypes_5_ ();
        DoRegressionTests_DurationsDateTime_6_ ();
        DoRegressionTests_VariantValue_7_ ();
        DoRegressionTests_MakeCommonSerializer_8_ ();
        DoRegressionTests_Subclass_9_ ();
        DoRegressionTests_FileTypeConverterOverride_10_ ();
        DoRegressionTests_CustomMapper_11_ ();
        DoRegressionTests_MakeCommonSerializer_EnumAsInt_12_ ();
        Test13_ObjVarMapperAndGUID_::DoIt ();
        Test14_ObjVarMapperAsStringVsToString_STK_909_::DoIt ();
        Test15_JIRA_951_ObjectMapper_SortedMultiset_::DoIt ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
