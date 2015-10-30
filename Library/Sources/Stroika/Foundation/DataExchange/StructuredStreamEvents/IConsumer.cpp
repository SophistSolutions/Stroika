/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "IConsumer.h"



using   namespace  Stroika::Foundation::DataExchange;
using   namespace  Stroika::Foundation::DataExchange::StructuredStreamEvents;




/*
 ********************************************************************************
 ***************************** DataExchange::IConsumer **************************
 ********************************************************************************
 */

void    IConsumer::StartDocument ()
{
}

void    IConsumer::EndDocument ()
{
}

void    IConsumer::StartElement (const String& uri, const String& localName, const Mapping<String, VariantValue>& attrs)
{
}

void    IConsumer::EndElement (const String& uri, const String& localName)
{
}

void    IConsumer::CharactersInsideElement (const String& text)
{
}
