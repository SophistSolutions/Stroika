/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/Format.h"
#include    "../../Streams/iostream/InputStreamFromStdIStream.h"
#include    "../../Streams/TextReader.h"

#include    "Reader.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;


using   Streams::iostream::InputStreamFromStdIStream;



/*
 ********************************************************************************
 **************************** Variant::VariantReader ****************************
 ********************************************************************************
 */
VariantValue    Variant::VariantReader::Read (const Traversal::Iterable<Characters::Character>& in)
{
    return Read (Streams::TextReader (in));
}

VariantValue    Variant::VariantReader::Read (const Memory::BLOB& in)
{
    return Read (in.As<Streams::InputStream<Memory::Byte>> ());
}

VariantValue    Variant::VariantReader::Read (istream& in)
{
    return Read (InputStreamFromStdIStream<Memory::Byte> (in));
}

VariantValue    Variant::VariantReader::Read (wistream& in)
{
    return Read (InputStreamFromStdIStream<Characters::Character> (in));
}
