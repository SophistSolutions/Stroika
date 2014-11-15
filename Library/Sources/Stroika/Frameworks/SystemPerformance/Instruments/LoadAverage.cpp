/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Debug/Assertions.h"

#include    "../CommonMeasurementTypes.h"

#include    "LoadAverage.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;

using   Characters::String_Constant;



Instruments::LoadAverage::Info::Info (double oneMinuteAve, double fiveMinuteAve, double fifteenMinuteAve)
    : f1MinuteAve (oneMinuteAve)
    , f5MinuteAve (fiveMinuteAve)
    , f15MinuteAve (fifteenMinuteAve)
{
}


/*
 ********************************************************************************
 ***************** Instruments::LoadAverage::GetObjectVariantMapper *************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::LoadAverage::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, f1MinuteAve), String_Constant (L"1-minute") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, f5MinuteAve), String_Constant (L"5-minute") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, f15MinuteAve), String_Constant (L"15-minute") },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        return mapper;
    } ();
    return sMapper_;
}



/*
 ********************************************************************************
 ******************* Instruments::LoadAverage::GetInstrument ********************
 ********************************************************************************
 */
#if     qSupport_SystemPerformance_Instruments_LoadAverage
Instrument  SystemPerformance::Instruments::LoadAverage::GetInstrument ()
{
    static  Instrument  kInstrument_    = Instrument (
            InstrumentNameType (String_Constant (L"Load-Average")),
    [] () -> MeasurementSet {
        MeasurementSet    results;
        double loadAve[3];
        DateTime    before = DateTime::Now ();
        int result = getloadavg(loadAve, 3);
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
        if (result == 3)
        {
            Measurement m;
            m.fValue = GetObjectVariantMapper ().FromObject (Info { loadAve[0], loadAve[1], loadAve[2] });
            m.fType = kLoadAverage;
            results.fMeasurements.Add (m);
        }
        return results;
    },
    {kLoadAverage},
    GetObjectVariantMapper ()
                                          );
    return kInstrument_;
}



Instrument  SystemPerformance::Instruments::GetLoadAverage ()
{
    return LoadAverage::GetInstrument ();
}
#endif
