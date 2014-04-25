/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../IO/FileSystem/BinaryFileInputStream.h"
#include    "../IO/FileSystem/BinaryFileOutputStream.h"
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
const   OptionsFile::LoggerType OptionsFile::kDefaultLogger =
    [] (const String& message)
{
    //Logger::Log (Logger::Priority::eError, L"%s", message.c_str ());
};

const   OptionsFile::ModuleNameToFileNameMapperType  OptionsFile::mkFilenameMapper (const String& appName)
{
	String useAppName = appName;
    return
    [useAppName] (const String & moduleFileName) -> String {
        return IO::FileSystem::WellKnownLocations::GetApplicationData() + useAppName + moduleFileName ;
    }
    ;
}

// Consider using XML by default when more mature
const   Reader  OptionsFile::kDefaultReader =   JSON::Reader ();
const   Writer  OptionsFile::kDefaultWriter =   JSON::Writer ();

OptionsFile::OptionsFile (
    const String& modName,
    const ObjectVariantMapper& mapper,
    ModuleNameToFileNameMapperType moduleNameToFileNameMapper,
    LoggerType logWarning,
    LoggerType logError,
    Reader reader,
    Writer writer
)
    : fModuleName_ (modName)
    , fMapper_ (mapper)
    , fModuleNameToFileNameMapper_ (moduleNameToFileNameMapper)
    , fLogWarning_ (logWarning)
    , fLogError_ (logError)
    , fReader_ (reader)
    , fWriter_ (writer)
{
}

BLOB    OptionsFile::ReadRaw () const
{
    return IO::FileSystem::BinaryFileInputStream (fModuleNameToFileNameMapper_(fModuleName_)).ReadAll ();
}

void    OptionsFile::WriteRaw (const BLOB& blob)
{
    try {
        IO::FileSystem::ThroughTmpFileWriter    tmpFile (fModuleNameToFileNameMapper_(fModuleName_));
        IO::FileSystem::BinaryFileOutputStream  outStream (tmpFile.GetFilePath ());
        outStream.Write (blob);
        outStream.Flush();
        outStream.clear ();
        tmpFile.Commit ();
    }
    catch (...) {
        fLogError_ (Characters::Format (L"Failed to write file: %s", fModuleNameToFileNameMapper_(fModuleName_).c_str ()));
    }
}

template    <>
Optional<VariantValue>  OptionsFile::Read ()
{
    try {
        return fReader_.Read (BasicBinaryInputStream (ReadRaw ()));
    }
    catch (...) {
        // @todo - check different exception cases and for some - like file not found - just no warning...
        fLogWarning_ (Characters::Format (L"Failed to read file: %s", fModuleNameToFileNameMapper_(fModuleName_).c_str ()));
        return Optional<VariantValue> ();
    }
}

template    <>
void            OptionsFile::Write (const VariantValue& optionsObject)
{
    BasicBinaryOutputStream tmp;
    fWriter_.Write (optionsObject, tmp);
    WriteRaw (tmp.As<BLOB> ());
}
