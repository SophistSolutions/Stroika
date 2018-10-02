/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"
#include "../Execution/Logger.h"
#include "../IO/FileSystem/FileInputStream.h"
#include "../IO/FileSystem/FileOutputStream.h"
#include "../IO/FileSystem/PathName.h"
#include "../IO/FileSystem/ThroughTmpFileWriter.h"
#include "../IO/FileSystem/WellKnownLocations.h"
#include "../Streams/MemoryStream.h"

#include "Variant/JSON/Reader.h"
#include "Variant/JSON/Writer.h"
#include "Variant/XML/Reader.h"
#include "Variant/XML/Writer.h"
#include "VariantValue.h"

#include "OptionsFile.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Streams;

using Memory::BLOB;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

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

String OptionsFile::LoggerMessage::FormatMessage () const
{
    switch (fMsg) {
        case Msg::eFailedToWriteFile:
            return Characters::Format (L"Failed to write file: %s", ValueOrDefault (fFileName).c_str ());
        case Msg::eFailedToReadFile:
            return Characters::Format (L"Failed to read file: %s", ValueOrDefault (fFileName).c_str ());
        case Msg::eFailedToParseReadFile:
            return Characters::Format (L"Error analyzing configuration file '%s' - using defaults.", ValueOrDefault (fFileName).c_str ());
        case Msg::eFailedToParseReadFileBadFormat:
            return Characters::Format (L"Error analyzing configuration file (because bad format) '%s' - using defaults.", ValueOrDefault (fFileName).c_str ());
        case Msg::eFailedToCompareReadFile:
            return Characters::Format (L"Failed to compare configuration file: %s", ValueOrDefault (fFileName).c_str ());
        case Msg::eWritingConfigFile_SoDefaultsEditable:
            return Characters::Format (L"Writing configuration file '%s' because not found (and so defaults are more easily seen and editable).", ValueOrDefault (fFileName).c_str ());
        case Msg::eWritingConfigFile_BecauseUpgraded:
            return Characters::Format (L"Writing configuration file '%s' in a new location because the software has been upgraded.", ValueOrDefault (fFileName).c_str ());
        case Msg::eWritingConfigFile_BecauseSomethingChanged:
            return Characters::Format (L"Writing configuration file '%s' because something changed (e.g. a default, or field added/removed).", ValueOrDefault (fFileName).c_str ());
        case Msg::eFailedToWriteInUseValues:
            return Characters::Format (L"Failed to write default (in use) values to file: %s", ValueOrDefault (fFileName).c_str ());
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
const OptionsFile::ModuleDataUpgraderType OptionsFile::kDefaultUpgrader = [](const optional<Configuration::Version>& /*version*/, const VariantValue& rawVariantValue) -> VariantValue {
    return rawVariantValue;
};

const OptionsFile::LoggerType OptionsFile::kDefaultLogger =
    [](const LoggerMessage& message) {
        using Execution::Logger;
        Logger::Priority priority = Logger::Priority::eError;
        using Msg                 = OptionsFile::LoggerMessage::Msg;
        switch (message.fMsg) {
            case Msg::eFailedToReadFile:
                priority = Logger::Priority::eWarning; // could be just because new system, no file
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
        Logger::Get ().Log (priority, L"%s", message.FormatMessage ().c_str ());
    };

OptionsFile::ModuleNameToFileNameMapperType OptionsFile::mkFilenameMapper (const String& appName)
{
    return
        [appName](const String& moduleName, const String& fileSuffix) -> String {
            return IO::FileSystem::WellKnownLocations::GetApplicationData () + appName + String (IO::FileSystem::kPathComponentSeperator) + moduleName + fileSuffix;
        };
}

const OptionsFile::ModuleNameToFileVersionMapperType OptionsFile::kDefaultModuleNameToFileVersionMapper = []([[maybe_unused]] const String & /*moduleName*/) -> optional<Configuration::Version> {
    return optional<Configuration::Version> (); // default to don't know
};

// Consider using XML by default when more mature
const Variant::Reader OptionsFile::kDefaultReader = Variant::JSON::Reader ();
const Variant::Writer OptionsFile::kDefaultWriter = Variant::JSON::Writer ();

OptionsFile::OptionsFile (
    const String&                     modName,
    const ObjectVariantMapper&        mapper,
    ModuleDataUpgraderType            moduleUpgrader,
    ModuleNameToFileNameMapperType    moduleNameToFileNameMapper,
    ModuleNameToFileVersionMapperType moduleNameToReadFileVersion,
    LoggerType                        logger,
    Variant::Reader                   reader,
    Variant::Writer                   writer)
    : OptionsFile (modName, mapper, moduleUpgrader, moduleNameToFileNameMapper, moduleNameToFileNameMapper, moduleNameToReadFileVersion, logger, reader, writer, reader.GetDefaultFileSuffix ())
{
}

OptionsFile::OptionsFile (
    const String&                     modName,
    const ObjectVariantMapper&        mapper,
    ModuleDataUpgraderType            moduleUpgrader,
    ModuleNameToFileNameMapperType    moduleNameToReadFileNameMapper,
    ModuleNameToFileNameMapperType    moduleNameToWriteFileNameMapper,
    ModuleNameToFileVersionMapperType moduleNameToReadFileVersion,
    LoggerType                        logger,
    Variant::Reader                   reader,
    Variant::Writer                   writer)
    : OptionsFile (modName, mapper, moduleUpgrader, moduleNameToReadFileNameMapper, moduleNameToWriteFileNameMapper, moduleNameToReadFileVersion, logger, reader, writer, reader.GetDefaultFileSuffix ())
{
}

OptionsFile::OptionsFile (
    const String&                     modName,
    const ObjectVariantMapper&        mapper,
    ModuleDataUpgraderType            moduleUpgrader,
    ModuleNameToFileNameMapperType    moduleNameToReadFileNameMapper,
    ModuleNameToFileNameMapperType    moduleNameToWriteFileNameMapper,
    ModuleNameToFileVersionMapperType moduleNameToReadFileVersion,
    LoggerType                        logger,
    Variant::Reader                   reader,
    Variant::Writer                   writer,
    const String&                     fileSuffix)
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

BLOB OptionsFile::ReadRaw () const
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"OptionsFile::ReadRaw", L"readfilename=%s", GetReadFilePath_ ().c_str ())};
    return IO::FileSystem::FileInputStream::New (GetReadFilePath_ ()).ReadAll ();
}

void OptionsFile::WriteRaw (const BLOB& blob)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"OptionsFile::WriteRaw", L"writefilename=%s", GetWriteFilePath_ ().c_str ())};
    if (GetReadFilePath_ () == GetWriteFilePath_ ()) {
        try {
            if (ReadRaw () == blob) {
                return;
            }
        }
        catch (...) {
            // No matter why we fail, nevermind. Just fall through and write.
        }
    }
    try {
        IO::FileSystem::ThroughTmpFileWriter  tmpFile (GetWriteFilePath_ ());
        IO::FileSystem::FileOutputStream::Ptr outStream = IO::FileSystem::FileOutputStream::New (tmpFile.GetFilePath ());
        outStream.Write (blob);
        outStream.Close (); // so any errors can be displayed as exceptions, and so closed before commit/rename
        tmpFile.Commit ();
    }
    catch (...) {
        fLogger_ (LoggerMessage (LoggerMessage::Msg::eFailedToWriteFile, GetWriteFilePath_ ()));
    }
}

template <>
optional<VariantValue> OptionsFile::Read ()
{
    Debug::TraceContextBumper ctx ("OptionsFile::Read");
    try {
        optional<VariantValue> r = fReader_.Read (MemoryStream<byte>::New (ReadRaw ()));
        if (r.has_value ()) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"present: upgrading module %s", fModuleName_.c_str ());
#endif
            r = fModuleDataUpgrader_ (fModuleNameToFileVersionMapper_ (fModuleName_), *r);
        }
        return r;
    }
    catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"exception");
#endif
        // @todo - check different exception cases and for some - like file not found - just no warning...
        fLogger_ (LoggerMessage (LoggerMessage::Msg::eFailedToReadFile, GetReadFilePath_ ()));
        return nullopt;
    }
}

template <>
void OptionsFile::Write (const VariantValue& optionsObject)
{
    Debug::TraceContextBumper ctx ("OptionsFile::Write");
    MemoryStream<byte>::Ptr   tmp = MemoryStream<Memory::byte>::New ();
    fWriter_.Write (optionsObject, tmp);
    WriteRaw (tmp.As<BLOB> ());
}

String OptionsFile::GetReadFilePath_ () const
{
    return fModuleNameToReadFileNameMapper_ (fModuleName_, fFileSuffix_);
}

String OptionsFile::GetWriteFilePath_ () const
{
    return fModuleNameToWriteFileNameMapper_ (fModuleName_, fFileSuffix_);
}
