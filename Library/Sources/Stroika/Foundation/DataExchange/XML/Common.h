/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Common_h_
#define _Stroika_Foundation_DataExchange_XML_Common_h_ 1

#include "../../StroikaPreComp.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::DataExchange::XML {

/**
 *  As of Stroika v3.0d4, we only support XML DOM if Xerces is builtin (could use libxml2 as well in the future).
 */
#ifndef qStroika_Foundation_DataExchange_XML_SupportDOM
#define qStroika_Foundation_DataExchange_XML_SupportDOM qHasFeature_Xerces
#endif

/**
 *  As of Stroika v3.0d4, we only support XML Schema if Xerces is builtin (could use libxml2 as well in the future).
 */
#ifndef qStroika_Foundation_DataExchange_XML_SupportSchema
#define qStroika_Foundation_DataExchange_XML_SupportSchema qHasFeature_Xerces or qFeature_HasFeature_libxml2
#endif

/**
 *  As of Stroika v3.0d4, we only support XML parsing if Xerces is builtin (could use libxml2 as well in the future).
 */
#ifndef qStroika_Foundation_DataExchange_XML_SupportParsing
#define qStroika_Foundation_DataExchange_XML_SupportParsing qHasFeature_Xerces
#endif

/**
 *  As of Stroika v3.0d4, we don't support XPath, but could re-include via XQilla (or less likely xalan or libxml2 in the future).
 */
#ifndef qStroika_Foundation_DataExchange_XML_SupportXPath
#define qStroika_Foundation_DataExchange_XML_SupportXPath 0
#endif

/**
 *  Track memory allocations internally, and assure no memory leaks within XML code
 */
#ifndef qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
#define qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations qDebug
#endif

}
#endif /*_Stroika_Foundation_DataExchange_XML_Common_h_*/
