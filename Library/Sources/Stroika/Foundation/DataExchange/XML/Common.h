/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Common_h_
#define _Stroika_Foundation_DataExchange_XML_Common_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::DataExchange::XML {

/**
 */
#ifndef qStroika_Foundation_DataExchange_XML_SupportDOM
#define qStroika_Foundation_DataExchange_XML_SupportDOM qStroika_HasComponent_xerces or qStroika_HasComponent_libxml2
#endif

/**
 */
#ifndef qStroika_Foundation_DataExchange_XML_SupportSchema
#define qStroika_Foundation_DataExchange_XML_SupportSchema qStroika_HasComponent_xerces or qStroika_HasComponent_libxml2
#endif

/**
 */
#ifndef qStroika_Foundation_DataExchange_XML_SupportParsing
#define qStroika_Foundation_DataExchange_XML_SupportParsing qStroika_HasComponent_xerces or qStroika_HasComponent_libxml2
#endif

/**
 *  Track memory allocations internally, and assure no memory leaks within XML code
 */
#ifndef qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations
#define qStroika_Foundation_DataExchange_XML_DebugMemoryAllocations qStroika_Foundation_Debug_AssertionsChecked
#endif

}
#endif /*_Stroika_Foundation_DataExchange_XML_Common_h_*/
