/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Providers_LibXML2_h_
#define _Stroika_Foundation_DataExchange_XML_Providers_LibXML2_h_ 1

#include "../../../StroikaPreComp.h"

static_assert (qHasFeature_libxml2, "Don't include this file if qHasFeature_libxml2 not set");

#include "../Common.h"
//include libxml2 stuff here

#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

//#include "../../../Characters/String.h"
namespace Stroika::Foundation::Characters {
    class String;
}

// @todo understand why cannot #include on windoze!!!
//#include "../DOM.h"
//#include "../Schema.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 * 
 *      \note - Generally most code should NOT include this file - its implemeantion specific data for each provider and just included in implemtatniosn of DOM etc code
 */

namespace Stroika::Foundation::DataExchange::XML::Providers::LibXML2 {

    using Characters::String;

    String libXMLString2String (const xmlChar* s, int len);
    String libXMLString2String (const xmlChar* t);

    /**
     */
    struct ILibXML2SchemaRep {
        virtual xmlSchema* GetSchemaLibRep () = 0;
    };

}

#endif /*_Stroika_Foundation_DataExchange_XML_Providers_LibXML2_h_*/
