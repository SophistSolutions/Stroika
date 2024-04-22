/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "SerializationConfiguration.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;

/*
 ********************************************************************************
 ********************* XML::SerializationConfiguration **************************
 ********************************************************************************
 */
SerializationConfiguration::SerializationConfiguration ()
    : fDocumentElementName_{"Document"sv}
    , fArrayElementName_{"Array"sv}
{
}
