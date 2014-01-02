/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "SerializationConfiguration.h"

using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::DataExchange;
using   namespace   Stroika::Foundation::DataExchange::XML;




/*
 ********************************************************************************
 ********************* XML::SerializationConfiguration **************************
 ********************************************************************************
 */
SerializationConfiguration::SerializationConfiguration ()
    : fDocumentElementName_ (L"Document")
    , fArrayElementName_ (L"Array")
{
}
