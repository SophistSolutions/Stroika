/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::DataExchange::Other
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/DataExchange/Atom.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Logger.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include "Stroika/Foundation/Streams/ExternallyOwnedSpanInputStream.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using namespace Stroika::Frameworks;

using Execution::ModuleGetterSetter;
using Traversal::Iterable;

#if qHasFeature_GoogleTest
namespace {
    GTEST_TEST (Foundation_DataExchange_Other, Test1_Atom_)
    {
        Debug::TraceContextBumper ctx{"{}::Test1_Atom_"};
        {
            Atom<> a = L"d";
            Atom<> b = "d";
            EXPECT_EQ (a, b);
            EXPECT_EQ (a.GetPrintName (), L"d");
            EXPECT_EQ (a.As<String> (), L"d");
            EXPECT_EQ (a.As<wstring> (), L"d");
            EXPECT_TRUE (not a.empty ());
        }
        {
            EXPECT_TRUE (Atom<> ().empty ());
        }
        {
            Atom<> a = L"d";
            Atom<> b = L"e";
            EXPECT_TRUE (a != b);
            EXPECT_TRUE (not a.empty ());
            Atom<> c = a;
            EXPECT_EQ (c, a);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_Other, Test2_OptionsFile_)
    {
        Debug::TraceContextBumper ctx{"{}::Test2_OptionsFile_"};
        struct MyData_ {
            bool               fEnabled = false;
            optional<DateTime> fLastSynchronizedAt;
        };
        OptionsFile of{"MyModule"sv,
                       [] () -> ObjectVariantMapper {
                           ObjectVariantMapper mapper;
                           mapper.AddClass<MyData_> ({
                               {"Enabled"sv, StructFieldMetaInfo{&MyData_::fEnabled}},
                               {"Last-Synchronized-At"sv, StructFieldMetaInfo{&MyData_::fLastSynchronizedAt}},
                           });
                           return mapper;
                       }(),
                       OptionsFile::kDefaultUpgrader,
                       [] (const String& moduleName, const String& fileSuffix) -> filesystem::path {
                           return IO::FileSystem::WellKnownLocations::GetTemporary () / IO::FileSystem::ToPath (moduleName + fileSuffix);
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
                                    {"Enabled"sv, StructFieldMetaInfo{&MyData_::fEnabled}},
                                    {"Last-Synchronized-At"sv, StructFieldMetaInfo{&MyData_::fLastSynchronizedAt}},
                                });
                                return mapper;
                            }(),
                            OptionsFile::kDefaultUpgrader,
                            [] (const String& moduleName, const String& fileSuffix) -> filesystem::path {
                                // for regression tests write to /tmp
                                return IO::FileSystem::WellKnownLocations::GetTemporary () / IO::FileSystem::ToPath (moduleName + fileSuffix);
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
    GTEST_TEST (Foundation_DataExchange_Other, Test4_VariantValue_)
    {
        Debug::TraceContextBumper ctx{"{}::Test4_VariantValue_"};

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
    }
}

namespace {
    GTEST_TEST (Foundation_DataExchange_Other, Test5_InternetMediaType_)
    {
        Debug::TraceContextBumper ctx{"{}::Test5_InternetMediaType_"};
        {
            InternetMediaType ct0{"text/plain"};
            EXPECT_EQ (ct0.GetType (), "text");
            EXPECT_EQ (ct0.GetSubType (), "plain");
            EXPECT_EQ (ct0.GetSuffix (), nullopt);

            InternetMediaType ct1{L"text/plain;charset=ascii"};
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
            EXPECT_TRUE (InternetMediaTypeRegistry::Get ().IsTextFormat (ct1));
        }
        {
            auto dumpCT = [] ([[maybe_unused]] const String& label, InternetMediaType i) {
                [[maybe_unused]] InternetMediaTypeRegistry r = InternetMediaTypeRegistry::Get ();
                DbgTrace (L"SUFFIX(%s)=%s", label.As<wstring> ().c_str (), Characters::ToString (r.GetPreferredAssociatedFileSuffix (i)).c_str ());
                DbgTrace (L"ASSOCFILESUFFIXES(%s)=%s", label.As<wstring> ().c_str (), Characters::ToString (r.GetAssociatedFileSuffixes (i)).c_str ());
                DbgTrace (L"GetAssociatedPrettyName(%s)=%s", label.As<wstring> ().c_str (),
                          Characters::ToString (r.GetAssociatedPrettyName (i)).c_str ());
            };
            auto checkCT = [] (InternetMediaType i, const Set<String>& possibleFileSuffixes) {
                [[maybe_unused]] InternetMediaTypeRegistry r = InternetMediaTypeRegistry::Get ();
                using namespace Characters;
                if (not possibleFileSuffixes.Contains (r.GetPreferredAssociatedFileSuffix (i).value_or (L""))) {
                    Stroika::Frameworks::Test::WarnTestIssue (Format (L"File suffix mismatch for %s: got %s, expected %s", ToString (i).c_str (),
                                                                      ToString (r.GetPreferredAssociatedFileSuffix (i)).c_str (),
                                                                      ToString (possibleFileSuffixes).c_str ())
                                                                  .c_str ());
                }
                if (not possibleFileSuffixes.Any ([&] (String suffix) -> bool { return r.GetAssociatedContentType (suffix) == i; })) {
                    Stroika::Frameworks::Test::WarnTestIssue (Format (L"GetAssociatedContentType for fileSuffixes %s (expected %s, got %s)",
                                                                      ToString (possibleFileSuffixes).c_str (), ToString (i).c_str (),
                                                                      ToString (possibleFileSuffixes
                                                                                    .Map<Iterable<InternetMediaType>> ([&] (String suffix) {
                                                                                        return r.GetAssociatedContentType (suffix);
                                                                                    })
                                                                                    .As<Set<InternetMediaType>> ())
                                                                          .c_str ())
                                                                  .c_str ());
                }
            };
            dumpCT (L"PLAINTEXT", InternetMediaTypes::kText_PLAIN);
            checkCT (InternetMediaTypes::kText_PLAIN, {".txt"});
            dumpCT (L"HTML", InternetMediaTypes::kHTML);
            checkCT (InternetMediaTypes::kHTML, {".html", ".htm"});
            dumpCT (L"JSON", InternetMediaTypes::kJSON);
            checkCT (InternetMediaTypes::kJSON, {".json"});
            dumpCT (L"PNG", InternetMediaTypes::kPNG);
            checkCT (InternetMediaTypes::kPNG, {".png"});
            {
                EXPECT_TRUE (InternetMediaTypeRegistry::Get ().IsImageFormat (InternetMediaTypes::kPNG));
                EXPECT_TRUE (not InternetMediaTypeRegistry::Get ().IsImageFormat (InternetMediaTypes::kJSON));
                EXPECT_TRUE (InternetMediaTypeRegistry::Get ().IsXMLFormat (InternetMediaTypes::kXML));
                EXPECT_TRUE (not InternetMediaTypeRegistry::Get ().IsXMLFormat (InternetMediaTypes::kText_PLAIN));
                EXPECT_TRUE (InternetMediaTypeRegistry::Get ().IsTextFormat (InternetMediaTypes::kText_PLAIN));
                EXPECT_TRUE (InternetMediaTypeRegistry::Get ().IsTextFormat (InternetMediaTypes::kXML));
                EXPECT_TRUE (InternetMediaTypeRegistry::Get ().IsTextFormat (InternetMediaTypes::kHTML));
                EXPECT_TRUE (InternetMediaTypeRegistry::Get ().IsTextFormat (InternetMediaTypes::kJSON));
                EXPECT_TRUE (not InternetMediaTypeRegistry::Get ().IsTextFormat (InternetMediaTypes::kPNG));
                EXPECT_TRUE (not InternetMediaTypeRegistry::Get ().IsXMLFormat (InternetMediaType{"text/foobar"}));
                EXPECT_TRUE (InternetMediaTypeRegistry::Get ().IsXMLFormat (InternetMediaType{"text/foobar+xml"}));
            }
        }
        {
            Debug::TraceContextBumper ctx1{"InternetMediaTypeRegistry::Get ().GetMediaTypes()"};
            // enumerate all content types
            for (auto ct : InternetMediaTypeRegistry::Get ().GetMediaTypes ()) {
                DbgTrace (L"i=%s", Characters::ToString (ct).c_str ());
            }
        }
        {
            Debug::TraceContextBumper ctx1{"InternetMediaTypeRegistry - updating"};
            InternetMediaTypeRegistry origRegistry    = InternetMediaTypeRegistry::Get ();
            InternetMediaTypeRegistry updatedRegistry = origRegistry;
            const auto                kHFType_        = InternetMediaType{"application/fake-heatlthframe-phr+xml"};
            EXPECT_TRUE (not InternetMediaTypeRegistry::Get ().GetMediaTypes ().Contains (kHFType_));
            updatedRegistry.AddOverride (kHFType_, InternetMediaTypeRegistry::OverrideRecord{nullopt, Containers::Set<String>{".HPHR"}, ".HPHR"});
            InternetMediaTypeRegistry::Set (updatedRegistry);
            EXPECT_TRUE (InternetMediaTypeRegistry::Get ().IsXMLFormat (kHFType_));
            EXPECT_TRUE (InternetMediaTypeRegistry::Get ().GetMediaTypes ().Contains (kHFType_));
            EXPECT_TRUE (not origRegistry.GetMediaTypes ().Contains (kHFType_));
            EXPECT_TRUE (updatedRegistry.GetMediaTypes ().Contains (kHFType_));
        }
        {
            // Noticed mistake in InternetMediaTypeRegistry::Get ().IsA () -- as=application/x-ccrddd should not match ISA relationship with application/c-ccr
            const InternetMediaType kCCR{"application/x-ccr"sv};
            const InternetMediaType kCCR_LegitAlt{"application/x-ccr+xml"sv};
            const InternetMediaType kCCR_Typo1{"application/x-ccx"sv};
            const InternetMediaType kCCR_Typo2{"application/x-ccrPLUSEXTRASTUFFATTHEEND"sv};
            const InternetMediaType kCCR_WRONG_CASE1{"APPLICATION/x-ccr"sv};
            const InternetMediaType kCCR_WRONG_CASE2{"APPLICATION/X-CCR"sv};
            EXPECT_EQ (kCCR, kCCR_WRONG_CASE1);
            EXPECT_EQ (kCCR, kCCR_WRONG_CASE2);
            EXPECT_TRUE (InternetMediaTypeRegistry::Get ().IsA (kCCR, kCCR));
            EXPECT_TRUE (InternetMediaTypeRegistry::Get ().IsA (kCCR, kCCR_LegitAlt));
            EXPECT_TRUE (not InternetMediaTypeRegistry::Get ().IsA (kCCR, kCCR_Typo1));
            EXPECT_TRUE (not InternetMediaTypeRegistry::Get ().IsA (kCCR, kCCR_Typo2));
        }
    }
}
#endif

int main (int argc, const char* argv[])
{
    Execution::Logger::Activator logMgrActivator; // for OptionsFile test
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
