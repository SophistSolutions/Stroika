/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Characters/StringUtils.h"
#include    "../../Characters/Format.h"
#include    "../../Streams/TextInputStreamBinaryAdapter.h"

#include    "../BadFormatException.h"

#include    "Reader.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchangeFormat;







/*
 ********************************************************************************
 *********************** DataExchangeFormat::XML::Reader ************************
 ********************************************************************************
 */
Memory::VariantValue    DataExchangeFormat::XML::Reader (const Streams::TextInputStream& in)
{
#if     qHasLibrary_Xerces
    AssertNotImplemented ();
    return Memory::VariantValue ();
#else
    AssertNotImplemented ();
    return Memory::VariantValue ();
#endif
}

Memory::VariantValue    DataExchangeFormat::XML::Reader (const Streams::BinaryInputStream& in)
{
    return Reader (Streams::TextInputStreamBinaryAdapter (in));
}

