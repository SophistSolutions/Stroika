/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "IProvider.h"

#if qHasFeature_Xerces
#include "Xerces.h"
#endif
#if qHasFeature_libxml2
#include "LibXML2.h"
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;
using namespace Stroika::Foundation::DataExchange::XML;

#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
const XML::Providers::IXMLProvider* XML::Providers::Private_::GetDefaultProvider_ ()
{
#if qHasFeature_Xerces
    return &Xerces::kDefaultProvider;
#endif
#if qHasFeature_libxml2
    return &LibXML2::kDefaultProvider;
#endif
}
#endif
