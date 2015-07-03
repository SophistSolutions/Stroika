/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Execution/Logger.h"
#include    "../IO/FileSystem/BinaryFileInputStream.h"
#include    "../IO/FileSystem/BinaryFileOutputStream.h"
#include    "../IO/FileSystem/PathName.h"
#include    "../IO/FileSystem/ThroughTmpFileWriter.h"
#include    "../IO/FileSystem/WellKnownLocations.h"
#include    "../Streams/MemoryStream.h"

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
 ***************** DataExchange::OptionsFile::LoggerMessage *********************
 ********************************************************************************
 */
OptionsFile::LoggerMessage::LoggerMessage (Msg msg, String fn)
    : fMsg (msg)
    , fFileName (fn)
{
}

String  OptionsFile::LoggerMessage::FormatMessage () const
{
    switch (fMsg) {
        case Msg::eFailedToWriteFile:
            return Characters::Format (L"Failed to write file: %s", fFileName.Value ().c_str ());
        case Msg::eFailedToReadFile:
            return Characters::Format (L"Failed to read file: %s", fFileName.Value ().c_str ());
        case Msg::eFailedToParseReadFile:
            return Characters::Format (L"Error analyzing configuration file '%s' - using defaults.", fFileName.Value ().c_str ());
        case Msg::eFailedToParseReadFileBadFormat:
            return Characters::Format (L"Error analyzing configuration file (because bad format) '%s' - using defaults.", fFileName.Value ().c_str ());
        case Msg::eFailedToCompareReadFile:
            return Characters::Format (L"Failed to compare configuration file: %s", fFileName.Value ().c_str ());
        case Msg::eWritingConfigFile_SoDefaultsEditable:
            return Characters::Format (L"Writing configuration file '%s' because not found (and so defaults are more easily seen and editable).", fFileName.Value ().c_str ());
        case Msg::eWritingConfigFile_BecauseUpgraded:
            return Characters::Format (L"Writing configuration file '%s' in a new location because the software has been upgraded.", fFileName.Value ().c_str ());
        case Msg::eWritingConfigFile_BecauseSomethingChanged:
            return Characters::Format (L"Writing configuration file '%s' because something changed (e.g. a default, or field added/removed).", fFileName.Value ().c_str ());
        case Msg::eFailedToWriteInUseValues:
            return Characters::Format (L"Failed to write default (in use) values to file: %s", fFileName.Value ().c_str ());
        default:
            RequireNotReached ();
            return String ();
    }
}





/*
 ********************************************************************************
 ************************** DataExchange::OptionsFile ***************************
 ********************************************************************************
 */
const   OptionsFile::ModuleDataUpgraderType  OptionsFile::kDefaultUpgrader = [] (const Memory::Optional<Configuration::Version>& version, const VariantValue& rawVariantValue) -> VariantValue {
    return rawVariantValue;
};

const   OptionsFile::LoggerType OptionsFile::kDefaultLogger =
    [] (const LoggerMessage& message)
{
    using   Execution::Logger;
    Logger::Priority priority = Logger::Priority::eError;
    using Msg = OptionsFile::LoggerMessage::Msg;
    switch (message.fMsg) {
        case Msg::eFailedToReadFile:
            priority = Logger::Priority::eWarning;   // could be just because new system, no file
            break;
        case Msg::eWritingConfigFile_SoDefaultsEditable:
        case Msg::eWritingConfigFile_BecauseUpgraded:
        case Msg::eWritingConfigFile_BecauseSomethingChanged:
            priority = Logger::Priority::eInfo;
            break;

        case Msg::eFailedToParseReadFile:
        case Msg::eFailedToParseReadFileBadFormat:
            // Most likely very bad - as critical configuration data will be lost, and overwritten with 'defaults'
            priority = Logger::Priority::eCriticalError;
            break;
    }
    Logger::Log (priority, L"%s", message.FormatMessage ().c_str ());
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

const   OptionsFile::ModuleNameToFileVersionMapperType  OptionsFile::kDefaultModuleNameToFileVersionMapper = [] (const String& moduleName) -> Optional<Configuration::Version> {
    return Optional<Configuration::Version> (); // default to dont know
};



// Consider using XML by default when more mature
const   Reader  OptionsFile::kDefaultReader =   JSON::Reader ();
const   Writer  OptionsFile::kDefaultWriter =   JSON::Writer ();






OptionsFile::OptionsFile (
    const String& modName,
    const ObjectVariantMapper& mapper,
    ModuleDataUpgraderType moduleUpgrader,
    ModuleNameToFileNameMapperType moduleNameToFileNameMapper,
    ModuleNameToFileVersionMapperType moduleNameToReadFileVersion,
    LoggerType logger,
    Reader reader,
    Writer writer
)
    : OptionsFile (modName, mapper, moduleUpgrader, moduleNameToFileNameMapper, moduleNameToFileNameMapper, moduleNameToReadFileVersion, logger, reader, writer, reader.GetDefaultFileSuffix ())
{
}

OptionsFile::OptionsFile (
    const String& modName,
    const ObjectVariantMapper& mapper,
    ModuleDataUpgraderType moduleUpgrader,
    ModuleNameToFileNameMapperType moduleNameToReadFileNameMapper,
    ModuleNameToFileNameMapperType moduleNameToWriteFileNameMapper,
    ModuleNameToFileVersionMapperType moduleNameToReadFileVersion,
    LoggerType logger,
    Reader reader,
    Writer writer
)
    : OptionsFile (modName, mapper, moduleUpgrader, moduleNameToReadFileNameMapper, moduleNameToWriteFileNameMapper, moduleNameToReadFileVersion, logger, reader, writer, reader.GetDefaultFileSuffix ())
{
}

OptionsFile::OptionsFile (
    const String& modName,
    const ObjectVariantMapper& mapper,
    ModuleDataUpgraderType moduleUpgrader,
    ModuleNameToFileNameMapperType moduleNameToReadFileNameMapper,
    ModuleNameToFileNameMapperType moduleNameToWriteFileNameMapper,
    ModuleNameToFileVersionMapperType moduleNameToReadFileVersion,
    LoggerType logger,
    Reader reader,
    Writer writer,
    const String& fileSuffix
)
    : fModuleName_ (modName)
    , fMapper_ (mapper)
    , fModuleDataUpgrader_ (moduleUpgrader)
    , fModuleNameToReadFileNameMapper_ (moduleNameToReadFileNameMapper)
    , fModuleNameToWriteFileNameMapper_ (moduleNameToWriteFileNameMapper)
    , fModuleNameToFileVersionMapper_ (moduleNameToReadFileVersion)
    , fLogger_ (logger)
    , fReader_ (reader)
    , fWriter_ (writer)
    , fFileSuffix_ (fileSuffix)
{
}

BLOB    OptionsFile::ReadRaw () const
{
    return IO::FileSystem::BinaryFileInputStream::mk (GetReadFilePath_ ()).ReadAll ();
}

void    OptionsFile::WriteRaw (const BLOB& blob)
{
    try {
        IO::FileSystem::ThroughTmpFileWriter    tmpFile (GetWriteFilePath_ ());
        IO::FileSystem::BinaryFileOutputStream  outStream (tmpFile.GetFilePath ());
        outStream.Write (blob);
        outStream.Flush ();
        outStream.clear ();     // so any errors can be displayed as exceptions, and so closed before commit/rename
        tmpFile.Commit ();
    }
    catch (...) {
        fLogger_ (LoggerMessage (LoggerMessage::Msg::eFailedToWriteFile, GetWriteFilePath_ ()));
    }
}

template    <>
Optional<VariantValue>  OptionsFile::Read ()
{
    try {
        Optional<VariantValue>  r   =   fReader_.Read (MemoryStream<Byte> (ReadRaw ()));
        if (r.IsPresent ()) {
            r = fModuleDataUpgrader_ (fModuleNameToFileVersionMapper_ (fModuleName_), *r);
        }
        return r;
    }
    catch (...) {
        // @todo - check different exception cases and for some - like file not found - just no warning...
        fLogger_ (LoggerMessage (LoggerMessage::Msg::eFailedToReadFile, GetReadFilePath_ ()));
        return Optional<VariantValue> ();
    }
}

template    <>
void        OptionsFile::Write (const VariantValue& optionsObject)
{
    MemoryStream<Byte> tmp;
    fWriter_.Write (optionsObject, tmp);
    WriteRaw (tmp.As<BLOB> ());
}

String  OptionsFile::GetReadFilePath_ () const
{
    return fModuleNameToReadFileNameMapper_(fModuleName_, fFileSuffix_);
}

String  OptionsFile::GetWriteFilePath_ () const
{
    return fModuleNameToWriteFileNameMapper_(fModuleName_, fFileSuffix_);
}
