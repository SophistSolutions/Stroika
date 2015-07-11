/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Streams/iostream/InputStreamFromStdIStream.h"

#include    "Reader.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;


using   Streams::iostream::InputStreamFromStdIStream;



/*
 ********************************************************************************
 ****************************** DataExchange::Reader ****************************
 ********************************************************************************
 */
VariantValue    Reader::Read (istream& in)
{
    return Read (InputStreamFromStdIStream<Memory::Byte> (in));
}

VariantValue    Reader::Read (wistream& in)
{
    return Read (InputStreamFromStdIStream<Characters::Character> (in));
}
