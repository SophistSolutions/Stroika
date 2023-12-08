/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::DataExchange::Other
#include "Stroika/Foundation/StroikaPreComp.h"

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
#include "Stroika/Foundation/Streams/ExternallyOwnedMemoryInputStream.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using Execution::ModuleGetterSetter;
using Traversal::Iterable;

namespace {
    void Test1_Atom_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test1_Atom_"};
        {
            Atom<> a = L"d";
            Atom<> b = L"d";
            VerifyTestResult (a == b);
            VerifyTestResult (a.GetPrintName () == L"d");
            VerifyTestResult (a.As<String> () == L"d");
            VerifyTestResult (a.As<wstring> () == L"d");
            VerifyTestResult (not a.empty ());
        }
        {
            VerifyTestResult (Atom<> ().empty ());
        }
        {
            Atom<> a = L"d";
            Atom<> b = L"e";
            VerifyTestResult (a != b);
            VerifyTestResult (not a.empty ());
            Atom<> c = a;
            VerifyTestResult (c == a);
        }
    }
}

namespace {
    void Test2_OptionsFile_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test2_OptionsFile_"};
        struct MyData_ {
            bool               fEnabled = false;
            optional<DateTime> fLastSynchronizedAt;
        };
        OptionsFile of{L"MyModule",
                       [] () -> ObjectVariantMapper {
                           ObjectVariantMapper mapper;
                           mapper.AddClass<MyData_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                               {L"Enabled", StructFieldMetaInfo{&MyData_::fEnabled}},
                               {L"Last-Synchronized-At", StructFieldMetaInfo{&MyData_::fLastSynchronizedAt}},
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
            : fOptionsFile_{L"MyModule",
                            [] () -> ObjectVariantMapper {
                                ObjectVariantMapper mapper;
                                mapper.AddClass<MyData_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                                    {L"Enabled", StructFieldMetaInfo{&MyData_::fEnabled}},
                                    {L"Last-Synchronized-At", StructFieldMetaInfo{&MyData_::fLastSynchronizedAt}},
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

    void Test3_ModuleGetterSetter_ ()
    {
        Debug::TraceContextBumper ctx{"{}::Test3_ModuleGetterSetter_"};
        if (sModuleConfiguration_.Get ().fEnabled) {
            auto n = sModuleConfiguration_.Get ();
            sModuleConfiguration_.Set (n);
        }
    }
}

namespace Test4_VariantValue_ {
    void RunTests ()
    {
        Debug::TraceContextBumper            ctx{"{}::Test4_VariantValue_"};
        Containers::Collection<VariantValue> vc;
        VariantValue                         vv{vc};
    }
}

namespace {
    namespace Test5_InternetMediaType_ {
        void RunTests ()
        {
            Debug::TraceContextBumper ctx{"{}::Test5_InternetMediaType_"};
            {
                InternetMediaType ct0{"text/plain"};
                VerifyTestResult (ct0.GetType () == "text");
                VerifyTestResult (ct0.GetSubType () == "plain");
                VerifyTestResult (ct0.GetSuffix () == nullopt);

                InternetMediaType ct1{L"text/plain;charset=ascii"};
                VerifyTestResult ((ct1.GetParameters () == Containers::Mapping{Common::KeyValuePair<String, String>{"charset", "ascii"}}));
                VerifyTestResult (ct1.GetSuffix () == nullopt);

                InternetMediaType ct2{"text/plain; charset = ascii"};
                VerifyTestResult (ct1 == ct2);

                InternetMediaType ct3{"text/plain; charset = \"ascii\""};
                VerifyTestResult (ct1 == ct3);

                InternetMediaType ct4{"text/plain; charset = \"ASCII\""}; // case insensitive compare key, but not value
                VerifyTestResult (ct1 != ct4);

                InternetMediaType ct5{"application/vnd.ms-excel"};
                VerifyTestResult (ct5.GetType () == "application");
                VerifyTestResult (ct5.GetSubType () == "vnd.ms-excel");
                VerifyTestResult (ct5.GetSuffix () == nullopt);

                InternetMediaType ct6{"application/mathml+xml"};
                VerifyTestResult (ct6.GetType () == "application");
                VerifyTestResult (ct6.GetSubType () == "mathml");
                VerifyTestResult (ct6.GetSuffix () == "xml");
                VerifyTestResult (ct6.As<wstring> () == L"application/mathml+xml");
            }
            {
                // Example from https://tools.ietf.org/html/rfc2045#page-10 - comments ignored, and quotes on value
                InternetMediaType ct1{"text/plain; charset=us-ascii (Plain text)"};
                InternetMediaType ct2{"text/plain; charset=\"us-ascii\""};
                VerifyTestResult (ct1 == ct2);
                VerifyTestResult (InternetMediaTypeRegistry::Get ().IsTextFormat (ct1));
            }
            {
                auto dumpCT = [] ([[maybe_unused]] const String& label, InternetMediaType i) {
                    [[maybe_unused]] InternetMediaTypeRegistry r = InternetMediaTypeRegistry::Get ();
                    DbgTrace (L"SUFFIX(%s)=%s", label.As<wstring> ().c_str (), Characters::ToString (r.GetPreferredAssociatedFileSuffix (i)).c_str ());
                    DbgTrace (L"ASSOCFILESUFFIXES(%s)=%s", label.As<wstring> ().c_str (),
                              Characters::ToString (r.GetAssociatedFileSuffixes (i)).c_str ());
                    DbgTrace (L"GetAssociatedPrettyName(%s)=%s", label.As<wstring> ().c_str (),
                              Characters::ToString (r.GetAssociatedPrettyName (i)).c_str ());
                };
                auto checkCT = [] (InternetMediaType i, const Set<String>& possibleFileSuffixes) {
                    [[maybe_unused]] InternetMediaTypeRegistry r = InternetMediaTypeRegistry::Get ();
                    using namespace Characters;
                    if (not possibleFileSuffixes.Contains (r.GetPreferredAssociatedFileSuffix (i).value_or (L""))) {
                        Stroika::TestHarness::WarnTestIssue (Format (L"File suffix mismatch for %s: got %s, expected %s", ToString (i).c_str (),
                                                                     ToString (r.GetPreferredAssociatedFileSuffix (i)).c_str (),
                                                                     ToString (possibleFileSuffixes).c_str ())
                                                                 .c_str ());
                    }
                    if (not possibleFileSuffixes.Any ([&] (String suffix) -> bool { return r.GetAssociatedContentType (suffix) == i; })) {
                        Stroika::TestHarness::WarnTestIssue (Format (L"GetAssociatedContentType for fileSuffixes %s (expected %s, got %s)",
                                                                     ToString (possibleFileSuffixes).c_str (), ToString (i).c_str (),
                                                                     ToString (possibleFileSuffixes
                                                                                   .Map5<Iterable<InternetMediaType>> ([&] (String suffix) {
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
                    VerifyTestResult (InternetMediaTypeRegistry::Get ().IsImageFormat (InternetMediaTypes::kPNG));
                    VerifyTestResult (not InternetMediaTypeRegistry::Get ().IsImageFormat (InternetMediaTypes::kJSON));
                    VerifyTestResult (InternetMediaTypeRegistry::Get ().IsXMLFormat (InternetMediaTypes::kXML));
                    VerifyTestResult (not InternetMediaTypeRegistry::Get ().IsXMLFormat (InternetMediaTypes::kText_PLAIN));
                    VerifyTestResult (InternetMediaTypeRegistry::Get ().IsTextFormat (InternetMediaTypes::kText_PLAIN));
                    VerifyTestResult (InternetMediaTypeRegistry::Get ().IsTextFormat (InternetMediaTypes::kXML));
                    VerifyTestResult (InternetMediaTypeRegistry::Get ().IsTextFormat (InternetMediaTypes::kHTML));
                    VerifyTestResult (InternetMediaTypeRegistry::Get ().IsTextFormat (InternetMediaTypes::kJSON));
                    VerifyTestResult (not InternetMediaTypeRegistry::Get ().IsTextFormat (InternetMediaTypes::kPNG));
                    VerifyTestResult (not InternetMediaTypeRegistry::Get ().IsXMLFormat (InternetMediaType{"text/foobar"}));
                    VerifyTestResult (InternetMediaTypeRegistry::Get ().IsXMLFormat (InternetMediaType{"text/foobar+xml"}));
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
                VerifyTestResult (not InternetMediaTypeRegistry::Get ().GetMediaTypes ().Contains (kHFType_));
                updatedRegistry.AddOverride (kHFType_, InternetMediaTypeRegistry::OverrideRecord{nullopt, Containers::Set<String>{".HPHR"}, ".HPHR"});
                InternetMediaTypeRegistry::Set (updatedRegistry);
                VerifyTestResult (InternetMediaTypeRegistry::Get ().IsXMLFormat (kHFType_));
                VerifyTestResult (InternetMediaTypeRegistry::Get ().GetMediaTypes ().Contains (kHFType_));
                VerifyTestResult (not origRegistry.GetMediaTypes ().Contains (kHFType_));
                VerifyTestResult (updatedRegistry.GetMediaTypes ().Contains (kHFType_));
            }
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        Test1_Atom_ ();
        Test2_OptionsFile_ ();
        Test3_ModuleGetterSetter_ ();
        Test4_VariantValue_::RunTests ();
        Test5_InternetMediaType_::RunTests ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Execution::Logger::Activator logMgrActivator; // for OptionsFile test
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
