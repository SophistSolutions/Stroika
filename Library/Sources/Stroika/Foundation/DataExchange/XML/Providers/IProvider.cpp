/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "IProvider.h"

#if qStroika_HasComponent_xerces
#include "Xerces.h"
#endif
#if qStroika_HasComponent_libxml2
#include "LibXML2.h"
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;

#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
const XML::Providers::IXMLProvider* XML::Providers::Private_::GetDefaultProvider_ ()
{
    // libxml2 has better XPath support than Xerces (and so therefore also better schema validation).
    // so make it the default as of 2024-01-14
#if qStroika_HasComponent_libxml2
    return &LibXML2::kDefaultProvider;
#elif qStroika_HasComponent_xerces
    return &Xerces::kDefaultProvider;
#endif
}
#endif
