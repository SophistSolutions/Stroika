/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
//  TEST    Foundation::DataExchange::Other
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/Atom.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include "Stroika/Foundation/Streams/ExternallyOwnedMemoryInputStream.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using Execution::ModuleGetterSetter;

namespace {
    void Test1_Atom_ ()
    {
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
        struct MyData_ {
            bool               fEnabled = false;
            optional<DateTime> fLastSynchronizedAt;
        };
        OptionsFile of{
            L"MyModule",
            [] () -> ObjectVariantMapper {
                ObjectVariantMapper mapper;
                mapper.AddClass<MyData_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                    {L"Enabled", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MyData_, fEnabled)},
                    {L"Last-Synchronized-At", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MyData_, fLastSynchronizedAt)},
                });
                return mapper;
            }(),
            OptionsFile::kDefaultUpgrader,
            [] (const String& moduleName, const String& fileSuffix) -> String {
                return IO::FileSystem::WellKnownLocations::GetTemporary () + moduleName + fileSuffix;
            }};
        MyData_ m = of.Read<MyData_> (MyData_ ()); // will return default values if file not present
        of.Write (m);                              // test writing
    }
}

namespace {
    struct MyData_ {
        bool               fEnabled = false;
        optional<DateTime> fLastSynchronizedAt;
    };
    struct ModuleGetterSetter_Implementation_MyData_ {
        ModuleGetterSetter_Implementation_MyData_ ()
            : fOptionsFile_{
                  L"MyModule",
                  [] () -> ObjectVariantMapper {
                      ObjectVariantMapper mapper;
                      mapper.AddClass<MyData_> (initializer_list<ObjectVariantMapper::StructFieldInfo>{
                          {L"Enabled", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MyData_, fEnabled)},
                          {L"Last-Synchronized-At", Stroika_Foundation_DataExchange_StructFieldMetaInfo (MyData_, fLastSynchronizedAt)},
                      });
                      return mapper;
                  }(),
                  OptionsFile::kDefaultUpgrader, [] (const String& moduleName, const String& fileSuffix) -> String {
                // for regression tests write to /tmp
                return  IO::FileSystem::WellKnownLocations::GetTemporary () + moduleName + fileSuffix; }}
            , fActualCurrentConfigData_ (fOptionsFile_.Read<MyData_> (MyData_ ()))
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
        if (sModuleConfiguration_.Get ().fEnabled) {
            auto n = sModuleConfiguration_.Get ();
            sModuleConfiguration_.Set (n);
        }
    }
}

namespace Test4_VariantValue_ {
    void RunTests ()
    {
        Containers::Collection<VariantValue> vc;
        VariantValue                         vv{vc};
    }
}

namespace {
    namespace Test5_InternetMediaType_ {
        void RunTests ()
        {
            {
                InternetMediaType ct1{L"text/plain;charset=ascii"};
                VerifyTestResult ((ct1.GetParameters () == Containers::Mapping{Common::KeyValuePair<String, String>{L"charset", L"ascii"}}));

                InternetMediaType ct2{L"text/plain; charset = ascii"};
                VerifyTestResult (ct1 == ct2);

                InternetMediaType ct3{L"text/plain; charset = \"ascii\""};
                VerifyTestResult (ct1 == ct3);

                InternetMediaType ct4{L"text/plain; charset = \"ASCII\""}; // case insensitive compare key, but not value
                VerifyTestResult (ct1 != ct4);
            }
            {
                // Example from https://tools.ietf.org/html/rfc2045#page-10 - comments ignored, and quotes on value
                InternetMediaType ct1{L"text/plain; charset=us-ascii (Plain text)"};
                InternetMediaType ct2{L"text/plain; charset=\"us-ascii\""};
                VerifyTestResult (ct1 == ct2);
                VerifyTestResult (ct1.IsTextFormat ());
            }
            {
                InternetMediaTypeRegistry r = InternetMediaTypeRegistry::Default ();
                DbgTrace (L"SUFFIX(JSON)=%s", Characters::ToString (r.GetPreferredAssociatedFileSuffix (InternetMediaTypes::kJSON)).c_str ());
                DbgTrace (L"MOREGEN(JSON)=%s", Characters::ToString (r.GetMoreGeneralTypes (InternetMediaTypes::kJSON)).c_str ());
                DbgTrace (L"MORESPECIFIC(JSON)=%s", Characters::ToString (r.GetMoreSpecificTypes (InternetMediaTypes::kJSON)).c_str ());
                DbgTrace (L"ASSOCFILESUFFIXES(JSON)=%s", Characters::ToString (r.GetAssociatedFileSuffixes (InternetMediaTypes::kJSON)).c_str ());
                DbgTrace (L"GetAssociatedPrettyName(JSON)=%s", Characters::ToString (r.GetAssociatedPrettyName (InternetMediaTypes::kJSON)).c_str ());
            }
            {
                InternetMediaTypeRegistry r = InternetMediaTypeRegistry::Default ();
                DbgTrace (L"SUFFIX(PNG)=%s", Characters::ToString (r.GetPreferredAssociatedFileSuffix (InternetMediaTypes::kImage_PNG)).c_str ());
                DbgTrace (L"MOREGEN(PNG)=%s", Characters::ToString (r.GetMoreGeneralTypes (InternetMediaTypes::kImage_PNG)).c_str ());
                DbgTrace (L"MORESPECIFIC(PNG)=%s", Characters::ToString (r.GetMoreSpecificTypes (InternetMediaTypes::kImage_PNG)).c_str ());
                DbgTrace (L"ASSOCFILESUFFIXES(PNG)=%s", Characters::ToString (r.GetAssociatedFileSuffixes (InternetMediaTypes::kImage_PNG)).c_str ());
                DbgTrace (L"GetAssociatedPrettyName(PNG)=%s", Characters::ToString (r.GetAssociatedPrettyName (InternetMediaTypes::kImage_PNG)).c_str ());
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
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
