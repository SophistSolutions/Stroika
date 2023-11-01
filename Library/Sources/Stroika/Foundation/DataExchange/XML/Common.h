/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Common_h_
#define _Stroika_Foundation_DataExchange_XML_Common_h_ 1

#include "../../StroikaPreComp.h"

#include <memory>

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
