/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Common_h_
#define _Stroika_Foundation_DataExchange_XML_Common_h_ 1

#include "../../StroikaPreComp.h"

#include <memory>

#include "../../Configuration/Enumeration.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

/**
 *  \def qHasFeature_Xerces
 *      Stroika currently depends on Xerces to provide most XML services/functions./p>
 */
#ifndef qHasFeature_Xerces
#error "qHasFeature_Xerces should normally be defined indirectly by StroikaConfig.h"
#endif

namespace Stroika::Foundation::DataExchange::XML {

    using namespace std;

#if 0
#if __has_include(<xercesc/dom/DOM.hpp>)
    constexpr bool kHasXerces = true;
#else
    constexpr bool kHasXerces  = false;
#endif
#if __has_include(<libxml2/libxml/xmlversion.h>)
    constexpr bool kHasLibXML2 = true;
#else
    constexpr bool kHasLibXML2 = false;
#endif
#endif

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

    /**
        @todo add DefaultNames suport and range (begin/end bounds) and add ifdefs for ef each of these values around.
    
    */
    enum class Provider {
#if qHasFeature_Xerces
        eXerces,
#endif
#if qHasFeature_libxml2
        eLibXml2,
#endif

#if qHasFeature_Xerces and qHasFeature_libxml2
        Stroika_Define_Enum_Bounds (eXerces, eLibXml2)
#elif qHasFeature_Xerces
        Stroika_Define_Enum_Bounds (eXerces, eXerces)
#elif qHasFeature_libxml2
        Stroika_Define_Enum_Bounds (eLibXml2, eLibXml2)
#endif
    };

    /*
     * Automatically manage initialization of dependent libraries by any code which includes this module
     */
#if qHasFeature_Xerces
    struct DependencyLibraryInitializer {
        struct LibXerces;
        shared_ptr<LibXerces> fXERCES;
        DependencyLibraryInitializer ();
        static const DependencyLibraryInitializer sThe;
    };
    inline const DependencyLibraryInitializer DependencyLibraryInitializer::sThe;
#endif

    // Expose private details for regression testing
    namespace Private {
        size_t GetNetAllocationCount ();
        size_t GetNetAllocatedByteCount ();
    }

}

#endif /*_Stroika_Foundation_DataExchange_XML_Common_h_*/
