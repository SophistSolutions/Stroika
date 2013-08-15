/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Streams/iostream/BinaryInputStreamFromIStreamAdapter.h"

#include    "Reader.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchangeFormat;


using   Streams::iostream::BinaryInputStreamFromIStreamAdapter;



/*
 ********************************************************************************
 ************************ DataExchangeFormat::Reader ****************************
 ********************************************************************************
 */
Memory::VariantValue    Reader::Read (istream& in)
{
    return Read (BinaryInputStreamFromIStreamAdapter (in));
}

