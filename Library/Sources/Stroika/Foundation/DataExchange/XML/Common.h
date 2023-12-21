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
#if qHasFeature_Xerces or qHasFeature_libxml2
    struct DependencyLibraryInitializer {
    private:
        mutex fMutex_;
#if qHasFeature_Xerces
        struct LibXerces;
        shared_ptr<LibXerces> fXERCES;
#endif
#if qHasFeature_libxml2
        struct LibXML2;
        shared_ptr<LibXML2> fLibXML2;
#endif
    public:
        DependencyLibraryInitializer ()                              = default;
        DependencyLibraryInitializer (DependencyLibraryInitializer&) = delete;
        ~DependencyLibraryInitializer ()                             = default;

    public:
        void UsingProvider (Provider p)
        {
            // check outside of lock cuz once set, not unset, and then lock if needed
            // this can false positive because no lock but we recheck inside UsingProvider_
#if qHasFeature_Xerces
            if (p == Provider::eXerces and not fXERCES) [[unlikely]] {
                UsingProvider_ (p);
            }
#endif
#if qHasFeature_libxml2
            if (p == Provider::eLibXml2 and not fLibXML2) [[unlikely]] {
                UsingProvider_ (p);
            }
#endif
        }

    private:
        void UsingProvider_ (Provider p);

    public:
        static DependencyLibraryInitializer sThe;
    };
    inline DependencyLibraryInitializer DependencyLibraryInitializer::sThe;
#endif

    // Expose private details for regression testing
    namespace Private {
        size_t GetNetAllocationCount ();
        size_t GetNetAllocatedByteCount ();
    }

}

#endif /*_Stroika_Foundation_DataExchange_XML_Common_h_*/
