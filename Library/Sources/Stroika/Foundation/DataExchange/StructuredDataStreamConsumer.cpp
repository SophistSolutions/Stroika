/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "StructuredDataStreamConsumer.h"



using   namespace  Stroika::Foundation::DataExchange;




/*
 ********************************************************************************
 *************** DataExchange::IStructuredDataStreamConsumer ********************
 ********************************************************************************
 */

void    IStructuredDataStreamConsumer::StartDocument ()
{
}

void    IStructuredDataStreamConsumer::EndDocument ()
{
}

void    IStructuredDataStreamConsumer::StartElement (const String& uri, const String& localName, const Mapping<String, VariantValue>& attrs)
{
}

void    IStructuredDataStreamConsumer::EndElement (const String& uri, const String& localName)
{
}

void    IStructuredDataStreamConsumer::CharactersInsideElement (const String& text)
{
}
