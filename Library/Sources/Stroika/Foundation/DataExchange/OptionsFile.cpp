/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "JSON/Reader.h"
#include    "JSON/Writer.h"
#include    "XML/Reader.h"
#include    "XML/Writer.h"

#include    "OptionsFile.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::DataExchange;




const   OptionsFile::LoggerType OptionsFile::kDefaultLogger =   [] (const String& message)
{
};


// Consider using XML by default when more mature
const   Reader  OptionsFile::kDefaultReader =   JSON::Reader ();
const   Writer  OptionsFile::kDefaultWriter =   JSON::Writer ();


OptionsFile::OptionsFile (const String& modName, const ObjectVariantMapper& mapper, LoggerType logger)
    : fModuleName_ (modName)
    , fMapper_ (mapper)
    , fLogError_ (logger)
{
}
