/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Execution/Logger.h"
#include "Stroika/Foundation/IO/FileSystem/FileInputStream.h"
#include "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#include "Stroika/Foundation/IO/FileSystem/PathName.h"
#include "Stroika/Foundation/IO/FileSystem/ThroughTmpFileWriter.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"

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
OptionsFile::LoggerMessage::LoggerMessage (Msg msg, const filesystem::path& fn, const optional<String>& details)
    : fMsg{msg}
    , fFileName{fn}
    , fDetails{details}
{
}

String OptionsFile::LoggerMessage::FormatMessage () const
{
    String details{};
    if (fDetails) {
        StringBuilder sb;
        sb << "; "sv << *fDetails;
        details = sb.str ();
    }
    switch (fMsg) {
        case Msg::eSuccessfullyReadFile:
            return Characters::Format ("Successfully read configuration file {}."_f, fFileName);
        case Msg::eFailedToWriteFile:
            return Characters::Format ("Failed to write file {}: {}."_f, fFileName, details);
        case Msg::eFailedToReadFile:
            return Characters::Format ("Failed to read file {}: {}."_f, fFileName, details);
        case Msg::eFailedToParseReadFile:
            return Characters::Format ("Error analyzing configuration file {} - using defaults{}."_f, fFileName, details);
        case Msg::eFailedToParseReadFileBadFormat:
            return Characters::Format ("Error analyzing configuration file (because bad format) {} - using defaults{}."_f, fFileName, details);
        case Msg::eFailedToCompareReadFile:
            return Characters::Format ("Failed to compare configuration file: {}{}."_f, fFileName, details);
        case Msg::eWritingConfigFile_SoDefaultsEditable:
            return Characters::Format ("Writing configuration file {} because not found (and so defaults are more easily seen and editable){}."_f,
                                       fFileName, details);
        case Msg::eWritingConfigFile_BecauseUpgraded:
            return Characters::Format ("Writing configuration file {} in a new location because the software has been upgraded{}."_f, fFileName, details);
        case Msg::eWritingConfigFile_BecauseSomethingChanged:
            return Characters::Format ("Writing configuration file {} because something changed (e.g. a default, or field added/removed){}."_f,
                                       fFileName, details);
        case Msg::eFailedToWriteInUseValues:
            return Characters::Format ("Failed to write default (in use) values to file: {}{}."_f, fFileName, details);
        default:
            RequireNotReached ();
            return String{};
    }
}

/*
 ********************************************************************************
 ************************** DataExchange::OptionsFile ***************************
 ********************************************************************************
 */
const OptionsFile::ModuleDataUpgraderType OptionsFile::kDefaultUpgrader =
    [] (const optional<Configuration::Version>& /*version*/, const VariantValue& rawVariantValue) -> VariantValue { return rawVariantValue; };

const OptionsFile::LoggerType OptionsFile::kDefaultLogger = [] (const LoggerMessage& message) {
    using Execution::Logger;
    Logger::Priority priority = Logger::eError;
    using Msg                 = OptionsFile::LoggerMessage::Msg;
    switch (message.fMsg) {
        case Msg::eSuccessfullyReadFile:
            priority = Logger::eInfo;
            break;
        case Msg::eFailedToReadFile:
            priority = Logger::eWarning; // could be just because new system, no file
            break;
        case Msg::eWritingConfigFile_SoDefaultsEditable:
        case Msg::eWritingConfigFile_BecauseUpgraded:
        case Msg::eWritingConfigFile_BecauseSomethingChanged:
            priority = Logger::eInfo;
            break;

        case Msg::eFailedToParseReadFile:
        case Msg::eFailedToParseReadFileBadFormat:
            // Most likely very bad - as critical configuration data will be lost, and overwritten with 'defaults'
            priority = Logger::eCriticalError;
            break;
    }
    Logger::sThe.Log (priority, "{}"_f, message.FormatMessage ());
};

OptionsFile::ModuleNameToFileNameMapperType OptionsFile::mkFilenameMapper (const String& appName)
{
    return [appName] (const String& moduleName, const String& fileSuffix) -> filesystem::path {
        return IO::FileSystem::WellKnownLocations::GetApplicationData () / IO::FileSystem::ToPath (appName) /
               IO::FileSystem::ToPath (moduleName + fileSuffix);
    };
}

const OptionsFile::ModuleNameToFileVersionMapperType OptionsFile::kDefaultModuleNameToFileVersionMapper =
    [] ([[maybe_unused]] const String& /*moduleName*/) -> optional<Configuration::Version> {
    return optional<Configuration::Version> (); // default to don't know
};

// Consider using XML by default when more mature
const Variant::Reader OptionsFile::kDefaultReader = Variant::JSON::Reader{};
const Variant::Writer OptionsFile::kDefaultWriter = Variant::JSON::Writer{};

OptionsFile::OptionsFile (const String& modName, const ObjectVariantMapper& mapper, ModuleDataUpgraderType moduleUpgrader,
                          ModuleNameToFileNameMapperType moduleNameToFileNameMapper, ModuleNameToFileVersionMapperType moduleNameToReadFileVersion,
                          LoggerType logger, Variant::Reader reader, Variant::Writer writer)
    : OptionsFile{modName, mapper, moduleUpgrader, moduleNameToFileNameMapper,    moduleNameToFileNameMapper, moduleNameToReadFileVersion,
                  logger,  reader, writer,         reader.GetDefaultFileSuffix ()}
{
}

OptionsFile::OptionsFile (const String& modName, const ObjectVariantMapper& mapper, ModuleDataUpgraderType moduleUpgrader,
                          ModuleNameToFileNameMapperType moduleNameToReadFileNameMapper, ModuleNameToFileNameMapperType moduleNameToWriteFileNameMapper,
                          ModuleNameToFileVersionMapperType moduleNameToReadFileVersion, LoggerType logger, Variant::Reader reader, Variant::Writer writer)
    : OptionsFile{
          modName, mapper, moduleUpgrader, moduleNameToReadFileNameMapper, moduleNameToWriteFileNameMapper, moduleNameToReadFileVersion,
          logger,  reader, writer,         reader.GetDefaultFileSuffix ()}
{
}

OptionsFile::OptionsFile (const String& modName, const ObjectVariantMapper& mapper, ModuleDataUpgraderType moduleUpgrader,
                          ModuleNameToFileNameMapperType moduleNameToReadFileNameMapper, ModuleNameToFileNameMapperType moduleNameToWriteFileNameMapper,
                          ModuleNameToFileVersionMapperType moduleNameToReadFileVersion, LoggerType logger, Variant::Reader reader,
                          Variant::Writer writer, const String& fileSuffix)
    : fModuleName_{modName}
    , fMapper_{mapper}
    , fModuleDataUpgrader_{moduleUpgrader}
    , fModuleNameToReadFileNameMapper_{moduleNameToReadFileNameMapper}
    , fModuleNameToWriteFileNameMapper_{moduleNameToWriteFileNameMapper}
    , fModuleNameToFileVersionMapper_{moduleNameToReadFileVersion}
    , fLogger_{logger}
    , fReader_{reader}
    , fWriter_{writer}
    , fFileSuffix_{fileSuffix}
{
}

BLOB OptionsFile::ReadRaw () const
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("OptionsFile::ReadRaw", L"readfilename={}"_f, GetReadFilePath_ ())};
    return IO::FileSystem::FileInputStream::New (GetReadFilePath_ ()).ReadAll ();
}

void OptionsFile::WriteRaw (const BLOB& blob)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("OptionsFile::WriteRaw", L"writefilename={}"_f, GetWriteFilePath_ ())};
    if (GetReadFilePath_ () == GetWriteFilePath_ ()) {
        try {
            if (ReadRaw () == blob) {
                return;
            }
        }
        catch (...) {
            // No matter why we fail, never-mind. Just fall through and write.
        }
    }
    try {
        IO::FileSystem::ThroughTmpFileWriter  tmpFile{GetWriteFilePath_ ()};
        IO::FileSystem::FileOutputStream::Ptr outStream = IO::FileSystem::FileOutputStream::New (tmpFile.GetFilePath ());
        outStream.Write (blob);
        outStream.Close (); // so any errors can be displayed as exceptions, and so closed before commit/rename
        tmpFile.Commit ();
    }
    catch (...) {
        fLogger_ (LoggerMessage{LoggerMessage::Msg::eFailedToWriteFile, GetWriteFilePath_ (), Characters::ToString (current_exception ())});
    }
}

template <>
optional<VariantValue> OptionsFile::Read ()
{
    Debug::TraceContextBumper ctx{"OptionsFile::Read"};
    try {
        optional<VariantValue> r = fReader_.Read (MemoryStream::New<byte> (ReadRaw ()));
        if (r.has_value ()) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("present: upgrading module {}"_f, fModuleName_);
#endif
            r = fModuleDataUpgrader_ (fModuleNameToFileVersionMapper_ (fModuleName_), *r);
        }
        fLogger_ (LoggerMessage{LoggerMessage::Msg::eSuccessfullyReadFile, GetReadFilePath_ ()});
        return r;
    }
    catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("exception");
#endif
        // @todo - check different exception cases and for some - like file not found - just no warning...
        fLogger_ (LoggerMessage{LoggerMessage::Msg::eFailedToReadFile, GetReadFilePath_ (), Characters::ToString (current_exception ())});
        return nullopt;
    }
}

template <>
void OptionsFile::Write (const VariantValue& optionsObject)
{
    Debug::TraceContextBumper ctx{"OptionsFile::Write"};
    MemoryStream::Ptr<byte>   tmp = MemoryStream::New<byte> ();
    fWriter_.Write (optionsObject, tmp);
    WriteRaw (tmp.As<BLOB> ());
}

filesystem::path OptionsFile::GetReadFilePath_ () const
{
    return fModuleNameToReadFileNameMapper_ (fModuleName_, fFileSuffix_);
}

filesystem::path OptionsFile::GetWriteFilePath_ () const
{
    return fModuleNameToWriteFileNameMapper_ (fModuleName_, fFileSuffix_);
}
