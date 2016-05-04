/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Streams/iostream/OutputStreamFromStdOStream.h"
#include    "../Streams/MemoryStream.h"
#include    "../Streams/TextReader.h"

#include    "VariantWriter.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;


using   Streams::iostream::OutputStreamFromStdOStream;



/*
 ********************************************************************************
 *********************** DataExchange::VariantWriter ****************************
 ********************************************************************************
 */
void    VariantWriter::Write (const VariantValue& v, ostream& out)
{
    Write (v, OutputStreamFromStdOStream<Memory::Byte> (out));
}

void    VariantWriter::Write (const VariantValue& v, wostream& out)
{
    Write (v, OutputStreamFromStdOStream<Characters::Character> (out));
}

Memory::BLOB    VariantWriter::WriteAsBLOB (const VariantValue& v)
{
    Streams::MemoryStream<Memory::Byte> buf;
    Write(v, buf);
    return buf.As<Memory::BLOB>();
}

String  VariantWriter::WriteAsString (const VariantValue& v)
{
    return Streams::TextReader (WriteAsBLOB (v)).ReadAll ();
}