/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Streams/iostream/BinaryOutputStreamFromOStreamAdapter.h"
#include    "../Streams/iostream/TextOutputStreamFromOStreamAdapter.h"

#include    "Writer.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;


using   Streams::iostream::BinaryOutputStreamFromOStreamAdapter;
using   Streams::iostream::TextOutputStreamFromOStreamAdapter;



/*
 ********************************************************************************
 ****************************** DataExchange::Writer ****************************
 ********************************************************************************
 */
void    Writer::Write (const VariantValue& v, ostream& out)
{
    Write (v, BinaryOutputStreamFromOStreamAdapter (out));
}

void    Writer::Write (const VariantValue& v, wostream& out)
{
    AssertNotImplemented ();    // just because we never implemented
    Write (v, TextOutputStreamFromOStreamAdapter (out));
}

