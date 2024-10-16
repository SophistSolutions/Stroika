/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

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

void IConsumer::StartElement ([[maybe_unused]] const Name& name, [[maybe_unused]] const Mapping<Name, String>& attributes)
{
}

void IConsumer::EndElement ([[maybe_unused]] const Name& name)
{
}

void IConsumer::TextInsideElement ([[maybe_unused]] const String& text)
{
}
