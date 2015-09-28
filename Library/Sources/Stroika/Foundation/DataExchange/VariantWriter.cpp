/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Streams/iostream/OutputStreamFromStdOStream.h"

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
    AssertNotImplemented ();    // just because we never implemented
    Write (v, OutputStreamFromStdOStream<Characters::Character> (out));
}

