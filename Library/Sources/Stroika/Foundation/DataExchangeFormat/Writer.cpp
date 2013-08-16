/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/Format.h"
#include    "../Streams/iostream/BinaryOutputStreamFromOStreamAdapter.h"
//#include    "../Streams/iostream/TextOutputStreamFromOStreamAdapter.h"

#include    "Writer.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchangeFormat;


using   Streams::iostream::BinaryOutputStreamFromOStreamAdapter;
//using   Streams::iostream::TextOutputStreamFromOStreamAdapter;



/*
 ********************************************************************************
 ************************ DataExchangeFormat::Writer ****************************
 ********************************************************************************
 */
void    Writer::Write (const Memory::VariantValue& v, ostream& out)
{
	Write (v, BinaryOutputStreamFromOStreamAdapter (out));
}

void    Writer::Write (const Memory::VariantValue& v, wostream& out)
{
	AssertNotImplemented ();	// just because we never implemented
	//Write (v, TextOutputStreamFromOStreamAdapter (out));
}

