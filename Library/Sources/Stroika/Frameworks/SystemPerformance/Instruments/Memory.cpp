/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Debug/Assertions.h"

#include    "../CommonMeasurementTypes.h"

#include    "Memory.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;

using   Characters::String_Constant;








/*
 ********************************************************************************
 ********************** Instruments::Memory::GetObjectVariantMapper *************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::Memory::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
#if 0
			{ Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, f1MinuteAve), String_Constant (L"1-minute") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, f5MinuteAve), String_Constant (L"5-minute") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, f15MinuteAve), String_Constant (L"15-minute") },
#endif
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        return mapper;
    } ();
    return sMapper_;
}



/*
 ********************************************************************************
 ******************* Instruments::Memory::GetInstrument *************************
 ********************************************************************************
 */
Instrument  SystemPerformance::Instruments::Memory::GetInstrument ()
{
    static  Instrument  kInstrument_    = Instrument (
            InstrumentNameType (String_Constant (L"Memory")),
    [] () -> MeasurementSet {
        MeasurementSet    results;
//        double loadAve[3];
        DateTime    before = DateTime::Now ();
#if 0
        int result = getloadavg(loadAve, 3);
#endif
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
#if 0
        if (result == 3)
        {
            Measurement m;
            m.fValue = GetObjectVariantMapper ().FromObject (Info { loadAve[0], loadAve[1], loadAve[2] });
            m.fType = kMemory;
            results.fMeasurements.Add (m);
        }
#endif
        return results;
    },
    {kMemoryUsage}
                                          );
    return kInstrument_;
}
