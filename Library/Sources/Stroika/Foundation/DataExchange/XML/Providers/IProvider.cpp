/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "IProvider.h"

#if qHasFeature_libxml2
#include "LibXML2.h"
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;

const XML::Providers::IXMLProvider* XML::Providers::Private_::GetDefaultProvider_ ()
{
#if qHasFeature_libxml2
    return &LibXML2::kDefault;
#endif
    AssertNotReached ();
    return static_cast<XML::Providers::IXMLProvider*> (nullptr);
}
