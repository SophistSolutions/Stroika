/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/String_Constant.h"

#include "SerializationConfiguration.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;

using Characters::String_Constant;

/*
 ********************************************************************************
 ********************* XML::SerializationConfiguration **************************
 ********************************************************************************
 */
SerializationConfiguration::SerializationConfiguration ()
    : fDocumentElementName_ (String_Constant (L"Document"))
    , fArrayElementName_ (String_Constant (L"Array"))
{
}
