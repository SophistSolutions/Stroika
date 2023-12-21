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

#include "../../../Characters/String.h"

// @todo understand why cannot #include on windoze!!!
//#include "../DOM.h"
//#include "../Schema.h"
namespace Stroika::Foundation::Characters {
    class String;
}

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 * 
 *      \note - Generally most code should NOT include this file - its implemeantion specific data for each provider and just included in implemtatniosn of DOM etc code
 */

namespace Stroika::Foundation::DataExchange::XML::Providers::LibXML2 {

    inline String libXMLString2String (const xmlChar* s, int len)
    {
        return String{span{reinterpret_cast<const char*> (s), static_cast<size_t> (len)}};
    }
    inline String libXMLString2String (const xmlChar* t)
    {
        return String::FromUTF8 (reinterpret_cast<const char*> (t));
    }

}

#endif /*_Stroika_Foundation_DataExchange_XML_Providers_LibXML2_h_*/
