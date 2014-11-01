/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Float.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Containers/Sequence.h"
#include    "../../../Foundation/Containers/Set.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Debug/Trace.h"
#include    "../../../Foundation/DataExchange/CharacterDelimitedLines/Reader.h"
#include    "../../../Foundation/IO/FileSystem/BinaryFileInputStream.h"
#include    "../../../Foundation/Streams/BinaryInputStream.h"

#include    "../CommonMeasurementTypes.h"

#include    "Memory.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;


using   Characters::Character;
using   Characters::String_Constant;
using   Containers::Mapping;
using   Containers::Sequence;
using   Containers::Set;



// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1




namespace {
    template <typename T>
    void    ReadX_ (Optional<T>* result, const String& n, const Sequence<String>& line)
    {
        if (line.size () >= 3 and line[0] == n) {
            String  unit = line[2];
            double  factor = (unit == L"kB") ? 1024 : 1;
            *result = static_cast<T> (round (Characters::String2Float<double> (line[1]) * factor));
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"Set %s = %ld", n.c_str (), static_cast<long> (**result));
#endif
        }
    }
    Instruments::Memory::Info capture_ ()
    {
        Instruments::Memory::Info   result;
#if     qPlatform_POSIX
        DataExchange::CharacterDelimitedLines::Reader reader {{ ':', ' ', '\t' }};
        const   String_Constant kProcMemInfoFileName_ { L"/proc/meminfo" };
        //const String_Constant kProcMemInfoFileName_ { L"c:\\Sandbox\\VMSharedFolder\\meminfo" };
        for (Sequence<String> line : reader.ReadAs2DArray (IO::FileSystem::BinaryFileInputStream (kProcMemInfoFileName_))) {
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"***in Instruments::Memory::Info capture_ linesize=%d, line[0]=%s", line.size(), line.empty ()? L"" : line[0].c_str ());
#endif
            ReadX_ (&result.fFreePhysicalMemory, String_Constant (L"MemFree"), line);
            ReadX_ (&result.fTotalVirtualMemory, String_Constant (L"VmallocTotal"), line);
            ReadX_ (&result.fUsedVirtualMemory, String_Constant (L"VmallocUsed"), line);
            ReadX_ (&result.fLargestAvailableVirtualChunk, String_Constant (L"VmallocChunk"), line);
        }
#elif   qPlatform_Windows
#endif
        return result;
    }
}




const   MeasurementType Instruments::Memory::kSystemMemoryMeasurement = MeasurementType (String_Constant (L"System-Memory"));




/*
 ********************************************************************************
 ******************** Instruments::Memory::GetObjectVariantMapper ***************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::Memory::GetObjectVariantMapper ()
{
    using   StructureFieldInfo = ObjectVariantMapper::StructureFieldInfo;
    ObjectVariantMapper sMapper_ = [] () -> ObjectVariantMapper {
        ObjectVariantMapper mapper;
        mapper.AddCommonType<Optional<uint64_t>> ();
        mapper.AddCommonType<Optional<double>> ();
        DISABLE_COMPILER_CLANG_WARNING_START("clang diagnostic ignored \"-Winvalid-offsetof\"");   // Really probably an issue, but not to debug here -- LGP 2014-01-04
        DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"-Winvalid-offsetof\"");       // Really probably an issue, but not to debug here -- LGP 2014-01-04
        mapper.AddClass<Info> (initializer_list<StructureFieldInfo> {
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fFreePhysicalMemory), String_Constant (L"Free-Physical-Memory") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fTotalVirtualMemory), String_Constant (L"Total-Virtual-Memory") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fUsedVirtualMemory), String_Constant (L"UsedV-irtual-Memory") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fLargestAvailableVirtualChunk), String_Constant (L"Largest-Available-Virtual-Chunk") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fMajorPageFaultsSinceBoot), String_Constant (L"Major-Page-Faults-Since-Boot") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fMinorPageFaultsSinceBoot), String_Constant (L"Minor-Page-Faults-Since-Boot") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fMajorPageFaultsPerSecond), String_Constant (L"Major-Page-Faults-Per-Second") },
            { Stroika_Foundation_DataExchange_ObjectVariantMapper_FieldInfoKey (Info, fMinorPageFaultsPerSecond), String_Constant (L"Minor-Page-Faults-Per-Second") },
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
        DateTime    before = DateTime::Now ();
        Instruments::Memory::Info rawMeasurement = capture_ ();
        results.fMeasuredAt = DateTimeRange (before, DateTime::Now ());
        Measurement m;
        m.fValue = GetObjectVariantMapper ().FromObject (rawMeasurement);
        m.fType = kSystemMemoryMeasurement;
        results.fMeasurements.Add (m);
        return results;
    },
    {kMemoryUsage}
                                          );
    return kInstrument_;
}
