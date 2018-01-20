/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "IConsumer.h"

using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::StructuredStreamEvents;

/*
 ********************************************************************************
 ***************************** DataExchange::IConsumer **************************
 ********************************************************************************
 */
void IConsumer::StartDocument ()
{
}

void IConsumer::EndDocument ()
{
}

void IConsumer::StartElement (const Name& name)
{
}

void IConsumer::EndElement (const Name& name)
{
}

void IConsumer::TextInsideElement (const String& text)
{
}
