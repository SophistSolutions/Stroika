/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../../Foundation/Characters/String_Constant.h"
#include    "../../../Foundation/Characters/String2Int.h"
#include    "../../../Foundation/Characters/StringBuilder.h"
#include    "../../../Foundation/Characters/Tokenize.h"
#include    "../../../Foundation/Containers/Mapping.h"
#include    "../../../Foundation/Debug/Assertions.h"
#include    "../../../Foundation/Execution/ThreadAbortException.h"
#include    "../../../Foundation/IO/FileSystem/BinaryFileInputStream.h"
#include    "../../../Foundation/IO/FileSystem/DirectoryIterable.h"
#include    "../../../Foundation/IO/FileSystem/FileSystem.h"
#include    "../../../Foundation/Memory/BLOB.h"
#include    "../../../Foundation/Memory/Optional.h"
#include    "../../../Foundation/Streams/BufferedBinaryInputStream.h"

#include    "../CommonMeasurementTypes.h"

#include    "ProcessDetails.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::SystemPerformance;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments;
using   namespace   Stroika::Frameworks::SystemPerformance::Instruments::ProcessDetails;

using   Characters::String_Constant;




const   MeasurementType Instruments::ProcessDetails::kProcessMapMeasurement = MeasurementType (String_Constant (L"Process-Details"));

//tmphack to test
//#define qUseProcFS_ 1

#ifndef     qUseProcFS_
#define     qUseProcFS_ qPlatform_POSIX
#endif



/*
 ********************************************************************************
 ************** Instruments::ProcessDetails::GetObjectVariantMapper *************
 ********************************************************************************
 */
ObjectVariantMapper Instruments::ProcessDetails::GetObjectVariantMapper ()
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
    // this reads /proc format files - meaning that a trialing nul-byte is the EOS
    String  ReadFileString_(const String& fullPath)
    {
        Memory::BLOB    b = IO::FileSystem::BinaryFileInputStream (fullPath).ReadAll ();
        const char* s = reinterpret_cast<const char*> (b.begin ());
        const char* e = s + b.size ();
        if (s < e and * (e - 1) == '\0') {
            e--;
        }
        return String::FromNarrowSDKString (s, e);
    }
    Sequence<String>  ReadFileStrings_(const String& fullPath)
    {
        Sequence<String>    results;
        Streams::BinaryInputStream   in = Streams::BufferedBinaryInputStream (IO::FileSystem::BinaryFileInputStream (fullPath));
        StringBuilder sb;
        for (Memory::Optional<Memory::Byte> b; (b = in.Read ()).IsPresent ();) {
            if (*b == '\0') {
                results.Append (sb.As<String> ());
                sb.clear();
            }
            else {
                sb.Append ((char) (*b));    // for now assume no charset
            }
        }
        return results;
    }
    Mapping<String, String>  ReadFileStringsMap_(const String& fullPath)
    {
        Mapping<String, String>    results;
        for (auto i : ReadFileStrings_ (fullPath)) {
            auto tokens = Tokenize<String> (i, L"=");
            if (tokens.size () == 2) {
                results.Add (tokens[0], tokens[1]);
            }
        }
        return results;
    }

    ProcessMapType  ExtractFromProcFS_ ()
    {
        /// Most status - like time - come from http://linux.die.net/man/5/proc
        ///proc/[pid]/stat
        //  Status information about the process. This is used by ps(1). It is defined in /usr/src/linux/fs/proc/array.c.
        //
        ProcessMapType  tmp;

#if     qUseProcFS_
        for (String dir : IO::FileSystem::DirectoryIterable (L"/proc")) {
            bool isAllNumeric = dir.FindFirstThat ([] (Character c) -> bool { return not c.IsDigit (); });
            pid_t pid = String2Int<pid_t> (dir);
            ProcessType processDetails;
            IgnoreExceptionsExceptThreadAbortForCall (processDetails.fCommandLine = ReadFileString_ (L"/proc/" + dir + L"/cmdline"));
            IgnoreExceptionsExceptThreadAbortForCall (processDetails.fCurrentWorkingDirectory = IO::FileSystem::FileSystem::Default ().ResolveShortcut (L"/proc/" + dir + L"/cwd"));
            IgnoreExceptionsExceptThreadAbortForCall (processDetails.fEnvironmentVariables = ReadFileStringsMap_ (L"/proc/" + dir + L"/environ"));
            IgnoreExceptionsExceptThreadAbortForCall (processDetails.fEXEPath = IO::FileSystem::FileSystem::Default ().ResolveShortcut (L"/proc/" + dir + L"/exe"));
            tmp.Add (pid, processDetails);
        }
#else
        ProcessType test;
        test.fCommandLine = L"Hi mom comamndline";
        Mapping<String, String> env;
        env.Add (L"Home", L"/home/lewis");
        test.fEnvironmentVariables = env;
        tmp.Add (101, test);
#endif
        return tmp;
    }
}





/*
 ********************************************************************************
 ************* Instruments::ProcessDetails::GetInstrument **********************
 ********************************************************************************
 */
Instrument          SystemPerformance::Instruments::ProcessDetails::GetInstrument (
    const Optional<Set<Fields2Capture>>& onlyCaptureFields,
    const Optional<Set<pid_t>>& restrictToPIDs,
    const Optional<Set<pid_t>>& omitPIDs,
    CachePolicy cachePolicy
)
{
    // @todo can only use static one if right options passed in...
    static  Instrument  kInstrument_    = Instrument (
            InstrumentNameType (String_Constant (L"ProcessDetails")),
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
