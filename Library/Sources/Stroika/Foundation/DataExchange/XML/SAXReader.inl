/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_SAXReader_inl_
#define _Stroika_Foundation_DataExchange_XML_SAXReader_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Execution/ModuleInit.h"

namespace Stroika::Foundation::DataExchange::XML {

    /*
     ********************************************************************************
     ****************************** SAXReader_ModuleInit_ ***************************
     ********************************************************************************
     */
    struct SAXReader_ModuleInit_ {
        SAXReader_ModuleInit_ ();
        ~SAXReader_ModuleInit_ ();
#if qDefaultTracingOn
        Execution::ModuleDependency fDebugTraceDependency;
#endif
    };

}

CompileTimeFlagChecker_HEADER (Stroika::Foundation::DataExchange::XML, qHasFeature_Xerces, qHasFeature_Xerces);
namespace {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::DataExchange::XML::SAXReader_ModuleInit_> _Stroika_Foundation_DataExchange_XML_SAXReader_ModuleInit_; // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif /*_Stroika_Foundation_DataExchange_XML_SAXReader_inl_*/
