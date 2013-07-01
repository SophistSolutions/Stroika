/*
 * Copyright(c) Records For Living, Inc. 2004-2012.  All rights reserved
 */
//  TEST    Foundation::DataExchangeFormat::ObjectVariantMapper
#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Foundation/Configuration/Locale.h"
#include    "Stroika/Foundation/DataExchangeFormat/BadFormatException.h"
#include    "Stroika/Foundation/DataExchangeFormat/ObjectVariantMapper.h"
#include    "Stroika/Foundation/DataExchangeFormat/JSON/Reader.h"
#include    "Stroika/Foundation/DataExchangeFormat/JSON/Writer.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Memory/VariantValue.h"
#include    "Stroika/Foundation/Streams/BasicBinaryInputOutputStream.h"
#include    "Stroika/Foundation/Math/Common.h"
#include    "Stroika/Foundation/Time/DateTime.h"

#include    "../TestHarness/TestHarness.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchangeFormat;

using   Memory::Byte;
using   Memory::VariantValue;
using   Time::DateTime;





namespace   {
    void    DoRegressionTests_SimpleRegisterEtc_1_ ()
    {
    }
}

namespace   {
    void    DoRegressionTests_SimpleMapToFromJSON_2_ ()
    {
        struct SharedContactsConfig_ {
            bool                    fEnabled;
            DateTime                fLastSynchronizedAt;
            Mapping<String, String>  fThisPHRsIDToSharedContactID;

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
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<StructureFieldInfo> ( {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fEnabled, L"Enabled"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fLastSynchronizedAt, L"Last-Synchronized-At"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fThisPHRsIDToSharedContactID, L"This-HR-ContactID-To-SharedContactID-Map"),
        }));
#else
        DataExchangeFormat::ObjectVariantMapper::StructureFieldInfo kInfo[] = {
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fEnabled, L"Enabled"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fLastSynchronizedAt, L"Last-Synchronized-At"),
            ObjectVariantMapper_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fThisPHRsIDToSharedContactID, L"This-HR-ContactID-To-SharedContactID-Map"),
        };
        mapper.RegisterClass<SharedContactsConfig_> (Sequence<DataExchangeFormat::ObjectVariantMapper::StructureFieldInfo> (std::begin (kInfo), std::end (kInfo)));
#endif

        bool newEnabled = true;
        SharedContactsConfig_   tmp;
        tmp.fEnabled = newEnabled;
        VariantValue v = mapper.Serialize  (tmp);
        tmp.fThisPHRsIDToSharedContactID.Add (L"A", L"B");
        tmp.fLastSynchronizedAt = DateTime (Time::Date (Time::Year (1998), Time::MonthOfYear::eApril, Time::DayOfMonth::e11), Time::TimeOfDay::Parse (L"3pm", locale::classic ()));

        // at this point - we should have VariantValue object with "Enabled" field.
        // This can then be serialized using

        Streams::BasicBinaryInputOutputStream   tmpStream;
        DataExchangeFormat::JSON::PrettyPrint (v, tmpStream);

        // THEN deserialized, and mapped back to C++ object form
        SharedContactsConfig_    tmp2 = mapper.Deserialize<SharedContactsConfig_> (DataExchangeFormat::JSON::Reader   (tmpStream));
        VerifyTestResult (tmp2 == tmp);
    }
}


namespace   {

    void    DoRegressionTests_ ()
    {
        DoRegressionTests_SimpleRegisterEtc_1_ ();
        DoRegressionTests_SimpleMapToFromJSON_2_ ();
    }
}




int     main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
    return EXIT_SUCCESS;
}

