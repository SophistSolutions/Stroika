/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Debug/Assertions.h"

#include    "../CommonMeasurementTypes.h"

#include    "ProcFSProcesses.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments::ProcFSProcesses;

using   Characters::String_Constant;




const   MeasurementType Instruments::ProcFSProcesses::kProcessMapMeasurement = MeasurementType (String_Constant (L"Process-Details"));



/*
 ********************************************************************************
 ************* Instruments::ProcFSProcesses::GetObjectVariantMapper *************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::ProcFSProcesses::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<Optional<String>> ();
        mapper.AddCommonType<Optional<Mapping<String, String>>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<ProcessType> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fCommandLine), String_Constant (L"Command-Line"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fCurrentWorkingDirectory), String_Constant (L"Current-Working-Directory"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fEnvironmentVariables), String_Constant (L"Environment-Variables"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fEXEPath), String_Constant (L"EXE-Path"), StructureFieldInfo::NullFieldHandling::eOmit },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (ProcessType, fRoot), String_Constant (L"Root"), StructureFieldInfo::NullFieldHandling::eOmit },
        });
        DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"-Winvalid-offsetof\"");
        DISABLE_COMPILER_CLANG_WARNING_END("clang diagnostic ignored \"-Winvalid-offsetof\"");
        mapper.AddCommonType<ProcessMapType> ();
        return mapper;
    } ();
    return sMapper_;
}



namespace {
    ProcessMapType  ExtractFromProcFS_ ()
    {
        /// Most status - like time - come from http://linux.die.net/man/5/proc
        ///proc/[pid]/stat
        //  Status information about the process. This is used by ps(1). It is defined in /usr/src/linux/fs/proc/array.c.
        //
        ProcessMapType  tmp;

        ProcessType test;
        test.fCommandLine = L"Hi mom comamndline";
        Mapping<String, String> env;
        env.Add (L"Home", L"/home/lewis");
        test.fEnvironmentVariables = env;
        tmp.Add (101, test);
        return tmp;
    }
}





/*
 ********************************************************************************
 ************* Instruments::ProcFSProcesses::GetInstrument **********************
 ********************************************************************************
 */
#if     qSupport_SystemPerformance_Instruments_ProcFSProcesses
Instrument          SystemPerformance::Instruments::ProcFSProcesses::GetInstrument (
    const Optional<Set<Fields2Capture>>& onlyCaptureFields,
    const Optional<Set<pid_t>>& restrictToPIDs,
    const Optional<Set<pid_t>>& omitPIDs,
    CachePolicy cachePolicy
)
{
    // @todo can only use static one if right options passed in...
    static  Instrument  kInstrument_    = Instrument (
            InstrumentNameType (String_Constant (L"ProcFSProcesses")),
    [] () -> MeasurementSet {
        MeasurementSet    results;
        DateTime    before = DateTime::Now ();
        auto rawMeasurement = ExtractFromProcFS_ ();
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
        Measurement m;
        m.fValue = GetObjectVariantMapper ().FromObject (rawMeasurement);
        m.fType = kProcessMapMeasurement;
        results.fMeasurements.Add (m);
        return results;
    },
    {kProcessMapMeasurement}
                                          );
    return kInstrument_;
}
#endif
