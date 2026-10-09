/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::DataExchange::Other
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/DataExchange/Atom.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/Encoding/Hex.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/JSON/JWT.h"
#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Logger.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include "Stroika/Foundation/Streams/ExternallyOwnedSpanInputStream.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;

using Execution::ModuleGetterSetter;
using Traversal::Iterable;

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Foundation_DataExchange_Other, Atom_)
    {
        Debug::TraceContextBumper ctx{"{}::Atom_"};
        {
            Atom<> a = "d";
            Atom<> b = "d";
            EXPECT_EQ (a, b);
            EXPECT_EQ (a.GetPrintName (), "d");
            EXPECT_EQ (a.As<String> (), "d");
            EXPECT_EQ (a.As<wstring> (), L"d");
            EXPECT_TRUE (not a.empty ());
        }
        {
            EXPECT_TRUE (Atom<> ().empty ());
        }
        {
            Atom<> a = "d";
            Atom<> b = "e";
            EXPECT_TRUE (a != b);
            EXPECT_TRUE (not a.empty ());
            Atom<> c = a;
            EXPECT_EQ (c, a);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_Other, OptionsFile_)
    {
        Debug::TraceContextBumper ctx{"{}::OptionsFile_"};
        struct MyData_ {
            bool               fEnabled = false;
            optional<DateTime> fLastSynchronizedAt;
        };
        OptionsFile of{"MyModule"sv,
                       [] () -> ObjectVariantMapper {
                           ObjectVariantMapper mapper;
                           mapper.AddClass<MyData_> ({
                               {"Enabled"sv, &MyData_::fEnabled},
                               {"Last-Synchronized-At"sv, &MyData_::fLastSynchronizedAt},
                           });
                           return mapper;
                       }(),
                       OptionsFile::kDefaultUpgrader,
                       [] (const String& moduleName, const String& fileSuffix) -> filesystem::path {
                           return IO::FileSystem::WellKnownLocations::GetTemporary () / (moduleName + fileSuffix).As<filesystem::path> ();
                       }};
        MyData_     m = of.Read<MyData_> (MyData_{}); // will return default values if file not present
        of.Write (m);                                 // test writing
    }
}

namespace {
    struct MyData_ {
        bool               fEnabled = false;
        optional<DateTime> fLastSynchronizedAt;
    };
    struct ModuleGetterSetter_Implementation_MyData_ {
        ModuleGetterSetter_Implementation_MyData_ ()
            : fOptionsFile_{"MyModule"sv,
                            [] () -> ObjectVariantMapper {
                                ObjectVariantMapper mapper;
                                mapper.AddClass<MyData_> ({
                                    {"Enabled"sv, &MyData_::fEnabled},
                                    {"Last-Synchronized-At"sv, &MyData_::fLastSynchronizedAt},
                                });
                                return mapper;
                            }(),
                            OptionsFile::kDefaultUpgrader,
                            [] (const String& moduleName, const String& fileSuffix) -> filesystem::path {
                                // for regression tests write to /tmp
                                return IO::FileSystem::WellKnownLocations::GetTemporary () / (moduleName + fileSuffix).As<filesystem::path> ();
                            }}
            , fActualCurrentConfigData_{fOptionsFile_.Read<MyData_> (MyData_{})}
        {
            Set (fActualCurrentConfigData_); // assure derived data (and changed fields etc) up to date
        }
        MyData_ Get () const
        {
            return fActualCurrentConfigData_;
        }
        void Set (const MyData_& v)
        {
            fActualCurrentConfigData_ = v;
            fOptionsFile_.Write (v);
        }

    private:
        OptionsFile fOptionsFile_;
        MyData_     fActualCurrentConfigData_; // automatically initialized just in time, and externally synchronized
    };

    ModuleGetterSetter<MyData_, ModuleGetterSetter_Implementation_MyData_> sModuleConfiguration_;

    GTEST_TEST (Foundation_DataExchange_Other, Test3_ModuleGetterSetter_)
    {
        Debug::TraceContextBumper ctx{"{}::Test3_ModuleGetterSetter_"};
        if (sModuleConfiguration_.Get ().fEnabled) {
            auto n = sModuleConfiguration_.Get ();
            sModuleConfiguration_.Set (n);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_Other, VariantValue_)
    {
        Debug::TraceContextBumper ctx{"{}::VariantValue_"};

        using namespace Containers;
        using namespace Characters;
        using namespace Time;
        {
            Collection<VariantValue> vc;
            VariantValue             vv{vc};
        }
        {
            optional<String> x1;
            optional<Date>   x2;
            String           representAs1 = VariantValue{x1}.As<String> ();
            String           representAs2 = VariantValue{x2}.As<String> ();
            x1                            = VariantValue{representAs1}.As<optional<String>> ();
            x2                            = VariantValue{representAs2}.As<optional<Date>> ();
        }
        {
            auto roundTrip = [] (auto tValue) {
                using T               = remove_cvref_t<decltype (tValue)>;
                String representation = VariantValue{tValue}.As<String> ();
                return VariantValue{representation}.As<T> ();
            };
            EXPECT_EQ (roundTrip ("v"_k), "v");
            EXPECT_EQ (roundTrip (5), 5);
            EXPECT_EQ (roundTrip (optional<int>{}), optional<int>{});
            EXPECT_EQ (roundTrip (optional<Date>{}), optional<Date>{});
            constexpr DateTime kT1_ = DateTime{Date{January / 3 / 1944}};
            EXPECT_EQ (roundTrip (kT1_), kT1_);

            // But doesn't work perfectly. Empty string and optional<String>{} get represented as the same so that's ambiguous
            EXPECT_EQ (roundTrip (String{}), String{});
            EXPECT_EQ (roundTrip (optional<String>{}), nullopt);
            EXPECT_EQ (roundTrip (optional<String>{String{}}), nullopt); // oops - but really how could it tell?
        }
        DbgTrace ("mapping_vv={}"_f, Mapping<String, VariantValue>{{"a", 3}});
        {
            EXPECT_EQ (VariantValue{8}, VariantValue{8});
            EXPECT_NE (VariantValue{8}, VariantValue{9}); // FAILED: Ensure; EqualsComparer{}(*this, rhs) == (ThreeWayComparer{}(*this, rhs) == 0);
        }
        {
            // VariantValue (bool) takes exactly a bool: any pointer converts to bool, but must not quietly become VariantValue{true}
            EXPECT_TRUE ((convertible_to<bool, VariantValue>));
            EXPECT_EQ (VariantValue{true}.GetType (), VariantValue::eBoolean);
            EXPECT_FALSE ((constructible_from<VariantValue, const int*>));
            EXPECT_FALSE ((convertible_to<const VariantValue*, VariantValue>));
            // were a pointer a VariantValue, a standard library with the C++26 draft's span (initializer_list) constructor (P2447,
            // withdrawn by P4144 - but in libstdc++ 16 and libc++ 22) would make the span{&item, 1} Append passes on a list of TWO
            Sequence<VariantValue> s;
            s.Append (VariantValue{3});
            s.Insert (0, VariantValue{"x"sv});
            EXPECT_EQ (s.size (), 2u);
            EXPECT_EQ (s[1], VariantValue{3});
        }
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_Other, JWT)
    {
        Debug::TraceContextBumper ctx{"{}::JTW"};
        using namespace DataExchange::JSON;
        auto encodedJWT =
            "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXUyJ9.eyJpc3MiOiJhdXRoMCIsInNhbXBsZSI6InRlc3QifQ.lQm3N2bVlqt2-1L-FsOjtR6uE-L4E9zJutMWKIe1v1M";
        JWT jwt{encodedJWT};
        for ([[maybe_unused]] auto& claim : jwt.GetHeaderClaims ()) {
            DbgTrace ("header claim: {}"_f, claim);
        }
        for ([[maybe_unused]] auto& claim : jwt.GetPayloadClaims ()) {
            DbgTrace ("payload claim: {}"_f, claim);
        }
        if ([[maybe_unused]] auto audience = jwt.GetAudience ()) {
            DbgTrace ("Audience is {}"_f, *audience);
        }
        DbgTrace ("ValidFor={}"_f, jwt.GetValidFor ());
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_Other, InternetMediaType_)
    {
        Debug::TraceContextBumper ctx{"{}::InternetMediaType_"};
        {
            InternetMediaType ct0{"text/plain"};
            EXPECT_EQ (ct0.GetType (), "text");
            EXPECT_EQ (ct0.GetSubType (), "plain");
            EXPECT_EQ (ct0.GetSuffix (), nullopt);

            InternetMediaType ct1{"text/plain;charset=ascii"};
            EXPECT_EQ (ct1.GetParameters (), (Containers::Mapping{Common::KeyValuePair<String, String>{"charset", "ascii"}}));
            EXPECT_EQ (ct1.GetSuffix (), nullopt);

            InternetMediaType ct2{"text/plain; charset = ascii"};
            EXPECT_EQ (ct1, ct2);

            InternetMediaType ct3{"text/plain; charset = \"ascii\""};
            EXPECT_EQ (ct1, ct3);

            InternetMediaType ct4{"text/plain; charset = \"ASCII\""}; // case insensitive compare key, but not value
            EXPECT_TRUE (ct1 != ct4);

            InternetMediaType ct5{"application/vnd.ms-excel"};
            EXPECT_EQ (ct5.GetType (), "application");
            EXPECT_EQ (ct5.GetSubType (), "vnd.ms-excel");
            EXPECT_EQ (ct5.GetSuffix (), nullopt);

            InternetMediaType ct6{"application/mathml+xml"};
            EXPECT_EQ (ct6.GetType (), "application");
            EXPECT_EQ (ct6.GetSubType (), "mathml");
            EXPECT_EQ (ct6.GetSuffix (), "xml");
            EXPECT_EQ (ct6.As<wstring> (), L"application/mathml+xml");
        }
        {
            // Example from https://tools.ietf.org/html/rfc2045#page-10 - comments ignored, and quotes on value
            InternetMediaType ct1{"text/plain; charset=us-ascii (Plain text)"};
            InternetMediaType ct2{"text/plain; charset=\"us-ascii\""};
            EXPECT_EQ (ct1, ct2);
            EXPECT_TRUE (InternetMediaTypeRegistry::sThe->IsA (InternetMediaTypes::Wildcards::kText, ct1));
        }
        {
            auto dumpCT = [] ([[maybe_unused]] const String& label, InternetMediaType i) {
                [[maybe_unused]] InternetMediaTypeRegistry r = InternetMediaTypeRegistry::sThe;
                DbgTrace ("SUFFIX({})={}"_f, label.As<wstring> (), Characters::ToString (r.GetPreferredAssociatedFileSuffix (i)));
                DbgTrace ("ASSOCFILESUFFIXES({})={}"_f, label.As<wstring> (), Characters::ToString (r.GetAssociatedFileSuffixes (i)));
                DbgTrace ("GetAssociatedPrettyName({})={}"_f, label, Characters::ToString (r.GetAssociatedPrettyName (i)));
            };
            auto checkCT = [] (InternetMediaType i, const Set<String>& possibleFileSuffixes) {
                [[maybe_unused]] InternetMediaTypeRegistry r = InternetMediaTypeRegistry::sThe;
                using namespace Characters;
                if (not possibleFileSuffixes.Contains (r.GetPreferredAssociatedFileSuffix (i).value_or (""))) {
                    Stroika::Frameworks::Test::WarnTestIssue (Format ("File suffix mismatch for {}: got {}, expected {}"_f, i,
                                                                      r.GetPreferredAssociatedFileSuffix (i), possibleFileSuffixes));
                }
                if (not possibleFileSuffixes.Any ([&] (String suffix) -> bool { return r.GetAssociatedContentType (suffix) == i; })) {
                    Stroika::Frameworks::Test::WarnTestIssue (
                        Format ("GetAssociatedContentType for fileSuffixes {} (expected {}, got {})"_f, possibleFileSuffixes, i,
                                possibleFileSuffixes
                                    .Map<Iterable<InternetMediaType>> ([&] (String suffix) { return r.GetAssociatedContentType (suffix); })
                                    .As<Set<InternetMediaType>> ()));
                }
            };
            dumpCT ("PLAINTEXT", InternetMediaTypes::kText_PLAIN);
            checkCT (InternetMediaTypes::kText_PLAIN, {".txt"});
            dumpCT ("HTML"sv, InternetMediaTypes::kHTML);
            checkCT (InternetMediaTypes::kHTML, {".html", ".htm"});
            dumpCT ("JSON", InternetMediaTypes::kJSON);
            checkCT (InternetMediaTypes::kJSON, {".json"});
            dumpCT ("PNG", InternetMediaTypes::kPNG);
            checkCT (InternetMediaTypes::kPNG, {".png"});
            {
                InternetMediaTypeRegistry registry = InternetMediaTypeRegistry::sThe;
                EXPECT_TRUE (registry.IsA (InternetMediaTypes::Wildcards::kImage, InternetMediaTypes::kPNG));
                EXPECT_TRUE (not registry.IsA (InternetMediaTypes::Wildcards::kImage, InternetMediaTypes::kJSON));
                EXPECT_TRUE (registry.IsA (InternetMediaTypes::kXML, InternetMediaTypes::kXML));
                EXPECT_TRUE (not registry.IsA (InternetMediaTypes::kXML, InternetMediaTypes::kText_PLAIN));
                EXPECT_TRUE (registry.IsA (InternetMediaTypes::Wildcards::kText, InternetMediaTypes::kText_PLAIN));
                EXPECT_TRUE (registry.IsA (InternetMediaTypes::Wildcards::kText, InternetMediaTypes::kXML));
                EXPECT_TRUE (registry.IsA (InternetMediaTypes::Wildcards::kText, InternetMediaTypes::kHTML));
                EXPECT_TRUE (registry.IsA (InternetMediaTypes::Wildcards::kText, InternetMediaTypes::kJSON));
                EXPECT_TRUE (not registry.IsA (InternetMediaTypes::Wildcards::kText, InternetMediaTypes::kPNG));
                EXPECT_TRUE (not registry.IsA (InternetMediaTypes::kXML, InternetMediaType{"text/foobar"sv}));
                EXPECT_TRUE (registry.IsA (InternetMediaTypes::kXML, InternetMediaType{"text/foobar+xml"sv}));
                EXPECT_TRUE (registry.IsA (InternetMediaTypes::kJSON, InternetMediaType{"application/openapi+json"sv}));
                EXPECT_TRUE (registry.IsA (InternetMediaTypes::Wildcards::kText, InternetMediaType{"application/openapi+json"sv}));
            }
        }
        {
            Debug::TraceContextBumper ctx1{"InternetMediaTypeRegistry::sThe->GetMediaTypes()"};
            // enumerate all content types
            for (auto ct : InternetMediaTypeRegistry::sThe->GetMediaTypes ()) {
                DbgTrace ("i={}"_f, ct);
            }
        }
        {
            Debug::TraceContextBumper ctx1{"InternetMediaTypeRegistry - updating"};
            InternetMediaTypeRegistry origRegistry    = InternetMediaTypeRegistry::sThe;
            InternetMediaTypeRegistry updatedRegistry = origRegistry;
            const auto                kHFType_        = InternetMediaType{"application/fake-heatlthframe-phr+xml"};
            EXPECT_TRUE (not InternetMediaTypeRegistry::sThe->GetMediaTypes ().Contains (kHFType_));
            updatedRegistry.AddOverride (kHFType_, InternetMediaTypeRegistry::OverrideRecord{nullopt, Containers::Set<String>{".HPHR"}, ".HPHR"});
            InternetMediaTypeRegistry::sThe.store (updatedRegistry);
            EXPECT_TRUE (InternetMediaTypeRegistry::sThe->IsA (InternetMediaTypes::kXML, kHFType_));
            EXPECT_TRUE (InternetMediaTypeRegistry::sThe->GetMediaTypes ().Contains (kHFType_));
            EXPECT_TRUE (not origRegistry.GetMediaTypes ().Contains (kHFType_));
            EXPECT_TRUE (updatedRegistry.GetMediaTypes ().Contains (kHFType_));
        }
        {
            // Noticed mistake in InternetMediaTypeRegistry::sThe->IsA () -- as=application/x-ccrddd should not match ISA relationship with application/c-ccr
            const InternetMediaType kCCR{"application/x-ccr"sv};
            const InternetMediaType kCCR_LegitAlt{"application/x-ccr+xml"sv};
            const InternetMediaType kCCR_Typo1{"application/x-ccx"sv};
            const InternetMediaType kCCR_Typo2{"application/x-ccrPLUSEXTRASTUFFATTHEEND"sv};
            const InternetMediaType kCCR_WRONG_CASE1{"APPLICATION/x-ccr"sv};
            const InternetMediaType kCCR_WRONG_CASE2{"APPLICATION/X-CCR"sv};
            EXPECT_EQ (kCCR, kCCR_WRONG_CASE1);
            EXPECT_EQ (kCCR, kCCR_WRONG_CASE2);
            EXPECT_TRUE (InternetMediaTypeRegistry::sThe->IsA (kCCR, kCCR));
            EXPECT_TRUE (InternetMediaTypeRegistry::sThe->IsA (kCCR, kCCR_LegitAlt));
            EXPECT_TRUE (not InternetMediaTypeRegistry::sThe->IsA (kCCR, kCCR_Typo1));
            EXPECT_TRUE (not InternetMediaTypeRegistry::sThe->IsA (kCCR, kCCR_Typo2));
        }
    }
}

namespace {
    /*
     *  Hex (base16): RFC 4648's test vectors (section 10) - written in lower case, read in either - spaces between bytes, what is
     *  not hex refused, every byte there and back; and BLOB's AsHex and FromHex, which forward to it.
     */
    GTEST_TEST (Foundation_DataExchange_Other, Hex_)
    {
        Debug::TraceContextBumper ctx{"{}::Hex_"};
        using namespace Encoding;
        auto bytesOf = [] (string_view s) { return Memory::BLOB{as_bytes (span<const char>{s})}; };
        for (auto [text, hex] : initializer_list<pair<string_view, string_view>>{
                 {""sv, ""sv}, {"f"sv, "66"sv}, {"fo"sv, "666f"sv}, {"foo"sv, "666f6f"sv}, {"foobar"sv, "666f6f626172"sv}}) {
            EXPECT_EQ (Hex::Encode (bytesOf (text)), hex);
            EXPECT_EQ (Hex::Decode (hex), bytesOf (text));
        }
        EXPECT_EQ (Hex::Decode ("666F6F626172"sv), bytesOf ("foobar"sv)); // as the RFC writes it
        EXPECT_EQ (Hex::Decode ("66 6f 6f"sv), bytesOf ("foo"sv));
        EXPECT_EQ (Hex::Decode (String{"666f6f"sv}), bytesOf ("foo"sv));
        EXPECT_THROW (Hex::Decode ("666"sv), BadFormatException); // a byte's second digit missing
        EXPECT_THROW (Hex::Decode ("6g"sv), BadFormatException);
        vector<std::byte> all;
        for (unsigned int i = 0; i < 256; ++i) {
            all.push_back (static_cast<std::byte> (i));
        }
        EXPECT_EQ (Hex::Decode (Hex::Encode (Memory::BLOB{all})), Memory::BLOB{all});
        // BLOB's, forwarding to it
        const Memory::BLOB foobar = bytesOf ("foobar"sv);
        EXPECT_EQ (foobar.AsHex (), "666f6f626172"sv);
        EXPECT_EQ (foobar.AsHex (2), "666f"sv); // just the first 2 bytes
        EXPECT_EQ (Memory::BLOB::FromHex ("666F6F626172"), foobar);
    }
}
#endif

int main (int argc, const char* argv[])
{
    Execution::Logger::Activator logMgrActivator; // for OptionsFile test
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "[  SKIPPED ] every test - Stroika regression tests require building with google test feature" << endl;
#endif
}
