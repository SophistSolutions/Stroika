/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Streams/iostream/BinaryOutputStreamFromOStreamAdapter.h"

#include    "Writer.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchangeFormat;


using   Streams::iostream::BinaryOutputStreamFromOStreamAdapter;



/*
 ********************************************************************************
 ************************ DataExchangeFormat::Writer ****************************
 ********************************************************************************
 */
void    Writer::Write (const Memory::VariantValue& v, ostream& out)
{
	Write (v, BinaryOutputStreamFromOStreamAdapter (out));
}

