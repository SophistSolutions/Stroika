/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_XML_Options_h_
#define _Stroika_Foundation_DataExchange_XML_Options_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/DataExchange/XML/Common.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

// declared, not included: Providers/IProvider.h includes DOM.h, SAXReader.h and Schema.h, which include this
namespace Stroika::Foundation::DataExchange::XML::Providers {
    struct IXMLProvider;
}

namespace Stroika::Foundation::DataExchange::XML {

    /**
     *  \brief How XML is read and written - for now, with which provider - taken last by each XML function (DOM::Document::New,
     *         SAXParse, Schema::New), defaulted, so a caller says only what it wants different
     *
     *  A module whose own Options say how it reads or writes XML holds one of these (fXMLOptions), rather than a provider of its
     *  own.
     *
     *  \par Example Usage
     *      \code
     *          // read with Xerces, whatever the process's default
     *          const XML::Options      kXerces_{.fProvider = &XML::Providers::Xerces::kDefaultProvider};
     *          XML::DOM::Document::Ptr d = XML::DOM::Document::New (in, nullptr, kXerces_); // nullptr: no schema
     *      \endcode
     */
    struct Options {
        /**
         *  \brief Read and write with this provider - nullptr, the default, meaning Providers::kDefaultProvider: libxml2 where it
         *         is configured, else Xerces
         *
         *  A provider is a static singleton, so this is a plain pointer and nothing owns it.
         */
        const Providers::IXMLProvider* fProvider{nullptr};

#if qStroika_Foundation_DataExchange_XML_SupportParsing and qStroika_Foundation_DataExchange_XML_SupportSchema and qStroika_Foundation_DataExchange_XML_SupportDOM
        /**
         *  \brief fProvider, or the default where that is nullptr - the one actually used, and the one place that rule is written
         */
        nonvirtual const Providers::IXMLProvider& GetProvider () const;
#endif
    };

}

#endif /*_Stroika_Foundation_DataExchange_XML_Options_h_*/
