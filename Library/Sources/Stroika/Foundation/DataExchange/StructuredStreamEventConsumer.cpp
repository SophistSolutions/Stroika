/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "StructuredStreamEventConsumer.h"



using   namespace  Stroika::Foundation::DataExchange;




/*
 ********************************************************************************
 *************** DataExchange::IStructuredStreamEventConsumer *******************
 ********************************************************************************
 */

void    IStructuredStreamEventConsumer::StartDocument ()
{
}

void    IStructuredStreamEventConsumer::EndDocument ()
{
}

void    IStructuredStreamEventConsumer::StartElement (const String& uri, const String& localName, const Mapping<String, VariantValue>& attrs)
{
}

void    IStructuredStreamEventConsumer::EndElement (const String& uri, const String& localName)
{
}

void    IStructuredStreamEventConsumer::CharactersInsideElement (const String& text)
{
}
