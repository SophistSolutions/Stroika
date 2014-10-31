/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Containers/Sequence.h"
#include    "../../../Foundation/Containers/Set.h"
#include    "../../../Foundation/Debug/Assertions.h"
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





///
//tmhack ti L move delimverldinear...
#include    "../../../Foundation/Streams/TextInputStreamBinaryAdapter.h"


namespace {

    struct DelimitedLinereader_  {

        DelimitedLinereader_ (Set<Character> columnDelimiters = { ' ', '\t' } )
        {
        }


        Iterable<Sequence<String>>  ReadAs2DArray (const Streams::BinaryInputStream& in) const
        {
            Sequence<Sequence<String>>  result;
            for (String line : Streams::TextInputStreamBinaryAdapter (in).ReadLines ()) {
                Sequence<String>    tokens  { line.Tokenize (Set<Character> { ' ', ':' }) };
            }
            return result;
        }

        // assumes 2D-array input, and creates key-value pairs
        Mapping<String, String>  ReadAsMapping (const Streams::BinaryInputStream& in) const
        {
            //tmphack
            return Mapping<String, String> ();
        }

    };

}




namespace {
    Instruments::Memory::Info capture_ ()
    {
        Instruments::Memory::Info   result;
#if     qPlatform_Windows
#elif   qPlatform_POSIX
        //INI::Profile p = DataExchange::INI::Reader ().ReadProfile (IO::FileSystem::BinaryFileInputStream (L"/var/lib/connman/" + interfaceID + L"/settings"));
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
