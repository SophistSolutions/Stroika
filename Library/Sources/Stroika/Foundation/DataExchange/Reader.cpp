/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Streams/iostream/InputStreamAdapter.h"

#include    "Reader.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;


using   Streams::iostream::InputStreamAdapter;



/*
 ********************************************************************************
 ****************************** DataExchange::Reader ****************************
 ********************************************************************************
 */
VariantValue    Reader::Read (istream& in)
{
    return Read (InputStreamAdapter<Memory::Byte> (in));
}

VariantValue    Reader::Read (wistream& in)
{
    return Read (InputStreamAdapter<Characters::Character> (in));
}
