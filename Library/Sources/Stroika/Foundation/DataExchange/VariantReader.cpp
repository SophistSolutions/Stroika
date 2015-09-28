/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Streams/iostream/InputStreamFromStdIStream.h"

#include    "VariantReader.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;


using   Streams::iostream::InputStreamFromStdIStream;



/*
 ********************************************************************************
 *********************** DataExchange::VariantReader ****************************
 ********************************************************************************
 */
VariantValue    VariantReader::Read (const Memory::BLOB& in)
{
    return Read (in.As<InputStreamFromStdIStream<Memory::Byte>> ());
}

VariantValue    VariantReader::Read (istream& in)
{
    return Read (InputStreamFromStdIStream<Memory::Byte> (in));
}

VariantValue    VariantReader::Read (wistream& in)
{
    return Read (InputStreamFromStdIStream<Characters::Character> (in));
}
