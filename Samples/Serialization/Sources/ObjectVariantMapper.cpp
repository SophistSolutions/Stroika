/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Mapping.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Reader.h"
#include "Stroika/Foundation/DataExchange/Variant/JSON/Writer.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"

#include "ObjectVariantMapper.h"

using namespace std;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;

using std::byte;

namespace {
    void SimpleGettingStarted_ ()
    {
        Debug::TraceContextBumper ctx{L"SimpleGettingStarted_"};

        // Define some types that you want serialized
        struct MyType2Serialize1_ {
            bool fEnabled{false};

            // Not needed to use ObjectVariantMapper - just needed to 'test' if the data round-tripped properly
            bool operator== (const MyType2Serialize1_& rhs) const = default;
        };

        // Define an ObjectVariantMapper which knows how to map your types to/from VariantValue objects
        ObjectVariantMapper mapper;

        // Add the types to the mapper, which it will need
        mapper.AddClass<MyType2Serialize1_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {"Enabled"sv, StructFieldMetaInfo{&MyType2Serialize1_::fEnabled}},
        });

        // Create a test object to serialize
        MyType2Serialize1_ tmp;
        tmp.fEnabled = true;

        /// Map any arbitrary (defined in ObjectVariantMapper) object to a VariantValue
        VariantValue v = mapper.FromObject (tmp);

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be displayed for debugging purposes using
        DbgTrace (L"v = %s", Characters::ToString (v).c_str ());

        // Serialize using any serialization writer defined in Stroika::Foundation::DataExchange::Variant (we chose JSON here)
        // And dump the results into a temporary memory-based stream
        Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
        Variant::JSON::Writer{}.Write (v, tmpStream);

        // THEN deserialize, and map back to C++ object form
        [[maybe_unused]] MyType2Serialize1_ tmp2 = mapper.ToObject<MyType2Serialize1_> (Variant::JSON::Reader{}.Read (tmpStream));

        // make sure new object matches
        Assert (tmp2 == tmp);
    }
}

namespace {
    void UseObjectVariantMapperTry2_ ()
    {
        Debug::TraceContextBumper ctx{L"UseObjectVariantMapperTry2_"};

        // Define some types that you want serialized
        struct SharedContactsConfig_ {
            bool                    fEnabled{false};
            optional<DateTime>      fLastSynchronizedAt;
            Mapping<String, String> fThisPHRsIDToSharedContactID;

            // Not needed to use ObjectVariantMapper - just needed to 'test' if the data round-tripped properly
            bool operator== (const SharedContactsConfig_& rhs) const = default;

            // Not needed to use ObjectVariantMapper - just to be able to print to debugger/trace-log this type
            Characters::String ToString () const
            {
                StringBuilder sb;
                sb << "{";
                sb << " Enabled: " << Characters::ToString (fEnabled) << ", ";
                sb << " Last-Synchronized-At: " << Characters::ToString (fLastSynchronizedAt) << ", ";
                sb << " This-PHRs-ID-To-Shared-Contact-ID: " << Characters::ToString (fThisPHRsIDToSharedContactID);
                sb << "}";
                return sb.str ();
            }
        };

        // Define an ObjectVariantMapper which knows how to map your types to/from VariantValue objects
        ObjectVariantMapper mapper;

        // Some types pre-loaded (see @ObjectVariantMapper::ResetToDefaultTypeRegistry)
        // The rest must be explicitly added to the registry before use.

        mapper.AddClass<SharedContactsConfig_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {"Enabled"sv, StructFieldMetaInfo{&SharedContactsConfig_::fEnabled}},
            {"Last-Synchronized-At"sv, StructFieldMetaInfo{&SharedContactsConfig_::fLastSynchronizedAt}},
            {"This-HR-ContactID-To-SharedContactID-Map"sv, StructFieldMetaInfo{&SharedContactsConfig_::fThisPHRsIDToSharedContactID}},
        });

        // fill in a sample object to write
        SharedContactsConfig_ tmp;
        {
            bool newEnabled = true;
            tmp.fEnabled    = newEnabled;
            tmp.fThisPHRsIDToSharedContactID.Add ("A", "B");
            tmp.fLastSynchronizedAt = DateTime{1998y / Time::April / 11d, Time::TimeOfDay::Parse ("3pm", locale::classic ())};
        }

        /// Map that object to a VariantValue
        VariantValue v = mapper.FromObject (tmp);

        // at this point - we should have VariantValue object with "Enabled", "Last-Synchronized-At" etc fields set properly.
        // This can then be displayed using
        DbgTrace (L"v = %s", Characters::ToString (v).c_str ());

        // Serialize using any serialization writer defined in Stroika::Foundation::DataExchange::Variant (we selected JSON)
        Streams::MemoryStream<byte>::Ptr tmpStream = Streams::MemoryStream<byte>::New ();
        Variant::JSON::Writer{}.Write (v, tmpStream);

        // You can persist these to file if you wish
        constexpr bool kWrite2FileAsWell_ = true;
        if (kWrite2FileAsWell_) {
            {
                IO::FileSystem::FileOutputStream::Ptr tmpFileStream =
                    IO::FileSystem::FileOutputStream::New (IO::FileSystem::WellKnownLocations::GetTemporary () / "t.txt");
                Variant::JSON::Writer{}.Write (v, tmpFileStream);
            }
            {
                // , and then if you want, try reading it back
                IO::FileSystem::FileInputStream::Ptr tmpFileStream =
                    IO::FileSystem::FileInputStream::New (IO::FileSystem::WellKnownLocations::GetTemporary () / "t.txt");
                SharedContactsConfig_ tmp2 = mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (tmpFileStream));
                DbgTrace (L"tmp2 = %s", Characters::ToString (tmp2).c_str ());
                Assert (tmp2 == tmp);
            }
        }

        // THEN deserialize, and map back to C++ object form
        SharedContactsConfig_ tmp2 = mapper.ToObject<SharedContactsConfig_> (Variant::JSON::Reader{}.Read (tmpStream));

        // and check the roundtrip worked
        Assert (tmp2 == tmp);
    }
}

namespace {
    void GeneratingReadOnlyFieldsTry3_ ()
    {
        Debug::TraceContextBumper ctx{L"GeneratingReadOnlyFieldsTry3_"};

        // Define some types that you want serialized
        struct MyType2Serialize1_ {
            bool fEnabled{false};
        };

        // Define an ObjectVariantMapper which knows how to map your types to/from VariantValue objects
        ObjectVariantMapper mapper;

        // Add the types to the mapper, which it will need
        mapper.AddClass<MyType2Serialize1_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {"Enabled"sv, StructFieldMetaInfo{&MyType2Serialize1_::fEnabled}},
        });

        auto trySerializing = [] (const ObjectVariantMapper& mapper, auto obj) {
            VariantValue vv = mapper.FromObject (obj);

            // at this point - we should have VariantValue object with "Enabled" field.
            // This can then be displayed for debugging purposes using
            DbgTrace (L"vv = %s", Characters::ToString (vv).c_str ());

            // Serialize using any serialization writer defined in Stroika::Foundation::DataExchange::Variant (we chose JSON here)
            // And dump the results into a temporary memory-based stream
            Streams::MemoryStream<Character>::Ptr tmpStream = Streams::MemoryStream<Character>::New ();
            Variant::JSON::Writer{}.Write (vv, tmpStream);
            DbgTrace (L"rendered as JSON: = %s", tmpStream.As<String> ().ReplaceAll (L"\n", L"").c_str ());
        };

        // Create a test object to serialize
        MyType2Serialize1_ tmp;
        tmp.fEnabled = true;
        trySerializing (mapper, tmp);

        // @todo should simplify this - see https://stroika.atlassian.net/browse/STK-955
        auto myReadOnlyPropertyTypeMapper = ObjectVariantMapper::TypeMappingDetails{
            ObjectVariantMapper::FromObjectMapperType<MyType2Serialize1_> ([] (const ObjectVariantMapper&, const MyType2Serialize1_* objOfType) -> VariantValue {
                return VariantValue{objOfType->fEnabled ? 2 : 99};
            }),
            ObjectVariantMapper::ToObjectMapperType<MyType2Serialize1_> (nullptr)};

        // Now a fancier mapper
        mapper.AddClass<MyType2Serialize1_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
            {"Enabled"sv, StructFieldMetaInfo{&MyType2Serialize1_::fEnabled}},
            {"RandomValue"sv, myReadOnlyPropertyTypeMapper},
        });
        trySerializing (mapper, tmp);
    }
}

void Samples::Serialization::ObjectVariantMapper::RunDemo ()
{
    SimpleGettingStarted_ ();
    UseObjectVariantMapperTry2_ ();
    GeneratingReadOnlyFieldsTry3_ ();
}