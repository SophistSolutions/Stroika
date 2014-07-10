/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Execution/Logger.h"
#include    "../IO/FileSystem/BinaryFileInputStream.h"
#include    "../IO/FileSystem/BinaryFileOutputStream.h"
#include    "../IO/FileSystem/PathName.h"
#include    "../IO/FileSystem/ThroughTmpFileWriter.h"
#include    "../IO/FileSystem/WellKnownLocations.h"

#include    "../Streams/BasicBinaryInputStream.h"
#include    "../Streams/BasicBinaryOutputStream.h"

#include    "JSON/Reader.h"
#include    "JSON/Writer.h"
#include    "XML/Reader.h"
#include    "XML/Writer.h"
#include    "VariantValue.h"

#include    "OptionsFile.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::Streams;

using   Memory::BLOB;



/*
 ********************************************************************************
 ************************** DataExchange::OptionsFile ***************************
 ********************************************************************************
 */
const   OptionsFile::ModuleDataUpgraderType  OptionsFile::kDefaultUpgrader = [] (const Memory::Optional<Configuration::Version>& version, const VariantValue& rawVariantValue) -> VariantValue {
    return rawVariantValue;
};

const   OptionsFile::LoggerType OptionsFile::kDefaultLogger =
    [] (Execution::Logger::Priority priority, const String& message)
{
    using   Execution::Logger;
    Logger::Log (priority, L"%s", message.c_str ());
};

const   OptionsFile::ModuleNameToFileNameMapperType  OptionsFile::mkFilenameMapper (const String& appName)
{
    String useAppName = appName;
    return
    [useAppName] (const String & moduleName, const String & fileSuffix) -> String {
        return IO::FileSystem::WellKnownLocations::GetApplicationData() + useAppName + String (IO::FileSystem::kPathComponentSeperator) + moduleName + fileSuffix;
    }
    ;
}

// Consider using XML by default when more mature
const   Reader  OptionsFile::kDefaultReader =   JSON::Reader ();
const   Writer  OptionsFile::kDefaultWriter =   JSON::Writer ();

OptionsFile::OptionsFile (
    const String& modName,
    const ObjectVariantMapper& mapper,
    ModuleDataUpgraderType moduleUpgrader,
    ModuleNameToFileNameMapperType moduleNameToFileNameMapper,
    LoggerType logger,
    Reader reader,
    Writer writer
)
    : OptionsFile (modName, mapper, moduleUpgrader, moduleNameToFileNameMapper, logger, reader, writer, reader.GetDefaultFileSuffix ())
{
}

OptionsFile::OptionsFile (
    const String& modName,
    const ObjectVariantMapper& mapper,
    ModuleDataUpgraderType moduleUpgrader,
    ModuleNameToFileNameMapperType moduleNameToFileNameMapper,
    LoggerType logger,
    Reader reader,
    Writer writer,
    const String& fileSuffix
)
    : fModuleName_ (modName)
    , fMapper_ (mapper)
    , fModuleDataUpgrader_ (moduleUpgrader)
    , fModuleNameToFileNameMapper_ (moduleNameToFileNameMapper)
    , fLogger_ (logger)
    , fReader_ (reader)
    , fWriter_ (writer)
    , fFileSuffix_ (fileSuffix)
{
}

BLOB    OptionsFile::ReadRaw () const
{
    return IO::FileSystem::BinaryFileInputStream (GetFilePath_ ()).ReadAll ();
}

void    OptionsFile::WriteRaw (const BLOB& blob)
{
    try {
        IO::FileSystem::ThroughTmpFileWriter    tmpFile (GetFilePath_ ());
        IO::FileSystem::BinaryFileOutputStream  outStream (tmpFile.GetFilePath ());
        outStream.Write (blob);
        outStream.Flush();
        outStream.clear ();
        tmpFile.Commit ();
    }
    catch (...) {
        fLogger_ (Execution::Logger::Priority::eError, Characters::Format (L"Failed to write file: %s", GetFilePath_ ().c_str ()));
    }
}

template    <>
Optional<VariantValue>  OptionsFile::Read ()
{
    try {
        Optional<VariantValue>  r   =   fReader_.Read (BasicBinaryInputStream (ReadRaw ()));
        if (r.IsPresent ()) {
            // @todo see module todo about better handling input version#s
            *r = fModuleDataUpgrader_ (Memory::Optional<Configuration::Version> (), *r);
        }
        return r;
    }
    catch (...) {
        // @todo - check different exception cases and for some - like file not found - just no warning...
        fLogger_ (Execution::Logger::Priority::eWarning, Characters::Format (L"Failed to read file: %s", GetFilePath_ ().c_str ()));
        return Optional<VariantValue> ();
    }
}

template    <>
void        OptionsFile::Write (const VariantValue& optionsObject)
{
    BasicBinaryOutputStream tmp;
    fWriter_.Write (optionsObject, tmp);
    WriteRaw (tmp.As<BLOB> ());
}

String  OptionsFile::GetFilePath_ () const
{
    return fModuleNameToFileNameMapper_(fModuleName_, fFileSuffix_);
}
