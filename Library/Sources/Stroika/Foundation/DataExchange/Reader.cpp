/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Streams/iostream/BinaryInputStreamFromIStreamAdapter.h"
#include    "../Streams/iostream/TextInputStreamFromIStreamAdapter.h"

#include    "Reader.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;


using   Streams::iostream::BinaryInputStreamFromIStreamAdapter;
using   Streams::iostream::TextInputStreamFromIStreamAdapter;



/*
 ********************************************************************************
 ****************************** DataExchange::Reader ****************************
 ********************************************************************************
 */
VariantValue    Reader::Read (istream& in)
{
    return Read (BinaryInputStreamFromIStreamAdapter (in));
}

VariantValue    Reader::Read (wistream& in)
{
    return Read (TextInputStreamFromIStreamAdapter (in));
}
